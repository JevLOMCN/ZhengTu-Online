<?php
/**
 * lottery_draw.php — 安全抽奖 + 10连抽 + 防乱码写入 + 高奖公告优化
 * 2025-11-03
 */
declare(strict_types=1);
require_once __DIR__.'/security_bootstrap.php';
require_once __DIR__.'/db.php';
require_once __DIR__.'/lottery_config.php';
header('Content-Type: application/json; charset=utf-8');

$pdo = get_pdo_safe();
$pdo->exec("SET NAMES latin1");
$pdo->exec("SET character_set_client=latin1");
$pdo->exec("SET character_set_connection=latin1");
$pdo->exec("SET character_set_results=latin1");

/** GBK→UTF-8（读） */
function gbk_to_utf8($s){
    if ($s === null || $s === '') return '';
    if (mb_check_encoding($s,'UTF-8')) return $s;
    return @iconv('GBK','UTF-8//IGNORE',$s);
}
/** UTF-8→GBK（写） */
function utf8_to_gbk($s){
    if ($s === null || $s === '') return '';
    return @iconv('UTF-8','GBK//IGNORE',$s);
}

try {

    /* ---------- 奖池列表 ---------- */
    if (($_GET['action'] ?? '') === 'list') {
        $rows=$pdo->query("SELECT ID,NAME,REWARD_POINTS,WEIGHT FROM LOTTERY_POOL WHERE IS_ACTIVE=1 ORDER BY ID ASC")->fetchAll(PDO::FETCH_ASSOC);
        foreach($rows as &$r){
            $r['NAME']=gbk_to_utf8($r['NAME']);
            $r['REWARD_POINTS']=(int)$r['REWARD_POINTS'];
            $r['WEIGHT']=(int)$r['WEIGHT'];
        }
        json_ok(['ok'=>true,'items'=>$rows]);exit;
    }

    /* ---------- 跑马灯读取 ---------- */
    if (($_GET['action'] ?? '') === 'marquee') {
        $rows=$pdo->query("SELECT ACCOUNT,PRIZE_NAME,CHANGE_POINTS,CREATED_AT FROM LOTTERY_LOG WHERE CHANGE_POINTS>=1000000 ORDER BY ID DESC LIMIT 10")->fetchAll(PDO::FETCH_ASSOC);
        $out=[];
        foreach($rows as $r){
            $acc=$r['ACCOUNT'];
            if(strpos($acc,'@')!==false){
                [$pre,$post]=explode('@',$acc,2);
                $acc=substr($pre,0,2).str_repeat('*',4).substr($pre,-2).'@'.$post;
            }elseif(strlen($acc)>6){
                $acc=substr($acc,0,2).str_repeat('*',4).substr($acc,-2);
            }
            $out[]=[
                'ACCOUNT'=>$acc,
                'PRIZE_NAME'=>gbk_to_utf8($r['PRIZE_NAME']),
                'CHANGE_POINTS'=>(int)$r['CHANGE_POINTS'],
                'TIME'=>substr($r['CREATED_AT'],11,8)
            ];
        }
        json_ok(['ok'=>true,'items'=>$out]);exit;
    }

    /* ---------- 抽奖 ---------- */
    $account=trim($_POST['account']??'');
    $passwd =trim($_POST['passwd']??'');
    $mode   =$_POST['mode']??'single';
    if($account===''||$passwd==='') json_err('缺少账号或密码');

    $st=$pdo->prepare("SELECT UID,ACCOUNT,PASSWD FROM ACCSTORE0000 WHERE ACCOUNT=? LIMIT 1");
    $st->execute([$account]);
    $u=$st->fetch(PDO::FETCH_ASSOC);
    if(!$u) json_err('账号不存在');
    if($u['PASSWD']!==$passwd) json_err('密码错误');

    $uid=(int)$u['UID'];$acc=$u['ACCOUNT'];
    $times=($mode==='ten')?10:1;
    $totalCost=LOTTERY_COST_POINTS*$times;
    $seedBase=substr(md5($account.microtime(true)),0,16);

    $pdo->prepare("INSERT IGNORE INTO POINTBONUS0000 (UID,ACCOUNT,POINT,BONUS,HADFILLED) VALUES (?,?,0,0,0)")
        ->execute([$uid,$acc]);

    $st=$pdo->prepare("SELECT POINT FROM POINTBONUS0000 WHERE UID=? LIMIT 1");
    $st->execute([$uid]);$balance=(int)$st->fetchColumn();
    if($balance<$totalCost) json_err("点数不足，至少需要 {$totalCost} 点");

    $pools=$pdo->query("SELECT ID,NAME,REWARD_POINTS,WEIGHT FROM LOTTERY_POOL WHERE IS_ACTIVE=1")->fetchAll(PDO::FETCH_ASSOC);
    foreach($pools as &$p){$p['NAME']=gbk_to_utf8($p['NAME']);}
    if(!$pools) json_err('奖池未配置');

    $results=[];$sumReward=0;
    for($i=1;$i<=$times;$i++){
        $seed=$seedBase.'_'.$i;
        $st=$pdo->prepare("SELECT NONCE FROM LOTTERY_LOG WHERE UID=? ORDER BY ID DESC LIMIT 1");
        $st->execute([$uid]);$nonce=((int)$st->fetchColumn())+1;

        $mix=LOTTERY_SERVER_SEED.'|'.$seed.'|'.$nonce;
        $hash=hash('sha256',$mix);
        $num=hexdec(substr($hash,0,13));
        $rand=$num/0x20000000000000;

        $total=0;foreach($pools as $p){$total+=(int)$p['WEIGHT'];}
        $cursor=$rand*$total;$accw=0;$hit=$pools[0];
        foreach($pools as $p){$accw+=(int)$p['WEIGHT'];if($cursor<=$accw){$hit=$p;break;}}
        $pid=(int)$hit['ID'];$pname=$hit['NAME'];$reward=(int)$hit['REWARD_POINTS'];
        $sumReward+=$reward;

        if($reward>0){
            $pdo->prepare("UPDATE POINTBONUS0000 SET POINT=POINT+? WHERE UID=?")->execute([$reward,$uid]);
        }

        // 写日志（转GBK）
        $pname_gbk=utf8_to_gbk($pname);
        $acc_gbk=utf8_to_gbk($acc);
        $pdo->prepare("INSERT INTO LOTTERY_LOG
            (UID,ACCOUNT,PRIZE_ID,PRIZE_NAME,CHANGE_POINTS,COST_POINTS,CLIENT_SEED,SERVER_SEED_HASH,NONCE,RESULT_HASH,CREATED_AT)
            VALUES (?,?,?,?,?,?,?,?,?,?,NOW())")
            ->execute([$uid,$acc_gbk,$pid,$pname_gbk,$reward,LOTTERY_COST_POINTS,$seed,hash('sha256',LOTTERY_SERVER_SEED),$nonce,$hash]);

        $results[]=['name'=>$pname,'reward'=>$reward];
    }

    // 扣除成本
    $pdo->prepare("UPDATE POINTBONUS0000 SET POINT=POINT-? WHERE UID=? AND POINT>=?")
        ->execute([$totalCost,$uid,$totalCost]);

    // 余额
    $st=$pdo->prepare("SELECT POINT FROM POINTBONUS0000 WHERE UID=? LIMIT 1");
    $st->execute([$uid]);$cur=(int)$st->fetchColumn();

    // 高奖公告写入（≥1,000,000）
    if($sumReward>=1000000){
        $pdo->prepare("INSERT INTO LOTTERY_LOG
          (UID,ACCOUNT,PRIZE_ID,PRIZE_NAME,CHANGE_POINTS,COST_POINTS,CLIENT_SEED,SERVER_SEED_HASH,NONCE,RESULT_HASH,CREATED_AT)
          VALUES (?,?,?,?,?,?,?,?,?,?,NOW())")
          ->execute([$uid,utf8_to_gbk($acc),0,utf8_to_gbk('[系统公告] '.($mode==='ten'?'十连抽':'单抽').' 总奖 '.$sumReward.' 点'),
            $sumReward,0,'','','','']);
    }

    $msg = ($times==1)
      ? ($sumReward>0
          ? "抽奖成功，获得 {$sumReward} 点，消耗 {$totalCost} 点，余额 {$cur} 点。"
          : "未中奖，消耗 {$totalCost} 点，余额 {$cur} 点。")
      : "10连抽完成，共获得 {$sumReward} 点，总消耗 {$totalCost} 点，余额 {$cur} 点。";

    json_ok([
        'ok'=>true,
        'mode'=>$mode,
        'reward'=>$sumReward,
        'balance'=>$cur,
        'items'=>$results,
        'msg'=>$msg
    ]);

}catch(Throwable $e){
    sec_log('lottery_draw',['err'=>$e->getMessage()]);
    json_err('系统繁忙，请稍后再试');
}
