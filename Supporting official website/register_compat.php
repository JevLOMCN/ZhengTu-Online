<?php
declare(strict_types=1);

ob_start();
header('Content-Type: application/json; charset=utf-8');
date_default_timezone_set('Asia/Shanghai');
ini_set('display_errors', '0');
error_reporting(E_ALL);

$_JSON_ERRORS = [];
set_error_handler(function($errno,$errstr,$errfile,$errline){
  global $_JSON_ERRORS; $_JSON_ERRORS[] = "PHP[$errno] $errstr @ $errfile:$errline"; return true;
});
set_exception_handler(function($e){ json_out(['ok'=>false,'msg'=>'未捕获异常','php_error'=>$e->getMessage()]); });

require_once __DIR__ . '/db.php';

const VERIFY_TTL_SEC      = 600;   // 10分钟
const SEND_COOLDOWN_SEC   = 31536000;  // 同邮箱一年内只发一次，防止频繁刷邮件

// === 推荐人奖励配置（可调） ===
const REF_REWARD_NEW       = 600000; // 被推荐人奖励
const REF_REWARD_REFERRER  = 500000; // 推荐人奖励
const REF_LIMIT_PER_DAY    = 10;      // 推荐人每日最多获得奖励次数
const REF_COOLDOWN_MINUTES = 5;      // 推荐奖励冷却（分钟）
const REF_SOURCE_CODE      = 9001;   // CONSUMELOG0000.SOURCE 用于标记“推荐奖励”

function db_connect(): mysqli {
  $host = $GLOBALS['DB_HOST'] ?? '121.204.249.166';
  $user = $GLOBALS['DB_USER'] ?? 'ztgame';
  $pass = $GLOBALS['DB_PASS'] ?? 'Cjy12140113';
  $name = $GLOBALS['DB_NAME'] ?? 'FLServerDB';
  $db = @new mysqli($host,$user,$pass,$name);
  if ($db->connect_errno) json_out(['ok'=>false,'msg'=>'数据库连接失败','php_error'=>$db->connect_error]);
  $db->set_charset('latin1');
  return $db;
}
function json_out(array $o){
  global $_JSON_ERRORS;
  if (!empty($_JSON_ERRORS)) { $o['php_error'] = $o['php_error'] ?? implode(' | ', $_JSON_ERRORS); }
  if (ob_get_length() !== false) ob_clean();
  echo json_encode($o, JSON_UNESCAPED_UNICODE); exit;
}
function make_code(): string {
  return str_pad((string)random_int(0,999999),6,'0',STR_PAD_LEFT);
}

/** 必走 SMTP 的发信器（只用 PHPMailer，不再回退 mail()） */
function send_mail_via_phpmailer(string $to, string $subject, string $html, string $text=''): array {
  $autoload = __DIR__ . '/vendor/autoload.php';
  if (file_exists($autoload)) { require_once $autoload; }
  if (!class_exists('PHPMailer\PHPMailer\PHPMailer')) {
    $base = __DIR__ . '/PHPMailer/src';
    $ok = file_exists("$base/PHPMailer.php") && file_exists("$base/SMTP.php") && file_exists("$base/Exception.php");
    if ($ok) {
      require_once "$base/PHPMailer.php";
      require_once "$base/SMTP.php";
      require_once "$base/Exception.php";
    }
  }
  if (!class_exists('PHPMailer\PHPMailer\PHPMailer')) {
    return [false, '未找到 PHPMailer：请安装 phpmailer/phpmailer 或上传 PHPMailer/src。'];
  }

  $host     = $GLOBALS['SMTP_HOST']   ?? $GLOBALS['MAIL_HOST']   ?? '';
  $port     = $GLOBALS['SMTP_PORT']   ?? $GLOBALS['MAIL_PORT']   ?? 465;
  $user     = $GLOBALS['SMTP_USER']   ?? $GLOBALS['MAIL_USER']   ?? '';
  $pass     = $GLOBALS['SMTP_PASS']   ?? $GLOBALS['MAIL_PASS']   ?? '';
  $secure   = $GLOBALS['SMTP_SECURE'] ?? $GLOBALS['MAIL_SECURE'] ?? 'ssl';
  $from     = $GLOBALS['SMTP_FROM']   ?? $GLOBALS['MAIL_FROM']   ?? $user;
  $fromName = $GLOBALS['SMTP_FROM_NAME'] ?? $GLOBALS['MAIL_FROM_NAME'] ?? '系统通知';
  if (!$host || !$user || !$pass) {
    return [false, 'SMTP 配置不完整（需 SMTP_HOST/SMTP_USER/SMTP_PASS）'];
  }
  try {
    $mail = new PHPMailer\PHPMailer\PHPMailer(true);
    $mail->isSMTP();
    $mail->Host       = $host;
    $mail->SMTPAuth   = true;
    $mail->Username   = $user;
    $mail->Password   = $pass;
    $mail->Port       = (int)$port;
    $mail->SMTPSecure = $secure ?: PHPMailer\PHPMailer\PHPMailer::ENCRYPTION_SMTPS;
    $mail->CharSet    = 'UTF-8';
    $mail->setFrom($from ?: $user, $fromName);
    $mail->addAddress($to);
    $mail->isHTML(true);
    $mail->Subject = $subject;
    $mail->Body    = $html;
    $mail->AltBody = $text ?: strip_tags($html);
    $mail->send();
    return [true, 'ok'];
  } catch (Throwable $e) {
    return [false, 'SMTP 发送失败: '.$e->getMessage()];
  }
}

