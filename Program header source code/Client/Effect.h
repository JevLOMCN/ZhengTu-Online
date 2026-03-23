#pragma once

class IBitmaps;

struct stEffectInfo
{
	stResourceLocation stResourceLocation;
	IBitmaps *pBitmaps;
	int nOffsetX;
	int nOffsetY;

	stEffectInfo();
};

class CEffect
{
public:
	CEffect(const std::string& strName);
	~CEffect();

	void Render(const stPointI& pos);

private:
	std::string m_strName;
	stEffectInfo m_stEffectInfo;
};