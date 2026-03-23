<?php
/**
 * festival_config.php
 * 节日充值折扣/返利配置（与 RECHARGE_RATE 协同）
 * - money 实付 = 原金额 * discount（四舍五入到分）
 * - points = floor(原金额 * RECHARGE_RATE * (1 + rebate_points_pct))
 */
declare(strict_types=1);

const FESTIVAL_RULES = [
  [
    'name' => '双11返利',
    'start'=> '2025-11-10 00:00:00',
    'end'  => '2025-11-12 23:59:59',
    'discount' => 0.95,
    'rebate_points_pct' => 0.20,
    'min_money' => 10.0,
  ],
  [
    'name' => '元旦加赠',
    'start'=> '2025-12-31 00:00:00',
    'end'  => '2026-01-02 23:59:59',
    'discount' => 1.00,
    'rebate_points_pct' => 0.10,
    'min_money' => 1.0,
  ],
];

function festival_compute(float $originMoney): array {
  $now = time();
  $hit = null;
  foreach (FESTIVAL_RULES as $r) {
    $st = strtotime($r['start']); $ed = strtotime($r['end']);
    if ($now >= $st && $now <= $ed && $originMoney >= (float)$r['min_money']) { $hit = $r; break; }
  }
  $discount = $hit ? (float)$hit['discount'] : 1.0;
  $rebPct   = $hit ? (float)$hit['rebate_points_pct'] : 0.0;
  $moneyToPay = round($originMoney * $discount, 2);
  $basePoints = (int)floor($originMoney * RECHARGE_RATE);
  $pointsToAdd = (int)floor($basePoints * (1.0 + $rebPct));
  $meta = $hit ? ['name'=>$hit['name'],'discount'=>$discount,'rebate'=>$rebPct] : ['name'=>'normal','discount'=>1.0,'rebate'=>0.0];
  return [$moneyToPay, $pointsToAdd, $meta];
}