$db = db_connect();
$ip = $_SERVER['REMOTE_ADDR'] ?? '';
$action = $_REQUEST['action'] ?? '';

// ======= 工具函数们 =======
function ensure_point_row(mysqli $db, int $uid, string $account): bool {
  $q = $db->prepare("SELECT UID FROM POINTBONUS0000 WHERE UID=?");
  if(!$q) return false;
  $q->bind_param('i',$uid);
  $q->execute();
  $q->store_result();
  $exists = $q->num_rows > 0;
  $q->free_result();
  $q->close();
  if ($exists) return true;
  $point=0; $bonus=0; $hadfilled=0;
  $ins = $db->prepare("INSERT INTO POINTBONUS0000(UID,ACCOUNT,POINT,BONUS,HADFILLED) VALUES(?,?,?,?,?)");
  if(!$ins) return false;
  $ins->bind_param('isiii',$uid,$account,$point,$bonus,$hadfilled);
  $ok = $ins->execute();
  $ins->close();
  return $ok;
}
function add_points_and_log(mysqli $db, int $uid, string $account, int $deltaPoint, string $tidPrefix, string $ip, int $sourceCode = REF_SOURCE_CODE): bool {
  if (!ensure_point_row($db, $uid, $account)) return false;
  $up = $db->prepare("UPDATE POINTBONUS0000 SET POINT=POINT+? WHERE UID=?");
  if(!$up) return false;
  $up->bind_param('ii',$deltaPoint,$uid);
  if(!$up->execute()){ $up->close(); return false; }
  $up->close();

  $curQ = $db->prepare("SELECT POINT FROM POINTBONUS0000 WHERE UID=?");
  if(!$curQ) return false;
  $curQ->bind_param('i',$uid);
  $curQ->execute();
  $curQ->bind_result($curPoint);
  $curQ->fetch();
  $curQ->close();
  $thisPoint = (int)$curPoint;

  $tid = uniqid($tidPrefix, true);
  $type = 1;
  $source = $sourceCode;
  $cardid = null;
  $fill = $deltaPoint;
  $consume = 0;

  $ins = $db->prepare("INSERT INTO CONSUMELOG0000(TID,UID,ACCOUNT,TYPE,SOURCE,CARDID,FILLPOINT,CONSUMEPOINT,THISPOINT,IP) VALUES(?,?,?,?,?,?,?,?,?,?)");
  if(!$ins) return false;
  $ins->bind_param('sisiisiiis',$tid,$uid,$account,$type,$source,$cardid,$fill,$consume,$thisPoint,$ip);
  $ok = $ins->execute();
  $ins->close();
  return $ok;
}
function get_uid_by_account(mysqli $db, string $account): ?int {
  $q = $db->prepare("SELECT UID FROM ACCSTORE0000 WHERE ACCOUNT=?");
  if(!$q) return null;
  $q->bind_param('s',$account);
  $q->execute();
  $q->bind_result($uid);
  if($q->fetch()){
    $q->close();
    return (int)$uid;
  }
  $q->close();
  return null;
}

