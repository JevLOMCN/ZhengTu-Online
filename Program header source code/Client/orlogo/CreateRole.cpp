
#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GameState.h"
#include "./GuiLogin.h"
#include ".\createrole.h"
#include "./GameCursor.h"
#include "./Country.h"
#include "./CountryInfo.h"
#include "./GuiUserInfo.h"
#include ".\NetAutoconnect.h"

#include "../RenderD3D/include/RenderD3D.h"
#include "../RenderD3D/include/D3DDevice.h"
#include "../RenderD3D/include/TextureManager.h"
#include "../RenderD3D/include/CustomTexture.h"
#include "../Media/SoundManager.h"
#include "SafetyUser.h"
////////////////////////////////////////////////////////////////////////////////
#define _USE_SINGLE_CHARACTER_

#ifdef _DEBUG
//#define _DEBUG_DOUBLE_PEOPLE
#endif 
#ifdef _DEBUG
//#define _DEBUG_RANGE_
#endif 

#define _USE_NUM_PASSWORD_

#undef ___CREATEROLE___NO___LIMIT___
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
#define  ROLE_COLOR_UNSELECT  COLOR_ARGB(255,177,177,181)
#define  ROLE_COLOR_SELECT    COLOR_ARGB(255,255,255,255)
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
const char* c_szRoleGrpPack  = "data\\Login.gl";		// 图包
const char* c_szLogoGrpPack  = "data\\Logo.gl";
const float fMoveSpeedTime   = 0.02f;
const int   nMoveSpeed	     = 120;
const int   nMoveLen         = 1000;
const int   nRoleAniSpeed    = 120;
const int   nRoleBackAniSpeed = 240;
/****/DWORD s_role_net_time   = 0;
const int   c_nDlgAniDlgUpHeight = 600;		 // 创建角色对话框向上滚动的高度
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////// 
// 所有 动画位置微调 和调试框
class CAniRoleRectAndPos
{
	static   stRectI m_rcRole1;
	static   stRectI m_rcRole2;
	static   stRectI m_rcSingleRole;
public:
	static   void InitRoleRectAndPos();
	static   BOOL PtInRectRole1(POINT ptMouse);
	static   BOOL PtInRectRole2(POINT ptMouse);
	static   void DebugRenderRectange();
	static   void AdjustRole1RenderX(LONG & iRenderX);
	static   stPointI GetRoleCreateHeadPos();
	static   stRectI  GetDemoShowRect();
	static   void AdjustDemoRoleRenderPos(int &x,int &y);
	static   RECT GetAniRoleDescRect();
};
stRectI CAniRoleRectAndPos::m_rcRole1 = stRectI(0,0,0,0);
stRectI CAniRoleRectAndPos::m_rcRole2 = stRectI(0,0,0,0);
stRectI CAniRoleRectAndPos::m_rcSingleRole = stRectI(0,0,0,0);

void  CAniRoleRectAndPos::InitRoleRectAndPos()
{
	RECT  rectRole1        = {147,249,316,578};
	RECT  rectRole2        = {643,263,816,572};

	m_rcRole1.left   = rectRole1.left   * CPixelContext::GetRenderScale().x;
	m_rcRole1.right  = rectRole1.right  * CPixelContext::GetRenderScale().x;
	m_rcRole1.top    = rectRole1.top    * CPixelContext::GetRenderScale().y;
	m_rcRole1.bottom = rectRole1.bottom * CPixelContext::GetRenderScale().y;

	m_rcRole2.left   = rectRole2.left   * CPixelContext::GetRenderScale().x;
	m_rcRole2.right  = rectRole2.right  * CPixelContext::GetRenderScale().x;
	m_rcRole2.top    = rectRole2.top    * CPixelContext::GetRenderScale().y;
	m_rcRole2.bottom = rectRole2.bottom * CPixelContext::GetRenderScale().y;

	m_rcSingleRole = m_rcRole1;
	m_rcSingleRole.OffsetRect(250,0);

}

RECT    CAniRoleRectAndPos::GetAniRoleDescRect()
{
	
	if(CPixelContext::IsLogin800By600())
	{
	         RECT rc;
		 rc.left = 267;
		 rc.right = 787;
		rc.top = 77;
		rc.bottom = 545;
		 return rc;
	}
	else 
	{
		RECT rect;
		rect.left = 281;
		rect.top = 77;
		rect.right = 1022;
		rect.bottom = 692;
		return rect;		
	}
}

stRectI  CAniRoleRectAndPos::GetDemoShowRect()
{
	if(CPixelContext::IsLogin800By600())
	{
		return stRectI(47,213,215,355);
	}
	else
	{
		return stRectI(61,261,232,443);
	}
}

void CAniRoleRectAndPos::AdjustDemoRoleRenderPos(int & x,int & y)
{
	if( CPixelContext::IsLogin800By600())
	{
		x -=126;
		y += 40;
	}
	else
	{
		x -=112;
		y +=100;
	}
}

stPointI CAniRoleRectAndPos::GetRoleCreateHeadPos()
{
	if( CPixelContext::IsLogin800By600())
	{
		return stPointI(101,98);
	}
	else 
		return stPointI(113,131);
}

void CAniRoleRectAndPos::AdjustRole1RenderX(LONG & iRenderX)
{
#ifdef _USE_SINGLE_CHARACTER_
	if( GetCreateRoleManager()->m_bSingle )
	{
		if( CPixelContext::IsLogin800By600())
			iRenderX -= 150;
		else
			iRenderX -= 250;
	}
	else
#endif 
		iRenderX -=500*CPixelContext::GetRenderScale().x;
}

BOOL CAniRoleRectAndPos::PtInRectRole1(POINT ptMouse)
{
#ifdef _USE_SINGLE_CHARACTER_
	if( GetCreateRoleManager()->m_bSingle)
		return PtInRect(&m_rcSingleRole,ptMouse);
#endif 
	return PtInRect(&m_rcRole1,ptMouse);
}

BOOL CAniRoleRectAndPos::PtInRectRole2(POINT ptMouse)
{
#ifdef _USE_SINGLE_CHARACTER_
	if( GetCreateRoleManager()->m_bSingle)
		return PtInRect(&m_rcSingleRole,ptMouse);
#endif 
	return PtInRect(&m_rcRole2,ptMouse);
}

void CAniRoleRectAndPos::DebugRenderRectange()
{
#ifdef _DEBUG
	#ifdef _DEBUG_RANGE_
		#ifdef _USE_SINGLE_CHARACTER_
			if( GetCreateRoleManager()->m_bSingle)
				GetDevice()->DrawRectangle(&m_rcSingleRole,COLOR_ARGB(255,126,248,61)); 
		#endif 
		if(!GetCreateRoleManager()->m_bSingle)
		{
			GetDevice()->DrawRectangle(&m_rcRole1,COLOR_ARGB(255,126,248,61));
			GetDevice()->DrawRectangle(&m_rcRole2,COLOR_ARGB(255,126,248,61));
		}            
		
	#endif 
#endif 
}
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
inline void stRole::init()
{
	FUNCTION_BEGIN;

	name[0]=0;                          // 角色名称
	type=PROFESSION_NONE;               // 职业类型
	oldtype = PROFESSION_NONE;
	conntryId=255;                      // 国家ID

	DstAniState = eAni_State1;
	SrcAniState = eAni_State1;
	head  =  -1;
	state =  eRoleState_NULL;
	dwLightColor = ROLE_COLOR_SELECT;
	//five=FIVE_NONE;

	FUNCTION_END;
}

/////////////////////////////////////////////////////////////////////////////////
// 对话框动画
////////////////////////////////////////////////////////////////////////////////
class CAniDlgManager
{
public:
	stGuiAnimationInfo stIn;
	stGuiAnimationInfo stOut;

	struct AnimationParam
	{ 
		enum SWITCH_WAY
		{
			WAY_NULL=0,
			LEFT_TO_RIGHT,
			TOP_TO_BOTTOM,
			BOTTOM_TO_TOP,
			RIGHT_TO_LEFT,
			YINGHUA
		}way;
		CGuiDialog * m_pOutDlg;
		CGuiDialog * m_pInDlg;
		bool m_bMoving;
		enumGuiAnimationEvent OutAniEvent;
		enumGuiAnimationEvent InAniEvent;
		AnimationParam() : way(WAY_NULL),m_pOutDlg(NULL),m_pInDlg(NULL) ,m_bMoving(false)
		{
		}
		void clear()
		{
			m_pOutDlg = NULL;
			m_pInDlg = NULL;
			m_bMoving = false;
		}
	};
	AnimationParam  param;

	CAniDlgManager()
	{
	}

	void SetMoveFlag(bool bMoving){param.m_bMoving = bMoving;};

	bool GetMoveFlag(){return param.m_bMoving;};

	void SwtichFromDlg1ToDlg2()
	{
		FUNCTION_BEGIN;
		assert(param.way != AnimationParam::WAY_NULL);
		if( AnimationParam::WAY_NULL == param.way) return;

		const float fMoveTime = 0.8f;

		stRectI rcOut;
		if(param.way == AnimationParam::LEFT_TO_RIGHT)
		{ 
			stOut.ptDst.x = 314;
			stOut.ptDst.y = 0;
			param.m_pInDlg->SetLocation(-314,0);
			stIn.ptDst.x = 0;
			stIn.ptDst.y = 0;
		}
		if(param.way == AnimationParam::RIGHT_TO_LEFT)
		{
			stOut.ptDst.x = -314;
			stOut.ptDst.y = 0;
			param.m_pInDlg->SetLocation(314,0);
			stIn.ptDst.x = 0;
			stIn.ptDst.y = 0;

		}
		if(param.way == AnimationParam::YINGHUA)
		{
			/*GetLogoAni()->m_AniMenHorse.SetFrame(0);
			GetLogoAni()->m_AniMenHorse.SetStop(false);*/
		}
		stIn.fTime = fMoveTime;
		stOut.fTime = fMoveTime;
		param.m_pInDlg->SetVisible(true);
		param.m_pInDlg->SetEnabled(true);
		param.m_pOutDlg->SetVisible(true);
		param.m_pOutDlg->SetEnabled(true);
		stIn.dwColorDst = 0xffffffff;
		stOut.dwColorDst= 0x00ffffff;
		stOut.eAniComplete = param.OutAniEvent; 
		stIn.eAniComplete = param.InAniEvent;

		FUNCTION_END;
	}
	void SwitchFromLoginToRoleSel()
	{
		FUNCTION_BEGIN;
		param.clear();
		if(GetGameGuiManager()->m_guiLogin)
		{ 
			param.m_pOutDlg = GetGameGuiManager()->m_guiLogin;
			param.m_pInDlg  = GetCreateRoleManager()->m_guiRoleSel;

			param.way = AnimationParam::YINGHUA;//樱花切换场景动态效果
			param.InAniEvent = GuiAniEvent_RunFun;
			param.OutAniEvent = GuiAniEvent_Hide;
			SwtichFromDlg1ToDlg2();
			GetGameGuiManager()->m_guiLogin->SetEnabled(false);
			SetMoveFlag(true);
		}
		FUNCTION_END;
	}
	void SwitchFromRoleSelToLogin()
	{
		FUNCTION_BEGIN;
		param.clear();
		if(GetCreateRoleManager()->m_guiRoleSel )
		{
			param.m_pOutDlg = GetCreateRoleManager()->m_guiRoleSel;
			param.m_pInDlg = GetGameGuiManager()->m_guiLogin;
			param.way = AnimationParam::YINGHUA;
			param.InAniEvent = GuiAniEvent_RunFun;
			SwtichFromDlg1ToDlg2();
			SetMoveFlag(true);
		}
		FUNCTION_END;
	}

	void PopupWindowFromTop(CGuiDialog * pDlg)
	{
		FUNCTION_BEGIN;
		param.clear();
		assert(pDlg!=NULL);
		if( NULL == pDlg ) return;

		POINT pt = pDlg->GetLocation();
		stIn.dwColorDst = 0xffffffff;
		stIn.eAniComplete = GuiAniEvent_None;
		stIn.fTime = 1.0f;
		stIn.ptDst.x =  pt.x;
		stIn.ptDst.y =  0;

		pDlg->SetVisible(true);
		pDlg->SetEnabled(true);
		pDlg->BeginAnimation(&stIn);

		FUNCTION_END;

	}

	void PopoutWindowFromTop(CGuiDialog *pDlg)
	{
		FUNCTION_BEGIN;
		param.clear();
		assert(pDlg!=NULL);
		if( NULL == pDlg) return;

		POINT pt = pDlg->GetLocation();
		stOut.dwColorDst = 0xffffffff;
		stOut.fTime = 1.0f;
		stOut.ptDst.x = pt.x;
		stOut.ptDst.y = -c_nDlgAniDlgUpHeight;
		stOut.eAniComplete = GuiAniEvent_RunFun;  
		pDlg->BeginAnimation(&stOut);

		FUNCTION_END;
	}

	void ExecuteAni()
	{
		FUNCTION_BEGIN;
		if( param.m_pOutDlg)
			param.m_pOutDlg->BeginAnimation(&stOut);
		if( param.m_pInDlg)
			param.m_pInDlg->BeginAnimation(&stIn);

		FUNCTION_END;
	}	
};

