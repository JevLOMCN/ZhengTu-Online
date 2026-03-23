<?php
/**
 * lottery_config.php
 * - 每次消耗点数
 * - 服务器种子（公开其哈希，种子本身定期轮换）
 */
declare(strict_types=1);

const LOTTERY_COST_POINTS = 100000;
const LOTTERY_SERVER_SEED = 'PLEASE_REPLACE_WITH_RANDOM_64_CHARS_SERVER_SEED';

function lottery_server_seed_hash(): string {
  return hash('sha256', LOTTERY_SERVER_SEED);
}
