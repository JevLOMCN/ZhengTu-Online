<?php
/**
 * security_bootstrap.php
 * - PHP 7.4 兼容
 * - 生产关闭错误回显
 * - 统一安全响应头
 * - 脱敏日志工具 & JSON 输出助手
 */
declare(strict_types=1);
date_default_timezone_set('Asia/Shanghai');

if (!defined('APP_ENV')) {
  define('APP_ENV', getenv('APP_ENV') ?: 'prod');
}

if (APP_ENV === 'prod') {
  ini_set('display_errors', '0');
  error_reporting(E_ALL & ~E_NOTICE & ~E_DEPRECATED & ~E_STRICT);
} else {
  ini_set('display_errors', '1');
  error_reporting(E_ALL);
}

// 安全响应头
header('X-Content-Type-Options: nosniff');
header('X-Frame-Options: DENY');
header('Referrer-Policy: strict-origin-when-cross-origin');
header('Permissions-Policy: geolocation=(), microphone=(), camera=()');
header("Content-Security-Policy: default-src 'self'; img-src 'self' data:; connect-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'");

function sec_log(string $channel, array $data, string $level='INFO') {
  $maskKeys = ['pass','password','passwd','PASSWD','key','secret','sign','code','email'];
  $safe = [];
  foreach ($data as $k=>$v) {
    if (in_array($k, $maskKeys, true)) $safe[$k]='***';
    else $safe[$k] = is_scalar($v) ? (string)$v : json_encode($v, JSON_UNESCAPED_UNICODE);
  }
  $dir = __DIR__ . '/logs';
  if (!is_dir($dir)) @mkdir($dir, 0755, true);
  $file = sprintf('%s/%s-%s.log', $dir, $channel, date('Ymd'));
  @file_put_contents($file, sprintf("[%s][%s] %s
", date('Y-m-d H:i:s'), $level, json_encode($safe, JSON_UNESCAPED_UNICODE)), FILE_APPEND);
}

function json_ok(array $payload=[]) { header('Content-Type: application/json; charset=utf-8'); echo json_encode(['ok'=>true]+$payload, JSON_UNESCAPED_UNICODE); exit; }
function json_err(string $msg, array $extra=[]) { header('Content-Type: application/json; charset=utf-8'); echo json_encode(['ok'=>false,'msg'=>$msg]+$extra, JSON_UNESCAPED_UNICODE); exit; }

function client_ip(): string {
  $keys = ['HTTP_X_FORWARDED_FOR','HTTP_CLIENT_IP','HTTP_X_REAL_IP','REMOTE_ADDR'];
  foreach ($keys as $h) {
    if (!empty($_SERVER[$h])) {
      $ip = trim(explode(',', $_SERVER[$h])[0]);
      if ($ip) return $ip;
    }
  }
  return '0.0.0.0';
}

// 统一 PDO 连接尝试：优先 get_pdo()
function get_pdo_safe(): PDO {
  static $pdo = null;
  if ($pdo instanceof PDO) return $pdo;

  if (function_exists('get_pdo')) {
    $pdo = get_pdo(); return $pdo;
  }
  // 常见全局变量
  $host = isset($GLOBALS['DB_HOST']) ? $GLOBALS['DB_HOST'] : (defined('DB_HOST') ? DB_HOST : '127.0.0.1');
  $user = isset($GLOBALS['DB_USER']) ? $GLOBALS['DB_USER'] : (defined('DB_USER') ? DB_USER : 'root');
  $pass = isset($GLOBALS['DB_PASS']) ? $GLOBALS['DB_PASS'] : (defined('DB_PASS') ? DB_PASS : '');
  $name = isset($GLOBALS['DB_NAME']) ? $GLOBALS['DB_NAME'] : (defined('DB_NAME') ? DB_NAME : 'FLServerDB1');
  $port = isset($GLOBALS['DB_PORT']) ? $GLOBALS['DB_PORT'] : (defined('DB_PORT') ? DB_PORT : 3306);
  $charset = isset($GLOBALS['DB_CHARSET']) ? $GLOBALS['DB_CHARSET'] : (defined('DB_CHARSET') ? DB_CHARSET : 'latin1');
  $dsn = "mysql:host={$host};port={$port};dbname={$name};charset={$charset}";
  $options = [
    PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
    PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC,
    PDO::ATTR_EMULATE_PREPARES => false,
  ];
  $pdo = new PDO($dsn, $user, $pass, $options);
  return $pdo;
}
