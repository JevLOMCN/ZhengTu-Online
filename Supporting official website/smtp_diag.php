<?php
// smtp_diag.php — SMTP/PHPMailer 自检与连通性测试（只返回 JSON）
header('Content-Type: application/json; charset=utf-8');
date_default_timezone_set('Asia/Shanghai');
ini_set('display_errors', '0');
error_reporting(E_ALL);

require_once __DIR__ . '/db.php';

function mask($s){ if($s==='') return ''; if(strlen($s)<=4) return str_repeat('*', strlen($s)); return substr($s,0,2).str_repeat('*', max(0, strlen($s)-4)).substr($s,-2); }

// 1) 尝试解析 db.php 中的 SMTP 配置（兼容多种命名）
function resolve_smtp_config() {
  $c = [];

  // 常见命名
  $c['host']   = $GLOBALS['SMTP_HOST']   ?? $GLOBALS['MAIL_HOST']   ?? '';
  $c['port']   = $GLOBALS['SMTP_PORT']   ?? $GLOBALS['MAIL_PORT']   ?? 0;
  $c['user']   = $GLOBALS['SMTP_USER']   ?? $GLOBALS['MAIL_USER']   ?? '';
  $c['pass']   = $GLOBALS['SMTP_PASS']   ?? $GLOBALS['MAIL_PASS']   ?? '';
  $c['secure'] = $GLOBALS['SMTP_SECURE'] ?? $GLOBALS['MAIL_SECURE'] ?? 'ssl';
  $c['from']   = $GLOBALS['SMTP_FROM']   ?? $GLOBALS['MAIL_FROM']   ?? $c['user'];
  $c['name']   = $GLOBALS['SMTP_FROM_NAME'] ?? $GLOBALS['MAIL_FROM_NAME'] ?? '系统通知';

  // 兜底：如果你用的是别的命名，可以在这里额外挂接，例如：
  // $c['host'] = $c['host'] ?: ($GLOBALS['EMAIL_HOST'] ?? '');
  // $c['user'] = $c['user'] ?: ($GLOBALS['EMAIL_USER'] ?? '');
  // ...

  // 类型与默认
  $c['port'] = intval($c['port']) ?: 465;
  $c['secure'] = in_array(strtolower($c['secure']), ['ssl','tls']) ? strtolower($c['secure']) : 'ssl';
  return $c;
}

// 2) PHPMailer 检测：composer 或本地 src
function check_phpmailer() {
  $info = ['found'=>false,'mode'=>null,'detail'=>null];
  if (file_exists(__DIR__.'/vendor/autoload.php')) {
    require_once __DIR__.'/vendor/autoload.php';
    if (class_exists('PHPMailer\\PHPMailer\\PHPMailer')) {
      $info['found'] = true; $info['mode'] = 'composer';
      return $info;
    }
  }
  // 本地 src
  $base = __DIR__.'/PHPMailer/src';
  if (file_exists("$base/PHPMailer.php") && file_exists("$base/SMTP.php") && file_exists("$base/Exception.php")) {
    require_once "$base/PHPMailer.php";
    require_once "$base/SMTP.php";
    require_once "$base/Exception.php";
    if (class_exists('PHPMailer\\PHPMailer\\PHPMailer')) {
      $info['found'] = true; $info['mode'] = 'local-src';
      return $info;
    }
  }
  $info['detail'] = '未找到 PHPMailer（请 composer require phpmailer/phpmailer，或上传 PHPMailer/src）';
  return $info;
}

// 3) 端口连通性测试
function tcp_connect_test($host, $port, $timeout=8) {
  $start = microtime(true);
  $errno = 0; $errstr = '';
  $fp = @fsockopen($host, $port, $errno, $errstr, $timeout);
  $ms = round((microtime(true)-$start)*1000);
  if ($fp) { fclose($fp); return ['ok'=>true,'ms'=>$ms]; }
  return ['ok'=>false,'ms'=>$ms,'errno'=>$errno,'err'=>$errstr];
}

$out = [];
$out['openssl_loaded'] = extension_loaded('openssl');
$out['phpmailer'] = check_phpmailer();

$c = resolve_smtp_config();
$out['config_resolved'] = [
  'host'=>$c['host'],
  'port'=>$c['port'],
  'secure'=>$c['secure'],
  'user_masked'=>mask($c['user']),
  'from'=>$c['from'],
  'from_name'=>$c['name'],
  'has_password'=>($c['pass']!==''),
];

// 连通性
$out['tcp_connect'] = $c['host'] ? tcp_connect_test($c['host'], $c['port']) : ['ok'=>false,'err'=>'host 为空'];

// 可选：真实发信测试（带参数 ?send_to=xxx@xx.com）
$send_to = isset($_GET['send_to']) ? trim($_GET['send_to']) : '';
if ($send_to && $out['phpmailer']['found'] && $c['host'] && $c['user'] && $c['pass']) {
  try {
    $mail = new PHPMailer\PHPMailer\PHPMailer(true);
    $mail->isSMTP();
    $mail->Host       = $c['host'];
    $mail->SMTPAuth   = true;
    $mail->Username   = $c['user'];
    $mail->Password   = $c['pass'];
    $mail->Port       = $c['port'];
    $mail->SMTPSecure = $c['secure'];
    $mail->CharSet    = 'UTF-8';
    $mail->setFrom($c['from'] ?: $c['user'], $c['name']);
    $mail->addAddress($send_to);
    $mail->isHTML(true);
    $mail->Subject = 'SMTP 自检测试邮件';
    $mail->Body    = '<b>测试成功！</b> 这是一封来自 smtp_diag.php 的自检测试邮件。';
    $mail->AltBody = '测试成功！这是一封来自 smtp_diag.php 的自检测试邮件。';
    $mail->send();
    $out['send_test'] = ['ok'=>true,'to'=>$send_to];
  } catch (Throwable $e) {
    $out['send_test'] = ['ok'=>false,'to'=>$send_to,'error'=>$e->getMessage()];
  }
}

echo json_encode($out, JSON_UNESCAPED_UNICODE);
