<?php
/**
 * recharge_compat.php — 适配 zxspro.cn 老板版 XaPay
 * PHP7.4 兼容 / 真实签名算法：
 *    md5( urldecode(http_build_query($data)) . EPAY_KEY )
 * - 含节日返利 (festival_config.php)
 * - 高熵订单号 / 落库 RECHARGEORDERS
 * - 金额单位：分（AMOUNT 字段）
 * - 保留 timestamp 字段参与签名
 */

declare(strict_types=1);
header('Content-Type: application/json; charset=utf-8');

require_once __DIR__ . '/security_bootstrap.php';
require_once __DIR__ . '/db.php';
require_once __DIR__ . '/festival_config.php';

try {
    $account = trim((string)($_POST['account'] ?? ''));
    $money   = (float)($_POST['money'] ?? ($_POST['amount'] ?? 0));
    $ch      = trim((string)($_POST['ch'] ?? 'epay'));
    $payType = trim((string)($_POST['type'] ?? 'alipay'));

    if ($account === '' || $money <= 0) {
        json_err('账号或金额非法');
    }

    $pdo = get_pdo_safe();
    $st = $pdo->prepare("SELECT UID, ACCOUNT FROM ACCSTORE0000 WHERE ACCOUNT=:a LIMIT 1");
    $st->execute([':a' => $account]);
    $user = $st->fetch(PDO::FETCH_ASSOC);
    if (!$user) {
        json_err('账号不存在');
    }

    // 高熵订单号
    $rnd = bin2hex(openssl_random_pseudo_bytes(8));
    $tid = sprintf("ORD%s%s", date('YmdHis'), $rnd);

    // 节日返利计算
    list($moneyToPay, $pointsToAdd, $promoMeta) = festival_compute($money);
    $amountFen = (int)round($moneyToPay * 100);

    // 落库 RECHARGEORDERS
    $stmt = $pdo->prepare("INSERT INTO RECHARGEORDERS
        (TID, UID, ACCOUNT, CHANNEL, AMOUNT, POINTS, STATUS, CREATED_AT)
        VALUES (:tid, :uid, :acc, :ch, :amt, :pts, 1, NOW())");
    $stmt->execute([
        ':tid' => $tid,
        ':uid' => $user['UID'],
        ':acc' => $user['ACCOUNT'],
        ':ch'  => $ch,
        ':amt' => $amountFen,
        ':pts' => $pointsToAdd,
    ]);

    // 构建签名参数
    $data = [
        'pid'          => EPAY_PID,
        'type'         => $payType,
        'out_trade_no' => $tid,
        'notify_url'   => EPAY_NOTIFY_URL,
        'return_url'   => EPAY_RETURN_URL,
        'name'         => $user['ACCOUNT'] . '(' . $user['UID'] . ')',
        'money'        => number_format($moneyToPay, 2, '.', ''),
        'sitename'     => defined('SITE_NAME') ? SITE_NAME : '征途私服充值',
        'timestamp'    => time()
    ];

    // 按老板版真实算法签名
    ksort($data);
    $sign_str = urldecode(http_build_query($data)) . EPAY_KEY;
    $sign = md5($sign_str);

    $data['sign'] = $sign;
    $data['sign_type'] = 'MD5';

    // 生成最终跳转链接（自动去除多余 /）
    $submitUrl = rtrim(EPAY_API, '/') . '/submit.php?' . http_build_query($data);

    json_ok([
        'payUrl' => $submitUrl,
        'tid'    => $tid,
        'money'  => $moneyToPay,
        'points' => $pointsToAdd,
        'promo'  => $promoMeta,
    ]);

} catch (Throwable $e) {
    sec_log('recharge', ['err' => $e->getMessage()]);
    json_err('系统繁忙，请稍后再试');
}
