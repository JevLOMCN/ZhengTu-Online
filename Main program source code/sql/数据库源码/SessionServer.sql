# host: 192.168.2.15
# Database: zebra2
# Table: 'SAMPLERELATION'
# 
# 社会关系表包括黑名单和好友等一对一的社会关系
#
CREATE TABLE `SAMPLERELATION` (
		`CHARID` int(10) unsigned NOT NULL default '0',          #角色的ID
		`RELATIONID` int(10) unsigned NOT NULL default '0',      #其关系人的ID
		`RELATIONNAME` varchar(33) NOT NULL default '',          #其关系人的名字
		`TYPE` tinyint(3) unsigned NOT NULL default '0',         #关系类型
		`LASTTIME` int(10) unsigned NOT NULL default '0',        #最后组队时间
		`OCCUPATION` smallint(5) unsigned NOT NULL default '0',  #头像定义
		`DEGREE` smallint(5) unsigned NOT NULL default '0',      #好友度
		PRIMARY KEY  (`CHARID`,`RELATIONID`)
		) ENGINE=MyISAM; 

# Host: 192.168.2.15
# Database: zebra2
# Table: 'SCHOOL'
# 
# 门派定义表
#
CREATE TABLE `SCHOOL` (
		`SCHOOLID` int(10) unsigned NOT NULL auto_increment,    #门派ID为0表示其为师徒关系(0这条记录不存档在内存中创建
		`NAME` varchar(33) NOT NULL default '',                 #门派名称
		`MASTERSERIAL` int(10) unsigned NOT NULL default '0',   #师尊的 CHARID
		`BULLETIN` blob NOT NULL,                               #公告
		PRIMARY KEY  (`SCHOOLID`)
		) ENGINE=MyISAM; 

# Host: 192.168.2.15
# Database: zebra2
# Table: 'SCHOOLMEMBER'
# 
# 门派成员和师徒成员表
#
CREATE TABLE `SCHOOLMEMBER` (
		`SERIALID` int(10) unsigned NOT NULL auto_increment,           #本节点的ID
		`MASTERID` int(10) unsigned NOT NULL default '0',              #师傅的ID
		`PRESERIALID` int(10) unsigned NOT NULL default '0',           #前一节点的ID 在门派中如果师傅被开那么MASTERID将会为0但是前一节点并不为0
		`CHARID` int(10) unsigned NOT NULL default '0',                #角色ID
		`NAME` varchar(33) NOT NULL default '',                        #角色名称
		`LEVEL` smallint(5) unsigned NOT NULL default '0',             #角色等级
		`JOINTIME` int(10) unsigned NOT NULL default '0',              #师徒关系建立时间
		`DEGREE` smallint(5) unsigned NOT NULL default '0',            #友好度
		`LASTTIME` int(10) unsigned NOT NULL default '0',              #最后组队时间
		`SCHOOLID` int(10) unsigned NOT NULL default '0',              #门派ID 为0表示是师徒关系
		`OCCUPATION` smallint(5) unsigned NOT NULL default '0',        #头像定义
		`TAG` tinyint(3) unsigned NOT NULL default '0',                #节点有效标志
		`MASTERBALANCE` int(10) unsigned NOT NULL default '0',		   #做为师傅,当前可以领取的进贡金额
		`PRENTICETOTAL` int(10) unsigned NOT NULL default '0',		   #做为徒弟,累计的进贡金额
		`PRENTICELASTLVL` int(10) unsigned NOT NULL default '0',	   #徒弟上次进贡时的等级
		`MASTERTOTAL` bigint(20) unsigned NOT NULL default '0',        #做为师傅,累计领取的进贡金额
		PRIMARY KEY  (`SERIALID`)
		) ENGINE=MyISAM; 

# Host: 192.168.2.15
# Database: zebra2
# Table: 'SEPT'
# 
# 家族定义表
#
CREATE TABLE `SEPT` (
		`SEPTID` int(10) unsigned NOT NULL auto_increment, # 家族ID
		`NAME` varchar(33) NOT NULL default '', # 家族名称
		`CHARID` int(10) unsigned NOT NULL default '0', # 族长ID
		`MASTER` varchar(33) NOT NULL default '',	# 族长姓名
		`VOTE` tinyint(3) unsigned NOT NULL default '0', # 投票状态
		`CREATETIME` int(10) unsigned NOT NULL default '0',# 创建时间
		`COUNTRYID` int(10) unsigned NOT NULL default '0',# 所属国家
		`REPUTE` int(10) unsigned NOT NULL default '0', # 声望
		`UNIONID` int(10) unsigned NOT NULL default '0', # 所属帮会
		`LEVEL` int(10) unsigned NOT NULL default '0', # 家族等级
		`NOTE` varchar(255) NOT NULL default '',	# 说明
		`SPENDGOLD` int(10) unsigned NOT NULL default '0', # 家族消费金币数
		`ISEXP` int(10) unsigned NOT NULL default '0', # 是否领取当日占领家族经验
		`NORMALEXPTIME` int(10) unsigned NOT NULL default '0', # 最后一次领取当日普通家族经验的时间
		`CALLTIMES` int(10) unsigned NOT NULL default '0',        #令牌使用次数
		`CALLDAYTIME` int(10) unsigned NOT NULL default '0',      #令牌使用时间
                `LEAVETIME` int(10) unsigned not null default 0,		## 记录家族离开帮会的时间
		`BLOOM` int(10) unsigned NOT NULL default '0',	    # 家族繁容度
		`BRICK` bigint(20) unsigned NOT NULL default '0',	    # 家族金砖
		`SEPTMONEY` int(10) unsigned NOT NULL default '0',	    # 家族繁容度
		PRIMARY KEY (`SEPTID`)
		) ENGINE=MyISAM; 

