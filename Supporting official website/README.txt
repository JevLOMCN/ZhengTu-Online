# hhemall_secure_update_v20251101

生成时间：2025-11-01 13:01:03

## 包含内容
- security_bootstrap.php（全站安全入口）
- rate_limit.php（注册/找回验证码限流）
- festival_config.php（节日返利配置）
- recharge_compat.php（高熵订单 + 返利 + 兼容表结构）
- epay_notify.php（幂等回调 + 固定返回）
- lottery_config.php / lottery_draw.php / lottery_recent.php / lottery.html（公平抽奖 + 跑马灯）
- update_reg_email_code.sql（数据库升级脚本）

## 部署步骤
1. **数据库升级**
   ```sql
-- 升级 REG_EMAIL_CODE 与回调字段、抽奖功能表
ALTER TABLE REG_EMAIL_CODE
ADD COLUMN scene VARCHAR(32) NOT NULL DEFAULT 'default',
ADD COLUMN is_fail TINYINT(1) NOT NULL DEFAULT 0;

ALTER TABLE RECHARGEORDERS
ADD COLUMN PAID_AT DATETIME DEFAULT NULL,
ADD COLUMN TRADE_NO VARCHAR(64) DEFAULT NULL;

CREATE TABLE IF NOT EXISTS LOTTERY_POOL (
  ID INT AUTO_INCREMENT PRIMARY KEY,
  NAME VARCHAR(64) NOT NULL,
  REWARD_POINTS INT NOT NULL DEFAULT 0,
  WEIGHT INT NOT NULL DEFAULT 0,
  IS_ACTIVE TINYINT(1) NOT NULL DEFAULT 1
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS LOTTERY_LOG (
  ID BIGINT AUTO_INCREMENT PRIMARY KEY,
  UID INT NOT NULL,
  ACCOUNT VARCHAR(48) NOT NULL,
  PRIZE_ID INT NOT NULL,
  PRIZE_NAME VARCHAR(64) NOT NULL,
  CHANGE_POINTS INT NOT NULL DEFAULT 0,
  COST_POINTS INT NOT NULL DEFAULT 0,
  CLIENT_SEED VARCHAR(64) NOT NULL,
  SERVER_SEED_HASH VARCHAR(64) NOT NULL,
  NONCE INT NOT NULL,
  RESULT_HASH VARCHAR(64) NOT NULL,
  CREATED_AT DATETIME NOT NULL,
  KEY idx_created_at (CREATED_AT)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

   ```

2. **覆盖文件**到 `/www/wwwroot/hhemall/` 目录。

3. **全站引入**（重要）：在你的入口 PHP（register_compat.php / reset_password_backend.php / recharge_compat.php / epay_notify.php / cdk_recharge.php / balance_query.php 等）文件的 `<?php` 之后第一行加入：
   ```php
   require_once __DIR__.'/security_bootstrap.php';
   ```

4. **注册/找回限流接入（两行即可）**
   - 在 **发送验证码** 前加入：
     ```php
     require_once __DIR__.'/rate_limit.php';
     $scene = 'send_register'; // 找回时用 'reset_pass'
     $rl = rl_check_and_touch(get_pdo_safe(), $scene, $email, client_ip());
     if (!$rl['ok']) json_err($rl['msg']);
     ```
   - 在 **验证码校验失败** 的地方补：
     ```php
     get_pdo_safe()->prepare("UPDATE REG_EMAIL_CODE SET is_fail=1 WHERE id=:id")->execute([':id'=>$codeRow['id']]);
     ```

5. **节日返利**
   - 修改 `festival_config.php` 中的 `FESTIVAL_RULES` 即可生效。

6. **公平抽奖**
   - 执行 SQL 后，在后台插入 `LOTTERY_POOL` 奖项（NAME/REWARD_POINTS/WEIGHT）。
   - 将 `lottery.html` 加入导航或充值页 Tab。
   - 修改 `lottery_config.php` 的 `LOTTERY_SERVER_SEED`（并对外公示 `lottery_server_seed_hash()` 的值）。

> 备注：本包按你的数据库字段（`POINTBONUS0000.POINT`, `RECHARGEORDERS.TID/AMOUNT/POINTS` 等）适配；`AMOUNT` 以**分**为单位落库。
