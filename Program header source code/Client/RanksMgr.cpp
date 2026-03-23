#include "public.h"

#include "RanksMgr.h"

namespace
{
	class LoongPearlCompare : public std::binary_function<LoongAltarInfo, LoongAltarInfo, bool>
	{
	public:
		bool operator()(const LoongAltarInfo& info1, const LoongAltarInfo& info2) const
		{
			return info1.pearlNum >= info2.pearlNum;
		}
	};
}

CRanksMgr::CRanksMgr()
{
	m_eCurRankType = RANKTYPE_LEVELUP;
}

CRanksMgr::~CRanksMgr()
{

}

RANK_TYPE CRanksMgr::GetCurRank() const
{
	return m_eCurRankType;
}

RANK_TAIMIAO_TYPE CRanksMgr::GetCurRanktaim() const
{
	return m_eCurRankTypetm;
}

RANK_CELEBRITY_TYPE CRanksMgr::GetCurRankMr() const
{
	return m_eCurRankTypeMr;
}

void CRanksMgr::SetCurRank(RANK_TYPE eRankType)
{
	m_eCurRankType = eRankType;
}

void CRanksMgr::SetCurRanktaim(RANK_TAIMIAO_TYPE eRankTypetm)
{
	m_eCurRankTypetm = eRankTypetm;
}

void CRanksMgr::SetCurRankMr(RANK_CELEBRITY_TYPE eRankTypeMr)
{
	m_eCurRankTypeMr = eRankTypeMr;
}

//太庙排行榜相关//////////////////////////////////////////////////////
void CRanksMgr::AddLoongPearlInfo(const LoongAltarInfo& info)
{
	m_vecLoongPearls.push_back(info);
	std::sort(m_vecLoongPearls.begin(), m_vecLoongPearls.end(), LoongPearlCompare());
}

void CRanksMgr::ClearLoongPearlList()
{
	m_vecLoongPearls.clear();
}

bool CRanksMgr::GetLoongPearlInfo(int index, LoongAltarInfo& info)
{
	if (index >= m_vecLoongPearls.size())
	{
		return false;
	}
	info = m_vecLoongPearls[index];

	return true;
}

int CRanksMgr::GetLoongPearlNum()
{
	return static_cast<int>(m_vecLoongPearls.size());
}

//等级排行榜相关//////////////////////////////////////////////////////
void CRanksMgr::AddLevelupInfo(const LevelupAltarInfo& info)
{
	m_vecLevelup.push_back(info);
	//std::sort(m_vecLevelup.begin(), m_vecLevelup.end(), LoongPearlCompare());
}

void CRanksMgr::ClearLevelupList()
{
	m_vecLevelup.clear();
}

bool CRanksMgr::GetLevelupInfo(int index, LevelupAltarInfo& info)
{
	if (index >= m_vecLevelup.size())
	{
		return false;
	}
	info = m_vecLevelup[index];

	return true;
}

int CRanksMgr::GetLevelupNum()
{
	return static_cast<int>(m_vecLevelup.size());
}


//战力排行榜相关//////////////////////////////////////////////////////
void CRanksMgr::AddZhanLiInfo(const ZhanLiAltarInfo& info)
{
	m_vecZhanLi.push_back(info);
	//std::sort(m_vecZhanLi.begin(), m_vecZhanLi.end(), LoongPearlCompare());
}

void CRanksMgr::ClearZhanLiList()
{
	m_vecZhanLi.clear();
}

bool CRanksMgr::GetZhanLiInfo(int index, ZhanLiAltarInfo& info)
{
	if (index >= m_vecZhanLi.size())
	{
		return false;
	}
	info = m_vecZhanLi[index];

	return true;
}

int CRanksMgr::GetZhanLiNum()
{
	return static_cast<int>(m_vecZhanLi.size());
}


//英雄排行榜相关//////////////////////////////////////////////////////
void CRanksMgr::AddHeroInfo(const HeroAltarInfo& info, bool bToday)
{
	if (bToday)
		m_vecHeroToday.push_back(info);
	else
		m_vecHeroYesterday.push_back(info);
}

void CRanksMgr::ClearHeroList(bool bClearToday)
{
	if (bClearToday)
		m_vecHeroToday.clear();
	else
		m_vecHeroYesterday.clear();
}

bool CRanksMgr::GetHeroInfo(int index, HeroAltarInfo& info, bool bToday)
{
	if (bToday)
	{
		if (index >= m_vecHeroToday.size())
		{
			return false;
		}
		info = m_vecHeroToday[index];
	}
	else
	{
		if (index >= m_vecHeroYesterday.size())
		{
			return false;
		}
		info = m_vecHeroYesterday[index];
	}
	return true;
}

int CRanksMgr::GetHeroNum(bool bToday)
{
	if (bToday)
		return static_cast<int>(m_vecHeroToday.size());
	else
		return static_cast<int>(m_vecHeroYesterday.size());
}

//积分排行榜相关//////////////////////////////////////////////////////
void CRanksMgr::AddTicketupInfo(const TicketupAltarInfo& info)
{
	m_vecTicketup.push_back(info);
	//std::sort(m_vecTicketup.begin(), m_vecTicketup.end(), LoongPearlCompare());
}

void CRanksMgr::ClearTicketupList()
{
	m_vecTicketup.clear();
}

bool CRanksMgr::GetTicketupInfo(int index, TicketupAltarInfo& info)
{
	if (index >= m_vecTicketup.size())
	{
		return false;
	}
	info = m_vecTicketup[index];

	return true;
}

int CRanksMgr::GetTicketupNum()
{
	return static_cast<int>(m_vecTicketup.size());
}

//鲜花行榜相关//////////////////////////////////////////////////////
void CRanksMgr::AddFlowerInfo(const FlowerAltarInfo& info)
{
	m_vecFlower.push_back(info);
}

void CRanksMgr::ClearFlowerList()
{
	m_vecFlower.clear();
}

bool CRanksMgr::GetFlowerInfo(int index, FlowerAltarInfo& info)
{
	if (index >= m_vecFlower.size())
	{
		return false;
	}
	info = m_vecFlower[index];

	return true;
}

int CRanksMgr::GetFlowerNum()
{
	return static_cast<int>(m_vecFlower.size());
}


CRanksMgr& GetRanksMgr()
{
	static CRanksMgr ranksMgr;
	return ranksMgr;
}
