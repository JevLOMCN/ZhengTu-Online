#pragma once

#include "iLight.h"

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class cLight : public iLight
{
private:
	ULONG m_ulCount;
	void* m_lData;
	POINT m_ptPos;
	DWORD m_crColor;
	int m_nRadius;
	LightTypeEnum m_eType;
public:
	void SetEnableShake(bool b) {}
	bool IsEnableShake() { return true;}

	int GetObjType(){ return eObjType_Light;}

	cLight(void);
	virtual ~cLight(void);

	ULONG AddRef() {return ++m_ulCount;};
	ULONG Release()
	{
		extern void Light_DestoryLight(iLight * p);
		int n = --m_ulCount;
		if (!m_ulCount) 
			Light_DestoryLight(this);
			//delete this; 
		return n;
	}
	//data
	
	void SetData(void* ldata) { m_lData = ldata;};
	void* GetData() {return m_lData;};

	void FinalRelease(){};
	
	POINT GetPos(){return m_ptPos;}
	void SetPos(POINT pt){m_ptPos = pt;}

	void SetColor(DWORD cr){m_crColor = cr;}
	DWORD GetColor(){return m_crColor;}

	void SetRadius(int r){m_nRadius = r;}
	int GetRadius(){return m_nRadius;}

	void SetType(LightTypeEnum e){m_eType = e;};
	LightTypeEnum GetType(){return m_eType;}

	LightEffectEnum m_eEffect;
	void SetEffect(LightEffectEnum e){m_eEffect = e;};
	LightEffectEnum GetEffect(){return m_eEffect;}

	bool m_bEnable;
	void EnableLight(bool b){m_bEnable = b;}
	bool IsEnable(){return m_bEnable;}

	int Run(long time_eslasped);

	bool Draw(POINT ptMapOffset);
	bool DrawAtScreen(POINT ptOffset);

	size_t GetDir(){return 0;}
	//character º÷’‘⁄µÿ…œ£°
	POINT GetSortPos(){return GetPos();};

	HRESULT InvalidateDeviceObjects(){return S_OK;}
	HRESULT RestoreDeviceObjects() {return S_OK;}
};
