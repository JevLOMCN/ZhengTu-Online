#pragma once
#include "iObj.h"

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class iLight : public iObj
{
public:
	virtual void SetRadius(int r) = 0;
	virtual int GetRadius() = 0;
	virtual void SetColor(DWORD cr) = 0;
	virtual DWORD GetColor() = 0;
	virtual void SetType(LightTypeEnum e) = 0;
	virtual LightTypeEnum GetType() = 0;
	virtual void SetEffect(LightEffectEnum e) = 0;
	virtual LightEffectEnum GetEffect() = 0;
	virtual void EnableLight(bool b) = 0;
	virtual bool IsEnable() = 0;

};

#ifdef _DEBUG
EXPORT_API iLight* Light_CreateLight(const char* fileName,int line);
#define LIGHT_CREATELIGHT Light_CreateLight(__FILE__,__LINE__)
#else
EXPORT_API iLight* Light_CreateLight();
#define LIGHT_CREATELIGHT Light_CreateLight()
#endif