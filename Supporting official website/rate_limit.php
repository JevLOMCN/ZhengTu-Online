<?php
/**
 * rate_limit.php
 * REG_EMAIL_CODE 的多维限流 + 失败退避（需要 scene / is_fail 字段）
 * 表字段：email, code, expire, expire_at, used, created_at, ip, scene, is_fail
 */
declare(strict_types=1);

require_once __DIR__.'/security_bootstrap.php';

function rl_check_and_touch(PDO $pdo, string $scene, string $email, string $ip): array {
  $perMinIpMax    = 3;
  $perHourIpMax   = 10;
  $perHourMailMax = 5;
  $freezeFailCnt  = 5;
  $freezeMinutes  = 30;

  $st = $pdo->prepare("SELECT COUNT(*) FROM REG_EMAIL_CODE WHERE ip=:ip AND scene=:scene AND created_at>=DATE_SUB(NOW(), INTERVAL 1 MINUTE)");
  $st->execute([':ip'=>$ip, ':scene'=>$scene]);
  $ipMin = (int)$st->fetchColumn();

  $st = $pdo->prepare("SELECT COUNT(*) FROM REG_EMAIL_CODE WHERE ip=:ip AND scene=:scene AND created_at>=DATE_SUB(NOW(), INTERVAL 1 HOUR)");
  $st->execute([':ip'=>$ip, ':scene'=>$scene]);
  $ipHour = (int)$st->fetchColumn();

  if ($ipMin >= $perMinIpMax)  return ['ok'=>false,'msg'=>'发送过于频繁，请稍后再试(IP-MIN)'];
  if ($ipHour >= $perHourIpMax) return ['ok'=>false,'msg'=>'发送过于频繁，请稍后再试(IP-HOUR)'];

  $st = $pdo->prepare("SELECT COUNT(*) FROM REG_EMAIL_CODE WHERE email=:em AND scene=:scene AND created_at>=DATE_SUB(NOW(), INTERVAL 1 HOUR)");
  $st->execute([':em'=>$email, ':scene'=>$scene]);
  $mailHour = (int)$st->fetchColumn();
  if ($mailHour >= $perHourMailMax) return ['ok'=>false,'msg'=>'该邮箱请求过多，请稍后再试(MAIL-HOUR)'];

  $st = $pdo->prepare("SELECT COUNT(*) FROM REG_EMAIL_CODE WHERE (email=:em OR ip=:ip) AND scene=:scene AND is_fail=1 AND created_at>=DATE_SUB(NOW(), INTERVAL :fz MINUTE)");
  $st->bindValue(':em', $email);
  $st->bindValue(':ip', $ip);
  $st->bindValue(':scene', $scene);
  $st->bindValue(':fz', $freezeMinutes, PDO::PARAM_INT);
  $st->execute();
  $failRecent = (int)$st->fetchColumn();
  if ($failRecent >= $freezeFailCnt) return ['ok'=>false,'msg'=>'尝试失败过多，已临时冻结，请稍后再试'];

  return ['ok'=>true];
}
