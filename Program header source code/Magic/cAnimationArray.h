#pragma	once
#include "cAnimation.h"
#include <vector>
#include <list>

// 人物残象效果
struct stKeepImage
{
	POINT ptPos;
	int nAlpha;
	std::vector<iAnimation* > m_aAnimation;		
	POINT ptCenter;
	BYTE* pOrder;

	stKeepImage()
	{
		nAlpha = 255;
		pOrder = NULL;
	};
	stKeepImage::~stKeepImage()
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->Release();
		m_aAnimation.clear();
	}
	void Draw(POINT ptScreen,bool bOrder = false);
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class cAnimationArray
{
private:
	std::vector<iAnimation* > m_aAnimation;
	int m_nMainIndex;						 // 主要的动作，如主角一般用body作为主动作
	stFade m_stFade;						 // 淡入淡出
	stKeepImageControl  m_stKeepImageControl;// 残象时间控制
	std::list<stKeepImage*> m_listKeepImage; // 残象
public:
	void SetAnimationCount(size_t count);
	int GetAnimationCount(){ return m_aAnimation.size();}
	void SetMainAniIndex(int i){ m_nMainIndex = i;}
	void Reserve(size_t num)
	{
		m_aAnimation.reserve(num);
	}
	
	cAnimationArray()
	{
		m_nMainIndex = 0;
		m_stFade.Clear();
		m_stKeepImageControl.Clear();
	}
	~cAnimationArray()
	{
		FinalRelease();
		ReleaseAnimation();
	}
	bool IsEmpty()
	{
		return m_aAnimation.empty();
	}
	void FinalRelease()
	{
		for (std::list<stKeepImage*>::iterator it = m_listKeepImage.begin(); it != m_listKeepImage.end(); ++it)
			delete (*it);
		m_listKeepImage.resize(0);
	}
	void ReleaseAnimation()
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->Release();
		m_aAnimation.resize(0);
	}

	void AddRunTime(long time)
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->AddRunTime(time);
	}

	void ToAniEnd()
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->ToAniEnd();
	}

	void Add(iAnimation* p){m_aAnimation.push_back(p);}
	size_t GetNum(){return m_aAnimation.size();}
	void ChangerColor(int part,DWORD cr){		// 改变动作part部分的颜色
		if (part >= 0 && part < (int)m_aAnimation.size())
			m_aAnimation[part]->SetAnimationColor(cr);			
	}

	void ChangerColor(DWORD cr){
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->SetAnimationColor(cr);
	}

	DWORD GetAnimationColor()
	{
		if(m_aAnimation.size() && m_aAnimation[m_nMainIndex]) return m_aAnimation[m_nMainIndex]->GetAnimationColor();
		return 0;
	}
	DWORD GetAnimationColor(int part)
	{
		if (part >= 0 && part < (int)m_aAnimation.size())
			return m_aAnimation[part]->GetAnimationColor();
		return 0;
	}
	BYTE GetAlpha(){
		if(!m_aAnimation.empty())
			return GetMainAnimation()->GetAnimationAlpha();
		return 0;
	}
	DWORD GetColor(){
		if(!m_aAnimation.empty())
			return GetMainAnimation()->GetAnimationColor();
		return 0;
	}
	BYTE GetAlpha(int iPart)
	{
		if((size_t)iPart < m_aAnimation.size()) return m_aAnimation[iPart]->GetAnimationAlpha();
		return 0;
	}
	
	DWORD GetColor(int iPart)
	{
		if((size_t)iPart < m_aAnimation.size()) return m_aAnimation[iPart]->GetAnimationColor();
		return 0;
	}

	void ChangeAlpha(int part,BYTE alpha)
	{
		if (part >= 0 && part < (int)m_aAnimation.size())
			m_aAnimation[part]->SetAnimationAlpha(alpha);
	}
	void ChangeAlpha(BYTE alpha)
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->SetAnimationAlpha(alpha);
	}

	void SetBlendType(stAnimationMagicDataStruct::BlendEnum eBlend)
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->SetBlendType(eBlend);
	}
	void SetBlendType(int part,stAnimationMagicDataStruct::BlendEnum eBlend)
	{
		if (part >= 0 && part < (int)m_aAnimation.size())
			m_aAnimation[part]->SetBlendType(eBlend);
	}
	// 指定包含的每个动作的当前方向的帧为nFrame
	void SetDirFrame(int nFrame)
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->SetDirFrame(nFrame);

	}
	stAnimationMagicDataStruct::BlendEnum GetBlendType()
	{
		if(m_aAnimation.size() && m_aAnimation[m_nMainIndex]) return m_aAnimation[m_nMainIndex]->GetBlendType();
		return stAnimationMagicDataStruct::Blend_Null;
	}

	stAnimationMagicDataStruct::BlendEnum GetBlendType(int iPart)
	{
		if( (size_t)iPart < m_aAnimation.size()) return m_aAnimation[iPart]->GetBlendType();
		return stAnimationMagicDataStruct::Blend_Null;
	}

	int GetFrameOfDir()
	{
		if (m_aAnimation.empty() || !m_aAnimation[m_nMainIndex])
			return 0;
		return (int)m_aAnimation[m_nMainIndex]->GetFrameOfDir();
	}
	
	int GetFrameOfDir(size_t nGroup)
	{
		if (m_aAnimation.empty() || !m_aAnimation[m_nMainIndex])
			return 0;
		return (int)m_aAnimation[m_nMainIndex]->GetFrameOfDir(nGroup);
	}

	int GetDirFrame()
	{
		if (m_aAnimation.empty() || !m_aAnimation[m_nMainIndex])
			return 0;
		return (int)m_aAnimation[m_nMainIndex]->GetDirFrame();
	}

	void CopyState(const cAnimation * pAni)
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
		{
			if( (cAnimation*)m_aAnimation[i] != pAni )
				((cAnimation*)m_aAnimation[i])->CopyState(pAni);
			//m_aAnimation[i]->ActionNext(time_eslasped);
		}
	}

	void Run(long time_eslasped)
	{
		if(m_aAnimation.size() <= (size_t)m_nMainIndex) return;

		m_aAnimation[m_nMainIndex]->ActionNext(time_eslasped);
		for (size_t i=0; i<m_aAnimation.size(); i++)
		{
			if(i != m_nMainIndex)
				((cAnimation*)m_aAnimation[i])->CopyState((cAnimation*)m_aAnimation[m_nMainIndex]);
			//m_aAnimation[i]->ActionNext(time_eslasped);
		}
	}

	void CopyState(const cAnimationArray * pAniArray)
	{
		if(pAniArray->m_aAnimation.size() > (size_t)pAniArray->m_nMainIndex)
		{
			CopyState((cAnimation*)pAniArray->GetMainAnimation());
		}
	}

	void DrawAtScreen(int ePart,POINT ptScreen,POINT ptCenter,stPointF scale = stPointF(1.0f,1.0f))
	{
		if (ePart < 0 || ePart >= (int)m_aAnimation.size())
			return;
		m_aAnimation[ePart]->Draw(ptScreen,ptCenter,scale);
	}
	void DrawAtScreen(POINT ptScreen,POINT ptCenter,stPointF scale = stPointF(1.0f,1.0f))
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->Draw(ptScreen,ptCenter,scale);
	}
	

	//缩骨
	void DrawAtScreen2(int ePart,POINT ptScreen,POINT ptCenter,stPointF scale = stPointF(0.5f,0.5f))
	{
		if (ePart < 0 || ePart >= (int)m_aAnimation.size())
			return;
		m_aAnimation[ePart]->Draw(ptScreen,ptCenter,scale);
	}
	void DrawAtScreen2(POINT ptScreen,POINT ptCenter,stPointF scale = stPointF(0.5f,0.5f))
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->Draw(ptScreen,ptCenter,scale);
	}

	bool Action(int nAni,const stAnimationStruct& st,bool bServerLogic = false)
	{
		if (m_aAnimation.empty())
			return false;
		bool bRet = false;
		for (size_t i=0; i<m_aAnimation.size(); i++)
		{
			if(i == m_nMainIndex)
				bRet = m_aAnimation[i]->Action(nAni,st,bServerLogic);
			else
				m_aAnimation[i]->Action(nAni,st,bServerLogic);
		}

		return bRet;
	}
	void TurnBack()
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->TurnBack();
	}
	void ChangeFrame(size_t iFrame)
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->ChangeFrame(iFrame);
	}	
	void ChangeFrameOfDir(size_t iFrame)
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->ChangeFrameOfDir(iFrame);
	}	
	void TurnTo(size_t n)
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->TurnTo(n);
	}
	void Turn(POINT pt)
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->Turn(pt);
	}
	size_t GetPartID(int part)
	{
		if (part >= 0 && part < (int)GetNum())
			return m_aAnimation[part]->GetAnimationID();
		return 0;
	}
	void ChangePartID(int part,int id)
	{
		if (part >= 0 && part < (int)GetNum())
			m_aAnimation[part]->SetAnimationID(id);
		if (m_aAnimation.size() > 1 && part > 0)
			m_aAnimation[part]->SetAnimationInfo(m_aAnimation[0]->GetAnimationInfo());
		UpdateAnimation(part);
	}
	void UpdateAnimation(int part)
	{
		if (part >= 0 && part < (int)GetNum())
			m_aAnimation[part]->UpdateAnimation();
	}
	void UpdateAnimation()
	{
		for (size_t i=0; i<m_aAnimation.size(); i++)
			m_aAnimation[i]->UpdateAnimation();
	}

	iAnimation* GetMainAnimation() const
	{
		if (m_aAnimation.empty())
			return NULL;
		return m_aAnimation[m_nMainIndex];
	}
	iAnimation* GetAnimation(size_t i)
	{
		if (i<m_aAnimation.size())
			return m_aAnimation[i];
		return NULL;
	}
	size_t GetDir()
	{
		if (GetMainAnimation())
			return GetMainAnimation()->GetDir();
		return 0;
	}
	int GetAnimationTime()
	{
		if (GetMainAnimation())
			return GetMainAnimation()->GetAnimationTime();
		return 0;
	}

