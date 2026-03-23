#pragma once

#include "Effect.h"

class IBitmaps;
class CEffectManager
{
	typedef std::map<std::string, stEffectInfo> TEffectsMap;
public:
	CEffectManager();
	~CEffectManager();

	bool GetEffect(const std::string& strName, stEffectInfo& info);

	// use this interface for test
	void ReloadEffects();

private:
	TEffectsMap m_mapEffectInfos;

private:
	void LoadEffects();
};

CEffectManager& GetEffectManager();
