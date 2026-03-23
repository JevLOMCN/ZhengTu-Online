// Magic.cpp : ¶¨Òå DLL µÄ³õÊ¼»¯Àý³Ì¡£
//
#include "../engine/include/engine.h"
#include "../Property/iProperty.h"
#include "iMagic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int Character_InitCharacter();
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class MagicPropertyCallBack: public iPropertyCallBack
{
	iProperty* FormatProperty(const iProperty* pRaw)
	{
		MagicProperty* p = new MagicProperty;
		MagicDataProperty* pP = (MagicDataProperty*) pRaw;
		memcpy(&p->strName,pP->strName,sizeof(pP->strName));
		memcpy(&p->stMagic , &pP->stMagic, sizeof(pP->stMagic));
		memcpy(&p->stMove , &pP->stMove, sizeof(pP->stMove));
		switch (p->stMove.eMove)
		{
		case stMoveDataStruct::Move_LineCircle:
		case stMoveDataStruct::Move_Line:
		case stMoveDataStruct::Move_LineDes:
		case stMoveDataStruct::Move_LineFollow:
		case stMoveDataStruct::Move_Circle:
			p->stMove.stCircle.Init();
			break;
		case stMoveDataStruct::Move_Parabola:
			//p->stMove.stParabola.InitParabola();
			break;
		}
		memcpy(&p->stBlock , &pP->stBlock, sizeof(pP->stBlock));
		memcpy(&p->stGenerate , &pP->stGenerate, sizeof(pP->stGenerate));
		for (int i=0; i<MAX_GENERATE; i++)
			memcpy(&p->aGenerate[i],&pP->aGenerate[i],sizeof(pP->aGenerate[i]));
		memcpy(&p->stCharacter , &pP->stCharacter, sizeof(pP->stCharacter));
		
		memcpy(&p->stAnimation , &pP->stAnimation, sizeof(stAnimationDataStruct));
		p->stAnimation.dwColor = pP->stAnimation.dwColor;
		p->stAnimation.nSortAddY = pP->stAnimation.nSortAddY;
		//p->stAnimation.nAnispeed = pP->stAnimation.nAnispeed1;
		memcpy(&p->stMusic , &pP->stMusic, sizeof(pP->stMusic));
		memcpy(&p->stLight , &pP->stLight, sizeof(pP->stLight));
		for (int i=0; i<MAX_EVENT; i++)
			memcpy(&p->aEvent[i],&pP->aEvent[i],sizeof(pP->aEvent[i]));
		Assert(p->stLight.nColor <= 32);
		Assert(p->stLight.nRadius <= 32);
		return p;
	}
};
#include "icharacter.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CharacterPropertyCallBack: public iPropertyCallBack
{
	iProperty* FormatProperty(const iProperty* pRaw)
	{
		CharacterProperty* p = new CharacterProperty;
		CharacterDataProperty* pP = (CharacterDataProperty*) pRaw;
		memcpy(&p->strName,pP->strName,sizeof(pP->strName));
		memcpy(&p->stAniinfo,&pP->stAniInfo,sizeof(pP->stAniInfo));
		for(size_t i=0;i<CHARACTER_ANI_COUNT;i++)
		{
			memcpy(&p->stAnimation[i],&pP->stAnimation[i],sizeof(stAnimationDataStruct));
			p->stAnimation[i].dwColor = 0xffffffff;
			p->stAnimation[i].nHeight = pP->stAnimation[i].nHeight;
			p->stAnimation[i].nMagicAdjust = pP->stAnimation[i].nMagicAdjust;
			p->stAnimation[i].nSortAddY = 0;
			//p->stAnimation[i].nAnispeed = pP->stAnimation[i].nAnispeed1;
		}
		
		for(int i=0;i<MAX_EVENT;i++)
			memcpy(&p->aEvent[i],&pP->aEvent[i],sizeof(stEventData));

		return p;
	}
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
EXPORT_API int Magic_InitModule()
{	
	static MagicPropertyCallBack s_mpCallBack;
	static CharacterPropertyCallBack s_chCallBack;

	Character_InitCharacter();
	Property_GetTableManager()->CreateProperty(Property_Magic,&s_mpCallBack,"magic");
	Property_GetTableManager()->CreateProperty(Property_Character,&s_chCallBack,"character",9999);
	//extern void LoadCharacterActionPri();
	//LoadCharacterActionPri();
	return 0;
}

