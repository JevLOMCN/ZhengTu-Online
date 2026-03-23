<?php
header('Content-Type: application/json; charset=utf-8');
date_default_timezone_set('Asia/Shanghai');
require_once __DIR__ . '/db.php';

define('TYPE_CDK', 2);

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

function json_out($o){ echo json_encode($o, JSON_UNESCAPED_UNICODE); exit; }

$account = trim($_POST['account'] ?? '');
$keyid   = trim($_POST['key'] ?? '');

if ($account === '' || $keyid === '') json_out(['ok'=>false,'msg'=>'参数不能为空']);

$db = db_connect();

// 校验账号存在 & 点数表存在
$q = $db->prepare("SELECT UID FROM ACCSTORE0000 WHERE ACCOUNT=? LIMIT 1");
$q->bind_param('s',$account);
$q->execute(); $q->bind_result($uid);
if(!$q->fetch()) json_out(['ok'=>false,'code'=>'nouser','msg'=>'账号不存在']);
$q->close();

$p = $db->prepare("SELECT POINT,BONUS FROM POINTBONUS0000 WHERE UID=? AND ACCOUNT=? LIMIT 1");
$p->bind_param('is',$uid,$account);
$p->execute(); $p->bind_result($point,$bonus);
if(!$p->fetch()) json_out(['ok'=>false,'code'=>'nouser','msg'=>'点数表中不存在该账号']);
$p->close();

// 校验 CDK
$c = $db->prepare("SELECT ID,POINT,STATE FROM CDKEY WHERE KEYID=? LIMIT 1");
$c->bind_param('s',$keyid);
$c->execute(); $c->bind_result($cid,$cpoint,$cstate);
if(!$c->fetch()) json_out(['ok'=>false,'msg'=>'CDK 不存在']);
if(intval($cstate) !== 1) json_out(['ok'=>false,'msg'=>'CDK 已使用']);
$c->close();

$db->begin_transaction();
try{
    // 标记已用
    $u = $db->prepare("UPDATE CDKEY SET STATE=0, ACCOUNT=?, UESTIME=NOW() WHERE ID=? AND STATE=1");
    $u->bind_param('si',$account,$cid);
    if(!$u->execute()) throw new Exception('CDK 更新失败(update)');
    if($u->affected_rows !== 1) throw new Exception('CDK 更新失败(affected=0)');
    $u->close();

    // 加点
    $new_point = intval($point) + intval($cpoint);
    $up = $db->prepare("UPDATE POINTBONUS0000 SET POINT=? WHERE UID=? AND ACCOUNT=?");
    $up->bind_param('iis',$new_point,$uid,$account);
    if(!$up->execute()) throw new Exception('增加点数失败');
    $up->close();

    // 流水
    $tid = 'CDK'.date('YmdHis').mt_rand(1000,9999);
    $type = TYPE_CDK; $source = $cid; $cardid = $keyid;
    $inslog = $db->prepare("INSERT INTO CONSUMELOG0000(TID,UID,ACCOUNT,TYPE,SOURCE,CARDID,FILLPOINT,CONSUMEPOINT,THISPOINT,OPERTIME) VALUES(?,?,?,?,?,?,?,?,?,NOW())");
    $zero=0;
    $inslog->bind_param('sisiisiii',$tid,$uid,$account,$type,$source,$cardid,$cpoint,$zero,$new_point);
    if(!$inslog->execute()) throw new Exception('写入流水失败');
    $inslog->close();

    $db->commit();
    json_out(['ok'=>true,'cdk'=>$keyid,'added'=>intval($cpoint),'total'=>intval($new_point),'time'=>date('Y-m-d H:i:s')]);
}catch(Exception $e){
    $db->rollback();
    json_out(['ok'=>false,'msg'=>$e->getMessage(),'code'=>'cdku']);
}
