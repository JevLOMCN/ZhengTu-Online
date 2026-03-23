<?php
/**
 * ======================================================
 * db.php —— 数据库与邮件统一配置（带填写说明）
 * ======================================================
 * 本文件会被 register_compat.php / register.php 等引入。
 * 修改后立即生效，无需重启。
 * ======================================================
 */

/* ------------------------------
 * 【数据库连接配置】必填
 * ------------------------------ */
$DB_HOST    = '121.204.249.166';       // 数据库主机（本地为127.0.0.1）
$DB_PORT    = 3306;              // MySQL端口（默认3306）
$DB_USER    = 'ztgame';            // 数据库用户名
$DB_PASS    = 'Cjy12140113';    // 数据库密码
$DB_NAME    = 'FLServerDB';              // 数据库名（请填你的主库名）
$DB_CHARSET = 'utf8';     // 旧版 MySQL/驱动更稳
// $DB_CHARSET = 'utf8mb4'; // 新版更好（支持表情），PHP已做降级兜底
$DB_DSN = "mysql:host={$DB_HOST};port={$DB_PORT};dbname={$DB_NAME};charset=latin1";

/* ------------------------------
 * 【注册限制配置】
 * ------------------------------
 * $REGISTER_IP_LIMIT_MIN：
 * - 限制同一 IP 发送验证码的间隔；
 * - 例如设置为 10，则同一 IP 在 10 分钟内最多发送 30 封验证码；
 * - 设为 0 表示不限制。
 */
$REGISTER_IP_LIMIT_MIN = 0; // 默认关闭频控，可根据情况调整

/* ------------------------------
 * 【邮件发送配置】
 * ------------------------------
 * 以下配置用于注册验证码邮件。
 * 当前版本默认使用 PHP mail() 函数发信；
 * 如服务器已配置 sendmail 或 postfix 可直接使用。
 * 若需要 SMTP（QQ邮箱、企业邮箱等），请告诉我，我可以帮你改为 PHPMailer 版本。
 */
$MAIL_FROM        = 'no-reply@yourdomain.com'; // 发信显示地址
$MAIL_SENDER_NAME = '征途客服中心';              // 发信显示名称
$MAIL_SUBJECT     = '征途注册验证码';            // 验证码主题

/* ======================================================
 * 【SMTP 发信】强烈推荐开启（替代 PHP mail()）
 * 说明：
 *  - 将 $SMTP_ENABLE 设为 true，并填写服务器与账号密码；
 *  - QQ邮箱：HOST=smtp.qq.com，PORT=465(SSL) 或 587(TLS)，
 *    USERNAME=你的QQ邮箱，PASSWORD=【SMTP授权码】（不是登录密码）；
 *  - 企业邮箱/163/Google 亦同理；
 *  - 若云厂商拦截 25 端口，请务必用 465/587。
 * ====================================================== */
//$SMTP_ENABLE   = true;              // true=启用 SMTP 发信
//$SMTP_HOST     = 'smtp.qq.com';      // SMTP服务器地址（QQ邮箱例）
//$SMTP_PORT     = 465;                // 端口（465:SSL / 587:TLS）
//$SMTP_SECURE   = 'ssl';              // 加密方式（ssl / tls）
//$SMTP_USERNAME = '236864889@qq.com';   // 登录账号
//$SMTP_PASSWORD = 'qjntkzkbewlwbiha';            // 邮箱SMTP授权码

// SMTP 基础配置 —— 按你使用的邮箱服务商填写
$SMTP_HOST = 'smtp.qq.com';   // 例：QQ= smtp.qq.com, 163= smtp.163.com, 企业邮看服务商文档
$SMTP_PORT = 465;             // 465 对应 ssl；若用 587 就把下行改为 'tls'
$SMTP_USER = '236864889@qq.com';    // 必须是能发信的完整邮箱
$SMTP_PASS = 'qjntkzkbewlwbiha';          // 注意不是登录密码；QQ/163 需要“授权码”
$SMTP_SECURE = 'ssl';         // 'ssl' 或 'tls'
$SMTP_FROM = '236864889@qq.com';     // 发件人地址（一般与 $SMTP_USER 相同）
$SMTP_FROM_NAME = '征途注册中心';            // 发件显示名

/* 调试：如需在发送失败时输出 SMTP 会话日志，设为 true（线上建议 false） */
$MAIL_DEBUG    = false;

/* ====== XaPay / 易支付 配置（按你截图） ====== */
// 开启验签（建议生产环境 true）
$EPAY_ENABLE_SIGN = true;

// 商户密钥、商户ID（与你后台一致）
$EPAY_KEY = 'CCx6YvrPBI5712x0xiEU';
$EPAY_PID = '10019';

// 网关根地址（务必以 / 结尾；按你截图）
$EPAY_API = 'https://api.zxspro.cn/xpay/epay/';

// 站点名称（下单显示）
$EPAY_SITENAME = '征途私服充值';

// 同步返回页（支付完成跳回的页面）
$EPAY_RETURN_URL = 'https://hhemall.com/recharge.html';

// 异步通知（务必能公网访问）
$EPAY_NOTIFY_URL = 'https://hhemall.com/epay_notify.php';

// 1元=多少点（与你要求一致：10元→1,000,000点）
$RECHARGE_RATE = 100000;
// ========== 兼容充值模块的常量定义 ==========
if (!defined('DB_DSN')) {
    define('DB_DSN', 'mysql:host=' . $DB_HOST . ';port=' . $DB_PORT . ';dbname=' . $DB_NAME . ';charset=' . $DB_CHARSET);
    define('DB_USER', $DB_USER);
    define('DB_PWD', $DB_PASS);
    define('RECHARGE_RATE', $RECHARGE_RATE);
    define('EPAY_PID', $EPAY_PID);
    define('EPAY_KEY', $EPAY_KEY);
    define('EPAY_NOTIFY_URL', $EPAY_NOTIFY_URL);
    define('EPAY_RETURN_URL', $EPAY_RETURN_URL);
    define('EPAY_API', $EPAY_API);
    define('SITE_NAME', '征途私服充值');
    define('EPAY_SIGN_STYLE', 'boss'); // 默认老板签名
}