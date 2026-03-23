/**
 * \file
 * \version  $Id: CountryInfo.h  $
 * \author 
 * \date 
 * \brief 国家信息
 *
 * 
 */

#ifndef _COUNTRYINFO_H_
#define _COUNTRYINFO_H_
#include "zSubNetService.h"
#include "Zebra.h"
#include "zMisc.h"
#include "SuperCommand.h"
#include "SessionClient.h"
#include <vector>
#include <string>
#include "SceneCommand.h"
#include "GateUser.h"
/**
 * \brief 国家配置文件信息
 *
 */
class CountryInfo
{
	public:
		struct Info
		{
			DWORD countryid;
			DWORD function;
			std::string countryname;
			std::string mapname;
			Info()
			{
				countryid=0;
				function=0;
			}
		};
	private:
		typedef std::vector<Info> StrVec;
		typedef StrVec::iterator StrVec_iterator;
		StrVec country_info;
		// 国家排序锁
		zMutex mutex;
		DWORD country_order[100];

		struct CountryDic
		{
			DWORD id;
			char name[MAX_NAMESIZE];
			DWORD mapid;
			DWORD function;
			CountryDic()
			{
				id=0;
				mapid=0;
				bzero(name ,sizeof(name));
				function=0; 
			}
		};
		struct MapDic
		{
			DWORD id;
			char name[MAX_NAMESIZE];
			char filename[MAX_NAMESIZE];
			DWORD backto;
			MapDic()
			{
				id=0;
				bzero(name ,sizeof(name));
				bzero(filename,sizeof(filename));
				backto=0;
			}
		};
		typedef std::map<DWORD , CountryDic> CountryMap;
		typedef CountryMap::iterator CountryMap_iter;
		typedef CountryMap::value_type CountryMap_value_type;
		CountryMap country_dic;
		typedef std::map<DWORD , MapDic> MapMap;
		typedef MapMap::value_type MapMap_value_type;
		typedef MapMap::iterator MapMap_iter;
		MapMap map_dic;

	public:
		CountryInfo()
		{
			bzero(country_order , sizeof(country_order));
		}
		~CountryInfo(){}
		Info *getInfo(unsigned int country_id);
		bool init();
		bool reload();
		int getCountrySize();
		int getAll(char *buf);
		unsigned int getCountryID(unsigned int country_id);
		unsigned int getRealMapID(unsigned int map_id);
		const char *getRealMapName(const char *name);
		void setCountryOrder(Cmd::Session::CountrOrder *ptCmd);
		std::string getCountryName(unsigned int country_id);
		std::string getMapName(unsigned int country_id);
		bool isEnableLogin(unsigned int country_id);
		bool isEnableRegister(unsigned int country_id);
		void processChange(GateUser *pUser, Cmd::Scene::t_ChangeCountryStatus *rev);
};

#endif
