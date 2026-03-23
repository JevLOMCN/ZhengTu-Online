-- MySQL dump 9.11
--
-- Host: localhost    Database: SuperServer
-- ------------------------------------------------------
-- Server version	4.0.21yes


CREATE TABLE `SERVERLIST` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `TYPE` int(10) unsigned NOT NULL default '0',
  `NAME` varchar(32) NOT NULL default '',
  `IP` varchar(16) NOT NULL default '127.0.0.1',
  `PORT` int(10) unsigned NOT NULL default '0',
  `EXTIP` varchar(16) NOT NULL default '127.0.0.1',
  `EXTPORT` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM; 

 CREATE TABLE `GAMETIME` (
 `GAMETIME` bigint(20) unsigned NOT NULL default '0'
 ) ENGINE=MyISAM;
