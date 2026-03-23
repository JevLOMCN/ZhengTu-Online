#pragma once

//#include "mapStruct.h"
#include "map.h"
///////////////////////////////////////////
// ∫£¿À∂‘œÛ
///////////////////////////////////////////
class CMapSurfGenerator;
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CSurfObject{
	friend class CMapSurfGenerator;

	stPointI	m_startPos;

	float		m_pos;
	float		m_speed;
	float		m_accel;

	float		m_alpha;
	float		m_alphaSpeed;
	float		m_alphaAccel;

	float		m_xScale;
	float		m_cxSpeed;
	float		m_cxAccel;
	IBitmaps*	m_bmp;
	float		m_angle;
	enumSurfType m_type;
public:
	void	Draw(POINT ptMapOffset,stPointF * scale = NULL);
	void	Run(float fTimeElasped);
	bool	IsDeleted() { return m_alpha < 0.0f; }
};

///////////////////////////////////////////
// ∫£¿À∑¢…˙∆˜
///////////////////////////////////////////
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CMapSurfGenerator
{
	stSurfData m_data;
	stPointI	m_pos;
	DWORD		m_nextRunTime;
public:

	CMapSurfGenerator(stSurfData  data,POINT gridPos);
	~CMapSurfGenerator(void);

	void	Run(float fTimeElasped);

	bool	Init(CSurfObject* pObj);
	bool	Generate(CSurfObject* pObj);
	void	SetData(stSurfData data)
	{
		m_data = data;
	}
};

#include "./MapAddinManager.h"
///////////////////////////////////////////
// ∫£¿Àπ‹¿Ì∆˜
///////////////////////////////////////////
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CMapSurfManager : public tMapAddinManager<CMapSurfGenerator> {

	typedef tMapAddinManager<CMapSurfGenerator> tParent;
	typedef std::vector<CSurfObject*> tSurfObjects;
	tSurfObjects m_surfObjects;

	IBitmapList*	m_pBmpList;

	CMapSurfGenerator* CreateObject(POINT pt,void* pData) { return new CMapSurfGenerator(*(stSurfData*)pData,pt);}
	void SetObjectData(CMapSurfGenerator* pObject,void* pData) { pObject->SetData(*(stSurfData*)pData);}

public:
	~CMapSurfManager();
	void Init();
	void Release();
	IBitmaps*		GetImage(enumSurfType type);
	void Draw(POINT ptMapOffset,stPointF * scale = NULL);
	void Run(float time_elasped);
	void PushSurfObject(CSurfObject* pObj)
	{
		m_surfObjects.push_back(pObj);
	}
};

CMapSurfManager* GetMapSurfManager();