CAniDlgManager * GetAniManager()
{
	FUNCTION_BEGIN;
	static CAniDlgManager AniManager;
	return &AniManager;

	FUNCTION_END;
}
/////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Demo 动画
////////////////////////////////////////////////////////////////////////////////
CGameRoleDemo::CGameRoleDemo()
: bInited(false)
, bUpdateFrame(false)
,bRenderBody(true)
,bRenderHair(true)
,pDemoBk(NULL)
{
	FUNCTION_BEGIN;
	rcShow = CAniRoleRectAndPos::GetDemoShowRect();
	FUNCTION_END;
}

void CGameRoleDemo::init()
{
	FUNCTION_BEGIN;

	bInited = true;

	////////////////////////////////////////////////////
	stResourceLocation rl;
	rl.SetFileName("data\\login.gl");
	rl.group = 3;
	rl.frame = 14;

	pDemoBk  = GetDevice()->FindBitmaps(&rl);

	IBitmap * pBitmap = pDemoBk->GetBitmap(0);
	nBmpWidth = pBitmap->GetWidth();
	nBmpHeight = pBitmap->GetHeight();

	rcClip = pBitmap->GetClipRect();
	/////////////////////////////////////////////////////
	fTotalTime = 20; // 20 seconds
	fRunTime = 0;
	fGrassSpeed = 100;

	nMaxDir = 8;
	nCurDir  = 4;

	// world pos 
	x = nBmpWidth;
	y = nBmpHeight;

	nCurDirFrame = 0;

	FUNCTION_END;
}
void CGameRoleDemo::RenderBk(POINT ptDlg)
{
	FUNCTION_BEGIN;
	if( bInited )
	{
		RenderBlock(ptDlg);
	}

	FUNCTION_END;
}


void CGameRoleDemo::ChangeCharacter(const stResourceLocation & rlBody,const  stResourceLocation & rlHair,bool bDrawBody,bool bDrawHair)
{
	FUNCTION_BEGIN;

	if( !bInited )	return;


#ifdef _DEBUG
	DWORD dwLastTime = xtimeGetTime();
#endif 
	if( !m_DemoBody.Create(&rlBody,false) ) return;// body must be created ok,but hair not necessary       
	m_DemoBody.SetLoopPlay(true);		
	int nDirFrame = m_DemoBody.GetFrameCount() / nMaxDir;
	nCurDir = 4;

	bool bCreateHairOk = m_DemoHair.Create(&rlHair,false);
	if( bCreateHairOk )
	{
		m_DemoHair.SetLoopPlay(true);
		m_DemoHair.SetFrame(nCurDir * nDirFrame);
	}

	m_DemoBody.SetFrame(nCurDir * nDirFrame);
	nFramePerDir = m_DemoBody.GetFrameCount() / nMaxDir;

	assert(nFramePerDir>0);
	if(nFramePerDir<= 0)  return;

	fCharacterSpeed = 640 / nFramePerDir;  // 640 ms per frame
	m_DemoBody.SetSpeed(fCharacterSpeed);
	if(bCreateHairOk) 
		m_DemoHair.SetSpeed(fCharacterSpeed);
	else 
		m_DemoHair.SetSpeed(0xffffffff);

	bRenderBody = bDrawBody;
	bRenderHair = bDrawHair;

#ifdef _DEBUG
	DWORD diff = xtimeGetTime() - dwLastTime;
	TRACE("changecharacter:花费时间:%d\n",diff);
#endif 
	FUNCTION_END;
}

void CGameRoleDemo::ChangeHair(const stResourceLocation & rlHair,bool bDrawHair)
{
	FUNCTION_BEGIN;

	if( !bInited )	return;

	bRenderHair = bDrawHair;

	if(!m_DemoHair.Create(&rlHair,false)) return; 
	
	m_DemoHair.SetLoopPlay(true);
	m_DemoHair.SetSpeed(fCharacterSpeed);

	float fCurFrame = m_DemoBody.GetFrame();
	m_DemoHair.SetFrame(fCurFrame);

	

	FUNCTION_END;
}


void CGameRoleDemo::Update(float fElaspedTime)
{
	FUNCTION_BEGIN;
	if( !bInited ) return;

	// if( (m_DemoHair.GetFrame() + fElaspedTime * m_DemoHair.GetSpeed()) /  nFramePerDir  != nCurDir )
	float fNextFrame = m_DemoBody.GetFrame() + fElaspedTime *  m_DemoBody.GetSpeed() ;
	if(  (int)fNextFrame  >= (nCurDir+1) * nFramePerDir )	    
	{
		bUpdateFrame = true;
		m_DemoHair.SetFrame(nCurDir * nFramePerDir);
		m_DemoBody.SetFrame(nCurDir * nFramePerDir);
	}
	else
		bUpdateFrame = true;

	const POINT c_ptDir[8] = {
		{0,-1},
		{1,-1},
		{1,0},
		{1,1},

		{0,1},
		{-1,1},
		{-1,0},
		{-1,-1},
	};
	float f_walk_x = fGrassSpeed * fElaspedTime * c_ptDir[nCurDir].x; 
	float f_walk_y = fGrassSpeed * fElaspedTime * c_ptDir[nCurDir].y / 2 ;
	
	x +=    f_walk_x; 
	y +=    f_walk_y;

	FUNCTION_END;
}

void CGameRoleDemo::RenderBlock(POINT ptDlg)
{
	FUNCTION_BEGIN;

	// 显示区域宽高
	static const int width = rcShow.Width(),height = rcShow.Height();
	static const int blockWidth = nBmpWidth,blockHeight = nBmpHeight; // 地图块的宽高
	// 假设人物在正中央
#ifdef _DEBUG
	if( (nBmpWidth - height/2) == y)
		int iii =0;
#endif 
#ifdef __TRACE__
	TRACE("x ,y=%f,%f @@@@@@",x,y);
	TRACE("x ,y=%f,%f @@@@@@",x,y);
	TRACE("x ,y=%f,%f @@@@@@",x,y);
#endif 
	// limit in the world pos in 2*2 size in number 
	// sometimes may occus stable because of the filter of the translate from float to int
	//x = ((int)(x -width / 2))% nBmpWidth + width / 2;
	//y = ((int)(y -height/ 2))% nBmpHeight + height / 2;

	if ((y - height/2) < 0)
		y += nBmpHeight;
	if ((y + height /2 ) > 2 * nBmpHeight)
		y -= nBmpHeight;
	if ((x - width/2) < 0 )
		x += nBmpWidth;
	if ((x + width/2) > 2 * nBmpWidth)
		x -= nBmpWidth;

	/*x = ((int)(x -width / 2))% nBmpWidth + width / 2;
	y = ((int)(y -height/ 2))% nBmpHeight + height / 2;*/

	if(( x - width /2 ) > nBmpWidth)
	{
		x  -= nBmpWidth;
	}

	if(( x - width /2 ) < 0 )
	{
		x  += nBmpWidth;
	}

	if ( (y - height/2) < 0 )
	{
		y += nBmpHeight;
	}

	if ( (y - height/2) > nBmpHeight )
	{
		y -= nBmpHeight;
	}

	//const int nOffsetMapX = nBmpWidth - width/2;
	//const int nOffsetMapY = nBmpHeight - height/2;
	int startBlockX = ((int)((x - width / 2) /blockWidth)) % 2;
	int startBlockY = ((int)((y-height/2 ) /blockHeight)) %2;
	int endBlockX   = ((int)((x + width/2)/blockWidth))%2;
	int endBlockY   = ((int)((y + height/2)/blockHeight))%2;


	int offsetX,offsetY;
	//offsetX =   -( (int)( x - width / 2)%blockWidth  );
	//offsetY =   -((int)(y-height / 2) % blockHeight  );
	offsetX =    (int)( x - width / 2)%blockWidth  ;
	offsetY =   (int)(y-height / 2) % blockHeight  ;



	for(int i = startBlockY; i<=endBlockY;++i)
	{
		for(int j=startBlockX;j<=endBlockX;++j)
		{
			int xx = offsetX ;//+ j * blockWidth;
			int yy = offsetY ;//+ i * blockHeight;

			//int xx = offsetX + (j-startBlockX) * blockWidth;
			//int yy = offsetY + (i-startBlockY) * blockHeight;

			static const int unused = 1;   // just to make a stRectI to get the benefit of OffsetRect
			stRectI pt(-xx,-yy,-xx+unused,-yy+unused);  

			//////////////////////////////////////////////////
			//stRectI pt(xx,yy,xx+unused,yy+unused);  


			stPointI ptClipBoxOffset(rcShow.left,rcShow.top);
			POINT ptOffset;
			ptOffset.x = ptDlg.x + ptClipBoxOffset.x;
			ptOffset.y = ptDlg.y + ptClipBoxOffset.y;

			pt.OffsetRect(ptOffset);

			///////////////////////////////////////////////////
			stPointI ptOffset2;
			ptOffset2.x = j  * nBmpWidth ;
			ptOffset2.y = i * nBmpHeight;

			pt.OffsetRect(ptOffset2);

			DrawBlock(pt.left,pt.top,i*2+j,ptDlg);	
		}
	}
	FUNCTION_END;

}
void   CGameRoleDemo::DrawBlock(int x,int y ,int index,POINT ptDlg)
{
	FUNCTION_BEGIN;

	assert( index >=0 && index <=3);

#if __RENDER__DEBUG
        pDemoBk->RenderAni(x,y,timeGetTime(),NULL,NULL,-1,Blend_Null);
#endif

	static stRectI rcIndex0,rcIndex1,rcIndex2;

	stPointI ptOffset(x,y);
	stRectI rcOkClipRect = rcClip;
	rcOkClipRect.OffsetRect(ptOffset);



	/////////////////////////////////////////////////////
	stRectI rcShow2 = rcShow;
	rcShow2.OffsetRect(ptDlg);
	/////////////////////////////////////////////////////


	rcOkClipRect &= rcShow2;
	stPointI ptOffset1(-x,-y);
	rcOkClipRect.OffsetRect(ptOffset1);

	if (index == 0)
	{
		rcIndex0.left = rcShow2.left;
		rcIndex0.top = rcShow2.top;
		rcIndex0.right = rcIndex0.left + rcOkClipRect.Width();
		rcIndex0.bottom = rcIndex0.top + rcOkClipRect.Height();
		//TRACE("rcIndex0: %d,%d\n",rcIndex0.left,rcIndex0.top);
		pDemoBk->RenderAni(rcIndex0.left , rcIndex0.top, NULL,&rcOkClipRect,0,-1,Blend_Null);
	}
	else if (index == 1 )
	{
		rcIndex1.left = rcIndex0.right;
		rcIndex1.top = rcShow2.top;
		rcIndex1.right = rcIndex1.left + rcOkClipRect.Width();
		rcIndex1.bottom = rcIndex1.top + rcOkClipRect.Height();
		//TRACE("rcIndex1: %d,%d\n",rcIndex1.left,rcIndex1.top);
		pDemoBk->RenderAni(rcIndex1.left , rcIndex1.top, NULL,&rcOkClipRect,0,-1,Blend_Null);

	}
	else if (index == 2 )
	{
		rcIndex2.left = rcShow2.left;
		rcIndex2.top = rcIndex0.bottom;
		rcIndex2.right = rcIndex2.left + rcOkClipRect.Width();
		rcIndex2.bottom = rcIndex2.top + rcOkClipRect.Height();
		//	TRACE("rcIndex2: %d,%d\n",rcIndex2.left,rcIndex2.top);
		pDemoBk->RenderAni(rcIndex2.left , rcIndex2.top, NULL,&rcOkClipRect,0,-1,Blend_Null);

	}
	else if (index == 3 )
	{
		//TRACE("fsdafffffffffffffffffffffff\n");
		pDemoBk->RenderAni(rcIndex2.right , rcIndex1.bottom, NULL,&rcOkClipRect,0,-1,Blend_Null);
	}	

	FUNCTION_END;
}

void CGameRoleDemo::UpdateFrameAsTime()
{
	FUNCTION_BEGIN;

	nCurDir --;
	if( nCurDir < 0 )
		nCurDir += nMaxDir;

	m_DemoHair.SetFrame(nCurDir * nFramePerDir);
	m_DemoBody.SetFrame(nCurDir * nFramePerDir);

	FUNCTION_END;
}

void CGameRoleDemo::UpdateFrameAsReverseTime()
{
	FUNCTION_BEGIN;

	nCurDir ++;
	if ( nCurDir >= nMaxDir)
		nCurDir -= nMaxDir;
	
	m_DemoHair.SetFrame(nCurDir * nFramePerDir);
	m_DemoBody.SetFrame(nCurDir * nFramePerDir);

	FUNCTION_END;
}


void CGameRoleDemo::RenderCharacter(int x,int y ,DWORD clrHair)
{
	FUNCTION_BEGIN;

	if( !bInited ) return;

	CAniRoleRectAndPos::AdjustDemoRoleRenderPos(x,y);

	if(bRenderBody)
		m_DemoBody.Render(x,y ,bUpdateFrame);
	if(bRenderHair)
		m_DemoHair.Render(x,y ,clrHair,bUpdateFrame);

	FUNCTION_END;
}	

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void ClearResEntryGame()
{
	FUNCTION_BEGIN;

	GetCreateRoleManager()->Release();
	GetDevice()->DeleteResource(c_szRoleGrpPack);
	GetDevice()->DeleteResource(c_szLogoGrpPack);
	if(GetGameGuiManager()->m_guiLogin)
	{
		GetGameGuiManager()->m_guiLogin->Close();
		GetGameGuiManager()->m_guiLogin = NULL;
	}
	FUNCTION_END;
}


