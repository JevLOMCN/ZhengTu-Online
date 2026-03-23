#include "../engine/include/engine.h"
#include "cmagic.h"
//#include <algorithm>
//#include "iMagic.h"
#include "iAnimation.h"
#include "iCharacter.h"
#include "iLight.h" 
#include "MapScene.h"
#include "../engine/include/lookaside_alloc.h"
//#include "magic_sound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//#define _DEBUG_MAGIC

const int c_nMagicResourceFlags = IBitmapList::flag_MapView | IBitmapList::flag_AlwaysOpen | IBitmapList::flag_LoadByGroup | IBitmapList::flag_LoadByThread;
//const int c_nMapResourceFlags = IBitmapList::flag_AlwaysOpen | IBitmapList::flag_LoadByThread | IBitmapList::flag_NoRelease | IBitmapList::flag_LoadHdr;
//const int c_nTerrResourceFlags = IBitmapList::flag_AlwaysOpen | IBitmapList::flag_LoadByThread | IBitmapList::flag_NoRelease | IBitmapList::flag_LoadHdr;

const int c_nMapResourceFlags = IBitmapList::flag_AlwaysOpen | IBitmapList::flag_LoadByThread | IBitmapList::flag_MapView | IBitmapList::flag_LoadHdr;
const int c_nTerrResourceFlags = IBitmapList::flag_AlwaysOpen | IBitmapList::flag_LoadByThread | IBitmapList::flag_MapView | IBitmapList::flag_LoadHdr;
 
using namespace std;



