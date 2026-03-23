#pragma once

class CCanvas;
struct stAnimationStruct;

#include "iAnimation.h"
#include "iMagic.h"


const int c_nInvalidPack = 0;

extern const float tg1;
extern const float tg2;
extern const float tg3;
extern const float tg4;
extern const float tg5;
extern const float tg6;
extern const float tg7;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class cAnimation : public iAnimation
{
private:
	ULONG m_ulCount;
	size_t m_nFrame;
	size_t m_nFrameOfDir;
	size_t m_nFrame0;
	long m_nTimeMS;		// 播到目前阶段所花时间(动画播放已花时间)
	long m_nOverTime;	// 播完所有动画结束时的时间(播放完所有动画片所花时间)
	DWORD m_crColor;
	size_t m_nGroup;
	size_t m_nDir;
	IBitmap* m_pTexture;
	IBitmapList*  m_pRes;
	size_t m_nPack;
	int m_nAni;

	char m_strType[32];
	char m_strName[32];
	bool m_bLooped;
	bool m_bStopped;   // 动画是否已经停止播放
	bool m_bMirror;	   // 镜像否，目前不用
	bool m_bBack;      // 动画是否正在回播状态中
public:
	~cAnimation(void);
	cAnimation();

	ULONG AddRef() {return ++m_ulCount;};
	ULONG Release();

	bool CreateAnimation(LPCSTR szType, size_t pack, int nAni,const stAnimationStruct& stAnimation,int eUsage);
	bool IsExistAction(int ani);

	TextureUsageType m_eUsage;
	TextureUsageType GetUsage(){return m_eUsage;};

	
	size_t GetFrame(){return m_nFrame;}
	size_t GetDirFrame(){
		//TRACE("m_nFrame %d, m_nFrame0 %d\n",m_nFrame,m_nFrame0);
		return m_nFrame - m_nFrame0;
	};
	void SetDirFrame(size_t nFrame);
	void ChangeFrame(size_t n) ;
	void ChangeFrameOfDir(size_t n);
	
	size_t GetFrameOfDir(){
		return m_nFrameOfDir;
	};
	
	bool GetFrameConst();
	long GetAnimationSpeed(){return m_stAnimation.nAnispeed;}// 每帧多少毫秒     // nAnispeed对应character.mdb中的"动画速度"字段
	int GetAnimationTime(){return GetFrameOfDir()*GetAnimationSpeed();}

	void SetAnimationColor(DWORD cr){m_crColor = cr;}
	DWORD GetAnimationColor(){return m_crColor;}
	void SetAnimationAlpha(BYTE alpha) {m_crColor &= 0x00ffffff; m_crColor |= (alpha << 24);};
	BYTE GetAnimationAlpha() {return (BYTE)(m_crColor >> 24);};

	void LoadFrame();

	size_t GetDir() {return m_nDir;};
	bool SetAnimationID(size_t nPack);
	void Reload();
	size_t GetAnimationID() {return m_nPack;};
	bool IsAnimationInvalid(){return m_nPack == c_nInvalidPack;}

	bool Action(int nAni,const stAnimationStruct& stAnimation,bool bServerLogic);
	stAnimationStruct m_stAnimation;
	void GetAction(stAnimationStruct& stAnimation){stAnimation = m_stAnimation;};
	size_t GetAction() {return m_nAni;};
	
	stAnimationStruct::MirrorEnum GetMirror(){return m_stAnimation.eMirror;}
	size_t GetMaxDir(){return m_stAnimation.nDir;}
	stAnimationStruct::AnimationTypeEnum GetType(){return m_stAnimation.eType;}//  eType对应character.mdb中的"动画类型"字段 
	
	void SetBlendType(stAnimationMagicDataStruct::BlendEnum eBlend)
	{
		m_stAnimation.eBlend = eBlend;
	}
	
	stAnimationMagicDataStruct::BlendEnum GetBlendType()
	{
		return m_stAnimation.eBlend;
	}

	void ActionNext(long time_eslasped);
	void Draw(POINT pt);
	void Draw(POINT pt,POINT ptCenter,stPointF scale = stPointF(1.0f,1.0f));
	void Turn(POINT ptSpeed);
	void TurnBack();
	void TurnTo(size_t dir);

	bool IsDie();
	bool IsActionEnd();
	bool IsActionStart();
	bool IsStoped();

	bool IsAt(POINT pt,POINT ptCenter,bool bJustInRect) ;
	bool GetImageSize(int& w,int& h) ;
	bool GetWholeSize(int& w,int& h);

	size_t GetGroup();
	bool IsBreak(iAnimation* p,POINT ptOffset,int radius);
	IBitmap* GetTexture(){return m_pTexture;}

	void Clear();
	iAnimation* Clone();
	void UpdateAnimation();

	HRESULT InvalidateDeviceObjects() ;	//before call reset alt+tab 
	HRESULT RestoreDeviceObjects() ;		//after alt+tab

	stAnimationInfo GetAnimationInfo();
	void SetAnimationInfo(stAnimationInfo& info);

	bool GetDrawRect(POINT pt,POINT ptCenter,RECT & rc);
	size_t GetFrameOfDir(size_t nGroup,int iDirNum = 8);
	bool GetClipRect(RECT* prect);
	void ToAniEnd();
	void AddRunTime(long time);
	void CopyState(const cAnimation* pAni);
};


void Magic_DestroyAnimation(iAnimation * p);