void LoadLoginRes()
{
	FUNCTION_BEGIN;

	if( GetDevice()->FindResource("data\\my.pak",IBitmapList::flag_AlwaysOpen | IBitmapList::flag_LoadByGroup| IBitmapList::flag_NoRelease | IBitmapList::flag_MapView ,Usage_Interface) == NULL )
	{
		Engine_WriteLogF("Load Resource field! %s\n",c_szLogoGrpPack);
	}

	if( GetDevice()->FindResource("data\\logo.gl",IBitmapList::flag_AlwaysOpen | IBitmapList::flag_LoadByGroup| IBitmapList::flag_NoRelease | IBitmapList::flag_MapView ,Usage_Interface) == NULL )
	{
		Engine_WriteLogF("Load Resource field! %s\n",c_szLogoGrpPack);
	}

	if( GetDevice()->FindResource(c_szRoleGrpPack,IBitmapList::flag_AlwaysOpen | IBitmapList::flag_LoadByGroup | IBitmapList::flag_NoRelease | IBitmapList::flag_MapView ,Usage_Interface) == NULL )
	{
		Engine_WriteLogF("Load Resource field! %s\n",c_szRoleGrpPack);
	}
	if( GetDevice()->FindResource("data\\body.gl",c_nMagicResourceFlags ,Usage_Character) == NULL )
	{
		Engine_WriteLogF("Load Resource field! data\\body.gl \n");
	}
	if( GetDevice()->FindResource("data\\hair.gl",c_nMagicResourceFlags ,Usage_Character) == NULL )
	{
		Engine_WriteLogF("Load Resource field! data\\hair.gl\n");
	}
	
	FUNCTION_END;
}

void ExitGameQuery()
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->AddMessageBox("确定要关闭游戏吗？",eAction_ExitGame,NULL,c_szMBOKCancel);

	FUNCTION_END;
}


////////////////////////////////////////////////////////////////////////////////
//  从游戏退到登陆画面或退到选择人物界面是要调用
////////////////////////////////////////////////////////////////////////////////
void ResetLoginDevice()
{
	FUNCTION_BEGIN;

	CPixelContext::ClearFirstFlag();
	GetDevice()->SetRenderScene(false);

	int nWidth = GetDevice()->GetWidth();
	int nHeight = GetDevice()->GetHeight();
	if(nWidth >= 1024 || nHeight >= 768)
	{
		if( nWidth == 1024 &&  nHeight== 768)
		{}
		else 
			GetDevice()->Reset(1024,768,GetDevice()->GetPixelBits(),GetDevice()->GetFlags());
	}
	else 
	{
		if(nWidth == 800 && nHeight == 600)
		{}
		else 
			GetDevice()->Reset(800,600,GetDevice()->GetPixelBits(),GetDevice()->GetFlags());
	}

	FUNCTION_END;
}

void ReturntoLogin( )
{
	FUNCTION_BEGIN;
	if( GetGameGuiManager()->m_guiLogin)
	{
		// 如果创建角色的时候断线
		if( GetCreateRoleManager()->m_guiRoleCreate && GetCreateRoleManager()->m_guiRoleCreate->IsVisible())
		{
			GetCreateRoleManager()->m_guiRoleCreate->SetVisible(false);
			GetCreateRoleManager()->m_guiRoleCreate->SetEnabled(false);
		}
		GetGameCursor()->SetCursor(CCursor::CursorType_Normal,eCursorNormal);
		GetGameGuiManager()->m_guiLogin->SetVisible(true);
		GetGameGuiManager()->m_guiLogin->SetEnabled(true);
		GetGameGuiManager()->m_guiLogin->ClearPassText();
		enumGameState state = GetGameState()->GetState();
		if ( !GetGameGuiManager()->m_guiLogin->IsLogining()  )
		{
			GetAniManager()->SwitchFromRoleSelToLogin();
			GetGameGuiManager()->m_guiLogin->SetLogining(TRUE);
			GetAniManager()->ExecuteAni();
		}
		
	}

	FUNCTION_END;
}

WORD GetTypeFromHead(WORD head)
{
	FUNCTION_BEGIN;

	if( head == (WORD)-1)
	{
		return PROFESSION_NONE;
	}
	return (head%2)==0?PROFESSION_2:PROFESSION_1;  

	FUNCTION_END;
}

////////////////////////////////////////////////////////////////////////////////
//  服务器必须等3秒才能再接消息
////////////////////////////////////////////////////////////////////////////////
void WaitToCanSendMsg()
{
	FUNCTION_BEGIN;

	int nTime = 3000 - (int)(xtimeGetTime() - s_role_net_time);
	if(nTime > 0) 
	{
		//TRACE("****************WaitCanSendCommand time=%d***************\n",nTime);
		Sleep(nTime);
	}

	FUNCTION_END;
}

void CreateRole(SelectUserInfo * info)
{
	FUNCTION_BEGIN;

        if(GetGameGuiManager()->m_guiLogin)
		GetGameGuiManager()->m_guiLogin->SetLogining(FALSE);

	s_role_net_time = xtimeGetTime();

	CCountry::Instance()->Reset();
	CCountryConn::Instance()->ResetQuest();
	LoadLoginRes();
	CSafetyUser::Instance()->Reset();

	///////////////////////////////////////////////////////////////////////////////////
	if( GetCreateRoleManager()->m_bInCreating 
		&& GetCreateRoleManager()->m_guiRoleCreate 
		&& GetCreateRoleManager()->m_guiRoleCreate->IsVisible())
	       GetCreateRoleManager()->m_guiRoleCreate->SetVisible(false); // PopoutWindow() sometimes error
	///////////////////////////////////////////////////////////////////////////////////

	bool bReLoad = true;
	if( GetCreateRoleManager()->m_guiRoleSel)
	{
		POINT pt = GetCreateRoleManager()->m_guiRoleSel->GetLocation();
		if(GetCreateRoleManager()->m_guiRoleSel->IsVisible() &&  pt.x ==0 && pt.y == 0)
		{
			bReLoad = false;  // m_guiRoleSel is the focus windows, it don't come from login,so no need to do sth 
		}
	}

	if( bReLoad )
		GetCreateRoleManager()->init();	    // prepare the gui dialogs 

	GetCreateRoleManager()->m_eSelRoleID  = eRoleSel_NULL;
	GetCreateRoleManager()->GetUserInfo(info);  // userinfo(how many roles have i created) 

	if( bReLoad)
	{
		GetAniManager()->SwitchFromLoginToRoleSel();
		GetAniManager()->ExecuteAni();
	}

	GetCreateRoleManager()->m_guiRoleSel->SetEnabled(true);
	GetCreateRoleManager()->m_guiRoleCreate->SetEnabled(true);

	GetCreateRoleManager()->m_guiRoleSel->ShowCreateAndDeleteBtn(true);

	GetCreateRoleManager()->m_eCreateingRoleID = eRoleSel_NULL;
	GetCreateRoleManager()->m_bInCreating = false; 
	GetCreateRoleManager()->m_guiRoleSel->ReLoad();
	GetCreateRoleManager()->m_guiRoleSel->CreatePPSound();

	/*if(GetTextEffect()->m_pControl)
	{
	     GetTextEffect()->m_pControl->SetVisible(false);
	}	*/

	GetCreateRoleManager()->m_guiRoleCreate->ReleaseAniDesc();

#ifdef ___AUTO___NET___CONNECT___
	for(int i = 0 ; i < __MAX_CHARINFO__ ; ++i)
	{
		const stRole & role = GetCreateRoleManager()->m_role[i];
		if( !CNetAutoConnect::Instance()->m_strAccount.empty() && strcmp(role.name,CNetAutoConnect::Instance()->m_strAccount.c_str()) == 0)
		{
			CNetAutoConnect::Instance()->AutoEnterGS(i);
			break;
		}
	}	
#endif 

	FUNCTION_END;
}


////////////////////////////////////////////////////////////////////////////////
//  角色创建管理器
////////////////////////////////////////////////////////////////////////////////
CCreateRoleManager::CCreateRoleManager(void)
: m_guiRoleCreate(NULL)
, m_guiRoleSel(NULL)
, m_eSelRoleID(eRoleSel_NULL)
, m_eCreateingRoleID(eRoleSel_NULL)
, m_bInCreating(false)
, m_guiDelRole(NULL)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}


