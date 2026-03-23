-- MySQL dump 9.11
--
-- Host: localhost    Database: FLServerDB
-- ------------------------------------------------------
/*数据表 `ACCSTORE0000` 的表结构*/
-- Server version	4.0.21yes

CREATE TABLE `ACCSTORE0000` (
		`UID` int(10) unsigned NOT NULL auto_increment,
		`ACCOUNT` char(48) NOT NULL default '',
                `PASSWD` char(32) NOT NULL default '',
                `STATE` tinyint(4) NOT NULL default '0',
                `TYPE` int(10) unsigned NOT NULL default '0',
		`LASTACTIVEDATE` datetime default NULL,
		PRIMARY KEY  (`UID`),
                UNIQUE KEY `index` (`ACCOUNT`)
              ) ENGINE=MyISAM DEFAULT CHARSET=latin1;

              /*数据表 `ACCSTORE0000` 的数据*/

/*数据表 `BASEINFO0000` 的表结构*/

CREATE TABLE `BASEINFO0000` (
             `UID` int(10) unsigned NOT NULL default '0',
             `ACCOUNT` char(48) NOT NULL default '',
             `EMAIL` char(48) NOT NULL default '',
             `TOKENRING` char(64) NOT NULL default '',
             `SUPERPASSWD` char(32) NOT NULL default '',
             `TNAME` char(32) NOT NULL default '',
             `NICKNAME` char(32) NOT NULL default '',
             `SEX` tinyint(4) NOT NULL default '0',
             `BIRTH` date default '1900-00-00',
             `PHONE` char(32) default '',
             `ADDRESS` char(200) default '',
             `POSTNUM` char(6) default '',
             `IDCARD` char(18) default '',
             `MPHONE` char(20) default '',
             `MPTYPE` tinyint(4) default '0',
             `CREATETIME` bigint(20) unsigned default '0',
             PRIMARY KEY (`UID`),
             UNIQUE KEY `index` (`ACCOUNT`)
           ) ENGINE=MyISAM;

           /*数据表 `BASEINFO0000` 的数据*/
/*数据表 `CONSUMELOG0000` 的表结构*/

DROP TABLE IF EXISTS `CONSUMELOG0000`;

CREATE TABLE `CONSUMELOG0000` (
             `CONSUMEID` int(10) unsigned NOT NULL AUTO_INCREMENT,
             `TID` char(24) NOT NULL default '',
             `UID` int(10) unsigned NOT NULL default '0',
             `ACCOUNT` char(48) NOT NULL default '',
             `TYPE` tinyint(4) NOT NULL default '0',
             `SOURCE` int(11) NOT NULL default '0',
             `CARDID` char(16) default NULL,
             `FILLPOINT` int(11) default '0',
             `CONSUMEPOINT` int(11) default '0',
             `THISPOINT` int(11) NOT NULL default '0',
             `OPERTIME` timestamp NOT NULL default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
             PRIMARY KEY (`CONSUMEID`),
             UNIQUE KEY `index` (`TID`),
             KEY `UID` (`UID`),
             KEY `ACCOUNT` (`ACCOUNT`)
           ) ENGINE=MyISAM;

           /*数据表 `CONSUMELOG0000` 的数据*/

           /*数据表 `POINTBONUS0000` 的表结构*/

CREATE TABLE `POINTBONUS0000` (
             `UID` int(10) unsigned NOT NULL default '0',
             `ACCOUNT` char(48) NOT NULL default '',
             `POINT` int(11) NOT NULL default '0',
             `BONUS` int(11) NOT NULL default '0',
             `HADFILLED` tinyint(4) NOT NULL default '0',
             PRIMARY KEY (`UID`),
             UNIQUE KEY `index` (`ACCOUNT`)
           ) ENGINE=MyISAM;

           /*数据表 `POINTBONUS0000` 的数据*/

           /*数据表 `ZTGOODBUY` 的表结构*/

CREATE TABLE `ZTGOODBUY` (
             `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
             `GOODID` int(10) NOT NULL default '0',
             `GOODNUM` int(10) NOT NULL default '1',
             `POINT` int(10) NOT NULL default '0',
             `MEMO` char(24) default NULL,
             PRIMARY KEY (`ID`)
           ) ENGINE=MyISAM;

           /*数据表 `ZTGOODBUY` 的数据*/

           /*数据表 `ZTGOODDATA` 的表结构*/

CREATE TABLE `ZTGOODDATA` (
             `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
             `GOODID` int(10) NOT NULL default '0',
             `BINDATA` blob,
             `MEMO` varchar(24) default NULL,
             PRIMARY KEY (`ID`)
           ) ENGINE=MyISAM;

           /*数据表 `ZTGOODDATA` 的数据*/

           /*数据表 `ZTLEVELBUY` 的表结构*/

CREATE TABLE `ZTLEVELBUY` (
             `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
             `LEVEL` int(10) NOT NULL default '0',
             `POINT` int(10) NOT NULL default '0',
             `MEMO` char(24) default NULL,
             PRIMARY KEY (`ID`)
           ) ENGINE=MyISAM;

           /*数据表 `ZTLEVELBUY` 的数据*/
