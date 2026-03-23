<?php
/**
 * epay_notify.php — PHP7.4
 * - 幂等入账（RECHARGEORDERS.STATUS 1->2）
 * - 固定文本返回
 * - 日志脱敏
 * - 签名：去 sign/sign_type/空值 → 按键名升序 → &key= → md5小写
 * - 根据表结构写 POINTBONUS0000(POINT) & CONSUMELOG0000 字段
 */
require_once __DIR__.'/security_bootstrap.php';
require_once __DIR__.'/db.php';

header('Content-Type: text/plain; charset=utf-8');

try {
  $post = $_POST ?: $_GET;
  if (!$post) { echo 'fail'; exit; }

  $data = [];
  foreach ($post as $k=>$v) {
    if ($k==='sign' || $k==='sign_type') continue;
    if ($v==='' || $v===null) continue;
    $data[$k] = $v;
  }
  ksort($data);
  $sign_str = urldecode(http_build_query($data)) . EPAY_KEY;
  $expect = strtolower(md5($sign_str));
  $sign = strtolower((string)($post['sign'] ?? ''));
  if ($sign !== $expect) { sec_log('notify', ['msg'=>'sign_error','post'=>$post]); echo 'sign_error'; exit; }

  $ts = isset($post['timestamp']) ? (int)$post['timestamp'] : 0;
  if ($ts>0 && abs(time()-$ts) > 300) { sec_log('notify',['msg'=>'timeout','ts'=>$ts]); echo 'timeout'; exit; }

  $tid         = (string)($post['out_trade_no'] ?? '');
  $trade_no    = (string)($post['trade_no'] ?? '');
  $trade_status= (string)($post['trade_status'] ?? 'TRADE_SUCCESS');
  $paid_money  = (float)($post['money'] ?? 0);

  if ($tid==='') { echo 'no_order'; exit; }
  if ($trade_status!=='TRADE_SUCCESS') { echo 'ignore'; exit; }

  $pdo = get_pdo_safe();
  $pdo->beginTransaction();

  $st = $pdo->prepare("SELECT * FROM RECHARGEORDERS WHERE TID=:tid FOR UPDATE");
  $st->execute([':tid'=>$tid]);
  $ord = $st->fetch(PDO::FETCH_ASSOC);
  if (!$ord) { $pdo->rollBack(); echo 'no_local_order'; exit; }

  if ((int)$ord['STATUS'] !== 1) { $pdo->rollBack(); echo 'success'; exit; }

  $localAmtYuan = ((int)$ord['AMOUNT'])/100.0;
  if (abs($localAmtYuan - $paid_money) > 0.01) {
    sec_log('notify', ['msg'=>'money_mismatch','local'=>$localAmtYuan,'paid'=>$paid_money]);
  }

  $uid = (int)$ord['UID'];
  $account = $ord['ACCOUNT'];
  $points = (int)$ord['POINTS'];

  $pdo->prepare("INSERT IGNORE INTO POINTBONUS0000 (UID, ACCOUNT, POINT, BONUS, HADFILLED) VALUES (:uid,:acc,0,0,0)")
      ->execute([':uid'=>$uid, ':acc'=>$account]);

  $pdo->prepare("UPDATE POINTBONUS0000 SET POINT=POINT+:p WHERE UID=:uid")
      ->execute([':p'=>$points, ':uid'=>$uid]);

  $st = $pdo->prepare("SELECT POINT FROM POINTBONUS0000 WHERE UID=:uid LIMIT 1");
  $st->execute([':uid'=>$uid]);
  $curPoint = (int)$st->fetchColumn();

  $tidFill = $tid;
  $pdo->prepare("INSERT INTO CONSUMELOG0000 (TID, UID, ACCOUNT, TYPE, SOURCE, CARDID, FILLPOINT, CONSUMEPOINT, THISPOINT, IP)
    VALUES (:tid,:uid,:acc,1,0,NULL,:fp,0,:tp,:ip)")
      ->execute([':tid'=>$tidFill, ':uid'=>$uid, ':acc'=>$account, ':fp'=>$points, ':tp'=>$curPoint, ':ip'=>client_ip()]);

  $pdo->prepare("UPDATE RECHARGEORDERS SET STATUS=2, PAID_AT=NOW(), TRADE_NO=:tn WHERE TID=:tid AND STATUS=1")
      ->execute([':tn'=>$trade_no, ':tid'=>$tid]);

  $pdo->commit();
  echo 'success';
} catch (Throwable $e) {
  sec_log('notify', ['err'=>$e->getMessage(), 'line'=>$e->getLine(), 'file'=>$e->getFile()]);
  echo 'fail';
}