// === 发送验证码 ===
if ($action === 'send_code') {
  $email = trim($_POST['email'] ?? '');
  if ($email === '') json_out(['ok'=>false,'msg'=>'email 不能为空']);
  if (!filter_var($email, FILTER_VALIDATE_EMAIL)) json_out(['ok'=>false,'msg'=>'email 格式不正确']);

  $q = $db->prepare("SELECT created_at FROM REG_EMAIL_CODE WHERE email=? ORDER BY created_at DESC LIMIT 1");
  $q->bind_param('s',$email);
  $q->execute();
  $q->bind_result($lastCreated);
  if ($q->fetch()) {
    $last = strtotime((string)$lastCreated);
    if (time() - $last < SEND_COOLDOWN_SEC) {
      $left = SEND_COOLDOWN_SEC - (time() - $last);
      json_out(['ok'=>false,'msg'=>"发送太频繁，请 {$left} 秒后再试"]);
    }
  }
  $q->close();

  $code = make_code();
  $now = date('Y-m-d H:i:s');
  $exp = date('Y-m-d H:i:s', time()+VERIFY_TTL_SEC);
  $used=0;

  $ins = $db->prepare("INSERT INTO REG_EMAIL_CODE(email, code, expire_at, used, created_at, ip) VALUES(?,?,?,?,?,?)");
  if(!$ins) json_out(['ok'=>false,'msg'=>'写入验证码失败','php_error'=>$db->error]);
  $ins->bind_param('ssssss',$email,$code,$exp,$used,$now,$ip);
  $ok = $ins->execute();
  $ins->close();
  if(!$ok) json_out(['ok'=>false,'msg'=>'写入验证码失败','php_error'=>$db->error]);

  $subject = '您的验证码 / Verification Code';
  $html = "<div style='font:14px/1.6 -apple-system,Segoe UI,Roboto,Helvetica,Arial,sans-serif'>
    <p>您好！您正在进行注册验证操作。</p>
    <p>本次验证码为：<b style=\"font-size:20px\">{$code}</b></p>
    <p>有效期：".(VERIFY_TTL_SEC/60)." 分钟，请勿泄露给他人。</p>
    <hr><p style='color:#888'>邮箱：{$email}，IP：{$ip}，时间：{$now}</p></div>";
  [$ok,$err] = send_mail_via_phpmailer($email, $subject, $html);
  if (!$ok) json_out(['ok'=>false,'msg'=>'邮件发送失败','detail'=>$err]);
  json_out(['ok'=>true,'尊敬的玩家'=>'验证码已发送，请查收邮箱','时间'=>$exp]);
}

