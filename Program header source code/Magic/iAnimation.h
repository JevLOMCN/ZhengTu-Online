#pragma once
class CCanvas;

#include "public.h"
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class IBitmap;

struct stAnimationInfo
{
	int nDir;
	int nFrame;
	bool bStop;
	int nTimeMS;
	int nFrameOfDir;
	int nFrame0;
	bool bBack;
	stAnimationStruct stAni;
};

//////////////////////////////////////////////////////////
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class iAnimation
{
public:
	virtual bool GetClipRect(RECT* prect) = 0;
	virtual bool IsExistAction(int ani) = 0;
	virtual ULONG AddRef() = 0;
	virtual ULONG Release() = 0;

	virtual stAnimationInfo GetAnimationInfo() = 0;
	virtual void SetAnimationInfo(stAnimationInfo& info) = 0;

	virtual bool CreateAnimation(LPCSTR szType, size_t pack, int nAni,const stAnimationStruct& stAnimation,int eUsage) = 0;
	virtual void ActionNext(long time_eslasped) = 0;
	virtual void Draw(POINT pt) = 0;
	virtual void Draw(POINT pt,POINT ptCenter,stPointF scale = stPointF(1.0f,1.0f)) = 0;
//	virtual void DrawNoCenter(POINT pt) = 0;
	virtual void TurnBack() = 0;

	virtual void Reload() = 0;

	virtual bool Action(int nAni,const stAnimationStruct& stAnimation,bool bServerLogic = false) = 0;
	virtual void GetAction(stAnimationStruct& stAnimation) = 0;

	virtual size_t GetAction() = 0;
	virtual void Turn(POINT ptSpeed) = 0;
	virtual void TurnTo(size_t n) = 0;
	virtual size_t GetDir() = 0;
	virtual size_t GetFrame() = 0;
	virtual void SetDirFrame(size_t nFrame)=0;
	virtual size_t GetGroup() = 0;
	virtual size_t GetFrameOfDir() = 0;
	virtual size_t GetDirFrame() = 0;
	virtual void ChangeFrame(size_t n) = 0;
	virtual void ChangeFrameOfDir(size_t n) = 0;

	virtual void AddRunTime(long time) = 0;
	virtual bool IsDie() = 0;
	virtual bool IsStoped() = 0;
	virtual bool IsActionEnd() = 0;
	virtual bool IsActionStart() = 0;

//	virtual void SetAnimationSpeed(int f) = 0;
//	virtual int GetAnimationSpeed() = 0;
	virtual int GetAnimationTime() = 0;
	virtual void SetAnimationColor(DWORD cr) = 0;
	virtual DWORD GetAnimationColor() = 0;
	virtual void SetAnimationAlpha(BYTE alpha) = 0;
	virtual BYTE GetAnimationAlpha() = 0;
	virtual bool SetAnimationID(size_t n) = 0;
	virtual size_t GetAnimationID() = 0;

	virtual bool IsAt(POINT pt,POINT ptCenter, bool bJustInRect = false) = 0;

	virtual IBitmap* GetTexture() = 0;

	virtual bool IsBreak(iAnimation* p,POINT ptOffset,int radius) = 0;
	virtual iAnimation* Clone() = 0;
	virtual void UpdateAnimation() = 0;

	virtual HRESULT InvalidateDeviceObjects() = 0;	//before call reset alt+tab 
	virtual HRESULT RestoreDeviceObjects() = 0;		//after alt+tab

	virtual bool GetWholeSize(int& w,int& h) = 0;
	virtual bool GetImageSize(int& w,int& h)  = 0;
	virtual void SetBlendType(stAnimationMagicDataStruct::BlendEnum eBlend) = 0;

	virtual stAnimationMagicDataStruct::BlendEnum GetBlendType() = 0;
	virtual bool GetDrawRect(POINT pt,POINT ptCenter,RECT & rc) = 0;
	virtual size_t GetFrameOfDir(size_t nGroup,int iDirNum = 8)=0;
	virtual void ToAniEnd() = 0;
};

EXPORT_API iAnimation* Magic_CreateAnimation();