//fade
	
	void Fade(const stFade& st) {m_stFade = st;};
	void Fade(long time_eslasped)
	{
		if (!m_stFade.IsRun())
			return;

		if (m_stFade.Loop(time_eslasped))
		{
			DWORD dwColor = GetMainAnimation()->GetAnimationColor();
			int alpha = COLOR_GET_ALPHA(dwColor);
			alpha += m_stFade.nAlpha;
			if (m_stFade.nAlpha > 0)
			{
				if (alpha < m_stFade.nStart)
					alpha = m_stFade.nStart;
				if (alpha > m_stFade.nEnd)
				{
					alpha = m_stFade.nEnd;
					m_stFade.Clear();
				}
			}
			else
			{
				if (alpha > m_stFade.nStart)
					alpha = m_stFade.nStart;
				if (alpha < m_stFade.nEnd)
				{
					alpha = m_stFade.nEnd;
					m_stFade.Clear();
				}
			}
			ChangerColor(COLOR_ACOLOR(alpha ,dwColor) );
			//ChangerColor(alpha);
		}
	}

//keepimage
	
	void StartKeepImage(const stKeepImageControl& st){m_stKeepImageControl = st;};

	void KeepImage(long time_eslasped,POINT ptPos,POINT ptCenter,BYTE* pOrder = NULL);
	void DrawKeepImage(POINT ptScreen,bool bOrder = false);
	void ClearKeepImage(){m_stKeepImageControl.Clear();}

	HRESULT InvalidateDeviceObjects() ;	//before call reset alt+tab 
	HRESULT RestoreDeviceObjects() ;		//after alt+tab

	bool GetWholeSize(int part,int& w,int& h)
	{
		if (part >= 0 && part < (int)m_aAnimation.size())
			return m_aAnimation[part]->GetWholeSize(w,h);
		return false;
	}

	bool GetImageSize(int part,int& w,int& h)
	{
		if (part >= 0 && part < (int)m_aAnimation.size())
			return m_aAnimation[part]->GetImageSize(w,h);
		return false;
	}

	bool GetClipRect(RECT* prect)
	{
		if(m_aAnimation.empty()) return false;
		return m_aAnimation[0]->GetClipRect(prect);
	}

	bool GetDrawRect(POINT pt,POINT ptCenter,RECT & rc)
	{
		stRectI & r = (stRectI&)rc;
		stRectI rc0;
		for(size_t i=0; i< m_aAnimation.size();++i)
		{
			if(m_aAnimation[i]->GetDrawRect(pt,ptCenter,rc0))
			{
				r |= rc0;
			}
		}
		return !r.IsRectEmpty();
	}
	bool IsExistAction(int n)
	{
		if(m_nMainIndex >= m_aAnimation.size()) return false;
		return m_aAnimation[m_nMainIndex]->IsExistAction(n);
	}
};

