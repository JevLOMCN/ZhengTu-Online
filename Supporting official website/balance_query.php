<?php
header('Content-Type: application/json; charset=utf-8');
date_default_timezone_set('Asia/Shanghai');
require_once __DIR__ . '/db.php';

function db_connect() {
    $host = isset($GLOBALS['DB_HOST']) ? $GLOBALS['DB_HOST'] : '121.204.249.166';
    $user = isset($GLOBALS['DB_USER']) ? $GLOBALS['DB_USER'] : 'ztgame';
    $pass = isset($GLOBALS['DB_PASS']) ? $GLOBALS['DB_PASS'] : 'Cjy12140113';
    $name = isset($GLOBALS['DB_NAME']) ? $GLOBALS['DB_NAME'] : 'FLServerDB';
    $db = @new mysqli($host, $user, $pass, $name);
    if ($db->connect_errno) { echo json_encode(['ok'=>false,'msg'=>'数据库连接失败: '.$db->connect_error]); exit; }
    $db->set_charset('latin1');
    return $db;
}

$account = trim($_POST['account'] ?? '');
$passwd  = strval($_POST['passwd'] ?? '');

if ($account === '' || $passwd === '') { echo json_encode(['ok'=>false,'msg'=>'账号与密码不能为空']); exit; }

$db = db_connect();

// 明文校验
$stmt = $db->prepare("SELECT UID FROM ACCSTORE0000 WHERE ACCOUNT=? AND PASSWD=?");
$stmt->bind_param('ss', $account, $passwd);
$stmt->execute();
$stmt->bind_result($uid);
if (!$stmt->fetch()) { echo json_encode(['ok'=>false,'msg'=>'账号不存在或密码错误']); exit; }
$stmt->close();

// 点数信息
$stmt = $db->prepare("SELECT POINT,BONUS FROM POINTBONUS0000 WHERE UID=? AND ACCOUNT=?");
$stmt->bind_param('is', $uid, $account);
$stmt->execute();
$stmt->bind_result($point,$bonus);
if (!$stmt->fetch()) { echo json_encode(['ok'=>false,'code'=>'nouser','msg'=>'点数表中不存在该账号']); exit; }
$stmt->close();

// 最近时间（如无则返回空字符串）
$time = '';
$q = $db->prepare("SELECT OPERTIME FROM CONSUMELOG0000 WHERE UID=? AND ACCOUNT=? ORDER BY OPERTIME DESC LIMIT 1");
$q->bind_param('is', $uid, $account);
$q->execute();
$q->bind_result($t);
if ($q->fetch()) { $time = $t; }
$q->close();

echo json_encode(['ok'=>true,'account'=>$account,'uid'=>$uid,'point'=>intval($point),'bonus'=>intval($bonus),'time'=>$time]);