// === 注册 ===
if ($action === 'register') {
  $email    = trim($_POST['email'] ?? '');
  $account  = trim($_POST['account'] ?? $email);
  $passwd   = strval($_POST['passwd'] ?? '');
  $code     = trim($_POST['code'] ?? '');
  $referrer = trim($_POST['referrer'] ?? '');

  if ($email === '' || $passwd === '') json_out(['ok'=>false,'msg'=>'邮箱与密码不能为空']);
  if (!filter_var($email, FILTER_VALIDATE_EMAIL)) json_out(['ok'=>false,'msg'=>'email 格式不正确']);
  if ($code === '') json_out(['ok'=>false,'msg'=>'验证码不能为空']);

  // 验证验证码
  $q = $db->prepare("SELECT id, code, expire_at, used FROM REG_EMAIL_CODE WHERE email=? ORDER BY created_at DESC LIMIT 1");
  $q->bind_param('s',$email);
  $q->execute();
  $q->bind_result($rid,$rc,$rexp,$rused);
  if (!$q->fetch()) json_out(['ok'=>false,'msg'=>'请先获取验证码']);
  $q->close();
  if ((int)$rused === 1) json_out(['ok'=>false,'msg'=>'验证码已使用，请重新获取']);
  if (time() > strtotime((string)$rexp)) json_out(['ok'=>false,'msg'=>'验证码已过期，请重新获取']);
  if ($code !== (string)$rc) json_out(['ok'=>false,'msg'=>'验证码错误']);

  // 同IP仅能注册1账号
  $checkIp = $db->prepare("SELECT COUNT(*) FROM ACCSTORE0000 WHERE REGIP=?");
  $checkIp->bind_param('s',$ip);
  $checkIp->execute();
  $checkIp->bind_result($ipcnt);
  $checkIp->fetch();
  $checkIp->close();
  if ((int)$ipcnt > 0) json_out(['ok'=>false,'msg'=>'同一IP仅能注册1个账号']);

  // 账号查重
  $chk = $db->prepare("SELECT UID FROM ACCSTORE0000 WHERE ACCOUNT=?");
  $chk->bind_param('s',$account);
  $chk->execute();
  $chk->bind_result($exist);
  if ($chk->fetch()) json_out(['ok'=>false,'msg'=>'账号已存在','account'=>$account]);
  $chk->close();

  // 注册事务
  $db->begin_transaction();
  try {
    $now = date('Y-m-d H:i:s');
    $state=0;
    $type=0;
    $regip=$_SERVER['REMOTE_ADDR'] ?? '';

    $ins1 = $db->prepare("INSERT INTO ACCSTORE0000(ACCOUNT,PASSWD,STATE,TYPE,LASTACTIVEDATE,REGIP) VALUES(?,?,?,?,?,?)");
    $ins1->bind_param('ssisss',$account,$passwd,$state,$type,$now,$regip);
    $ins1->execute();
    $uid = $db->insert_id;
    $ins1->close();

    $point=0; $bonus=0; $hadfilled=0;
    $ins2 = $db->prepare("INSERT INTO POINTBONUS0000(UID,ACCOUNT,POINT,BONUS,HADFILLED) VALUES(?,?,?,?,?)");
    $ins2->bind_param('isiii',$uid,$account,$point,$bonus,$hadfilled);
    $ins2->execute();
    $ins2->close();

    $upd = $db->prepare("UPDATE REG_EMAIL_CODE SET used=1 WHERE id=?");
    $upd->bind_param('i',$rid);
    $upd->execute();
    $upd->close();

    $db->commit();
  } catch (Throwable $e) {
    $db->rollback();
    json_out(['ok'=>false,'msg'=>'注册事务失败','detail'=>$e->getMessage()]);
  }

  // 无推荐人：直接返回成功，不再进入推荐奖励逻辑
  if ($referrer === '') {
    json_out(['ok'=>true,'msg'=>'注册成功','uid'=>$uid,'account'=>$account]);
  }

  // 推荐奖励限制逻辑（每日10次 + 冷却5分钟）
  $dateToday = date('Y-m-d');
  $stmt = $db->prepare("SELECT COUNT(*) FROM ACCSTORE0000 WHERE REFERRER=? AND DATE(REFERRER_TIME)=?");
  $stmt->bind_param('ss', $referrer, $dateToday);
  $stmt->execute();
  $stmt->bind_result($ref_today_count);
  $stmt->fetch();
  $stmt->close();
  if ($ref_today_count >= REF_LIMIT_PER_DAY) {
    json_out(['ok'=>true,'msg'=>'注册成功（推荐人今日已达奖励上限）','uid'=>$uid,'account'=>$account]);
  }

  $stmt = $db->prepare("SELECT MAX(REFERRER_TIME) FROM ACCSTORE0000 WHERE REFERRER=?");
  $stmt->bind_param('s', $referrer);
  $stmt->execute();
  $stmt->bind_result($last_time);
  $stmt->fetch();
  $stmt->close();
  if ($last_time && (time() - strtotime($last_time)) < (REF_COOLDOWN_MINUTES * 60)) {
    json_out(['ok'=>true,'msg'=>'注册成功（推荐奖励冷却中）','uid'=>$uid,'account'=>$account]);
  }

  $ref_uid = get_uid_by_account($db,$referrer);
  if ($ref_uid === null) json_out(['ok'=>true,'msg'=>'注册成功（推荐人不存在）','uid'=>$uid,'account'=>$account,'referrer'=>$referrer]);

  // 防刷
  $todayCnt = 0;
  $q1 = $db->prepare("SELECT COUNT(*) FROM CONSUMELOG0000 WHERE ACCOUNT=? AND SOURCE=? AND DATE(OPERTIME)=CURDATE()");
  if($q1){
    $src=REF_SOURCE_CODE;
    $q1->bind_param('si',$referrer,$src);
    $q1->execute();
    $q1->bind_result($todayCnt);
    $q1->fetch();
    $q1->close();
  }

  $coolOk=true; $lastTime=null;
  $q2=$db->prepare("SELECT OPERTIME FROM CONSUMELOG0000 WHERE ACCOUNT=? AND SOURCE=? ORDER BY OPERTIME DESC LIMIT 1");
  if($q2){
    $src=REF_SOURCE_CODE;
    $q2->bind_param('si',$referrer,$src);
    $q2->execute();
    $q2->bind_result($lastTime);
    $q2->fetch();
    $q2->close();
    if($lastTime && (time()-strtotime($lastTime))<(REF_COOLDOWN_MINUTES*60)){
      $coolOk=false;
    }
  }

  if($todayCnt>=REF_LIMIT_PER_DAY || !$coolOk){
    $upRel=$db->prepare("UPDATE ACCSTORE0000 SET REFERRER=?,REFERRER_TIME=? WHERE UID=?");
    if($upRel){
      $now=date('Y-m-d H:i:s');
      $upRel->bind_param('ssi',$referrer,$now,$uid);
      $upRel->execute();
      $upRel->close();
    }
    json_out(['ok'=>true,'msg'=>'注册成功（推荐人今日奖励已满或冷却中）','uid'=>$uid,'account'=>$account,'referrer'=>$referrer]);
  }

  // 发放基础推荐奖励
  $db->begin_transaction();
  try{
    $now=date('Y-m-d H:i:s');
    if(!add_points_and_log($db,$ref_uid,$referrer,REF_REWARD_REFERRER,'REF',$ip,REF_SOURCE_CODE)) throw new Exception('推荐人加点写入失败');
    if(!add_points_and_log($db,$uid,$account,REF_REWARD_NEW,'NEW',$ip,REF_SOURCE_CODE)) throw new Exception('新用户加点写入失败');

    $upRel=$db->prepare("UPDATE ACCSTORE0000 SET REFERRER=?,REFERRER_TIME=? WHERE UID=?");
    $upRel->bind_param('ssi',$referrer,$now,$uid);
    $upRel->execute();
    $upRel->close();
    $db->commit();

    // === 阶段性推广奖励检测 ===
    try {
      $total_ref=0;
      $q=$db->prepare("SELECT COUNT(*) FROM ACCSTORE0000 WHERE REFERRER=?");
      if($q){
        $q->bind_param('s',$referrer);
        $q->execute();
        $q->bind_result($total_ref);
        $q->fetch();
        $q->close();
      }
      $stages=[10=>5000000,20=>20000000,50=>100000000];
      $logTbl="REFERRER_STAGE_LOG";
      $checkSql="SELECT AWARDED FROM {$logTbl} WHERE REFERRER=? AND STAGE=? LIMIT 1";
      $insSql="REPLACE INTO {$logTbl}(REFERRER,STAGE,AWARDED,AWARD_TIME) VALUES(?,?,1,?)";
      foreach($stages as $stage=>$award){
        if($total_ref>=$stage){
          $awarded=0;
          $qc=$db->prepare($checkSql);
          if($qc){
            $qc->bind_param('si',$referrer,$stage);
            $qc->execute();
            $qc->bind_result($awarded);
            $qc->fetch();
            $qc->close();
          }
          if((int)$awarded===1) continue;

          $db->begin_transaction();
          try{
            if(!add_points_and_log($db,$ref_uid,$referrer,$award,"BONUS{$stage}",$ip,9002))
              throw new Exception("发放阶段奖励失败：{$stage}人档");
            $ins=$db->prepare($insSql);
            $nowStage=date('Y-m-d H:i:s');
            $ins->bind_param('sis',$referrer,$stage,$nowStage);
            $ins->execute();
            $ins->close();
            $db->commit();
          }catch(Throwable $e){
            $db->rollback();
            error_log('阶段奖励失败: '.$e->getMessage());
          }
        }
      }
    }catch(Throwable $e){
      error_log('阶段奖励检测异常: '.$e->getMessage());
    }

    json_out([
      'ok'=>true,
      'msg'=>'注册成功并发放推荐奖励',
      'uid'=>$uid,
      'account'=>$account,
      'referrer'=>$referrer,
      'bonus_new'=>REF_REWARD_NEW,
      'bonus_ref'=>REF_REWARD_REFERRER
    ]);
  }catch(Throwable $e){
    $db->rollback();
    json_out([
      'ok'=>true,
      'msg'=>'注册成功但发放推荐奖励失败',
      'detail'=>$e->getMessage(),
      'uid'=>$uid,
      'account'=>$account,
      'referrer'=>$referrer
    ]);
  }
}

json_out(['ok'=>false,'msg'=>'无效的 action']);
?>
