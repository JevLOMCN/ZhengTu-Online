<?php
/**
 * reset_password_backend.php
 * —— 使用与 register_compat.php 完全相同的发信逻辑（PHPMailer）
 */
declare(strict_types=1);
ob_start();
header('Content-Type: application/json; charset=utf-8');
date_default_timezone_set('Asia/Shanghai');
error_reporting(E_ALL & ~E_NOTICE);

require_once __DIR__ . '/db.php';

function jret($ok, $msg, $extra = []) {
    ob_clean();
    echo json_encode(array_merge(['ok'=>$ok,'msg'=>$msg],$extra), JSON_UNESCAPED_UNICODE);
    exit;
}

function getDB() {
    global $DB_HOST,$DB_PORT,$DB_USER,$DB_PASS,$DB_NAME;
    return new PDO("mysql:host={$DB_HOST};port={$DB_PORT};dbname={$DB_NAME};charset=utf8mb4",
        $DB_USER,$DB_PASS,[PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION]);
}

try {
    $pdo = getDB();
    $act = $_POST['action'] ?? '';
    $email = trim($_POST['email'] ?? '');
    $code = trim($_POST['code'] ?? '');
    $pass = trim($_POST['passwd'] ?? '');
    $ip = $_SERVER['REMOTE_ADDR'] ?? '0.0.0.0';

    if ($act === 'send_code') {
        if (!$email) jret(false, '邮箱不能为空');

        // 生成验证码并写库
        $vcode = strval(random_int(100000,999999));
        $expire = time() + 300;
        $pdo->prepare("INSERT INTO REG_EMAIL_CODE(email, code, expire, used, ip) VALUES(?,?,?,?,?)")
            ->execute([$email, $vcode, $expire, 0, $ip]);

        // === 与注册模块一致的发信逻辑 ===
        require_once __DIR__.'/PHPMailer/src/Exception.php';
        require_once __DIR__.'/PHPMailer/src/PHPMailer.php';
        require_once __DIR__.'/PHPMailer/src/SMTP.php';
        $mail = new PHPMailer\PHPMailer\PHPMailer();
        $mail->CharSet = 'UTF-8';
        $mail->isSMTP();
        $mail->SMTPAuth = true;
        $mail->Host = $GLOBALS['SMTP_HOST'];
        $mail->Username = $GLOBALS['SMTP_USER'];
        $mail->Password = $GLOBALS['SMTP_PASS'];
        $mail->SMTPSecure = $GLOBALS['SMTP_SECURE'];
        $mail->Port = $GLOBALS['SMTP_PORT'];
        $mail->setFrom($GLOBALS['SMTP_FROM'], $GLOBALS['SMTP_FROM_NAME']);
        $mail->addAddress($email);
        $mail->isHTML(true);
        $mail->Subject = '征途密码重置验证码';
        $mail->Body = "您的验证码是：<b>{$vcode}</b><br>5分钟内有效。";

        if (!$mail->send()) {
            jret(false, '邮件发送失败：'.$mail->ErrorInfo);
        }
        jret(true, '验证码已发送');
    }

    if ($act === 'reset_pass') {
        if (!$email || !$code || !$pass) jret(false, '参数不完整');

        $stmt = $pdo->prepare("SELECT * FROM REG_EMAIL_CODE WHERE email=? AND code=? ORDER BY expire DESC LIMIT 1");
        $stmt->execute([$email, $code]);
        $row = $stmt->fetch(PDO::FETCH_ASSOC);
        if (!$row) jret(false, '验证码错误');
        if ($row['used']) jret(false, '验证码已使用');
        if ($row['expire'] < time()) jret(false, '验证码已过期');

        $acc = $pdo->prepare("SELECT UID FROM ACCSTORE0000 WHERE ACCOUNT=? LIMIT 1");
        $acc->execute([$email]);
        if (!$acc->fetch()) jret(false, '账号不存在');

        $pdo->prepare("UPDATE ACCSTORE0000 SET PASSWD=? WHERE ACCOUNT=?")->execute([$pass, $email]);
        $pdo->prepare("UPDATE REG_EMAIL_CODE SET used=1 WHERE email=? AND code=?")->execute([$email, $code]);

        jret(true, '密码重置成功');
    }

    jret(false, '无效请求');
} catch (Throwable $e) {
    jret(false, '服务器异常：'.$e->getMessage());
}