/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nTable : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
#ifdef _DEBUG
static bool s_bDeleteMagic = false;
static bool s_bNewMagic = false;
#endif
//static std::lookaside_allocator<cMagic> s_allocMagic;
cMagic::cMagic(int nTable)
{
#ifdef _DEBUG
	Assert(s_bNewMagic);
#endif
	m_ulCount = 1;
	m_pDes = NULL;
	m_pSender = NULL;
	m_pShadow = NULL;
	m_bBlockGenerate = false;
	m_pLight = NULL;
	m_nInitLife = 0;
	m_nTable = nTable;
	m_lData = 0;
	m_nSetLife = 0;
	m_ptCenter = stPointI(-1,-1);	
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
cMagic::~cMagic(void)
{
#ifdef _DEBUG
	Assert(s_bDeleteMagic);
#endif
	FinalRelease();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cMagic::FinalRelease()
{

	m_Animation.FinalRelease();
#ifdef _DEBUG
        if( 0 == strcmp("¶¾ÑÀ¹¥»÷±¬Õ¨1",m_stProperty.strName))
	{
		int ii = 0;
	}
#endif 
	m_Sound.Destroy();
	std::for_each(m_listFollow.begin(),m_listFollow.end(),std::mem_fun<ULONG,iMagic>(&iMagic::Release));
	m_listFollow.clear();
	SAFE_RELEASE(m_pSender);
	SAFE_RELEASE(m_pDes);
	SAFE_RELEASE(m_pShadow);
	SAFE_RELEASE(m_pLight);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
ULONG cMagic::AddRef() 
{
	return ++m_ulCount;
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
ULONG cMagic::Release() 
{
	int n = --m_ulCount;
	if (!m_ulCount) 
	{
#ifdef _DEBUG
		s_bDeleteMagic = true;
#endif
		
		//s_allocMagic.freePointer(this);
		delete this;
#ifdef _DEBUG
		s_bDeleteMagic = false;
#endif
		//s_allocMagic.freePointer(this);
		//delete this; 
	}
	return n;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \param rc : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//*
bool cMagic::GetDrawRectA(POINT pt,RECT &rc)
{
		return m_Animation.GetDrawRect(pt,GetCenter(),rc);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \param rc : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cMagic::GetDrawRect(POINT pt,RECT &rc)
{
	stRectI & r = (stRectI&) rc;
	r.SetRect(0,0,0,0);

	stRectI rc0;
	if (m_pShadow)
	{
		m_pShadow->GetDrawRect(pt,rc0);
		r |= rc0;
	}
	
	for(size_t i=0;i<m_listFollow.size();++i)
	{
		m_listFollow[i]->GetDrawRect(pt,rc0);
		r |= rc0;
	}
	
	GetDrawRectA(pt,rc0);
	r |= rc0;

	return !r.IsRectEmpty();

}
//*/
POINT cMagic::GetDrawPosition()
{
	POINT pt = GetPos();
	pt += GetCenter();
	return pt;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptScreen : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cMagic::DrawAtScreenA(POINT ptScreen)
{
	if ( !GetMapScene()->IsNeedDrawMagicEffect() )
		return false;

	if (m_stProperty.stMove.IsDie())
		return false;

#ifdef _DEBUG_BOW
	if( 0 == strcmp(m_stProperty.GetName(),"¹­¼ı·ÉĞĞ"))
	{
		TRACE("¹­¼ı·ÉĞĞ±»äÖÔÚ:(%d,%d)\n",ptScreen.x,ptScreen.y);
	}
#endif
	if(m_stProperty.stAnimation.eMirror ==  stAnimationDataStruct::Mirror_AutoDir)
	{
		stPointI ptCenter = GetCenter();
		if(GetMaxDir() == 8) m_Animation.TurnTo(2);
		stRectI rcClip;
		stPointI ptR = ptScreen;
		if(m_Animation.GetClipRect(&rcClip))
		{
			ptR.y += ptCenter.y;
			ptR.y += rcClip.top + rcClip.Height()/2;
		}
		float f = m_stProperty.stMove.GetRotationAngle();

		GetDevice()->SetRotation( ptR,m_stProperty.stMove.GetRotationAngle() );
		/*if(iCharacter::s_bDrawCover)
		{
			int alpha = m_Animation.GetAlpha();
			m_Animation.ChangeAlpha(128);
			m_Animation.DrawAtScreen(ptScreen,GetCenter());
			m_Animation.ChangeAlpha(alpha);
		}
		else*/
			m_Animation.DrawAtScreen(ptScreen,GetCenter());
		GetDevice()->IdentityTransform();
	}
	else
	{
		if(GetType() == 12)
		{
			int iiiiiii=0;
		}
		/*if(iCharacter::s_bDrawCover)
		{
			int alpha = m_Animation.GetAlpha();
			m_Animation.ChangeAlpha(128);
			m_Animation.DrawAtScreen(ptScreen,GetCenter());
			m_Animation.ChangeAlpha(alpha);
		}
		else*/
			m_Animation.DrawAtScreen(ptScreen,GetCenter());
	}
	return true;
}

//Ëõ¹Ç
bool cMagic::DrawAtScreenA2(POINT ptScreen)
{
	if ( !GetMapScene()->IsNeedDrawMagicEffect() )
		return false;

	if (m_stProperty.stMove.IsDie())
		return false;

#ifdef _DEBUG_BOW
	if( 0 == strcmp(m_stProperty.GetName(),"¹­¼ı·ÉĞĞ"))
	{
		TRACE("¹­¼ı·ÉĞĞ±»äÖÔÚ:(%d,%d)\n",ptScreen.x,ptScreen.y);
	}
#endif
	if(m_stProperty.stAnimation.eMirror ==  stAnimationDataStruct::Mirror_AutoDir)
	{
		stPointI ptCenter = GetCenter();
		if(GetMaxDir() == 8) m_Animation.TurnTo(2);
		stRectI rcClip;
		stPointI ptR = ptScreen;
		if(m_Animation.GetClipRect(&rcClip))
		{
			ptR.y += ptCenter.y;
			ptR.y += rcClip.top + rcClip.Height()/2;
		}
		float f = m_stProperty.stMove.GetRotationAngle();

		GetDevice()->SetRotation( ptR,m_stProperty.stMove.GetRotationAngle() );
		/*if(iCharacter::s_bDrawCover)
		{
			int alpha = m_Animation.GetAlpha();
			m_Animation.ChangeAlpha(128);
			m_Animation.DrawAtScreen(ptScreen,GetCenter());
			m_Animation.ChangeAlpha(alpha);
		}
		else*/
			m_Animation.DrawAtScreen2(ptScreen,GetCenter());
		GetDevice()->IdentityTransform();
	}
	else
	{
		if(GetType() == 12)
		{
			int iiiiiii=0;
		}
		/*if(iCharacter::s_bDrawCover)
		{
			int alpha = m_Animation.GetAlpha();
			m_Animation.ChangeAlpha(128);
			m_Animation.DrawAtScreen(ptScreen,GetCenter());
			m_Animation.ChangeAlpha(alpha);
		}
		else*/
			m_Animation.DrawAtScreen2(ptScreen,GetCenter());
	}
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptMapOffset : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cMagic::Draw(POINT ptMapOffset) 
{
	if ( !GetMapScene()->IsNeedDrawMagicEffect() )
		return false;

	if (m_pShadow)
		m_pShadow->Draw(ptMapOffset);
	m_Animation.DrawKeepImage(ptMapOffset);
	for_each(m_listFollow.begin(),m_listFollow.end(),bind2nd(mem_fun1<bool,iMagic,POINT>(&iMagic::Draw),ptMapOffset));
	POINT pt = (POINT)GetPos()- ptMapOffset;
	return DrawAtScreenA(pt);
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptScreen : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cMagic::DrawAtScreen(POINT ptScreen) 
{
	if ( !GetMapScene()->IsNeedDrawMagicEffect() )
		return false;

	if (m_pShadow)
		m_pShadow->DrawAtScreen(ptScreen);
	POINT pt = (POINT)GetPos() - ptScreen;
	m_Animation.DrawKeepImage(pt);
	for_each(m_listFollow.begin(),m_listFollow.end(),bind2nd(mem_fun1<bool,iMagic,POINT>(&iMagic::DrawAtScreen),ptScreen));
	return DrawAtScreenA(ptScreen);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param n : ÃèÊö
 * \param listGenerate : ÃèÊö
 * \param pCharacterBlock : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int cMagic::OnBreak(stBlockStruct::BlockTypeEnum n,tagListMagicGenerate& listGenerate,iCharacter* pCharacterBlock)
{
	if(GetLife() <= 0)
		return 0;
	if (m_stProperty.stBlock.eBlock != stBlockStruct::Block_Continue)
		m_stProperty.stBlock.bBlock = false;
	m_stProperty.stMove.OnBreak(n,m_stProperty.stBlock.eBlockResult);
	iMagic* p = m_stProperty.stGenerate.GenerateByBlock(this,n);	
	if (p)
	{
		if (p->GetFollowType() == stMoveStruct::Move_Follow_Block && pCharacterBlock)
			pCharacterBlock->AddMagic(p);
		else
			listGenerate.push_back(p);
	}
	return true;
}



/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cMagic::CreateShadow()
{
	Assert(m_pShadow == NULL);
	if (strcmp(m_stProperty.stGenerate.strGenerateShadow,"null") != 0)
	{
		//m_pShadow = dynamic_cast<cMagic*>(GenerateAMagic(this,m_stProperty.stGenerate.strGenerateShadow));
		m_pShadow = (cMagic*)(GenerateAMagic(this,m_stProperty.stGenerate.strGenerateShadow));
	}
}


void cMagic::SetInitLife()
{
	m_nInitLife = GetLife();

	for (int i=0; i<MAX_GENERATE; i++)
	{
		m_stProperty.aGenerate[i].nInitLife = GetLife();
		m_stProperty.aGenerate[i].ptInitPos = GetPos();
	}
}
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bLight : ÃèÊö
 * \param szFile : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cMagic::CreateMagic(bool bLight,LPCSTR szFile)
{
	if (!bLight)
	{
		SAFE_RELEASE(m_pLight);
	}
	else
	{
		if (m_pLight == NULL)
			m_pLight = LIGHT_CREATELIGHT;
		m_pLight->EnableLight(bLight);
		m_pLight->SetType(m_stProperty.stLight.eLight);
		m_pLight->SetEffect(m_stProperty.stLight.eEffect);
	}

	CreateShadow();

	SetInitLife();

	if (szFile[0] != 0)
		strcpy(m_stProperty.stAnimation.strAnimation,szFile);

	if (strcmp(m_stProperty.stAnimation.strAnimation,"null") != 0)
	{
		iAnimation* m_pAnimation = Magic_CreateAnimation();
		if (!m_pAnimation->CreateAnimation("magic",atoi(m_stProperty.stAnimation.strAnimation),0,m_stProperty.stAnimation,Usage_Magic))
		{
			Magic_DestroyAnimation(m_pAnimation);
			m_pAnimation = NULL;
			return false;
		}
		m_pAnimation->SetAnimationColor(m_stProperty.stAnimation.dwColor);
		//if the image is 256*256
		m_Animation.Add(m_pAnimation);
		m_ptCenter = GetDefaultCenter();
		//use resource of type
	}

	switch(m_stProperty.stCharacter.eMagicEffect )
	{
	case stCharacterStruct::Magic_Effect_Image:	// ²ĞÏñ
		{
			stKeepImageControl st;
			st.nAlphaDec = 25;
			StartKeepImage(st);	
			st.fSpeed = 20;
		}
		break;
	case stCharacterStruct::Magic_Effect_FadeIn:	// µ­Èë
		{
			stFade st(false);
			//st.nStart = 128;
			Fade(st);
		}
		break;
	case stCharacterStruct::Magic_Effect_FadeOut:	// µ­³ö
		{
			stFade st(true);
			//st.nEnd = 128;
			Fade(st);
		}
		break;		
	}

	for (int i=0; i<MAX_EVENT; i++)
		m_stProperty.aEvent[i].nMagicType = GetType();	
	return true;
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cMagic::IsDie()
{
	if (GetLife() > 0)
		return false;
	return 
		find_if( m_listFollow.begin(),m_listFollow.end(),not1(mem_fun<bool,iMagic>(&iMagic::IsDie)) ) == m_listFollow.end();	
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param listGenerate : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cMagic::Generate(tagListMagicGenerate& listGenerate) 
{
	if (GetLife() < 0)
		return 0;
	if (m_pShadow)
		m_pShadow->Generate(listGenerate);

	for (vector<iMagic*>::iterator it = m_listFollow.begin(); it != m_listFollow.end(); ++it)
	{
		(*it)->Generate(listGenerate);
	}

	tagListMagicGenerate listMagic;
	for (int i=0; i<MAX_GENERATE; i++)
	{
		m_stProperty.aGenerate[i].GenerateByLife(this,GetLife(),listMagic);
#ifdef _DEBUG_BOW
		if( 0 == strcmp(this->m_stProperty.GetName(),"¹­¼ı·ÉĞĞ") && listMagic.size() >= 1 )
			int iii = 0 ;
#endif
	}
	for (tagListMagicGenerate::iterator it = listMagic.begin(); it != listMagic.end(); ++it)
	{
		iMagic* p = (*it);
		if (m_nSetLife != 0)
			p->SetLife(m_nSetLife);
		if (p->GetFollowType() == stMoveStruct::Move_Follow_Magic)
		{
			AddFollow(p);
		}
		else if (m_pSender && p->GetFollowType() == stMoveStruct::Move_Follow_Send)
		{
			if(p->GetProperty()->stMagic.eType == Magic_Attack_Ring || p->GetProperty()->stMagic.eType == Magic_Ring)
				m_pSender->AddMagicRing(p);
			else
			{
				//m_pSender->FindAndAddMagic(p);
				m_pSender->AddMagic(p);	

				#ifdef _DEBUG_MAGIC
					cMagic * pMagic  = reinterpret_cast<cMagic*>(p);
					if(pMagic)
						TRACE("ÈËÎï¸úËæÄ§·¨ %s ²úÉúÁË\n",pMagic->m_stProperty.GetName());
				#endif

			}
		}
		else
			listGenerate.push_back(p);
	}
	return !listGenerate.empty();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cMagic::SetShadowPos() 
{
	if (m_pShadow)
	{
		m_pShadow->SetPos(GetPos());
		m_pShadow->SetShadowPos();
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cMagic::OnEnd()
{
	if (m_stProperty.stCharacter.eEndEffect1 != stCharacterStruct::Character_Effect_Null 
		|| m_stProperty.stCharacter.eEndEffect2 != stCharacterStruct::Character_Effect_Null)
	{
		if (GetSender() != NULL)
		{
			GetSender()->OnMagicEnd(this);
			//just use once!
			m_stProperty.stCharacter.eEndEffect1 = stCharacterStruct::Character_Effect_Null;
			m_stProperty.stCharacter.eEndEffect2 = stCharacterStruct::Character_Effect_Null;
		}
	}

	
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param time_eslasped : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int cMagic::Run(long time_eslasped) 
{

	//Trace move pos before change
	switch(GetProperty()->stMove.eFollow)
	{
	case stMoveDataStruct::Move_Follow_Send:
		{
			if(m_pSender)
			{
				if ( m_pSender->IsRide() )//&& (GetProperty()->stMove.eMove == stMoveDataStruct::Move_Static) )
					SetPos( stPointI(m_pSender->GetPos().x, m_pSender->GetPos().y - 20) );
				else
					SetPos(m_pSender->GetPos());
			}
			break;
		}		
	case stMoveDataStruct::Move_Follow_Des:
		if(m_pDes) SetPos(m_pDes->GetPos());
		break;
	case stMoveDataStruct::Move_Follow_Block:
		break;
	}

	if (m_pShadow)
		m_pShadow->Run(time_eslasped);

	m_Animation.KeepImage(time_eslasped,GetPos(),GetCenter());
	m_Animation.Fade(time_eslasped);

	for (std::vector<iMagic* >::iterator it = m_listFollow.begin(); it != m_listFollow.end(); ++it)
	{
		iMagic* p = *it;
		p->Run(time_eslasped);
		std::list<stEvent>& elist = p->GetEventList();
		for (list<stEvent>::iterator ite = elist.begin(); ite != elist.end(); ++ ite)
			OnEvent(*ite);
		p->ClearEventList();
	}
	//for_each(m_listFollow.begin(),m_listFollow.end(),bind2nd(mem_fun1<int,iMagic,DWORD>(&iMagic::Run),time_eslasped));
	if (GetLife() <= 0)
		return 0;

	POINT ptOld = GetPos();
	if (m_pDes)
	{
		switch(m_stProperty.stMove.eMove)
		{
		case stMoveStruct::Move_LineFollow : //¸ú×Ù
			{
				POINT ptDes = m_stProperty.stMove.ptDes;
				POINT ptOff = ptDes - m_pDes->GetPos();
				if (abs((int)ptOff.x) > 2*c_nMagicTileWidth || abs((int)ptOff.y) > 2*c_nMagicTileHeight)
					m_stProperty.stMove.eMove = stMoveStruct::Move_Line;
				else
					m_stProperty.stMove.FlyTo(m_pDes->GetMagicPosition());
			}
			break;
		case stMoveStruct::Move_DropFollow: // ¿ÕÖĞÏÂÂä¸ú×Ù
			{
				POINT ptDes = m_stProperty.stMove.ptDes;
				POINT ptOff = ptDes - m_pDes->GetPos();
				if (abs((int)ptOff.x) > 2*c_nMagicTileWidth || abs((int)ptOff.y) > 2*c_nMagicTileHeight)
					m_stProperty.stMove.eMove = stMoveStruct::Move_Drop;
				else
					m_stProperty.stMove.FlyTo(m_pDes->GetPos());
			}
			break;
		}
	}
	
	int life = m_stProperty.stMove.Move(time_eslasped,m_pDes);
	if(m_pDes)
	{
		switch(m_stProperty.stMove.eFollow)
		{
		case stMoveDataStruct::Move_Follow_Des_Fly:
			{
				stPointI ptDir = m_stProperty.stMove.GetDir();
				int newDir;
				if(m_stProperty.stAnimation.nDir == 16)
				{
					newDir = ::GetDir16(ptDir.x,ptDir.y);
				}
				else
				{
					newDir = ::GetDir8(ptDir.x,ptDir.y);
				}
				if(newDir != GetDir())
				{
					m_Animation.TurnTo(newDir);
				}
			}
			break;
		}
	}

	//if (ptOld != GetPos())
	{
		SetShadowPos();
//		if (m_stProperty.stBlock.IsBreak(Block_Obstacle))
		//afxDump << "MagicPos: y = " << GetPos().y << " x = " << GetPos().x << "\n";
	}
	{
		float fSpeedX,fSpeedY;
		GetSpeed(fSpeedX,fSpeedY);

		POINT ptSoundPos = GetPos();
		if (m_stProperty.stMove.eMove == stMoveStruct::Move_Circle)
		{       // Ğı×ªÔË¶¯·½Ê½Ê±GetPos()ÊÇÏà¶ÔÓÚÈËÎ»ÖÃµÄ	
			switch(GetProperty()->stMove.eFollow)
			{
			case stMoveDataStruct::Move_Follow_Send:
				{
				       if( m_pSender) 
				       {
					       POINT ptCharacterPos = m_pSender->GetPos();
					       ptSoundPos.x += ptCharacterPos.x;
					       ptSoundPos.y += ptCharacterPos.y;
				       }
				}
				break;
			case stMoveDataStruct::Move_Follow_Des:
				{
					if( m_pDes) 
					{
						POINT ptCharacterPos = m_pDes->GetPos();
						ptSoundPos.x += ptCharacterPos.x;
						ptSoundPos.y += ptCharacterPos.y;
					}
				}
				break;
			}
		}

		m_stProperty.stMusic.Run(&m_Sound,ptSoundPos,stPointI(fSpeedX,fSpeedY),m_nInitLife,GetLife(),GetType());
	}
	

	int preFrame = m_Animation.GetDirFrame();
	m_Animation.Run(time_eslasped);
	int nowFrame = m_Animation.GetDirFrame();
	int allframe = m_Animation.GetFrameOfDir();
	for (int i=0; i<MAX_EVENT; i++)
	{
		stEvent& st = m_stProperty.aEvent[i];
		if (st.Run(time_eslasped,preFrame,nowFrame,allframe))
			OnEvent(st);
	}

	if (m_Animation.GetMainAnimation() && m_Animation.GetMainAnimation()->IsDie())
	{
		life = 0;
		m_stProperty.stMove.SetLife(0);
	}

	if (life <= 0)
		OnEnd();

	return life;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void Magic_Shake()
{
	stPointI pt;
	pt.x = rand() % (c_nMagicTileWidth/2) - c_nMagicTileWidth/4;
	pt.y = rand() % (c_nMagicTileHeight/2) - c_nMagicTileHeight/4;

	GetMapScene()->SetAddinOffsetPoint(pt);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fx : ÃèÊö
 * \param fy : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cMagic::SetSpeedAndTurn(float fx,float fy)
{
	m_stProperty.stMove.SetSpeed(fx,fy);
	m_Animation.Turn(stPointI(fx,fy));
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptMapOffset : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cMagic::AddLight(POINT ptMapOffset)
{
	POINT ptScreen = (POINT)GetOvergroundPos() - ptMapOffset;
	return AddLightAtScreen(ptScreen);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptScreen : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cMagic::AddLightAtScreen(POINT ptScreen)
{
	for_each(m_listFollow.begin(),m_listFollow.end(),bind2nd(mem_fun1<bool,iMagic,POINT>(&iMagic::AddLightAtScreen),ptScreen));
	if (m_pLight)
	{
		if (m_Animation.GetMainAnimation() && m_Animation.GetMainAnimation()->IsStoped() == false)
		{
			m_pLight->SetRadius(m_stProperty.stLight.GetRadius(m_Animation.GetMainAnimation()->GetDirFrame(),m_Animation.GetFrameOfDir()));
			m_pLight->SetColor(m_stProperty.stLight.GetColor(m_Animation.GetMainAnimation()->GetDirFrame(),m_Animation.GetFrameOfDir()));
		}

		m_pLight->DrawAtScreen(ptScreen + m_stProperty.stLight.GetOffset());
	}
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param e : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cMagic::IsBreak(stBlockStruct::BlockTypeEnum e) 
{
	//µ±ÓĞÄ¿±êµÄÊ±ºò£¬²»¸úÕÏ°­Åö×²
	if (e == stBlockStruct::Block_Obstacle && GetTarget() != NULL)
		return false;
	return m_stProperty.stBlock.IsBreak(e);
};			

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param p : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cMagic::IsBreak(iCharacter* p)
{
	//Assert(p);
	if (!m_stProperty.stBlock.bBlock)
		return false;

	if(p == NULL) 
	{
		if(m_stProperty.stBlock.eBlock & stBlockDataStruct::Block_Obstacle)
			return m_stProperty.stBlock.IsBreak(p,GetPos());
		return false;
	}

	switch(m_stProperty.stBlock.eBlock) 
	{
	case stBlockStruct::Block_Obstacle:
		return false;
	case stBlockStruct::Block_Self:
		if (p != GetSender())
			return false;
	case stBlockStruct::Block_Target:
		if (p != GetTarget())
			return false;
	}
	return m_stProperty.stBlock.IsBreak(p,GetPos());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param st : ÃèÊö
 * \param iStartSpeed : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cMagic::Fly(const stFly& st,int iStartSpeed)
{
	SAFE_RELEASE(m_pSender);
	SAFE_RELEASE(m_pDes);
	
	m_pSender = st.pSender;
	m_pDes = st.pDes;
	
	if (m_pSender != NULL)
		m_pSender->AddRef();
	if (m_pDes != NULL)
		m_pDes->AddRef();

	m_stProperty.stMove.Fly(st,iStartSpeed);

	m_Animation.Turn(m_stProperty.stMove.GetDir());

	if (m_pShadow)
		m_pShadow->m_Animation.Turn(m_stProperty.stMove.GetDir());

	for(size_t i = 0;i< MAX_GENERATE;i++)
		m_stProperty.aGenerate[i].ptInitPos = GetPos();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
size_t cMagic::GetDir8()
{
	POINT ptDir = m_stProperty.stMove.GetDir();
	return ::GetDir8(ptDir.x,ptDir.y);
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
size_t cMagic::GetDir16()
{
	POINT ptDir = m_stProperty.stMove.GetDir();
	return ::GetDir16(ptDir.x,ptDir.y);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dir : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cMagic::TurnTo(size_t dir)
{
	return m_Animation.TurnTo(dir);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
size_t cMagic::GetDir()
{
	if (!m_Animation.IsEmpty())
		return m_Animation.GetDir();
	if (m_pSender != NULL)
		return m_pSender->GetDir();
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
#ifdef _GAME_TOOL
size_t cMagic::GetDirFrame()
{
	return m_Animation.GetDirFrame();
}

size_t cMagic::GetFrameOfDir()
{
	return m_Animation.GetFrameOfDir();
}
#endif
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
bool g_bHintError = true;
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö : Èç¹ûÄ³¸öÄ§·¨ÊÇ¸úËæÄ³¸öcharacter£¬»òÕßÊÇÄ³¸öcharacterµÄ¸úËæÄ§·¨Éú³ÉµÄ×ÓÄ§·¨»òÕßShadowÎïÌå(ÔË¶¯ÊôĞÔÒ»ÖÂ)
 *            ,ÄÇÃ´Õâ¸öÄ¥·¨µÄÔË¶¯·½Ê½±ØĞëÊÇĞı×ª»òÕß¾²Ö¹²»¶¯
 * \param ptScreen : ÃèÊö
 * \param ptMagic : ÃèÊö
 * \param e : ÃèÊö
 * \param dir : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cMagic::DrawSort(POINT ptScreen, POINT ptMagic,stMoveStruct::SortEnum e, int dir)
{
	if ( !GetMapScene()->IsNeedDrawMagicEffect() )
		return false;

	bool bDraw =false;
 
	if (m_stProperty.stMove.eMove == stMoveStruct::Move_Circle)
	{       // Ğı×ªÔË¶¯·½Ê½Ê±GetPos()ÊÇÏà¶ÔÓÚÈËÎ»ÖÃµÄ
		ptScreen.x += GetPos().x;
		ptScreen.y += GetPos().y;
	}
	else
	{       // ¾²Ö¹ÔË¶¯·½Ê½£¬assert´¥·¢¼ûÏêÏ¸ÃèÊö
#ifdef _GAME_TOOL
		if(m_stProperty.stMove.eMove != stMoveStruct::Move_Static)
		{
			if(g_bHintError){
				char szMessage[MAX_PATH];
				sprintf(szMessage,"¡°¸úËæÈËÎïµÄÄ§·¨¡±ÔËĞĞ·½Ê½Ö»ÄÜÊÇ¡°¾²Ö¹¡±»ò¡°Î§ÈÆÈËÎïĞı×ª¡±£¬´íÎóÄ§·¨¡°%s¡±",m_stProperty.GetName());
				MessageBox(GetDevice()->GetHWnd(),szMessage,NULL,MB_OK|MB_ICONERROR);
			}
		}
#else
		Assert(m_stProperty.stMove.eMove == stMoveStruct::Move_Static);
#endif
	}
	if (GetSort() == e)
	{
		if (dir == -1 || GetMaxDir() == dir)
		{
			if (IsBlockGenerate())
				DrawAtScreenA(ptMagic);
			else
				DrawAtScreenA(ptScreen);
			bDraw = true;
		}
	}
	if (m_pShadow)
	{	 
		if (m_pShadow->DrawSort(ptScreen,ptMagic,e,dir))
			bDraw = true;
		 		
	}
	for (vector<iMagic*>::iterator it = m_listFollow.begin(); it != m_listFollow.end(); ++it)
	{
		iMagic* p = *it;
		if (p->DrawSort(ptScreen,ptMagic,e,dir))
			bDraw = true;
	}
	return bDraw;
}


//Ëõ¹Ç
bool cMagic::DrawSort2(POINT ptScreen, POINT ptMagic,stMoveStruct::SortEnum e, int dir)
{
	if ( !GetMapScene()->IsNeedDrawMagicEffect() )
		return false;

	bool bDraw =false;
 
	if (m_stProperty.stMove.eMove == stMoveStruct::Move_Circle)
	{       // Ğı×ªÔË¶¯·½Ê½Ê±GetPos()ÊÇÏà¶ÔÓÚÈËÎ»ÖÃµÄ
		ptScreen.x += GetPos().x;
		ptScreen.y += GetPos().y;
	}
	else
	{       // ¾²Ö¹ÔË¶¯·½Ê½£¬assert´¥·¢¼ûÏêÏ¸ÃèÊö
#ifdef _GAME_TOOL
		if(m_stProperty.stMove.eMove != stMoveStruct::Move_Static)
		{
			if(g_bHintError){
				char szMessage[MAX_PATH];
				sprintf(szMessage,"¡°¸úËæÈËÎïµÄÄ§·¨¡±ÔËĞĞ·½Ê½Ö»ÄÜÊÇ¡°¾²Ö¹¡±»ò¡°Î§ÈÆÈËÎïĞı×ª¡±£¬´íÎóÄ§·¨¡°%s¡±",m_stProperty.GetName());
				MessageBox(GetDevice()->GetHWnd(),szMessage,NULL,MB_OK|MB_ICONERROR);
			}
		}
#else
		Assert(m_stProperty.stMove.eMove == stMoveStruct::Move_Static);
#endif
	}
	if (GetSort() == e)
	{
		if (dir == -1 || GetMaxDir() == dir)
		{
			if (IsBlockGenerate())
				DrawAtScreenA2(ptMagic);
			else
				DrawAtScreenA2(ptScreen);
			bDraw = true;
		}
	}
	if (m_pShadow)
	{	 
		if (m_pShadow->DrawSort2(ptScreen,ptMagic,e,dir))
			bDraw = true;
		 		
	}
	for (vector<iMagic*>::iterator it = m_listFollow.begin(); it != m_listFollow.end(); ++it)
	{
		iMagic* p = *it;
		if (p->DrawSort2(ptScreen,ptMagic,e,dir))
			bDraw = true;
	}
	return bDraw;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param st : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cMagic::OnEvent(const stEvent& st)
{
	m_listEvent.push_back(st);
	return true;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param typeMagic : Ä§·¨id
 * \param type2Magic : ±í
 * \return ·µ»ØÖµµÄÃèÊö
 */
EXPORT_API iMagic* Magic_CreateMagic(size_t typeMagic,size_t type2Magic)
{
	//cMagic* pMagic = new cMagic(type2Magic);

#ifdef _DEBUG
	s_bNewMagic = true;
#endif
	
	//cMagic* pMagic = LOOKASIDE_PALLOC(s_allocMagic,type2Magic);
	cMagic* pMagic = new cMagic(type2Magic);
#ifdef _DEBUG
	s_bNewMagic = false;
#endif

	if(typeMagic == 12)
	{
		int iiii=0;
	}
	pMagic->SetType(typeMagic);
	pMagic->SetTable(type2Magic);
	MagicProperty* p = Property_GetTableManager()->GetMagicProperty(typeMagic,type2Magic);
	if (p)
	{
		if(stricmp(p->strName,"null") == 0)
		{
			Assert(0);
		}
		pMagic->SetParam(*p);
		return pMagic;
	}

	pMagic->Release();
	//delete pMagic;
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szMagic : ÃèÊö
 * \param type2Magic : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
EXPORT_API int Magic_GetMagicID(LPCSTR szMagic,size_t type2Magic)
{
	int type = Property_GetTableManager()->GetiPropertyTable(Property_Magic,type2Magic)->FindType(szMagic);
	return type;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szMagic : ÃèÊö
 * \param type2Magic : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
EXPORT_API iMagic* Magic_CreateMagic(LPCSTR szMagic,size_t type2Magic)
{
	if(stricmp(szMagic,"null") == 0)
	{
		Assert(0);
	}

	int type = Property_GetTableManager()->GetiPropertyTable(Property_Magic,type2Magic)->FindType(szMagic);
	if (type < 0)
		return NULL;
	return Magic_CreateMagic(type,type2Magic);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
POINT cMagic::GetOvergroundPos()
{
	switch(m_stProperty.stMove.eMove)
	{
	case stMoveStruct::Move_Parabola:
		return m_stProperty.stMove.GetOvergroundPos();
	case stMoveStruct::Move_DropFollow:
	case stMoveStruct::Move_Drop:
	case stMoveStruct::Move_DropHere:
	case stMoveStruct::Move_DropStop:
		{
			POINT pt = {GetPos().x,m_stProperty.stMove.ptDes.y};
			return pt;
		}
	}
	return GetPos();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
POINT cMagic::GetSortPos()
{
	switch(m_stProperty.stMove.eMove)
	{
	case stMoveStruct::Move_DropFollow:
	case stMoveStruct::Move_Drop:
	case stMoveStruct::Move_DropHere:
	case stMoveStruct::Move_DropStop:
		{
			POINT pt = {GetPos().x,m_stProperty.stMove.ptDes.y + 8};
			return pt;
		}
	case stMoveStruct::Move_Parabola:
		{
			POINT pt = GetPos();
			pt.y -= atol(m_stProperty.stMove.strParam4);
			return pt;
		}
	}
	switch(m_stProperty.stMove.eSort)
	{
	case stMoveStruct::Sort_Lowest:
		{
			POINT pt = GetPos();
			pt.y -= 1000;
			return pt;
		}
		break;
	case stMoveStruct::Sort_Front:
		{
			POINT pt = GetPos();
			pt.y += 1000;
			return pt;
		}
		break;
	case stMoveStruct::Sort_Foot:
	case stMoveStruct::Sort_Back:
		{
			POINT pt = GetPos();
			pt.y -= 8;
			return pt;
		}
		break;
	}
	if(m_stProperty.stAnimation.nSortAddY)
	{
		POINT pt = GetPos();
		pt.y += m_stProperty.stAnimation.nSortAddY;
		pt.y += 8;
	}

	POINT pt = GetPos();
	pt.y += 8;
	return pt;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
POINT cMagic::GetDefaultCenter()
{
	POINT ptCenter = stPointI(-1,-1);
	iAnimation* m_pAnimation = m_Animation.GetMainAnimation();
	if (m_pAnimation == NULL)
		return ptCenter;
	int w=0;
	int h=0;
	if (!m_pAnimation->GetWholeSize(w,h))
		return ptCenter;

	static int c_MagicRockID = -1;
	static int c_MagicRockID2 = -1;

	if( w == 512 && h == 512 )
	{
		ptCenter.x = -256;
		ptCenter.y = -256-22; //278 -- 308??
	}
	else if( w == 256 && h == 256)
	{
		ptCenter = stPointI(-128,-150);
	}
	else
	{
		ptCenter.x = -w/2;
		ptCenter.y = -h/2;
	}
	return ptCenter;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT cMagic::InvalidateDeviceObjects()
{
	m_Animation.InvalidateDeviceObjects();
	std::for_each(m_listFollow.begin(),m_listFollow.end(),std::mem_fun<HRESULT,iMagic>(&iMagic::InvalidateDeviceObjects));
	if (m_pShadow)
		m_pShadow->InvalidateDeviceObjects();
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT cMagic::RestoreDeviceObjects()
{
	m_Animation.RestoreDeviceObjects();
	std::for_each(m_listFollow.begin(),m_listFollow.end(),std::mem_fun<HRESULT,iMagic>(&iMagic::RestoreDeviceObjects));	
	if (m_pShadow)
		m_pShadow->RestoreDeviceObjects();
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pSender : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cMagic::SetSender(iCharacter* pSender)
{
	SAFE_RELEASE(m_pSender);
	if (pSender)
	{
		pSender->AddRef();
		m_pSender = pSender;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
POINT cMagic::GetCenter()
{
	if (m_ptCenter == stPointI(-1,-1))
		m_ptCenter = GetDefaultCenter();
	return m_ptCenter;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool cMagic::CanAttackTarget()
{
	if (m_pDes == NULL)
		return false;
	//¼ÆËãÒ»Â·ÉÏµÄ×èµ²£¿
	if (GetMoveType() == stMoveDataStruct::Move_Line 
		|| GetMoveType() == stMoveDataStruct::Move_LineFollow)
	{
		return true;		
	}
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void cMagic::RemoveSubMagic()
{
	if(m_pShadow)
	{
		m_pShadow->RemoveSubMagic();
		m_pShadow->Release();
		m_pShadow = NULL;
	}
	for(std::vector<iMagic*>::iterator it = m_listFollow.begin(); it != m_listFollow.end();it++)
	{
		if((*it))
		{
			(*it)->RemoveSubMagic();
			(*it)->Release();
			(*it) = NULL;
		}
	}
	m_listFollow.resize(0);
}

bool cMagic::IsHasChild()
{
	for( int i = 0 ; i < MAX_GENERATE; i++ )
	{
		if( stGenerateDataStructA::Generate_Geometry_Null == m_stProperty.aGenerate[i].eGenerateGeometry)
			continue;
		if( stGenerateDataStructA::Generate_Event_Null == m_stProperty.aGenerate[i].eGenerateEvent)
			continue;
		if( 0 == m_stProperty.aGenerate[i].nGenerateTime )
			continue;
		if( stGenerateDataStructA::Generate_Speed_Null == m_stProperty.aGenerate[i].eGenerateSpeed)
			continue;
		if( (0 == strcmp(m_stProperty.aGenerate[i].strGenerateType,"")) || ( 0 == strcmp(m_stProperty.aGenerate[i].strGenerateType,"null") ) )
		        continue;
		break;
	}
	if( i == MAX_GENERATE)
	{
		return false;
	}
	return true;
      
}
bool cMagic::IsHasFollow()
{
	if( (0 ==strcmp(m_stProperty.stGenerate.strGenerateShadow,"")) || (0 ==strcmp(m_stProperty.stGenerate.strGenerateShadow,"null")))
	{
		return false;
	}
	return true;
}