CCreateRoleManager::~CCreateRoleManager(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

void  CCreateRoleManager::init()
{
	FUNCTION_BEGIN;

	Release();
	AddRoleCreate();
	AddRoleSel();

	FUNCTION_END;
}

void  CCreateRoleManager::Release()
{
	FUNCTION_BEGIN;

	if (m_guiRoleCreate)
	{
		m_guiRoleCreate->Close();
		m_guiRoleCreate=NULL;
	}

	if (m_guiRoleSel)
	{
		m_guiRoleSel->Close();
		m_guiRoleSel=NULL;
	}
        if( m_guiDelRole )
	{
		m_guiDelRole->Close();
		m_guiDelRole = NULL;
	}
	memset(m_role,0,sizeof(m_role));
	InitRole();
	m_DemoRole.init();

	FUNCTION_END;
}

void CCreateRoleManager::InitRole()
{
	FUNCTION_BEGIN;

	for(int i = 0 ; i < __MAX_CHARINFO__; i++)
	{
		m_role[i].init();
	}

	m_bSingle = false;

	FUNCTION_END;
}



CGuiRoleCreateDialog* CCreateRoleManager::AddRoleCreate()
{
	FUNCTION_BEGIN;

	if(!m_guiRoleCreate)
	{
		CGuiRoleCreateDialog * pDlg = new CGuiRoleCreateDialog;
		HRESULT hr;
		if(CPixelContext::IsLogin800By600())
		{
			hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\RoleCreateDialog800.xml",pDlg);
		}
		else
		{
			hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\RoleCreateDialog.xml",pDlg);
		}
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiRoleCreate = pDlg;
	}
	m_guiRoleCreate->SetVisible(false);
	m_guiRoleCreate->SetEnabled(false);

	return m_guiRoleCreate;

	FUNCTION_END;
}

CGuiDelRoleDialog* CCreateRoleManager::AddDelRole()
{
	FUNCTION_BEGIN;

	if(!m_guiDelRole)
	{
		CGuiDelRoleDialog * pDlg = new CGuiDelRoleDialog;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\RoldDelDialog.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiDelRole = pDlg;
	}
	m_guiDelRole->SetVisible(true);
	m_guiDelRole->SetEnabled(true);

	return m_guiDelRole;

	FUNCTION_END;
}

CGuiRoleSelDialog * CCreateRoleManager::AddRoleSel()
{
	FUNCTION_BEGIN;

	if(!m_guiRoleSel)
	{
		CGuiRoleSelDialog* pDlg = new CGuiRoleSelDialog;
		HRESULT hr;
		if(CPixelContext::IsLogin800By600())
			hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\RoleSelDialog800.xml",pDlg);
		else
		        hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\RoleSelDialog.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiRoleSel = pDlg;
	}
	m_guiRoleSel->SetVisible(false);
	m_guiRoleSel->SetEnabled(false);
	return m_guiRoleSel;

	FUNCTION_END;
}


enumRoleSel      CCreateRoleManager::FindRoleByName(const char *pName)
{
	FUNCTION_BEGIN;
	
	for(int i = 0 ; i < __MAX_CHARINFO__;i++ )
	{
		stRole & role = m_role[i];
		if(role.state == stRole::eRoleState_DONE)
		{
			if( 0 == strcmp(role.name,pName))
			{
				return enumRoleSel(i);
			}
		}
	}
	 
	return eRoleSel_NULL;

	FUNCTION_END;
}

////////////////////////////////////////////////////////////////////////////////
//  新建角色名合法后发送创建角色信息
////////////////////////////////////////////////////////////////////////////////
void  CCreateRoleManager::SendCreateUserCommand()
{
	FUNCTION_BEGIN;
	
	if( !GetCreateingRole() )  return ;

	stCreateSelectUserCmd cmd;
	strncpy(cmd.strUserName,GetCreateingRole()->name,sizeof(cmd.strUserName));
	//cmd.byHairType = rand() & 8;
	//cmd.byHairType = GetCreateRoleManager()->GetHairTypeIndex(
	//	GetCharTypeInfo(GetCreateingRole()->oldtype).hair,sizeof(GetCharTypeInfo(GetCreateingRole()->oldtype).hair)/sizeof(int));
	//cmd.byHairType = GetCreateRoleManager()->GetHairTypeIndex(
	//	GetCharTypeInfo(getCharTypeByFace(GetCreateingRole()->type)).hair,sizeof(GetCharTypeInfo(GetCreateingRole()->type).hair)/sizeof(int));
	//cmd.byRGB = GetRandColor24();
	cmd.byHairType = GetCreateingRole()->hairstyle;
	cmd.byRGB   = GetCreateingRole()->clrHair;

	//TRACE("A:R:G:B(%d,%d,%d,%d)\n",COLOR_GET_ALPHA(cmd.byRGB),COLOR_GET_R(cmd.byRGB),COLOR_GET_G(cmd.byRGB),COLOR_GET_B(cmd.byRGB));

	cmd.charType = GetCreateingRole()->type;   // 头像

	cmd.country = GetCreateingRole()->conntryId;

	//cmd.five=GetCurrRole()->five;
	cmd.five = FIVE_SOIL ;// cheat the server for debug 

	SEND_USER_CMD(cmd);

	FUNCTION_END;
}

////////////////////////////////////////////////////////////////////////////////
//  创建的角色名合法
////////////////////////////////////////////////////////////////////////////////
void  CCreateRoleManager::CreateUserSuccess()
{
	FUNCTION_BEGIN;
	SendCreateUserCommand();  // send new role create info
	FUNCTION_END;
}

void  CCreateRoleManager::GetUserInfo(SelectUserInfo * info)
{
	FUNCTION_BEGIN;

	for(int i = 0;i<__MAX_CHARINFO__;i++)
	{
		m_role[i].init();	
	}
	m_DemoRole.SrcAniState = stRole::eAni_State1;	
	m_DemoRole.state = stRole::eRoleState_DONE;

	if (info==NULL)	
	{
		m_guiRoleSel->EnableCreateBtn(true);
		return;
	}
	char szTmp[128]={0};

	int number  = 0;
	int nIndex = 0 ;
	for (int i=0;i<__MAX_CHARINFO__;i++)
	{
		if (strlen(info[i].name)>0)
		{
			strncpy(m_role[i].name,info[i].name,sizeof(m_role[i].name));
			m_role[i].level=info[i].level;
			m_role[i].type=GetTypeFromHead(info[i].type);  // 服务器这个时候发过来的是头像
			m_role[i].conntryId=-1;
			strncpy(m_role[i].countryName,info[i].countryName,sizeof(m_role[i].countryName));
			//m_role[i].five=info[i].five;
			m_role[i].state=stRole::eRoleState_DONE; // Set the state 
			m_role[i].dwBitMask = info[i].bitmask;
			nIndex = i;

			if(m_guiRoleSel)
			{
				sprintf(szTmp,"名称：%s",m_role[i].name);
				m_guiRoleSel->m_pLabelRoleName[i]->SetText(szTmp);


				//sprintf(szTmp,"国家：%s",GetCreateRoleManager()->getCountryName(m_role[i].conntryId));
				sprintf(szTmp,"国家：%s",m_role[i].countryName);
				m_guiRoleSel->m_pLabelRoleNation[i]->SetText(szTmp);

				sprintf(szTmp,"等级：%d",m_role[i].level);
				m_guiRoleSel->m_pLabelRoleLevel[i]->SetText(szTmp);
			}
                        number ++;
			
		}
		else
		{
			m_role[i].state=stRole::eRoleState_NULL;
		}
	}

	if( number >= 1  )
	{
		m_guiRoleSel->EnableCreateBtn(false);
		SetSelRoleID(eRoleSel_ONE);
	}

#ifdef _DEBUG_DOUBLE_PEOPLE
	if( number == 1)
	{
		strncpy(m_role[1].name,info[0].name,sizeof(m_role[1].name));
		m_role[1].level=info[0].level;
		m_role[1].type=GetTypeFromHead(info[0].type);  // 服务器这个时候发过来的是头像
		m_role[1].conntryId=-1;
		strncpy(m_role[1].countryName,info[0].countryName,sizeof(m_role[1].countryName));
		//m_role[1].five=info[0].five;
		m_role[1].state=stRole::eRoleState_DONE; // Set the state 
		nIndex = i;

		if(m_guiRoleSel)
		{
			sprintf(szTmp,"名称：%s",m_role[1].name);
			m_guiRoleSel->m_pLabelRoleName[1]->SetText(szTmp);


			//sprintf(szTmp,"国家：%s",GetCreateRoleManager()->getCountryName(m_role[1].conntryId));
			sprintf(szTmp,"国家：%s",m_role[1].countryName);
			m_guiRoleSel->m_pLabelRoleNation[1]->SetText(szTmp);

			sprintf(szTmp,"等级：%d",m_role[1].level);
			m_guiRoleSel->m_pLabelRoleLevel[1]->SetText(szTmp);
		}
		number ++;
	}
#endif
	if( number <= 1)
	{
#ifdef _USE_SINGLE_CHARACTER_ 
		m_bSingle = true;
#define Control_SetLocation(pControl,x,y)  { if(pControl)  pControl->SetLocation(x,y);pControl->SetVisible(true);}
		Control_SetLocation(m_guiRoleSel->m_pLabelRoleLevel[nIndex],488 * CPixelContext::GetRenderScale().x,143 * CPixelContext::GetRenderScale().y); 
		Control_SetLocation(m_guiRoleSel->m_pLabelRoleNation[nIndex],488 * CPixelContext::GetRenderScale().x,165 * CPixelContext::GetRenderScale().y); 
		Control_SetLocation(m_guiRoleSel->m_pLabelRoleName[nIndex],488 * CPixelContext::GetRenderScale().x,188 * CPixelContext::GetRenderScale().y); 
#endif 
	}

	FUNCTION_END;
}

void  CCreateRoleManager::DelUser()
{
	FUNCTION_BEGIN;

	/*if (m_eSelRoleID!=eRoleSel_NULL)
	{
		if (m_role[m_eSelRoleID].state==stRole::eRoleState_DONE)
		{
			char szMsg[256];
			_snprintf(szMsg,sizeof(szMsg),"请输入要删除的角色名:",m_role[m_eSelRoleID].name,m_role[m_eSelRoleID].level);
			szMsg[sizeof(szMsg)-1] = 0;
			GetGameGuiManager()->AddMessageBox(szMsg,eAction_DelUser,(void*)m_eSelRoleID,c_szMBOKCancel);
		}
	}
	else
	{
		GameMessageBox("请选中要删除的角色！");
	}*/
	AddDelRole();

	FUNCTION_END;
}


void  CCreateRoleManager::CreateUser()
{
	FUNCTION_BEGIN;

#ifdef ___CREATEROLE___NO___LIMIT___
#else 
	if (m_role[0].state==stRole::eRoleState_NULL)  // 如果第一个角色也没有，现在创建的就是第一个
	{
		m_eCreateingRoleID = eRoleSel_ONE;
	}
	//else if (m_role[1].state==stRole::eRoleState_NULL)  // 如果第一个角色已经有了，那么现在创建的就是第二个了
	//{
	//	m_eCreateingRoleID = eRoleSel_TWO;
	//}
	else
	{
		GetGameGuiManager()->AddMessageBox("您的角色已满,无法创建！");
		return;
	}
#endif

	//////////////////////////////////////////////////////////////
	// 请求最新国家信息
	stRequestSelectUserCmd cmd;
	SEND_USER_CMD(cmd);
        ///////////////////////////////////////////////////////////////

	// 弹出角色创建框
	m_guiRoleCreate->init();
	m_guiRoleCreate->SetLocation(0,-400);	

	GetAniManager()->PopupWindowFromTop(m_guiRoleCreate);


	m_guiRoleSel->UpdateRoleSelDemoAni();


	m_bInCreating = true;
	m_guiRoleSel->ReLoad();  // 加载两个可以被选择的角色
	m_guiRoleCreate->ReLoad();

	m_guiRoleSel->ShowCreateAndDeleteBtn(false);

	/////////////////////////////////////////////////////////////////////
	
	/*CGuiImage * pImage = static_cast<CGuiImage *>(GetTextEffect()->m_pControl);*/

        /*pImage->SetImage(rl);	
	pImage->SetClipRect(&rect);
	pImage->SetLocation(0,GetDevice()->GetHeight() - 20);

	POINT ptDst;
	ptDst.x = 0 ; 
	ptDst.y = - GetDevice()->GetHeight()-40;
	GetTextEffect()->BeginAni(40.0f, ptDst);*/

	m_guiRoleCreate->CreateAniRoleDesc();
	


	FUNCTION_END;
}

bool  CCreateRoleManager::SetSelRoleID(enumRoleSel newSelRoleID)
{
	FUNCTION_BEGIN;

	if( newSelRoleID == m_eSelRoleID)
		return true;

	assert( newSelRoleID >= eRoleSel_ONE && newSelRoleID <= eRoleSel_TWO);

	if( GetCreateRoleManager()->m_bInCreating)
	{
		m_DemoRole.DstAniState = stRole::eAni_State3;
		m_DemoRole.SrcAniState = stRole::eAni_State1;
		m_DemoRole.dwLightColor = ROLE_COLOR_SELECT;

	}
	else
	{
		// player selected a owned role ,so the selected is in the m_role

		if( m_role[newSelRoleID].state == stRole::eRoleState_NULL)
		{ 
			// really select a new one 
			return false;
		}
		if( m_eSelRoleID != eRoleSel_NULL)
		{// 不被选中的
			m_role[m_eSelRoleID].DstAniState = stRole::eAni_State1;
			m_role[m_eSelRoleID].dwLightColor = ROLE_COLOR_SELECT;
		}
		//重新选中的
		m_eSelRoleID = newSelRoleID;
		m_role[m_eSelRoleID].DstAniState = stRole::eAni_State3 ;
		m_role[m_eSelRoleID].dwLightColor = ROLE_COLOR_SELECT;


	}

	return true;

	FUNCTION_END;
}


int CCreateRoleManager::GetHairTypeIndex(const int* pHairType,size_t typenum)
{
	FUNCTION_BEGIN;

	for (int i=0;i<typenum;i++)
	{
		if (pHairType[i]!=0)
			return i;
	}
	return 0;

	FUNCTION_END;
}


////////////////////////////////////////////////////////////////////////////////
// 得到当前正在被创建的角色,不是当前被选中的角色
// 注意:只有在弹出和关闭创建界面时候才有用,其他时候一律用GetCurSelRole()
////////////////////////////////////////////////////////////////////////////////
stRole* CCreateRoleManager::GetCreateingRole()
{
	FUNCTION_BEGIN;

	if (m_eCreateingRoleID == eRoleSel_NULL)
		return NULL;
	return &m_role[m_eCreateingRoleID];

	FUNCTION_END;
}


////////////////////////////////////////////////////////////////////////////////
// 得到当前选中的角色,创建时用m_All2Role,选择已有角色用m_role
////////////////////////////////////////////////////////////////////////////////
stRole * CCreateRoleManager::GetCurSelRole()
{
	FUNCTION_BEGIN;

	if (m_eSelRoleID == eRoleSel_NULL)
		return NULL;
	return &m_role[m_eSelRoleID];

	FUNCTION_END;
}


CCreateRoleManager* GetCreateRoleManager()
{
	FUNCTION_BEGIN;

	static CCreateRoleManager sm;
	return &sm;

	FUNCTION_END;
}


////////////////////////////////////////////////////////////////////////////////
// 角色删除对话框
////////////////////////////////////////////////////////////////////////////////
CGuiDelRoleDialog::CGuiDelRoleDialog()
{
	FUNCTION_BEGIN;
	m_bSmDialog = true;
	FUNCTION_END;
}

void CGuiDelRoleDialog::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	CGuiButton * pOKBtn = GetButton(3);
	if( pOKBtn)
		pOKBtn->SetLocation(-200+GetClientX(),-60+GetClientY());
	CGuiButton * pCancelBtn = GetButton(4);
	if( pCancelBtn)
		pCancelBtn->SetLocation(180+GetClientX(),-345+GetClientY());

	OnResetScreenSize();

#ifndef _USE_NUM_PASSWORD_
	CGuiEditBox * pNumPass = GetEditBox(5);
	if(pNumPass){
		pNumPass->SetVisible(false);
		pNumPass->SetEnabled(false);
	}
	CGuiStatic * pStatic = GetStatic(6);
	if(pStatic){
		pStatic->SetVisible(false);
		pStatic->SetEnabled(false);
	}
#endif 
	FUNCTION_END;
}

bool    CGuiDelRoleDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{	
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		switch(nID)
		{
		case 3:   // 确定 
			{	

				enumRoleSel curSel = GetCreateRoleManager()->FindRoleByName( GetEditBox(2)->GetText() );
				if( eRoleSel_NULL !=  curSel)
				{
					if( GetCreateRoleManager()->SetSelRoleID(curSel))
					{
						stDeleteSelectUserCmd cmd;
						memset(cmd.numPassword,0,MAX_NUMPASSWORD);
						cmd.charNo=(DWORD)GetCreateRoleManager()->m_eSelRoleID;
#ifdef _USE_NUM_PASSWORD_
						const char * password = NULL;
						CGuiEditBox * pEditBox = GetEditBox(5);
						if(pEditBox)
						{
							password = pEditBox->GetText();
							if(password){
								_snprintf(cmd.numPassword,MAX_NUMPASSWORD,"%s",password);
							}
						}
#endif										
						SEND_USER_CMD(cmd);
						if(GetCreateRoleManager()->m_guiRoleSel)
							GetCreateRoleManager()->m_guiRoleSel->SetEnabled(false);
					}
					
				}
				else
				{
					GetGameGuiManager()->AddMessageBox("指定角色不存在");
				}
				Close();
				
			}
			break;
		case 4:   // 取消
			{
				Close();                                 
			}
			break;
		}// end switch(nID)
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiDelRoleDialog::OnResetScreenSize()
{
	FUNCTION_BEGIN;

	stRectI rcClip = GetClipRect();
	m_x = (GetDevice()->GetWidth() - GetWidth())/2 - rcClip.left;
	
	m_y = (GetDevice()->GetHeight() - rcClip.Height()) /2 - rcClip.top;

	FUNCTION_END;
}


HRESULT CGuiDelRoleDialog::OnRender(float fElapsedTime)
{	
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

#ifdef _USE_NUM_PASSWORD_
	int id[2] = {2,5};
#else
	int id[1] = {2};
#endif
	for(int i = 0 ; i < count_of(id);i++)
	{
		stRectI rect = GetEditBox(id[i])->GetBoundingBox();
		rect.OffsetRect(GetClientX()-2,GetClientY());
		GetDevice()->DrawRectangle(&rect,COLOR_ARGB(255,126,248,61));

	}

	return hr;

	FUNCTION_END;
}
bool    CGuiDelRoleDialog::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;
	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);
	FUNCTION_END;
}
void CGuiDelRoleDialog::OnClose(void)
{
	FUNCTION_BEGIN;
	GetCreateRoleManager()->m_guiDelRole = NULL;
	FUNCTION_END;
}

