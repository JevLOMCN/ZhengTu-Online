<?php
require_once __DIR__.'/security_bootstrap.php';
require_once __DIR__.'/db.php';
require_once __DIR__.'/lottery_config.php';
header('Content-Type: application/json; charset=utf-8');

try{
  if (isset($_GET['meta'])) {
    json_ok(['cost_points'=>LOTTERY_COST_POINTS, 'server_seed_hash'=>lottery_server_seed_hash()]);
  }
  $pdo = get_pdo_safe();
  $st = $pdo->query("SELECT ACCOUNT, PRIZE_NAME, CHANGE_POINTS, DATE_FORMAT(CREATED_AT,'%H:%i:%s') AS created_at
                     FROM LOTTERY_LOG WHERE CHANGE_POINTS>0 ORDER BY ID DESC LIMIT 10");
  $items = $st->fetchAll(PDO::FETCH_ASSOC);
  json_ok(['items'=>$items]);
}catch(Throwable $e){
  sec_log('lottery', ['err'=>$e->getMessage()]);
  json_err('err');
}
