#pragma once

#include "command.h"

enum RANK_TYPE
{
	RANKTYPE_LEVELUP, //世界等级排行榜
	RANKTYPE_ZHANLI, //世界战力排行榜
	RANKTYPE_T_HERO, //今日英雄排行榜
	RANKTYPE_Y_HERO, //昨日英雄排行榜
	RANKTYPE_TICKET, //个人积分排行榜
	RANKTYPE_COUNT, //排行类型数量
};

enum RANK_TAIMIAO_TYPE //太庙榜单
{
	RANK_TAIMIAO_TYPE_LOONGPEARL, //太庙资源排行榜
	RANK_TAIMIAO_TYPE_COUNT, //排行类型数量
};

enum RANK_CELEBRITY_TYPE //鲜花榜单
{
	RANK_CELEBRITY_TYPE_FLOWER, // 鲜花排行榜
	RANK_CELEBRITY_TYPE_COUNT, // 排行类型数量
};

class CRanksMgr
{
	typedef std::vector<LevelupAltarInfo> LevelupList;
	typedef std::vector<LoongAltarInfo> LoongPearlList;
	typedef std::vector<ZhanLiAltarInfo> ZhanLiList;
	typedef std::vector<TicketupAltarInfo> TicketupList;
	typedef std::vector<FlowerAltarInfo> FlowerList;
	typedef std::vector<HeroAltarInfo> HeroList;
	
public:
	CRanksMgr();
	~CRanksMgr();

	RANK_TYPE GetCurRank() const; //查看世界榜单
	void SetCurRank(RANK_TYPE eRankType);

	RANK_TAIMIAO_TYPE GetCurRanktaim() const; //查看太庙榜单
	void SetCurRanktaim(RANK_TAIMIAO_TYPE eRankTypetm);

	RANK_CELEBRITY_TYPE GetCurRankMr() const; //查看鲜花榜单
	void SetCurRankMr(RANK_CELEBRITY_TYPE eRankTypeMr);

	//太庙资源排行相关
	void AddLoongPearlInfo(const LoongAltarInfo& info);
	void ClearLoongPearlList();
	bool GetLoongPearlInfo(int index, LoongAltarInfo& info);
	int GetLoongPearlNum(); // 返回资源榜国家的总数

	//战力排行榜相关
	void AddZhanLiInfo(const ZhanLiAltarInfo& info);
	void ClearZhanLiList();
	bool GetZhanLiInfo(int index, ZhanLiAltarInfo& info);
	int GetZhanLiNum();

	//等级排行榜相关
	void AddLevelupInfo(const LevelupAltarInfo& info);
	void ClearLevelupList();
	bool GetLevelupInfo(int index, LevelupAltarInfo& info);
	int GetLevelupNum();

	// 英雄榜相关
	void AddHeroInfo(const HeroAltarInfo& info,bool bToday = true);
	void ClearHeroList(bool bClearToday = true);
	bool GetHeroInfo(int index, HeroAltarInfo& info,bool bToday = true);
	int GetHeroNum(bool bToday = true);

	//积分排行榜相关
	void AddTicketupInfo(const TicketupAltarInfo& info);
	void ClearTicketupList();
	bool GetTicketupInfo(int index, TicketupAltarInfo& info);
	int GetTicketupNum();

	//鲜花排行榜相关
	void AddFlowerInfo(const FlowerAltarInfo& info);
	void ClearFlowerList();
	bool GetFlowerInfo(int index, FlowerAltarInfo& info);
	int GetFlowerNum();

private:
	RANK_TYPE m_eCurRankType;
	RANK_TAIMIAO_TYPE m_eCurRankTypetm;
	RANK_CELEBRITY_TYPE m_eCurRankTypeMr;

	LevelupList m_vecLevelup;
	LoongPearlList m_vecLoongPearls;
	ZhanLiList m_vecZhanLi;
	HeroList m_vecHeroToday;
	HeroList m_vecHeroYesterday;
	TicketupList m_vecTicketup;
	FlowerList m_vecFlower;
};

CRanksMgr& GetRanksMgr();