////////////////////////////////////////////////////////////////////////////////
// 角色选择对话框
////////////////////////////////////////////////////////////////////////////////
CGuiRoleSelDialog::CGuiRoleSelDialog()
:m_pEditPass(NULL)
,m_iMouseOn(-1)
,m_pImageUpBord(NULL)
,m_pSound(NULL)
,m_pWeaponSound(NULL)
{
	FUNCTION_BEGIN;
	m_bSmDialog = true;
	FUNCTION_END;
}

void CGuiRoleSelDialog::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pLabelRoleName[0] = GetStatic(14);
	m_pLabelRoleNation[0] = GetStatic(15);
	m_pLabelRoleLevel[0] = GetStatic(16);


	m_pLabelRoleName[1] = GetStatic(18);
	m_pLabelRoleNation[1] = GetStatic(19);
	m_pLabelRoleLevel[1] = GetStatic(20);


	m_pEditPass = GetEditBox(22);
	m_pEditPass->SetVisible(false);
	m_pEditPass->SetEnabled(false);
	m_pPassport = GetTextureManager()->CreateCurstomTexture( 128, 32, D3DFMT_R5G6B5, 0 );

	if( !IsBackMusicPlaying())
	{	//PlayBackMusic("sel_role.mp3");
		PlayBackMusic("login.mp3");
	}


	m_pImageUpBord = GetImage(23);


	/*GetTextEffect()->m_pControl = GetImage(24);	
	GetTextEffect()->m_pControl->SetVisible(false);*/

	for(int index = 0;index < __MAX_CHARINFO__; index++)
	{
		m_pLabelRoleName[index]->SetVisible(false);
		m_pLabelRoleLevel[index]->SetVisible(false);
		m_pLabelRoleNation[index]->SetVisible(false);
	}

	CAniRoleRectAndPos::InitRoleRectAndPos();

	FUNCTION_END;
}

void CGuiRoleSelDialog::UpdateRoleSelDemoAni()
{
	FUNCTION_BEGIN;
	GetCreateRoleManager()->m_DemoRole.SrcAniState = stRole::eAni_State1;
	GetCreateRoleManager()->m_DemoRole.DstAniState = stRole::eAni_State3;
	GetCreateRoleManager()->m_DemoRole.dwLightColor = ROLE_COLOR_SELECT;
	GetCreateRoleManager()->m_DemoRole.state = stRole::eRoleState_DONE;

	WORD wFace = GetCreateRoleManager()->m_guiRoleCreate->m_EditingFace;
	WORD nCurType = getCharTypeByFace(wFace); 
	GetCreateRoleManager()->m_DemoRole.type = nCurType;
	FUNCTION_END;
}

void CGuiRoleSelDialog::OnClose(void)
{
	FUNCTION_BEGIN;
	GetCreateRoleManager()->m_guiRoleSel = NULL;
	DestroyPPSound();
	DestroyWeaponSound();
	FUNCTION_END;
}

void CGuiRoleSelDialog::OnCompleteAniEvent()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

void CGuiRoleSelDialog::MouseOn(bool bMouseOn,int index)
{
	FUNCTION_BEGIN;

	assert( index >= 0 && index <=1 );

	DWORD dwColor = ROLE_COLOR_SELECT;
	bool bAddLight = (bMouseOn)?true:false;

	stRole role;
	if( GetCreateRoleManager()->m_bInCreating)
		role = GetCreateRoleManager()->m_DemoRole;
	else
		role = GetCreateRoleManager()->m_role[index];



	if(role.state == stRole::eRoleState_DONE)
	{
		char szTmp[20] = {0};

		if(GetCreateRoleManager()->m_bInCreating)
		{
			if( index == 0 )
			{
				m_pLabelRoleLevel[index]->SetVisible(false);
			}
			else
			{
				if( role.type == PROFESSION_1)	
					strcpy(szTmp,"男");
				else
					strcpy(szTmp,"女");
				m_pLabelRoleLevel[index]->SetText(szTmp);
				m_pLabelRoleLevel[index]->SetVisible(true);

			
			}
			m_pLabelRoleName[index]->SetVisible(false);
			m_pLabelRoleNation[index]->SetVisible(false);
		
		}
		else
		{
			bool bNeedVisible = (bMouseOn || role.SrcAniState == stRole::eAni_State3);
#ifdef _USE_SINGLE_CHARACTER_
			if( !bNeedVisible && GetCreateRoleManager()->m_bSingle)
				bNeedVisible = true;
#endif 
			sprintf(szTmp,"等级：%d",role.level);	
			m_pLabelRoleLevel[index]->SetText(szTmp);
			m_pLabelRoleNation[index]->SetVisible(bNeedVisible);
			m_pLabelRoleName[index]->SetVisible(bNeedVisible);
			m_pLabelRoleLevel[index]->SetVisible(bNeedVisible);

		}

		//m_roleAni[index].m_ani.SetColor(dwColor);	
		//m_roleAni[index].m_ani.SetBlendType(blend);
		m_roleAni[index].bAddLight = bAddLight;
	}
	else
	{
		m_pLabelRoleName[index]->SetVisible(false);
		m_pLabelRoleLevel[index]->SetVisible(false);
		m_pLabelRoleNation[index]->SetVisible(false);
	}

	FUNCTION_END;
}

void CGuiRoleSelDialog::RenderBackground(float fElapsedTime)
{
	FUNCTION_BEGIN;

	POINT pt = GetLocation();
	GetLogoAni()->RenderBackAnimation(pt);

	CGuiDialog::RenderBackground(fElapsedTime);

	FUNCTION_END;
}


HRESULT CGuiRoleSelDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	if(!IsVisible())   return S_OK;

	if( GetCreateRoleManager()->m_bInCreating)
	{
		MouseOn(false,0);
		MouseOn(false,1);
	}
	else
	{
		MouseOn(false,0);
		MouseOn(false,1);
	}

	if( (GetCreateRoleManager()->m_eSelRoleID==eRoleSel_ONE ) || (m_iMouseOn==0) )
	{
		if( false == GetCreateRoleManager()->m_bInCreating ) 
			MouseOn(true,0);
	}
	if( (GetCreateRoleManager()->m_eSelRoleID==eRoleSel_TWO) || (m_iMouseOn==1) )
		MouseOn(true,1);

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	///////////////////////////////////////////////////////////////////////////////
	POINT pt = GetLocation();
	GetLogoAni()->RenderAnimation(pt);

	///////////////////////////////////////////////////////////////////////////////
	// character animation 
	UpdateAniState();

	DrawCharacterAni();

	///////////////////////////////////////////////////////////////////////////////
	if( GetCreateRoleManager()->m_bInCreating)
	{
		RenderTextEffect(fElapsedTime);	
	}
	else
	{
		RenderPassPort();
	}
#ifdef _DEBUG
	CAniRoleRectAndPos::DebugRenderRectange();
#endif 
 
	return hr;

	FUNCTION_END;
}


void CGuiRoleSelDialog::DrawCharacterAni()
{
	FUNCTION_BEGIN;

	stRole * pRole = NULL;
	for(int i =0 ; i< __MAX_CHARINFO__;i++)
	{	
		POINT ptWindow = GetLocation();
		if( GetCreateRoleManager()->m_bInCreating)
		{
			if( i == 0)	continue;
			pRole = &GetCreateRoleManager()->m_DemoRole;
		}
		else
		{
			pRole = &GetCreateRoleManager()->m_role[i];
		}

		if( !GetCreateRoleManager()->m_bInCreating )
		{
			if(pRole->state != stRole::eRoleState_DONE)
			{
				continue;
			} 
		}
		
		if( i == 0)
		{
			CAniRoleRectAndPos::AdjustRole1RenderX(ptWindow.x);
		}
		m_roleAni[i].m_ani.Render(ptWindow.x,ptWindow.y,NULL,&CPixelContext::GetRenderScale(),pRole->dwLightColor);

		//if(m_roleAni[i].bAddLight)
		//{
		//	m_roleAni[i].m_ani.SetBlendType(Blend_Add2);
		//	m_roleAni[i].m_ani.SetColor(COLOR_ARGB(255,50,50,50));
		//	m_roleAni[i].m_ani.Render(ptWindow.x,ptWindow.y,NULL,&scale);
		//	m_roleAni[i].m_ani.SetBlendType(Blend_Null);
		//	m_roleAni[i].m_ani.SetColor(SELECT_CLR);
		//}
	}
	FUNCTION_END;
}
void CGuiRoleSelDialog::RenderTextEffect(float fElapsedTime)
{
	FUNCTION_BEGIN;

	assert( GetCreateRoleManager()->m_bInCreating );
	//GetTextEffect()->UpdateAni(fElapsedTime);
	//GetTextEffect()->m_pControl->Render(fElapsedTime);

	FUNCTION_END;
}


void CGuiRoleSelDialog::RenderPassPort()
{
	FUNCTION_BEGIN;
	if( !m_pPassport ) return;
	// render the jpeg picture verify code 
	stRectI rcImg( 0, 0, 100, 20 );
	if ( !m_pPassport->IsCreated() )
	{
		if ( !g_JpegPassport.empty() )
		{
			MemStream str(g_JpegPassport.size(),&g_JpegPassport[0],true,false);
			m_pPassport->LoadFromJpegStream( &str,NULL,&rcImg );
		}
	}

	m_pPassport->Render( 515, 552,(stRectF*)&rcImg,NULL );
	FUNCTION_END;
}

void CGuiRoleSelDialog::UpdateAniState()
{
	FUNCTION_BEGIN;

	for(int  i = 0 ; i < __MAX_CHARINFO__;i++)
	{
		stRole * role;
		CAnimation *pAni; 
		if( GetCreateRoleManager()->m_bInCreating)
		{
			if ( i == 0 )
				continue;
			role = &GetCreateRoleManager()->m_DemoRole;
		}
		else
		{
			role = &GetCreateRoleManager()->m_role[i];
		}
		pAni = &m_roleAni[i].m_ani;
		if(role->state == stRole::eRoleState_DONE)
		{
			//if(role->DstAniState == stRole::eAni_State3 && !pAni->IsPlaying() && role->DstAniState == role->SrcAniState)
			//{
			//	////if( pAni->GetFrameCount() != 1)
			//	//{    // translate to the last frame 
			//	//	stResourceLocation rl;
			//	//	rl.SetFileName(c_szRoleGrpPack);
			//	//	if( role->type == PROFESSION_1)
			//	//		rl.group = 1;
			//	//	else if(role->type == PROFESSION_2)
			//	//		rl.group = 2;
			//	//	else 
			//	//		continue;
			//	//	role->SrcAniState = stRole::eAni_Select_last;
			//	//	role->DstAniState = stRole::eAni_Select_last;
			//	//	rl.frame = 1;  // will play the last frame of select 
			//	//	pAni->SetLoopPlay(true);
			//	//	pAni->Create(&rl,false);
			//	//	pAni->SetSpeed(nRoleAniSpeed);
			//	//}
			//}

			if( role->DstAniState != role->SrcAniState)
			{

				if( role->SrcAniState == stRole::eAni_State1 )
				{
					stResourceLocation rl;
					rl.SetFileName(c_szRoleGrpPack);
					role->SrcAniState = stRole::eAni_State2;
					if( role->type == PROFESSION_1)
						rl.group = 1;
					else if(role->type == PROFESSION_2)
						rl.group = 2;
					else 
						continue;
					rl.frame = 0;   // play the select directly 
					pAni->Create(&rl,false);
					pAni->SetSpeed(nRoleAniSpeed);
					pAni->SetLoopPlay(false);

					if( role->type == PROFESSION_1)
						PlayUISound(40001); // 挥动
					else
						PlayUISound(50001);
				}
				else if( role->SrcAniState == stRole::eAni_State2 )
				{
					if(!pAni->IsPlaying())
					{
						// translate player  have end 
						stResourceLocation rl;
						rl.SetFileName(c_szRoleGrpPack);
						if( role->type == PROFESSION_1)
							rl.group = 1;
						else if(role->type == PROFESSION_2)
							rl.group = 2;
						else 
							continue;
						role->SrcAniState = role->DstAniState;  
						if( role->DstAniState == stRole::eAni_State1)
						{// 最终状态不选中
							rl.frame = 0 ;  
						        role->dwLightColor = ROLE_COLOR_SELECT; 
							pAni->Create(&rl,true);
							pAni->SetSpeed2(0.0f);
							pAni->SetFrame(0);
							pAni->SetLoopPlay(true);
						}
						else 
						{// 最终状态选中 							
							rl.frame = 1 ;
							role->dwLightColor = ROLE_COLOR_SELECT;
							pAni->Create(&rl,true);
							pAni->SetSpeed(nRoleAniSpeed);
							pAni->SetLoopPlay(true);
						}
						
						
					}
				}
				else  if( role->SrcAniState == stRole::eAni_State3 )
				{  // if the old action is select, then translate 
					stResourceLocation rl;
					rl.SetFileName(c_szRoleGrpPack);
					if( role->type == PROFESSION_1)
					{
						rl.group = 1;   // play the translate
						rl.frame = 0;   // 男性倒播
					}
					else if(role->type == PROFESSION_2)
					{
						rl.group = 2;
						rl.frame = 2;
					}
					else 
						continue;
					role->SrcAniState = stRole::eAni_State2;	
					pAni->Create(&rl,false);
					pAni->SetSpeed(nRoleAniSpeed);
					pAni->SetLoopPlay(false);
					if( role->type == PROFESSION_1)
					{
						pAni->SetFrame(pAni->GetFrameCount()-1);
						pAni->Revrese();
					}
					if( role->type == PROFESSION_1)
						PlayUISound(40000); // 收刀
					//else
					//	PlayUISound(50000); // 女的收刀不播，
				}
			}
			////////////////////////////////////////////////////////////
			// 设置声音
			if( role->type == PROFESSION_2 && role->DstAniState == role->SrcAniState && role->DstAniState == stRole::eAni_State3)
			{
				//PlayUISound(50000); // 女的收刀持续，声音相应循环
				CreateWeaponSound();
			}
			else if(role->type == PROFESSION_1 && role->DstAniState == role->SrcAniState && role->DstAniState == stRole::eAni_State1)
			{	
			}
			else 
			{
				DestroyWeaponSound();
			}
		}

	}
	FUNCTION_END;
}

