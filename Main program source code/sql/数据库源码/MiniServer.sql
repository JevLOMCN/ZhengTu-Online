# Host: 192.168.5.23
# Database: xwl
# Table: 'MINIGAME'
# 
CREATE TABLE `MINIGAME` (
		`CHARID` int(10) unsigned NOT NULL default '0',				#角色charid
		`NAME` char(32) NOT NULL default '',						#角色名
		`COUNTRY` int(10) unsigned NOT NULL default '0',			#国家ID
		`FACE` int(10) unsigned NOT NULL default '0',				#头像
		`WIN` int(10) unsigned NOT NULL default '0',				#胜的次数
		`LOSE` int(10) unsigned NOT NULL default '0',				#输的次数	
		`DRAW` int(10) unsigned NOT NULL default '0',				#平局的次数
		`MONEY` int(10) unsigned NOT NULL default '0',				#钱
		`SCORE` int(10) NOT NULL default '0',						#积分
		PRIMARY KEY  (`CHARID`),
		KEY `TOP` (`SCORE`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

