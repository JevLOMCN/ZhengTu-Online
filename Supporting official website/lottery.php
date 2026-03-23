<?php
function post_api($url, $data) {
  $ch = curl_init($url);
  curl_setopt($ch, CURLOPT_POST, 1);
  curl_setopt($ch, CURLOPT_POSTFIELDS, http_build_query($data));
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
  curl_setopt($ch, CURLOPT_TIMEOUT, 10);
  $response = curl_exec($ch);
  curl_close($ch);
  return json_decode($response, true);
}

$DB_HOST = '121.204.249.166';
$DB_PORT = 3306;
$DB_USER = 'ztgame';
$DB_PASS = 'Cjy12140113';
$DB_NAME = 'FLServerDB';
$DB_CHARSET = 'utf8';

$mysqli = new mysqli($DB_HOST, $DB_USER, $DB_PASS, $DB_NAME, $DB_PORT);
$mysqli->set_charset($DB_CHARSET);
?><!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="utf-8">
  <title>点数抽奖中心</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: sans-serif;
      background: #f8fafc;
      padding: 20px;
      max-width: 600px;
      margin: auto;
    }
    h1 {
      color: #1e40af;
      text-align: center;
    }
    label {
      display: block;
      margin: 12px 0 4px;
      font-weight: bold;
    }
    input, select {
      width: 100%;
      padding: 10px;
      border-radius: 6px;
      border: 1px solid #ccc;
    }
    button {
      margin-top: 16px;
      width: 100%;
      padding: 12px;
      background: #2563eb;
      color: white;
      font-weight: bold;
      border: none;
      border-radius: 6px;
      cursor: pointer;
    }
    .result, .pool, .history {
      margin-top: 20px;
      background: #fff;
      border: 1px solid #ddd;
      padding: 12px;
      border-radius: 8px;
    }
    .pool-item {
      margin: 6px 0;
    }
    .footer-links {
      margin-top: 30px;
      display: flex;
      justify-content: space-between;
    }
    .footer-links a {
      padding: 10px 14px;
      border-radius: 6px;
      text-decoration: none;
      font-weight: bold;
      color: #fff;
    }
    .home { background: #22c55e; }
    .recharge { background: #3b82f6; }
  </style>
</head>
<body>
  <h1>点数抽奖中心</h1>
  <form method="POST">
    <label for="account">账号</label>
    <input type="text" id="account" name="account" required>

    <label for="passwd">密码</label>
    <input type="password" id="passwd" name="passwd" required>

    <label for="mode">抽奖模式</label>
    <select name="mode" id="mode">
      <option value="single">单抽</option>
      <option value="ten">10连抽</option>
    </select>

    <button type="submit">立即抽奖</button>
  </form>

  <?php
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
      $res = post_api("http://121.204.249.166/lottery_draw.php", [
        "account" => $_POST['account'],
        "passwd" => $_POST['passwd'],
        "mode" => $_POST['mode']
      ]);
      if ($res && isset($res['ok'])) {
        echo "<div class='result'><b>结果：</b> " . htmlspecialchars($res['msg']) . "</div>";
        if (!empty($res['items'])) {
          echo "<div class='history'><b>抽奖记录：</b><ul>";
          foreach ($res['items'] as $item) {
            echo "<li>🎁 " . htmlspecialchars($item['name']) . " + " . number_format($item['reward']) . " 点</li>";
          }
          echo "</ul></div>";
        }
      } else {
        echo "<div class='result'>⚠️ 接口请求失败或返回格式错误</div>";
      }
    }
  ?>

  <div class="pool">
    <b>当前奖池：</b><br>
    <?php
      $q = $mysqli->query("SELECT NAME, REWARD_POINTS FROM LOTTERY_POOL WHERE IS_ACTIVE = 1 ORDER BY REWARD_POINTS DESC");
      if ($q && $q->num_rows > 0) {
        while ($row = $q->fetch_assoc()) {
          $name = iconv('GB2312', 'UTF-8//IGNORE', $row['NAME']);
          echo "<div class='pool-item'>🎁 " . htmlspecialchars($name) . " - " . number_format($row['REWARD_POINTS']) . " 点</div>";
        }
      } else {
        echo "未能加载奖池信息。";
      }
    ?>
  </div>

  <div class="footer-links">
    <a href="index.html" class="home">🏠 返回首页</a>
    <a href="recharge.html" class="recharge">💰 充值中心</a>
  </div>
</body>
</html>