bool CGuiRoleSelDialog::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;
	m_iMouseOn = -1;
	switch(uMsg)
	{
	case WM_LBUTTONDBLCLK:
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };

			if( GetCreateRoleManager()->m_bInCreating == false)
			{
				if( CAniRoleRectAndPos::PtInRectRole1(mousePoint) )
				{
					if (GetCreateRoleManager()->SetSelRoleID(eRoleSel_ONE))
					{
						if( !GetCreateRoleManager()->m_bInCreating )
						{
							OnGuiEvent(EVENT_BUTTON_CLICKED,11,NULL);
						}
					}
				}
			}

			if (CAniRoleRectAndPos::PtInRectRole2(mousePoint))
			{
				if (GetCreateRoleManager()->SetSelRoleID(eRoleSel_TWO))
				{
					if( !GetCreateRoleManager()->m_bInCreating )
					{
						OnGuiEvent(EVENT_BUTTON_CLICKED,11,NULL);
					}
				}
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			if( GetCreateRoleManager()->m_bInCreating == false)
			{ 
				if( CAniRoleRectAndPos::PtInRectRole1(mousePoint) )
				{
					GetCreateRoleManager()->SetSelRoleID(eRoleSel_ONE);
				}
			}
			if( CAniRoleRectAndPos::PtInRectRole2(mousePoint) )
			{
				GetCreateRoleManager()->SetSelRoleID(eRoleSel_TWO);
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			if (CAniRoleRectAndPos::PtInRectRole1(mousePoint))
			{
				m_iMouseOn = 0;
			}
			if (CAniRoleRectAndPos::PtInRectRole2(mousePoint))
			{
				m_iMouseOn = 1;
			} 
		}
		break;
	}
	return  CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);

	FUNCTION_END;
}

#define ID_IMAGE_BK_CREATE_AND_DEL_ROLE   3 
#define ID_BUTTON_CREATE_ROLE  7 
#define ID_BUTTON_DELETE_ROLE  9 
#define ID_BUTTON_LOGIN_GAME  11
#define ID_BUTTON_FINISH_CREATEROLE  13

void CGuiRoleSelDialog::ShowCreateAndDeleteBtn(bool bShow)
{	
	FUNCTION_BEGIN;;
	//GetImage(ID_IMAGE_BK_CREATE_AND_DEL_ROLE)->SetVisible(bShow);  //  创建和删除底图

	// Create Button
	GetButton(ID_BUTTON_CREATE_ROLE)->SetVisible(bShow);

	//GetButton(ID_BUTTON_CREATE_ROLE)->SetEnabled(bShow);
	// Delete Button
	GetButton(ID_BUTTON_DELETE_ROLE)->SetVisible(bShow);
	GetButton(ID_BUTTON_DELETE_ROLE)->SetEnabled(bShow);


	// Entry Game Button
	GetButton(ID_BUTTON_LOGIN_GAME)->SetVisible(bShow);
	GetButton(ID_BUTTON_LOGIN_GAME)->SetEnabled(bShow);

	// Finish Button
	GetButton(ID_BUTTON_FINISH_CREATEROLE)->SetVisible(!bShow);
	GetButton(ID_BUTTON_FINISH_CREATEROLE)->SetEnabled(!bShow);

	FUNCTION_END;
}

void CGuiRoleSelDialog::EnableCreateBtn(bool bEnable)
{
#ifdef ___CREATEROLE___NO___LIMIT___
#else 
	CGuiButton * pBtn = GetButton(ID_BUTTON_CREATE_ROLE);
	if( pBtn )
	{
		pBtn->SetEnabled(bEnable);
	}
#endif
};

BOOL CGuiRoleSelDialog::ContainsPoint(POINT ptScreen)
{
	FUNCTION_BEGIN;
	//if( ptScreen.x > 
	//return (ptScreen.x >= m_x && ptScreen.x < m_x + m_width && ptScreen.y >= m_y && ptScreen.y < m_y + m_height);
	return CGuiDialog::ContainsPoint(ptScreen); 

	FUNCTION_END;
}

DWORD CGuiRoleSelDialog::OnHiTest(POINT ptScreen)
{  
	FUNCTION_BEGIN;
	// do this if for the highlight of the three buttons layed in the bottom 
	// and can echo the lbuttondown 
	//if( GetCreateRoleManager()->m_bInCreating)
	{
		if (IsFocus() || GetCreateRoleManager()->m_guiRoleCreate->IsFocus())
		{
			for( int i = 0; i < 4; i++)
			{
				if( GetButton(10+i)->ContainsPoint(ptScreen))
					return HTCLIENT;
			}
			//TRACE("focus ok\n");
		}
		else
		{
			//TRACE("focus lose\n");
			return HTTRANSPARENT;
		}
		
	}
	return CGuiDialog::OnHiTest(ptScreen);  

	FUNCTION_END;
}

void CGuiRoleSelDialog::CreatePPSound()
{
	FUNCTION_BEGIN;
	if( m_pSound != NULL)
		return;

	//GetSoundManager()->Play( "sound\\UI\\50005.wav",SoundType_Circumstance, );
	FUNCTION_END;
}

void CGuiRoleSelDialog::DestroyPPSound()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

void CGuiRoleSelDialog::CreateWeaponSound()
{
	FUNCTION_BEGIN;
	if( m_pWeaponSound != NULL)
		return;

	GetSoundManager()->Play( "sound\\UI\\50000.wav",SoundType_Circumstance);
	FUNCTION_END;
}

void CGuiRoleSelDialog::DestroyWeaponSound()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

bool CGuiRoleSelDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		switch(nID)
		{
		case 7:  //创建角色
			{	
				GetCreateRoleManager()->CreateUser();
			}
			break;
		case 9: //删除角色		
			{
				GetCreateRoleManager()->DelUser();
				DestroyWeaponSound();
			}
			break;
		case 10:  //关闭
			{
				ExitGameQuery();
			}
			break;
		case 12:  //   返回	  
			{
				if( GetCreateRoleManager()->m_bInCreating)
				{
					GetCreateRoleManager()->m_guiRoleCreate->PopoutWindow();


					GetCreateRoleManager()->m_eCreateingRoleID = eRoleSel_NULL;

					GetCreateRoleManager()->m_guiRoleSel->ShowCreateAndDeleteBtn(true);
					//GetCreateRoleManager()->m_eSelRoleID = eRoleSel_NULL;
					GetCreateRoleManager()->m_guiRoleSel->SetEnabled(true);

					GetCreateRoleManager()->m_guiRoleSel->ReLoad();
					//GetCreateRoleManager()->m_bInCreating = false;

					//GetTextEffect()->m_pControl->SetVisible(false);
				}
				else
				{       //返回到登陆
					DestroyPPSound();
					DestroyWeaponSound();
					GetCreateRoleManager()->InitRole();
					GetClient()->Destroy();
					GetGameState()->SetState(eGameState_Login);
				}
			}
			break;
		case 11:  // Entry Game 
			{
				if(GetCreateRoleManager()->GetCurSelRole() )
				{
					if (GetCreateRoleManager()->GetCurSelRole()->state==stRole::eRoleState_DONE)
					{
						stRole * pRole = GetCreateRoleManager()->GetCurSelRole();
						assert(pRole!=NULL);
						if(pRole->dwBitMask & CHARBASE_FORBID)
						{
							GameMessageBox("该角色已被禁止登陆!");
						}
						else
						{
							if( CCountry::Instance()->IsCountryCanLogin(pRole->countryName))
							{
								WaitToCanSendMsg();
								stLoginSelectUserCmd cmd;
								cmd.charNo = GetCreateRoleManager()->m_eSelRoleID;
								strncpy( cmd.jpegPassport, m_pEditPass->GetText(), sizeof( cmd.jpegPassport) );
#ifdef ___AUTO___NET___CONNECT___
								CNetAutoConnect::Instance()->BackUpLoginSelectCmd(&cmd);
								const stRole & role = GetCreateRoleManager()->m_role[GetCreateRoleManager()->m_eSelRoleID];
								CNetAutoConnect::Instance()->m_strAccount =  role.name;
#endif 
								SEND_USER_CMD(cmd);
								this->SetEnabled(false);
							}
							else 
							{
								stDisableLoginCountryCmd cmd;
								cmd.status = FALSE_LOGIN_COUNTRY;
								GetClient()->PushCommand(&cmd,sizeof(cmd));
							}

						}						
					}
					else
					{
						GameMessageBox("请创建角色！");
					}
				}
				else
				{
					GameMessageBox("请选择要进入游戏的角色！");
				}
			}
			break;
		case 13: // 完成创建
			{	
				assert(GetCreateRoleManager()->m_bInCreating);
				GetCreateRoleManager()->m_guiRoleCreate->FinishCreate();
			}
			break;
		}// end switch(nID)
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiRoleSelDialog::ReLoad()
{
	FUNCTION_BEGIN;

	GetGuiManager()->RequestFocus(this);

	stResourceLocation rl;
	rl.SetFileName(c_szRoleGrpPack);


	if( GetCreateRoleManager()->m_bInCreating )
	{
		stRole  & role = GetCreateRoleManager()->m_DemoRole;
		if( role.state == stRole::eRoleState_DONE )
		{	
			if( role.type == PROFESSION_1)
				rl.group = 1;// PROFESSION_1
			else  if(role.type == PROFESSION_2 )
				rl.group = 2;// PROFESSION_2
			else 
				return;
			rl.frame = 1;    // stand 
			m_roleAni[1].m_ani.Create(&rl,true);
			m_roleAni[1].m_ani.SetSpeed(nRoleAniSpeed);
			m_roleAni[1].m_ani.SetLoopPlay(true);

		}
	}
	else
	{
		for(int i= 0 ;i < __MAX_CHARINFO__; i++ )
		{
			stRole &  role = GetCreateRoleManager()->m_role[i];
			if(role.state == stRole::eRoleState_DONE)
			{		
				if( role.type == PROFESSION_1)
					rl.group = 1;// PROFESSION_1
				else  if(role.type == PROFESSION_2 )
					rl.group = 2;// PROFESSION_2
				else 
					continue;
				rl.frame = 0;    // stand 
				m_roleAni[i].m_ani.Create(&rl,true);
				m_roleAni[i].m_ani.SetFrame(0);
				m_roleAni[i].m_ani.SetSpeed2(0.0f);
				m_roleAni[i].m_ani.SetLoopPlay(true);

			}		
		}
		 
	}

	FUNCTION_END;
}

void CGuiRoleSelDialog::RenderUpBorder(float fElapsedTime)
{
	FUNCTION_BEGIN;
	if(m_pImageUpBord)
		m_pImageUpBord->Render(fElapsedTime);
	FUNCTION_END;
}


