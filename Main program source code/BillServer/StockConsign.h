#ifndef _STOCKCONSIGN_H_
#define _STOCKCONSIGN_H_
#include <ext/hash_map>
#include "zType.h"
#include "Command.h"
#include "zMetaData.h"
#include "zTime.h"
#include "BillUser.h"
#include "BillServer.h"
#include "BillUserManager.h"


struct ConsignTrait
{
	DWORD id;
	DWORD accid;
	DWORD num;
	DWORD price;
	DWORD time;
};
class Consign
{
	public:
		Consign()
		{
			bzero(Buf,sizeof(Buf));
			firstfive=(Cmd::stFirstFiveListStockUserCmd *)Buf;
			constructInPlace(firstfive);
		}
		virtual ~Consign()
		{
		}
		virtual bool init()=0;
		bool sendFirstFiveToUser(BillUser *pUser);
		bool sendWaitDataToUser(BillUser *pUser);
		static bool cancelListAll();
		static bool addGold(DWORD accid,DWORD num);
		static bool addMoney(DWORD accid,DWORD num);
		 
	protected:
		// 更新历史记录表
		bool updateHistory(DWORD id, DWORD acc, DWORD num  , DWORD commitprice, DWORD price , DWORD comtime,bool type,DWORD sysmoney);
		//PriceIndex priceindex;
		char Buf[1024];
		Cmd::stFirstFiveListStockUserCmd *firstfive;
};
class ConsignGoldManager :public Consign
{
	private:
		ConsignGoldManager();
		~ConsignGoldManager();
		static ConsignGoldManager *instance;
	public:
		static ConsignGoldManager *getInstance();
		static void delInstance();
		bool init();
		bool trade();
		bool trade(ConsignTrait &goldlist,DWORD &sysmoney);
		bool cancelList(BillUser *pUser , DWORD lsitid);
	public:
};
class ConsignMoneyManager :public Consign
{
	private:
		ConsignMoneyManager();
		~ConsignMoneyManager();
		static ConsignMoneyManager *instance;
	public:
		static ConsignMoneyManager *getInstance();
		static void delInstance();
		bool init();
		bool trade();
		bool trade(ConsignTrait &goldlist,DWORD &sysmoney);
		bool cancelList(BillUser *pUser , DWORD lsitid);
	public:
};

class ConsignHistoryManager
{
	private:
		ConsignHistoryManager();
		~ConsignHistoryManager();
		static ConsignHistoryManager *instance;
	public:
		static ConsignHistoryManager *getInstance();
		static void delInstance();
		bool init();
		bool update();
		bool sendDataToUser(BillUser *pUser , DWORD begintime,DWORD num=0);
		bool sendSelfDataToUser(BillUser *pUser , DWORD begintime,DWORD num);
	public:
		/// 类型定义
		typedef __gnu_cxx::hash_multimap<DWORD ,Cmd::ConsignHistoryType> HistoryIndex;

		/// 类型定义
		typedef __gnu_cxx::pair<HistoryIndex::iterator,HistoryIndex::iterator> HistoryRange;
	private:
		HistoryIndex historyGold;
		HistoryIndex historyMoney;
		Cmd::ConsignHistoryType newHistoryGold; 
		Cmd::ConsignHistoryType newHistoryMoney; 
		Timer _one_min;
};

#endif
