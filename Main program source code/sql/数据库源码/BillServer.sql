# Host: 192.168.2.31
# Database: zebra1
# Table: 'BALANCE'
# 股票帐号数据表 
CREATE TABLE `BALANCE` (
		`ACCOUNT` varchar(48) NOT NULL default '',	# 帐号
		`ACCID` int(10) unsigned NOT NULL default '0',	# #角色accid
		`PASSWORD` varchar(16) default '',	#密码
		`ALLGOLDIN` int(10) unsigned NOT NULL default '0',#总冲值金币数量
		`ALLGOLDOUT` int(10) unsigned NOT NULL default '0',# 总提取金币数量
		`ALLMONEYIN` int(10) unsigned NOT NULL default '0', # 总冲值银币数量
		`ALLMONEYOUT` int(10) unsigned NOT NULL default '0',# 总提取银币数量
		`LASTTIME` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,# 上次更新时间
		`GOLD` int(10) unsigned NOT NULL default '0',#金币数量
		`MONEY` int(10) unsigned NOT NULL default '0',	# 银币数量
		`GOLDTAX` int(10) unsigned NOT NULL default '0',	#总税收
		`MONEYTAX` int(10) unsigned NOT NULL default '0',	#总税收
		`GOLDLIST` int(10) unsigned NOT NULL default '0',	# # 个人股票卖单数量
		`MONEYLIST` int(10) unsigned NOT NULL default '0',	# 个人股票买单数量
		PRIMARY KEY  (`ACCID`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.31
# Database: zebra1
# Table: 'CONSIGNGOLD'
# 股票卖单数据表
CREATE TABLE `CONSIGNGOLD` (
		`ID` int(10) unsigned NOT NULL auto_increment,	#序号
		`ACCID` int(10) unsigned NOT NULL default '0',	#角色accid
		`NUM` int(10) unsigned NOT NULL default '0',	#数量
		`PRICE` int(10) unsigned NOT NULL default '0',	#价格
		`TIME` int(10) unsigned NOT NULL default '0',	#时间
		PRIMARY KEY  (`ID`),
		KEY `ACCID` (`ACCID`),
		KEY `PRICE` (`PRICE`,`NUM`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.31
# Database: zebra1
# Table: 'CONSIGNGOLDHISTORY'
# 股票金币交易历史
CREATE TABLE `CONSIGNGOLDHISTORY` (
		`ID` int(10) NOT NULL default '0',	#股票单序号,取自Table  `CONSIGNGOLD`
		`ACCID` int(10) unsigned NOT NULL default '0',	#角色accid
		`NUM` int(10) unsigned NOT NULL default '0',	 #数量
		`COMMITPRICE` int(10) unsigned NOT NULL default '0',#成交价格
		`PRICE` int(10) unsigned NOT NULL default '0',	#价格
		`COMMITTIME` int(10) unsigned NOT NULL default '0',		#交易时间
		`SYSMONEY` int(10) unsigned NOT NULL default '0',	#系统吃掉的差价
		`OKTIME` int(10) unsigned NOT NULL default '0',	 #存入时间
		KEY `ACCID` (`ACCID`),
		KEY `OKTIME` (`OKTIME`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.31
# Database: zebra1
# Table: 'CONSIGNMONEY'
# 股票买单数据表 
CREATE TABLE `CONSIGNMONEY` (
		`ID` int(10) unsigned NOT NULL auto_increment, 	#序号
		`ACCID` int(10) unsigned NOT NULL default '0',	# 角色accid
		`NUM` int(10) unsigned NOT NULL default '0',	# 数量
		`PRICE` int(10) unsigned NOT NULL default '0',	#价格
		`TIME` int(10) unsigned NOT NULL default '0',	#时间
		PRIMARY KEY  (`ID`),
		KEY `ACCID` (`ACCID`),
		KEY `PRICE` (`PRICE`,`NUM`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.31
# Database: zebra1
# Table: 'CONSIGNMONEYHISTORY'
# 股票银币交易历史
CREATE TABLE `CONSIGNMONEYHISTORY` (
		`ID` int(10) NOT NULL default '0',	#单号,取自TABLE `CONSIGNMONEY`
		`ACCID` int(10) unsigned NOT NULL default '0',#角色accid
		`NUM` int(10) unsigned NOT NULL default '0',#交易数量
		`COMMITPRICE` int(10) unsigned NOT NULL default '0',#成交价格
		`PRICE` int(10) unsigned NOT NULL default '0', #价格
		`COMMITTIME` int(10) unsigned NOT NULL default '0',#成交时间
		`SYSMONEY` int(10) unsigned NOT NULL default '0',#系统吃掉
		`OKTIME` int(10) unsigned NOT NULL default '0',#存入时间
		KEY `ACCID` (`ACCID`),
		KEY `OKTIME` (`OKTIME`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 