//void  CGuiRoleSelDialog::TipsRender(WORD role)
//{
//	FUNCTION_BEGIN;
//
//	char szText[1024]={0};
//	switch(role)
//	{
//		case PROFESSION_1:
//			strncpy(szText,PROFESSION_1_DESC,sizeof(szText));
//			break;
//		case PROFESSION_2:
//			strncpy(szText,PROFESSION_2_DESC,sizeof(szText));
//			break;
//		default:
//			return;
//	}
//
//	tips.Clear();
//	tips.SetCurColor(COLOR_ARGB(255,255,255,255);
//	tips.AddText(szText);
//	tips.Resize();
//	stPointI pt(Engine_GetCursor()->GetPosition());
//	int xPos=(pt.x+28+tips.GetWidth()>=1024)?1024-tips.GetWidth():pt.x+28  ;
//	tips.Render(xPos,pt.y+28);
//
//	FUNCTION_END;
//}

//-------------------------------------------------------------------------
// 角色创建对话框
//-------------------------------------------------------------------------
CGuiRoleCreateDialog::CGuiRoleCreateDialog()
:m_moveCountry(255)
,m_pImageTag(NULL)
,m_pHairCombox(NULL)
,m_pCountryCombox(NULL)
,m_pButtonMen(NULL)
,m_pButtonWomen(NULL)
{
	FUNCTION_BEGIN;
	m_bSmDialog = true;
	m_EditingFace = 1;
	FUNCTION_END;
}


void CGuiRoleCreateDialog::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();


	// 角色圆圈图片
	m_pImageRole = GetImage(3);

	//发型 Combox 
	m_pHairCombox   = GetComboBox(40);
	//m_pComBox->SetFont(0);
	m_pHairCombox->SetStyle(CGuiComboBox::GUI_CS_NOSKIN );

	// 国家 Combox
	m_pCountryCombox = GetComboBox(42);
	m_pCountryCombox->SetStyle(CGuiComboBox::GUI_CS_NOSKIN | CGuiComboBox::GUI_CS_DROPUP);

	// 角色名
	m_pEditRoleName = GetEditBox(41);
	m_pEditRoleName->ClearText();


	// 男 女
	m_pButtonMen = GetButton(30);
	m_pButtonWomen = GetButton(31);
	CGuiButton * pBtn =  GetButton(10);


	RefreshCountryList();


        // Initialize the CharInfo
	InitCharInfo();
			
	m_pRSlider = GetSlider(43);
	m_pGSlider = GetSlider(44);
	m_pBSlider = GetSlider(45);


	this->SetFocus((CGuiControl*)m_pEditRoleName);

	FUNCTION_END;
}


void CGuiRoleCreateDialog::OnCompleteAniEvent()
{
	FUNCTION_BEGIN;
	if( GetCreateRoleManager()->m_bInCreating) 
		GetCreateRoleManager()->m_bInCreating = false;
	SetVisible(false);
	FUNCTION_END;
}

void CGuiRoleCreateDialog::RenderBackground(float fElapsedTime)
{
	FUNCTION_BEGIN;
	if( m_RoleDemo.pDemoBk )
	{
		m_RoleDemo.Update(fElapsedTime);
		POINT pt = GetLocation();
		m_RoleDemo.RenderBk(pt);
	}
	return CGuiDialog::RenderBackground(fElapsedTime);
	FUNCTION_END;

}
//---------------------------------------------
// 重新进入创建角色对话框
//---------------------------------------------
void CGuiRoleCreateDialog::init()
{
	FUNCTION_BEGIN;

	m_EditingFace = 1;
	m_pButtonMen->SetChecked(TRUE);
	m_pButtonWomen->SetChecked(FALSE);
	RefreshHeadIcon();
	RefreshHairCombox();
	m_pEditRoleName->ClearText();

	m_curColorHair = COLOR_ARGB(255,128,128,128);

	m_pRSlider->SetValue(128);
	m_pGSlider->SetValue(128);
	m_pBSlider->SetValue(128);


	/*任务说明:目前 我们已经开放10个国家 玩家在创建角色时候 选择国家列表里 国家列表是按照目前
	注册人数少到多 从上往下排列的  出现在国家选择框里的是最少人数的国家
	而我们现在要做的是 在这列表里增加选项"默认"  "默认"的是其实就是人数最少的国家 
	我们只是以"默认"两字始终出现在选择国家栏里(玩家只看到是默认2字)  而列表里国家选
	择的排列 照旧 即是国家人数的少到多从上往下排列
	任务输入: 
	期望输出:
	*/
	if(m_pCountryCombox)
	{
		DWORD dwDefaultID;
	        BOOL bDefault = false;
		if( CCountry::Instance()->GetDefaultCountryID(dwDefaultID))
		{
			for(int k = 0 ;  k  <  m_pCountryCombox->GetItemCount(); ++k)
			{
				DWORD dwCountryID =  *(DWORD *)m_pCountryCombox->GetItemData(k);
				if(dwDefaultID ==dwCountryID)
				{
					      m_pCountryCombox->SetCurItem(k);
					      m_pCountryCombox->SetWindowText("默认");
					      bDefault = true;
					      break;
				}
			}
		}
		if(!bDefault)
		{
			Assert(0);
			m_pCountryCombox->SetCurItem(0);
		}
	}

	UpdateCreateDemoAni();

	FUNCTION_END;
}

//---------------------------------------------
// 刷新头像
//---------------------------------------------
void CGuiRoleCreateDialog::RefreshHeadIcon()
{
	FUNCTION_BEGIN;

	m_pImageRole->Clear();

	stResourceLocation rl;
	rl.SetFileName("data\\interfaces.gl");

	WORD nCurType = getCharTypeByFace(m_EditingFace); //GetCreateRoleManager()->GetCurSelRole()->type ;
	WORD nCurHead = m_EditingFace;//GetCreateRoleManager()->GetCurSelRole()->head;

	rl.group = GetCurGroup(nCurType);
	rl.frame = nCurHead; 
	m_pImageRole->SetImage(rl);
	stPointI __pt = CAniRoleRectAndPos::GetRoleCreateHeadPos();
	m_pImageRole->SetLocation(__pt.x,__pt.y);

	FUNCTION_END;

}

void CGuiRoleCreateDialog::RefreshCountryList()
{
	FUNCTION_BEGIN;

	m_pCountryCombox->RemoveAllItems();

	// refresh the countrylist 
	const CCountry::COUNTRYLIST  & countrylist = CCountry::Instance()->GetCountryList();
	for (int i=0;i<countrylist.size();i++)
	{	
		if( countrylist[i].enableRegister)
			m_pCountryCombox->AddItem(countrylist[i].pstrName,(void *)&countrylist[i].id);
	}

	FUNCTION_END;
}

//---------------------------------------------
// 刷新发型
//---------------------------------------------
void CGuiRoleCreateDialog::RefreshHairCombox()
{
	FUNCTION_BEGIN;
	// 初始化头发类型：\\ -1.0v-发型表2005-8-2.xls
	m_pHairCombox->RemoveAllItems();
	m_stHairList.clear();
	WORD CharType = getCharTypeByFace(m_EditingFace); 
	for( int i = 0 ; i< g_tableHairStyle.size() ; i++)
	{
		HairStyle_t & style = g_tableHairStyle[i];
		if( style.dwRegister == 1 && style.dwSex == CharType)
		{
			m_pHairCombox->AddItem(style.strName,(void *)&style.dwID);
		}
	}
	FUNCTION_END;
}

void CGuiRoleCreateDialog::UpdateData()
{
	FUNCTION_BEGIN;
	int nCurItem = m_pCountryCombox->GetCurItem();
	if( nCurItem != -1) 
	{
		DWORD nCountry = *(DWORD *)m_pCountryCombox->GetItemData(nCurItem);
		GetCreateRoleManager()->SetCreateingCountryID(nCountry);
	}
	nCurItem = m_pHairCombox->GetCurItem();
	if( nCurItem != -1)
	{
		DWORD nHairType = *(DWORD *)m_pHairCombox->GetItemData(nCurItem);
		GetCreateRoleManager()->SetCreateingHairType(nHairType);
	}
	FUNCTION_END;
}

void CGuiRoleCreateDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetCreateRoleManager()->m_guiRoleCreate = NULL;
	//GetTextEffect()->Release();
	m_AniRoleDesc.Release();

	FUNCTION_END;
}


HRESULT CGuiRoleCreateDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr ;


	hr = CGuiDialog::OnRender(fElapsedTime);


	POINT pt = GetLocation();      
	m_RoleDemo.RenderCharacter(pt.x,pt.y,m_curColorHair);

	// 上边框
	if(GetCreateRoleManager()->m_guiRoleSel)
		GetCreateRoleManager()->m_guiRoleSel->RenderUpBorder(fElapsedTime);

	m_AniRoleDesc.UpdateSprite(fElapsedTime);
	m_AniRoleDesc.RenderSprite();

	return hr;

	FUNCTION_END;
}


void CGuiRoleCreateDialog::ReLoad()
{
	FUNCTION_BEGIN;

	GetGuiManager()->RequestFocus(this);
	SetFocus(m_pEditRoleName);

	FUNCTION_END;
}

void CGuiRoleCreateDialog::InitCustomElements()
{
	FUNCTION_BEGIN;

	//stResourceLocation rl;
	//rl.SetFileName(c_szRoleGrpPack);
	//CGuiElement Element;
	//stRectI  rcTexture;
	////-----------------------------------
	//// Assign the Element
	//// SetCustomElement( GUI_CONTROL_BUTTON, 0, &Element );

	//stRectI rects[9];
	//rl.group = 3;
	//rl.frame = 1;
	//
	////------------------------------------
	//// Element for the 3D Border ComboBox(组合框)
	////------------------------------------
	//{
	//	rl.group = 3;
	//	rl.frame = 1;
	//	stGuiCtrlBorderInfo * pInfo = GetCustomCtrlBorderInfo(GUI_CONTROL_COMBOBOX);
	//	pInfo->rcTexture.SetRect(0,0,98,142);
	//	pInfo->ptLeftTop = stPointI(5,3);
	//	pInfo->ptRightBottom = stPointI(6,7);
	//	pInfo->ptBorderLeftTop = stPointI(5,3);
	//	pInfo->ptBorderRightBottom = stPointI(6,7);
	//	pInfo->CuttRect9(rects);
	//	for(size_t i=0;i<count_of(rects);++i)
	//	{
	//		pInfo->element[i].SetTexture(&rl,&rects[i]);
	//		pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
	//		pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
	//	}
	//}

	//// button 
	//rl.group = 3;
	//rl.frame = -1;
	//rcTexture.SetRect(-6, 0,13, 10 );

	//Element.SetTexture( &rl, &rcTexture );
	//SetCustomElement(GUI_CONTROL_COMBOBOX,1,&Element);


	//m_nSBWidth = 18;
	//////------------------------------------
	////// Element for the 3D Border ScrollBar(滚动条)
	//////------------------------------------
	//{
	//	stRectI rects[9];
	//	rl.group = 3;
	//	rl.frame = 54;
	//	stGuiCtrlBorderInfo * pInfo = GetCustomCtrlBorderInfo(GUI_CONTROL_SCROLLBAR);
	//	pInfo->rcTexture.SetRect(0,0,18,138);
	//	pInfo->ptLeftTop = stPointI(5,5);
	//	pInfo->ptRightBottom = stPointI(6,6);
	//	pInfo->ptBorderLeftTop = stPointI(5,5);
	//	pInfo->ptBorderRightBottom = stPointI(6,6);
	//	pInfo->CuttRect9(rects);
	//	m_nSBWidth = pInfo->rcTexture.Width();
	//	for(size_t i=0;i<count_of(rects);++i)
	//	{
	//		pInfo->element[i].SetTexture(&rl,&rects[i]);
	//		pInfo->element[i].FontColor.Current = COLOR_ARGB(255,255,255,255);
	//		pInfo->element[i].TextureColor.Current = COLOR_ARGB(255,255,255,255);
	//	}
	//}

	////-------------------------------------
	//// CGuiScrollBar - Up Arrow
	////-------------------------------------
	//rl.group = 3;
	//rl.frame = 50;
	//
	//rcTexture.SetRect( 0, 0, 13, 10);
	//Element.SetTexture( &rl, &rcTexture );
	//Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	//// Assign the Element
	//SetCustomElement( GUI_CONTROL_SCROLLBAR, 0, &Element );


	//rl.frame = 52;
	//rcTexture.SetRect( 0, 0, 13, 10);
	//Element.SetTexture( &rl, &rcTexture );
	//Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	//// Assign the Element
	//SetCustomElement( GUI_CONTROL_SCROLLBAR, 1, &Element );


	////-------------------------------------
	//// CGuiScrollBar - Down Arrow
	////-------------------------------------
	//rl.group = 3;
	//rl.frame = 9;
	//rcTexture.SetRect(0,0 ,13, 10 );
	//Element.SetTexture( &rl, &rcTexture );
	//Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	//// Assign the Element
	//SetCustomElement( GUI_CONTROL_SCROLLBAR, 2, &Element );

	//rl.frame = 11;
	//rcTexture.SetRect( 0 ,0 ,13, 10 );
	//Element.SetTexture( &rl, &rcTexture );
	//Element.TextureColor.States[ GUI_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

	//// Assign the Element
	//SetCustomElement( GUI_CONTROL_SCROLLBAR, 3, &Element );

	//////-------------------------------------
	////// CGuiScrollBar - Button
	//////-------------------------------------
	//rl.frame = 53;
	//rcTexture.SetRect( 0, 30, 13, 52 );
	//Element.SetTexture( &rl, &rcTexture );

	//// Assign the Element
	//SetCustomElement( GUI_CONTROL_SCROLLBAR, 4, &Element );

	FUNCTION_END;
}
void CGuiRoleCreateDialog::CreateAniRoleDesc()
{
	FUNCTION_BEGIN;

	stResourceLocation rl;
	rl.group = 5;
	rl.frame = 22;
	rl.SetFileName("data\\login.gl");
	IBitmap * pBitmap = GetDevice()->FindBitmap(&rl);

	m_AniRoleDesc.SetRenderRect(CAniRoleRectAndPos::GetAniRoleDescRect());
	m_AniRoleDesc.SetBitmap(pBitmap);
	m_AniRoleDesc.SetSpeed(stPointF(0,-30));
	m_AniRoleDesc.BeginSprite(false);

	FUNCTION_END;


}