# Host: 192.168.2.15
# Database: zebra2
# Table: 'SEPTMEMBER'
# 
CREATE TABLE `SEPTMEMBER` (										#家族成员表
	`SEPTID` int(10) unsigned NOT NULL default '0',				#家族ID
	`CHARID` int(10) unsigned NOT NULL default '0',				#族员charid
	`NAME` varchar(33) NOT NULL default '',						#族员名字
	`OCCUPATION` smallint(5) unsigned NOT NULL default '0',		#族员职业
	`CONTRIBUTE` int(10) unsigned NOT NULL default '0',				#族员贡献度
	`ALIASNAME` varchar(33) NOT NULL default '',				#族员别名
	`USETANK` tinyint(3) unsigned NOT NULL default '1',			#领用战车的权限
	`PLACE` int(10) unsigned not null default 0,			## 在家族中的职位，目前分为族长和副族长
	PRIMARY KEY  (`CHARID`,`SEPTID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.15
# Database: zebra2
# Table: 'UNION'
# 
# 帮会定义表
#
CREATE TABLE `UNION` (
		`UNIONID` int(10) unsigned NOT NULL auto_increment, # 帮会ID
		`NAME` varchar(33) NOT NULL default '',             # 帮会名称
		`CHARID` int(10) unsigned NOT NULL default '0',     # 帮主ID
		`LEVEL` smallint(5) NOT NULL default '0',           # 帮会级别
		`MASTER` varchar(33) NOT NULL default '',           # 帮主
		`EXP` bigint(20) NOT NULL default '0',              # 帮会经验值
		`VOTE` tinyint(3) unsigned NOT NULL default '0',    # 是否在投票期
		`CREATETIME` int(10) unsigned NOT NULL default '0', # 帮会建立时间
		`COUNTRYID` int(10) unsigned NOT NULL default '0',  # 国家ID
		`MANA` int(10) unsigned NOT NULL default '0',       # 帮会威望
		`ACTIONPOINT` int(10) unsigned NOT NULL default '0',# 帮会行动力
		`MONEY` int(10) unsigned NOT NULL default '0',	    # 帮会资金
		`BLOOM` int(10) unsigned NOT NULL default '0',	    # 帮会繁容度
		`BRICK` bigint(20) unsigned NOT NULL default '0',	    # 金砖
		`NOTE` varchar(255) NOT NULL default '',            # 帮会描述
		`CALLTIMES` int(10) unsigned NOT NULL default '0',  #令牌使用次数
		PRIMARY KEY  (`UNIONID`)
		) ENGINE=MyISAM; 

# Host: 192.168.2.15
# Database: zebra2
# Table: 'UNIONMEMBER'
# 
# 帮会成员表
#
CREATE TABLE `UNIONMEMBER` (
		`UNIONID` int(10) unsigned NOT NULL default '0', # 帮会ID
		`CHARID` int(10) unsigned NOT NULL default '0', # 帮众ID
		`ALIASNAME` varchar(33) NOT NULL default '', # 帮众别名
		`NAME` varchar(33) NOT NULL default '', # 帮众姓名
		`OCCUPATION` smallint(5) unsigned NOT NULL default '0', # 帮众成员职业
		`POWER` int(10) unsigned NOT NULL default '0',   # 帮众成员权限
		`SEPTID` int(10) unsigned NOT NULL default '0', # 会员所属家族ID
		PRIMARY KEY (`UNIONID`,`CHARID`)
		) ENGINE=MyISAM; 

CREATE TABLE `ANSWER` (
		`id` bigint(20) unsigned NOT NULL auto_increment, # 问题ID
		`type` varchar(100) NOT NULL default '',		  # 问题类型
		`ask` blob NOT NULL,							  # 问题描述
		`answer_a` blob NOT NULL,						  # 答案1描述
		`answer_b` blob NOT NULL,						  # 答案2描述
		`answer_c` blob NOT NULL,						  # 答案3描述
		`answer_d` blob NOT NULL,						  # 答案4描述
		`answer_e` blob NOT NULL,
		`answer_f` blob NOT NULL,
		`the_answer` bigint(20) NOT NULL default '0',     # 正确答案编号
		`quiz_type` int(11) NOT NULL default '0',		  # 竞赛类型,0为全国答题,1为个人答题
		PRIMARY KEY  (`id`)
		) ENGINE=MyISAM; 

# Host: 192.168.2.32
# Database: zebra3
# Table: 'NPCDARE'
# 
# NPC争夺战定义用表，数据记录里包含可争夺NPC的 国家ID, 地图ID, NPC ID, 坐标， 控制家族的ID, 挑战家族的ID, 当前可领金币
#
CREATE TABLE `NPCDARE` (
  `COUNTRY` int(10) unsigned NOT NULL default '0',     #NPC所属国家ID
  `MAPID` int(10) unsigned NOT NULL default '0',       #NPC所属地图ID
  `NPCID` int(10) unsigned NOT NULL default '0',       #NPC  ID
  `POSX` int(10) unsigned NOT NULL default '0',        #NPC在地图中的X坐标
  `POSY` int(10) unsigned NOT NULL default '0',        #NPC在地图中的Y坐标
  `HOLDSEPTID` int(10) unsigned NOT NULL default '0',  #控制该NPC的家族ID
  `DARESEPTID` int(10) unsigned NOT NULL default '0',  #争夺该NPC的家族ID
  `GOLD` int(10) unsigned NOT NULL default '0'         #当前玩家可领取的银子
) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.32
# Database: zjw
# Table: 'COUNTRY'
# 
CREATE TABLE `COUNTRY` (
	  `ID` int(10) unsigned NOT NULL default '0',				#国家ID
	  `KINGNAME` varchar(100) NOT NULL default '',				#国王姓名
	  `KINGUNIONID` int(10) unsigned NOT NULL default '0',		#国王所属帮会ID
	  `DARETIME` int(10) unsigned NOT NULL default '0',			#上次发起挑战的时间
	  `LASTDARETIME` int(10) unsigned NOT NULL default '0',		#上次被挑战的时间
	  `NAME` varchar(33) NOT NULL default '',					#国家名称
	  `FORMALWIN` int(10) unsigned NOT NULL default '0',		#正式国战胜利次数
	  `FORMALFAIL` int(10) unsigned NOT NULL default '0',		#正式国战失败次数
	  `ANNOYWIN` int(10) unsigned NOT NULL default '0',			#骚扰国战胜利次数
	  `ANNOYFAIL` int(10) unsigned NOT NULL default '0',		#骚扰国战失败次数
	  `DARECOUNTRYID` int(10) unsigned NOT NULL default '0',	#挑战国ID
	  `STAR` int(10) unsigned NOT NULL default '0',				#国家星号个数
	  `TAX` int(10) unsigned NOT NULL default '0',				#国家税收
	  `GOLD` bigint(20) unsigned NOT NULL default '0',			#国库银两
	  `SILK` bigint(20) unsigned NOT NULL default '0',			#未用
	  `ORE` bigint(20) unsigned NOT NULL default '0',			#未用
	  `BOWLDER` bigint(20) unsigned NOT NULL default '0',		#未用
	  `WOOD` bigint(20) unsigned NOT NULL default '0',			#未用
	  `COAT` bigint(20) unsigned NOT NULL default '0',			#未用
	  `HERBAL` bigint(20) unsigned NOT NULL default '0',		#未用
	  `MATERIAL` bigint(20) unsigned NOT NULL default '0',		#国家物资
	  `NOTE` varchar(255) NOT NULL default '',					#国家说明
	  `STOCK` bigint(20) unsigned NOT NULL default '0',			#国家原料
	  `FORBIDTALK` int(10) NOT NULL default '0',				#禁言
	  `SENDPRISON` int(10) NOT NULL default '0',				#国王关监狱
	  `GEN_EXP` int(10) unsigned NOT NULL default '0',			#大将军的经验
	  `GEN_MAXEXP` int(10) unsigned NOT NULL default '0',		#大将军的最大经验
	  `GEN_LEVEL` int(10) unsigned NOT NULL default '0',		#大将军的等级
	  `DIPLOMATNAME` varchar(33) NOT NULL default '',		#外交官姓名
	  `CATCHERNAME1` varchar(100) NOT NULL default '',		#捕头姓名
          `CATCHERNAME2` varchar(100) NOT NULL default '',		#捕头姓名
	  `CALLTIMES` int(10) unsigned NOT NULL default '0',		#令牌使用次数
	  `LASTDAILYMONEY` int(10) unsigned NOT NULL default '0',	#皇帝上次领取薪资的时间
          `LASTKINGMONEY` int(10) unsigned NOT NULL default '0',	#国王上次领取薪资的时间
          `LASTYSMONEY` int(10) unsigned NOT NULL default '0',	        #元帅上次领取薪资的时间
          `LASTZXMONEY` int(10) unsigned NOT NULL default '0',	        #宰相上次领取薪资的时间
          `LASTCRMONEY` int(10) unsigned NOT NULL default '0',	        #捕头(左上)上次领取薪资的时间
          `LASTCXMONEY` int(10) unsigned NOT NULL default '0',	        #捕头(左下)上次领取薪资的时间
          `LASTORMONEY` int(10) unsigned NOT NULL default '0',	        #御史(右上)上次领取薪资的时间
          `LASTOSMONEY` int(10) unsigned NOT NULL default '0',	        #御史(右下)上次领取薪资的时间
	  `KINGTIME` int(10) unsigned NOT NULL default '0',		#国王在位时间(小时)
	  `KINGPOWER` int(10) unsigned NOT NULL default '100',		#国王统帅力
	  `LAST_DWID` int(10) unsigned NOT NULL default '0',		#上次被罢免国王ID
	  `LAST_TIMES` int(10) unsigned NOT NULL default '0',		#上次罢免国王的时间
	  `SURPRISE_TIME` int(10) unsigned NOT NULL default '0',	#发起奇袭战挑战的时间
	  `SURPRISE_DARE_COUNTRY` int(10) unsigned NOT NULL default '0',#发起奇袭战挑战的国家
	  `LAST_SURPRISE_TIME` int(10) unsigned NOT NULL default '0',	#上次接受奇袭战挑战的时间
	  `YUSHINAME1` varchar(100) NOT NULL default '',		#御史大夫名称
	  `YUSHINAME2` varchar(100) NOT NULL default '',		#御史大夫名称
          `YUANSHUAINAME` varchar(100) NOT NULL default '',		#元帅名称
          `ZAIXIANGNAME` varchar(100) NOT NULL default '',		#宰相名称
	   `CAPTION_TIMES` int(10) unsigned not null default 0,#称号授予次数
	  `PKRATING` int(10) unsigned NOT NULL default '0',			#大国
	  `COUNTRYLEVEL` int(10) unsigned not null default 0, #国家强弱度量
	  `STRONG_COUNTRY` int(10) unsigned not null default 1, #1:强国，0：弱国
	  `REVENGE_TIME` int(10) unsigned not null default 1, #上次发起复仇战的时间
	  `REVENGE_DARE_COUNTRY` int(10) unsigned not null default 1, #发起复仇战的国家
	  `LAST_REVENGE_TIME` int(10) unsigned not null default 1, #上次接受复仇战的时间
	  `IMPEECHTIME` int(10) unsigned NOT NULL default '0',		#最近一次弹劾国王的时间
        `PEARLNUM` int(10) NOT NULL DEFAULT '600',		#太庙资源数量
	  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1; 


# 主机: 192.168.2.32
# 数据库: zebra1
# 表: 'CITY'
# 
CREATE TABLE `CITY` (
		`COUNTRY` int(10) unsigned NOT NULL default '0',		#所属国家ID
		`CITYID` int(10) unsigned NOT NULL default '0',			#地图ID
		`UNIONID` int(10) unsigned NOT NULL default '0',		#所属帮会ID
		`LASTAWARDTIME` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,	#挑战时间
		`ISAWARD` int(11) NOT NULL default '0',					#是否领取税金
		`DAREUNIONID` int(10) unsigned NOT NULL default '0',	#未用
		`DAREUNIONLIST` varchar(255) not null default '',	#挑战者列表
		`GOLD` int(10) unsigned NOT NULL default '0',			#税金
		`CATCHERNAME` varchar(33) not null default '',	#捕头
		`IMPEECHTIME` bigint(20)  unsigned not null default '0'
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# 主机: 192.168.2.32
# 数据库: zebra1
# 表: 'FORBIDTALK'
# 通过GM工具处罚玩家用的表
#
CREATE TABLE `FORBIDTALK` (
		`NAME` varchar(33) NOT NULL default '',
		`LEVEL` smallint(5) unsigned NOT NULL default '0',
		`ACCID` int(10) unsigned NOT NULL default '0',
		`SERVER` int(10) unsigned NOT NULL default '0',
		`COUNTRY` int(10) unsigned NOT NULL default '0',
		`REASON` varchar(255) NOT NULL default '',
		`OPERATION` smallint(5) unsigned NOT NULL default '0',
		`STARTTIME` bigint(20) unsigned NOT NULL default '0',
		`DELAY` int(10) NOT NULL default '0',
		`ISVALID` smallint(5) unsigned NOT NULL default '0',
		`GM` varchar(33) NOT NULL default '0',
		PRIMARY KEY  (`NAME`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.33
# Database: xwl
# Table: 'GMMESSAGE'
# 玩家向GM提交定单用的表
# 
# CREATE TABLE `GMMESSAGE` (
#        	`ID` int(10) unsigned NOT NULL auto_increment,
#        	`NAME` varchar(33) NOT NULL default '',
#        	`TYPE` smallint(5) unsigned NOT NULL default '0',
#        	`CONTENT` varchar(255) NOT NULL default '',
# 	`CONTACT` smallint(5) unsigned NOT NULL default '0',
# 	`TELE` varchar(100) NOT NULL default '',
# 	PRIMARY KEY  (`ID`)
# 	) ENGINE=MyISAM DEFAULT CHARSET=latin1; 
# 

# Host: 192.168.2.32
# Database: zjw
# Table: 'DARERECORD'
# 
CREATE TABLE `DARERECORD` (
		`ID` int(11) NOT NULL auto_increment,					#ID
		`DARETIME` int(10) unsigned NOT NULL default '0',		#对战时间
		`ATTCOUNTRY` int(10) unsigned NOT NULL default '0',		#挑战方国家ID
		`DEFCOUNTRY` int(10) unsigned NOT NULL default '0',		#防守方国家ID
		`ATTKINGNAME` varchar(33) NOT NULL default '',			#挑战方国王
		`DEFKINGNAME` varchar(33) NOT NULL default '',			#防守方国王
		`RESULT` int(10) unsigned NOT NULL default '0',			#对战结果,0为攻方胜,1为守方胜
		`ATTNUM` int(10) unsigned NOT NULL DEFAULT '0',
                `DFNUM` int(10) unsigned NOT NULL DEFAULT '0',
		PRIMARY KEY  (`ID`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.33
# Database: xwl
# Table: 'AUCTION'
# 拍卖表
# 
CREATE TABLE `AUCTION` (
	  `ID` int(10) unsigned NOT NULL auto_increment,			#ID
	  `OWNER` varchar(100) NOT NULL default '',                 #卖家
	  `STATE` tinyint(3) unsigned NOT NULL default '0',         #交易状态
	  `NAME` varchar(32) NOT NULL default '',                   #物品名字
	  `TYPE` tinyint(3) NOT NULL default '0',                   #类型
	  `QUALITY` tinyint(3) NOT NULL default '0',                #品质
	  `NEEDLEVEL` smallint(5) NOT NULL default '0',             #需求等级
	  `MINMONEY` int(10) unsigned NOT NULL default '0',			#拍卖价格
	  `MAXMONEY` int(10) unsigned NOT NULL default '0',			#一口价
	  `MINGOLD` int(10) unsigned NOT NULL default '0',			#金子，没用
	  `MAXGOLD` int(10) unsigned NOT NULL default '0',			#金子，没用
	  `BIDDER` varchar(32) NOT NULL default '',					#购买者
	  `BIDDER2` varchar(32) NOT NULL default '',				#前一个购买者
	  `STARTTIME` int(10) unsigned NOT NULL default '0',		#开始时间
	  `ENDTIME` int(10) unsigned NOT NULL default '0',			#结束时间
	  `ITEM` blob NOT NULL,										#物品
	  `CHARGE` int(10) unsigned NOT NULL default '0',			#手续费
	  `BIDTYPE` tinyint(3) unsigned NOT NULL default '0',		#竞价类型
	  `OWNERID` int(10) unsigned NOT NULL default '0',			#卖家ID
	  `BIDDERID` int(10) unsigned NOT NULL default '0',			#买家ID
	  `BIDDER2ID` int(10) unsigned NOT NULL default '0',		#前一个买家ID
	  `ITEMID` int(10) unsigned NOT NULL default '0',			#物品ID
	  `OBJID` int(10) unsigned NOT NULL default '0',			#物品唯一ID
	  PRIMARY KEY  (`ID`),
	  KEY `NAME` (`STATE`,`TYPE`,`QUALITY`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.33
# Database: xwl
# Table: 'MAIL'
# 邮件
# 
CREATE TABLE `MAIL` (
	  `ID` int(10) unsigned NOT NULL auto_increment,			#ID
	  `STATE` tinyint(3) unsigned NOT NULL default '0',         #状态1新 3删除
	  `FROMNAME` varchar(32) NOT NULL default '',               #寄件人
	  `TONAME` varchar(32) NOT NULL default '',                 #收件人
	  `TITLE` varchar(32) NOT NULL default '',                  #标题
	  `TYPE` tinyint(3) unsigned NOT NULL default '0',          #邮件类型
	  `CREATETIME` int(10) unsigned NOT NULL default '0',       #发送时间
	  `DELTIME` int(10) unsigned NOT NULL default '0',          #删除时间
	  `ACCESSORY` tinyint(3) unsigned NOT NULL default '0',     #有无附件
	  `ITEMGOT` tinyint(3) unsigned NOT NULL default '0',       #物品是否已收取
	  `TEXT` varchar(255) NOT NULL default '',                  #内容
	  `SENDMONEY` int(10) unsigned NOT NULL default '0',        #发送银子
	  `RECVMONEY` int(10) unsigned NOT NULL default '0',        #收取银子
	  `SENDGOLD` int(10) unsigned NOT NULL default '0',         #发送金子
	  `RECVGOLD` int(10) unsigned NOT NULL default '0',         #收取金子
	  `BIN` blob,                                               #物品
	  `TOID` int(10) unsigned NOT NULL default '0',             #受件人ID
	  `FROMID` int(10) unsigned NOT NULL default '0',           #寄信人ID
	  `ITEMID` int(10) unsigned NOT NULL default '0',           #物品唯一ID
	  PRIMARY KEY  (`ID`),
	  KEY `two` (`STATE`,`DELTIME`,`ACCESSORY`),
	  KEY `three` (`TONAME`,`STATE`,`ID`,`ACCESSORY`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

# Host: 192.168.2.33
# Database: xwl
# Table: 'SN'
# 序列号，运行活动发放序列号用
# 
CREATE TABLE `SN` (
		`SN` varchar(16) NOT NULL default '',
		`LEVEL` smallint(5) unsigned NOT NULL default '0',
		`DAY` tinyint(3) unsigned NOT NULL default '0',
		`USER` varchar(33) NOT NULL default '',
		PRIMARY KEY  (`SN`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.33
# Database: xwl
# Table: 'CARTOONPET'
# 
CREATE TABLE `CARTOONPET` (
	  `CARTOONID` int(10) unsigned NOT NULL auto_increment,		#ID
	  `NAME` char(32) NOT NULL default '0',                     #名字
	  `NPCID` int(10) unsigned NOT NULL default '0',            #种类
	  `MASTERID` int(10) unsigned NOT NULL default '0',         #主人ID
	  `MASTERNAME` char(32) NOT NULL default '',                #主人名字
	  `LEVEL` tinyint(3) unsigned NOT NULL default '0',         #等级
	  `EXP` int(10) unsigned NOT NULL default '0',              #经验
	  `MAXEXP` int(10) unsigned NOT NULL default '0',           #升级经验
	  `ADDEXP` int(10) unsigned NOT NULL default '0',           #积累经验
	  `STATE` tinyint(3) unsigned NOT NULL default '0',         #状态
	  `ADOPTER` char(32) NOT NULL default '',                   #领养人
	  `TIME` int(10) unsigned NOT NULL default '0',             #剩余时间
	  `SP` int(10) unsigned NOT NULL default '0',               #没用
	  `MAXSP` int(10) unsigned NOT NULL default '0',            #没用
	  `MASTERLEVEL` int(10) unsigned NOT NULL default '0',      #主人等级
	  `REPAIR` tinyint(3) unsigned NOT NULL default '0',        #自动修理设置
	  `SYSADOPT` int(10) unsigned NOT NULL default '0',         #系统领养设置
	  `AUTOADOPT` tinyint(3) unsigned NOT NULL default '0',     #自动领养设置
	  `AUTOCHARGE` tinyint(3) unsigned NOT NULL default '0',    #自动充值设置
	  `STARTADOPT` int(10) unsigned NOT NULL default '0',		#开始领养时间
	  `ADOPTCOUNT` int(10) unsigned NOT NULL default '0',		#领养计时
	  PRIMARY KEY  (`CARTOONID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.32
# Database: zjw
# Table: 'VOTE'
# 
CREATE TABLE `VOTE` (
		`ID` int(10) unsigned NOT NULL auto_increment,	#投票ID
		`COUNTRYID` int(10) unsigned NOT NULL default '0',	#投票国家
		`STATUS` int(10) unsigned NOT NULL default '0',	#投票状态
		`TYPE` int(10) unsigned NOT NULL default '0',	#投票类型
		PRIMARY KEY  (`ID`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 


# Host: 192.168.2.32
# Database: zjw
# Table: 'VOTEDPLAYER'
# 
CREATE TABLE `VOTEDPLAYER` (
		`VOTEID` int(10) unsigned NOT NULL default '0',	#投票ID
		`CHARID` int(10) unsigned NOT NULL default '0',	#玩家ID
		PRIMARY KEY  (`VOTEID`,`CHARID`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.32
# Database: zjw
# Table: 'VOTEITEM'
# 
CREATE TABLE `VOTEITEM` (
		`VOTEID` int(10) unsigned NOT NULL default '0',	#投票ID
		`OPTIONID` int(11) NOT NULL default '0',	#选项ID
		`OPTIONDESC` varchar(100) NOT NULL default '',	#选项描述
		`BALLOT` int(10) unsigned NOT NULL default '0',	#投票数
		PRIMARY KEY  (`VOTEID`,`OPTIONID`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.32
# Database: zjw
# Table: 'TECH'
# 
CREATE TABLE `TECH` (
		`UID` int(10) unsigned NOT NULL auto_increment,		#科技ID
		`COUNTRYID` int(10) unsigned NOT NULL default '0',	#国家ID
		`TYPE` int(10) unsigned NOT NULL default '0',		#科技类型
		`NAME` varchar(33) NOT NULL default '',				#科技名称
		`PROGRESS` int(10) unsigned NOT NULL default '0',	#研究进度
		`RESEARCHID` int(10) unsigned default '0',			#研究员ID
		`RESEARCHNAME` varchar(33) default '',				#研究员姓名
		`LEVEL` int(10) unsigned NOT NULL default '1',		#科技等级
		`STATUS` int(10) unsigned NOT NULL default '0',		#研究状态
		PRIMARY KEY  (`UID`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 


# 主机: 192.168.2.32
# 数据库: zebra3
# 表: 'SORTLIST'
# 
# 角色等级排名表，此表存储前2000名角色的id,级别，经验等数据
#
CREATE TABLE `SORTLIST` (
  `CHARID` int(10) unsigned NOT NULL default '0',      #角色ID
  `LEVEL` smallint(5) unsigned NOT NULL default '0',   #角色等级
  `EXP` bigint(20) unsigned NOT NULL default '0',      #角色经验
  `USERNAME` char(33) NOT NULL DEFAULT '',      #角色经验
  PRIMARY KEY  (`CHARID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1; 


# Host: 192.168.2.32
# Database: zjw
# Table: 'CAPTAIN'
# 军队成员表
# 
CREATE TABLE `CAPTAIN` (
		`ARMYID` int(10) unsigned NOT NULL default '0',	#军队ID
		`CHARID` int(10) unsigned NOT NULL default '0',	#玩家ID
		`CHARNAME` varchar(33) NOT NULL default '',	#玩家姓名
		`NPCNUM` int(10) NOT NULL default '0',	#带领的NPC数,未用
		PRIMARY KEY  (`ARMYID`,`CHARID`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 


# Host: 192.168.2.32
# Database: zjw
# Table: 'ARMY'
# 国家军队表
# 
CREATE TABLE `ARMY` (
		`ID` int(10) unsigned NOT NULL auto_increment,	#军队ID
		`COUNTRYID` int(10) unsigned NOT NULL default '0',	#国家ID
		`CITYID` int(10) unsigned NOT NULL default '0',	#城市ID
		`NAME` varchar(33) NOT NULL default '',	#军队名字
		`GENNAME` varchar(33) NOT NULL default '',	#将军名
		`GENID` int(10) unsigned NOT NULL default '0',	#将军ID
		PRIMARY KEY  (`ID`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.32
# Database: zjw
# Table: 'ALLY'
# 
CREATE TABLE `ALLY` (
		`COUNTRYID` int(10) unsigned NOT NULL default '0',	# 联盟国ID1
		`ALLYCOUNTRYID` int(10) unsigned NOT NULL default '0',  # 联盟国ID2
		`CREATETIME` int(10) unsigned NOT NULL default '0',     # 联盟建立时间
		`LASTUPTIME` int(10) unsigned NOT NULL default '0',     # 联盟数据最后更新时间
		`FRIENDDEGREE` int(10) unsigned NOT NULL default '0'    # 联盟友好度
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.33
# Database: xwl
# Table: 'EMPERORFORBID'
# 皇帝禁言玩家的表
CREATE TABLE `EMPERORFORBID` ( 
		`DATA` char(40) character set latin1 collate latin1_bin NOT NULL default '0'
		) ENGINE=MyISAM DEFAULT CHARSET=latin1;

# Host: 192.168.2.32
# Database: zjw
# Table: 'RECOMMENDSUB'
# 被推荐人表
CREATE TABLE `RECOMMENDSUB` (
		`ID` int(10) unsigned NOT NULL default '0', # 被推荐人CHARID
		`NAME` varchar(33) NOT NULL default '', # 被推荐人姓名
		`LASTLEVEL` int(10) unsigned NOT NULL default '0', # 被推荐人上次领取奖励的等级
		`TOTAL` int(10) unsigned NOT NULL default '0', # 累计领取奖励值
		`LASTTIME` int(10) unsigned NOT NULL default '0', # 上次汇报的时间
		`RECOMMENDID` int(10) unsigned NOT NULL default '0', # 推荐人ID
		PRIMARY KEY  (`ID`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.2.32
# Database: zjw
# Table: 'RECOMMEND'
# 推荐人表
CREATE TABLE `RECOMMEND` (
		`ID` int(10) unsigned NOT NULL default '0', # 推荐人ID
		`NAME` varchar(33) NOT NULL default '', # 推荐人姓名
		`TOTAL` int(10) unsigned NOT NULL default '0', # 累计领取奖励值
		`BALANCE` int(10) unsigned NOT NULL default '0', # 当前可领取的奖金额
		PRIMARY KEY  (`ID`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: 192.168.5.37
# Database: xwl
# Table: 'PKGAME'
# 
CREATE TABLE `PKGAME` (
	  `CHARID` int(10) unsigned NOT NULL default '0',		#角色ID
	  `NAME` char(32) NOT NULL default '',					#名字
	  `LEVEL` int(10) unsigned NOT NULL default '0',		#等级
	  `GAMEID` int(10) unsigned NOT NULL default '0',		#比赛ID
	  `TEAMID` int(10) unsigned NOT NULL default '0',		#国家ID
	  `JOB` int(10) unsigned NOT NULL default '0',			#队长、队员
	  `TITLE` int(10) unsigned NOT NULL default '0',		#称号
	  PRIMARY KEY  (`CHARID`),
	  UNIQUE KEY `name` (`NAME`),
	  KEY `GAMEID` (`GAMEID`,`TEAMID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1; 
# Host:
# Database: lzl
# Table: 'CDAREGAMBLE'
# 国战下注表
CREATE TABLE `CDAREGAMBLE` (
		`USERID` int(10) unsigned NOT NULL default '0',#用户ID
		`ATTID` int(10) unsigned NOT NULL default '0',#挑战方ID
		`DFID` int(10) unsigned NOT NULL default '0',#防守方ID
		`DARETIME` int(10) unsigned NOT NULL default '0',#国战开始时间
		`HASFIRE` int(10) NOT NULL default '0',#1国战已经结束0国战尚未开始
		`NUM` int(10) unsigned NOT NULL default '0',#注金
		`STATE` int(10) unsigned NOT NULL default '0',#0:押攻方，1，押防守方
		`WINNUM` int(10) NOT NULL default '0' , #赢得数量 
		`TEMPNUM` int(10) unsigned NOT NULL default '0',#临时存放
		PRIMARY KEY (`USERID`,`ATTID`,`DFID`,`DARETIME`,`STATE`)
		) ENGINE=MYISAM DEFAULT CHARSET=latin1;
# Host: 192.168.5.36
# Database: lzl
# Table: 'KINGNAME'
# 
CREATE TABLE `KINGNAME` (
		`ID` int(10) unsigned NOT NULL auto_increment,#ID自动生成
		`NAME` varchar(33) default 'NULL',            #下台国王名字
		`COUNTRY` int(10) unsigned NOT NULL default '0', #国家ID
		`TIME` int(10) unsigned NOT NULL default '0', #下台时间
		PRIMARY KEY  (`ID`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 
# Host: 192.168.5.36
# Database: zjwback_l
# Table: 'DARE_CAPTION'
# 
CREATE TABLE `DARE_CAPTION` (
		  `COUNTRYID` int(10) unsigned NOT NULL default '0',
		    `ID` int(10) unsigned NOT NULL default '0',
			  `DARE_CAPTION` varchar(33) NOT NULL default ''
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 
# Host: 192.168.5.36
# Database: lzl
# Table: 'ALLYTIME'
# 
CREATE TABLE `ALLYTIME` (
		`ID` int(10) unsigned NOT NULL auto_increment,					#序号
		`DWID` int(10) unsigned NOT NULL default '0',					#国家ID
		`ALLYID` int(10) unsigned NOT NULL default '0',					#盟国ID
		`TYPE` int(10) unsigned NOT NULL default '0',						#结盟状态
		`TIME` int(10) unsigned NOT NULL default '0',					#结盟时间
		PRIMARY KEY  (`ID`)
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# ghost cheng 06-11-07
# Host: 192.168.5.37
# Database: ghost
# Table: 'FUND'
# 
CREATE TABLE `FUND` (															#储蓄系统
  `ID` int(10) unsigned NOT NULL auto_increment,								#序号
  `CHARID` int(10) unsigned NOT NULL default '0',								#charid
  `BUYTIME` int(10) unsigned NOT NULL default '0',								#购买基金的时间
  `LEVEL` smallint(5) unsigned NOT NULL default '1',							#购买时的等级
  `SUM` smallint(5) unsigned NOT NULL default '1',								#购买的数量
  `LASTLEVEL` smallint(5) unsigned NOT NULL default '0',						#最后一次领取等级
  `ACCID` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY  (`ID`),
  KEY `FINDEX` (`CHARID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1; 
# Host: 192.168.5.37
# Database: ghost
# Table: 'HEROLIST'
# 
CREATE TABLE `HEROLIST` (														#
  `COUNTRYID` tinyint(3) unsigned NOT NULL default '0',								#国家ID
  `DATEVALUE` tinyint(3) unsigned NOT NULL default '0',						#日期
  `CHARID` int(10) unsigned NOT NULL default '0',							#charid
  `NUM` int(10) unsigned NOT NULL default '0',								#杀人数量
  `CHARNAME` varchar(33) NOT NULL default '',
  `KILLTIME` time NOT NULL DEFAULT '00:00:00',
  `GETJIFEN` tinyint(3) unsigned NOT NULL DEFAULT '0',								#角色名
  KEY `cindex` (`COUNTRYID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
# Host: 192.168.5.36
# Database: lzl
# 改名记录
# Table: 'CHANGENAME'
# 
CREATE TABLE `CHANGENAME` (
		`USERID` int(10) unsigned NOT NULL default '0', #用户ID
		`TIME` int(10) unsigned NOT NULL default '0', #改名时间
		`OLDNAME` varchar(33) NOT NULL default '', #旧名字
		`NEWNAME` varchar(33) NOT NULL default '' #新名字
		) ENGINE=MyISAM DEFAULT CHARSET=latin1; 