BOOL CGuiRoleCreateDialog::ContainsPoint(POINT ptScreen)
{
	FUNCTION_BEGIN;
	if( !IsVisible())
		return FALSE;
	if(CGuiDialog::ContainsPoint(ptScreen)) return TRUE;
	stPointI pt1 = ptScreen;
	pt1.x -= m_x + m_clientOffsetTopLeft.x;
	pt1.y -= m_y + m_clientOffsetTopLeft.y;
	CGuiControl* p = this->GetControlAtPoint(pt1);
	if(p)
		return TRUE;
	return FALSE;
	FUNCTION_END;
}



bool CGuiRoleCreateDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	m_moveCountry=255;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		switch(nID)
		{
		case 32: // <--
			{
				WORD nCurType = getCharTypeByFace(m_EditingFace);//GetCreateRoleManager()->GetCurSelRole()->type;
				WORD nCurHead = m_EditingFace;//GetCreateRoleManager()->GetCurSelRole()->head;
				WORD nPrevHead = GetPrevHead(nCurType,nCurHead);
				if( nPrevHead != (WORD)-1) 
				{
					m_EditingFace = nPrevHead;
					RefreshHeadIcon();
				}
				break;
			}
			break;
		case 33:  // -->
			{
				WORD nCurType = getCharTypeByFace(m_EditingFace);//GetCreateRoleManager()->GetCurSelRole()->type;
				WORD nCurHead = m_EditingFace;//GetCreateRoleManager()->GetCurSelRole()->head;
				WORD nNextHead = GetNextHead(nCurType,nCurHead);
				if( nNextHead != (WORD)-1) 
				{
					m_EditingFace = nNextHead;
					RefreshHeadIcon();
				}
				break;
			}
			break;
		case 30 :  // "men"
			{
				if( !IsMaleFace(m_EditingFace) )
				{
					m_EditingFace = 1; 
					RefreshHeadIcon();
					m_pButtonMen->SetChecked(TRUE);
					m_pButtonWomen->SetChecked(FALSE);
					RefreshHairCombox();
					GetCreateRoleManager()->m_guiRoleSel->UpdateRoleSelDemoAni();
					UpdateCreateDemoAni();
				}   
			}
			break;
		case 31: // "women"
			{
				if( IsMaleFace(m_EditingFace) )
				{
					m_EditingFace = 2;
					RefreshHeadIcon();
					m_pButtonMen->SetChecked(FALSE);
					m_pButtonWomen->SetChecked(TRUE);
					RefreshHairCombox();
					GetCreateRoleManager()->m_guiRoleSel->UpdateRoleSelDemoAni();
					UpdateCreateDemoAni();
				}
			}
			break;
		case 34:     // 顺时间
			{
				m_RoleDemo.UpdateFrameAsTime();
			}
			break;
		case 35:     // 逆时间
			{
				m_RoleDemo.UpdateFrameAsReverseTime();
			}
			break;
		}
		break;
	case EVENT_SLIDER_VALUE_CHANGED:
		switch(nID)
		{
		case 43: //r
		case 44: //g
		case 45: //bUpdateHair
			{
				DWORD r = m_pRSlider->GetValue();
				DWORD g = m_pGSlider->GetValue();
				DWORD b = m_pBSlider->GetValue();
				m_curColorHair = COLOR_ARGB(255,r,g,b);  
				m_RoleDemo.m_DemoHair.SetColor(m_curColorHair);

			}
			break;
		}
		break;
	case EVENT_COMBOBOX_SELECTION_CHANGED:
		switch(nID)
		{
		case 40:
			UpdateHair();
			break;
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiRoleCreateDialog::UpdateHair()
{
	FUNCTION_BEGIN;

	int hairActionPicid  = GetCurSelHair();
	assert(hairActionPicid != -1);


	stResourceLocation  rlHair;
	rlHair.SetFileName("data\\hair.gl");

	bool bDrawHair = true;
	if( PROFESSION_1  == getCharTypeByFace(m_EditingFace) )
	{ 

		rlHair.group  =  hairActionPicid; // hairActionPicid 
		rlHair.frame  =  1;         // walk 
		if( rlHair.group == 0 )   // no hair 
			bDrawHair = false;
	}
	else
	{
		rlHair.group  =  hairActionPicid; // hairActionPicid 
		rlHair.frame  =  1;         // walk
		if( rlHair.group == 0 )   // no hair 
			bDrawHair = false;
	}
	m_RoleDemo.ChangeHair(rlHair,bDrawHair);

	FUNCTION_END;
}

int CGuiRoleCreateDialog::GetCurSelHair()
{
	FUNCTION_BEGIN;

	int nCurItem = m_pHairCombox->GetCurItem();
	if( nCurItem == -1 )
	{
		return -1 ;
	}
	int hairid = *(int *)m_pHairCombox->GetItemData(nCurItem);
	int hairActionPicid = -1;
	for( int i = 0 ; i < g_tableHairStyle.size() ; i++)
	{
		HairStyle_t & style =  g_tableHairStyle[i];
		if( style.dwID == hairid)
		{
			hairActionPicid = style.dwActionPicID;
			break;
		}
	}
	return hairActionPicid;

	FUNCTION_END;

}

void CGuiRoleCreateDialog::UpdateCreateDemoAni()
{
	FUNCTION_BEGIN;

	int hairActionPicid  = GetCurSelHair();
	assert(hairActionPicid != -1);
	if( hairActionPicid == -1)
		return;

	stResourceLocation  rlBody;
	rlBody.SetFileName("data\\body.gl");
	stResourceLocation  rlHair;
	rlHair.SetFileName("data\\hair.gl");

	bool bDrawBody = true;
	bool bDrawHair = true;
	if( PROFESSION_1  == getCharTypeByFace(m_EditingFace) )
	{ 
		rlBody.group  =  1;         // men
		rlBody.frame  =  1;         // walk 

		rlHair.group  =  hairActionPicid; // hairActionPicid 
		rlHair.frame  =  1;         // walk 
		if( rlHair.group == 0 )   // no hair 
			bDrawHair = false;
	}
	else
	{
		rlBody.group =   2;         // women
		rlBody.frame =   1;         // walk 


		rlHair.group  =  hairActionPicid; // hairActionPicid 
		rlHair.frame  =  1;         // walk
		if( rlHair.group == 0 )   // no hair 
			bDrawHair = false;
	}
	m_RoleDemo.init();
	m_RoleDemo.ChangeCharacter(rlBody,rlHair,bDrawBody,bDrawHair);
	

	FUNCTION_END;
}

void CGuiRoleCreateDialog::FinishCreate()
{
	FUNCTION_BEGIN;

	assert(GetCreateRoleManager()->m_bInCreating);
	if(!GetCreateRoleManager()->GetCreateingRole())
	{
		GameMessageBox("角色已满！");
		return;
	}

	int nCurItem = m_pCountryCombox->GetCurItem();
	if(nCurItem == -1)
	{
		GameMessageBox("请选择一个国家！");
		return;
	}

	// 新角色国家信息
	int nCurCountry = *(int *)m_pCountryCombox->GetItemData(nCurItem);
	GetCreateRoleManager()->GetCreateingRole()->conntryId = nCurCountry;

	// 新角色职业(2005-7-29改为头像) 
	GetCreateRoleManager()->GetCreateingRole()->type  = m_EditingFace;

	nCurItem = m_pHairCombox->GetCurItem();
	if( nCurItem == -1)
	{
		GameMessageBox("请选择一种发型");
		return;
	}
	int nCurHair = *(DWORD *)m_pHairCombox->GetItemData(nCurItem);
	GetCreateRoleManager()->GetCreateingRole()->hairstyle = nCurHair;

	GetCreateRoleManager()->GetCreateingRole()->clrHair  = m_curColorHair;

	// 新角色名称
	strncpy(GetCreateRoleManager()->GetCreateingRole()->name,m_pEditRoleName->GetText(),sizeof(GetCreateRoleManager()->GetCreateingRole()->name));						
	const char* szName = GetCreateRoleManager()->GetCreateingRole()->name;
	const char szNULL[] = " ";
	if(szName == NULL || szName[0] == 0 || szName[0] == ' ' || szName[strlen(szName)-1] == ' ')
	{
		GameMessageBox("名字不合法！");
	}
	else if(  NULL != strstr(szName,szNULL) )
	{
		GameMessageBox("名字不能包含空字符!");
	}
	else
	{
		stCheckNameSelectUserCmd cmd;
		strncpy(cmd.name, GetCreateRoleManager()->GetCreateingRole()->name, sizeof(cmd.name));

		SEND_USER_CMD(cmd);
		this->SetEnabled(false);
		GetCreateRoleManager()->m_guiRoleSel->SetEnabled(false);
	}
	FUNCTION_END;
}


void CGuiRoleCreateDialog::PopoutWindow()
{
	FUNCTION_BEGIN;

	GetAniManager()->PopoutWindowFromTop(this);
	m_AniRoleDesc.Release();

	FUNCTION_END;
}


void CGuiRoleCreateDialog::InitCharInfo()
{
	FUNCTION_BEGIN;

	const int c_group = 33;
	IBitmapList * pBmpList = GetDevice()->FindResource("data\\Interfaces.gl");
	size_t framecount = pBmpList->GetFrameCount(c_group);

	m_vecCharInfo.clear();

	szCharInfo InfoMen,InfoWomen;
	InfoMen.group = c_group;
	InfoMen.type = (WORD)PROFESSION_1;

	InfoWomen.group = c_group;
	InfoWomen.type = (WORD)PROFESSION_2;

	for(int j =1;j <= framecount;j++)
	{
		if( pBmpList->IsExist(c_group,j))
		{
			if( j % 2 == 1 )  // men
			{
				InfoMen.nVecFrame.push_back(j);
			}
			else
				InfoWomen.nVecFrame.push_back(j);
		}
	}
	m_vecCharInfo.push_back(InfoMen);
	m_vecCharInfo.push_back(InfoWomen);

	FUNCTION_END;
	
}


WORD CGuiRoleCreateDialog::GetNextHead(WORD type,int nCurHead)
{
	FUNCTION_BEGIN;

	for(int i = 0 ;i < m_vecCharInfo.size();i++)
	{
		if( m_vecCharInfo[i].type == type)
		{
			if( nCurHead == (WORD)-1 )     // no head now , give him the first 
			{
				return m_vecCharInfo[i].nVecFrame[0];
			}
			for(int j =0 ; j< m_vecCharInfo[i].nVecFrame.size();j++)
			{			
				if(m_vecCharInfo[i].nVecFrame[j] == nCurHead)
				{
					if( j < m_vecCharInfo[i].nVecFrame.size() - 1 )
						return m_vecCharInfo[i].nVecFrame[j+1];
					else // j == m_vecCharInfo[i].nVecFrame.size() - 1
						return m_vecCharInfo[i].nVecFrame[0];
				}
			}
			break;
		}
	}
	return (WORD)-1;

	FUNCTION_END;
}

WORD CGuiRoleCreateDialog::GetPrevHead(WORD type,int nCurHead)
{
	FUNCTION_BEGIN;

	for(int i = 0 ;i < m_vecCharInfo.size();i++)
	{
		if( m_vecCharInfo[i].type == type)
		{
			if( nCurHead == (WORD)-1 )  // no head now , give him the first 
			{
				return m_vecCharInfo[i].nVecFrame[0];
			}
			for(int j =0 ; j< m_vecCharInfo[i].nVecFrame.size();j++)
			{	
				if(m_vecCharInfo[i].nVecFrame[j] == nCurHead)
				{
					if( j >0 )
						return m_vecCharInfo[i].nVecFrame[j-1];
					else // j = 0 
						return m_vecCharInfo[i].nVecFrame.back();
				}
			}
			break;
		}
	}
	return (WORD)-1;

	FUNCTION_END;
}

WORD CGuiRoleCreateDialog::GetTheFirstHead(WORD type)
{
	FUNCTION_BEGIN;

	for(int i = 0 ;i <  m_vecCharInfo.size();i++)
	{
		if( m_vecCharInfo[i].type == type)
		{
			return m_vecCharInfo[i].nVecFrame[0];
		}
	}
	return (WORD)-1;

	FUNCTION_END;
}

int CGuiRoleCreateDialog::GetCurGroup(WORD type)
{
	FUNCTION_BEGIN;

	for(int i = 0 ;i < m_vecCharInfo.size();i++)
	{
		if( m_vecCharInfo[i].type == type)
		{
			return m_vecCharInfo[i].group;
		}
	}
	return (WORD)-1;

	FUNCTION_END;
}

