#include "../engine/include/engine.h"
#include "imagic.h"
#include "ccharacter.h"


stCharacterSortStruct*	s_pCharacterStruct	=	NULL;	// ÈËÎï»æÖÆË³ĞòÊı¾İ
int					s_nStructCount		=	0;	// ÖÖÀàÊı
BYTE cCharacter::m_aOrder[c_nMaxDir][Ani_Part_Num];
BYTE cCharacter::m_aOnRideOrder[c_nMaxDir][Ani_Part_Num];
int	 cCharacter::s_aRideOrder[c_nMaxDir];
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
/*
bool LoadCharacterStructData(void)
{
	return false;
	iZipFile * file = Unzip_GetZipManager()->GetZipFile("datas.zip");
	assert(file);
	if(!file->Open("Datas\\Character.cep"))
	{
		return false;
	}

	stCharacterFileHeader fi;
	file->Read(&fi,sizeof(fi));

	if(fi.lVersion != CEP_VERSION)
	{
		Assert(0);
		file->Close();
		return false;
	}
	s_pCharacterStruct = new stCharacterSortStruct[fi.lCharacterCount];
	s_nStructCount = fi.lCharacterCount;
	file->Read(s_pCharacterStruct,fi.lCharacterCount * sizeof(stCharacterStruct));
	file->Close();
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void FreeCharacterStructData(void)
{
	delete [] s_pCharacterStruct;
	s_pCharacterStruct = NULL;
	s_nStructCount = 0;
}
//*/
//EXPORT_API iCharacter* Magic_CreateCharacter()
//{
//	return new cCharacter;
//}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param type : ÃèÊö
 * \param nAni : ÃèÊö
 * \param dir : ÃèÊö
 * \param frame : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
BYTE* GetCharacterOrder(int type, int nAni, int dir, int frame)
{
	frame = 0;
	if (s_pCharacterStruct == NULL)
		return NULL;
	for(int i=0;i<s_nStructCount;i++)
	{
		if(s_pCharacterStruct[i].dwTypeID == type)
		{
			return s_pCharacterStruct[i].stActions[nAni].stFrames[dir][frame].aPartOrder;
		}
	}
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param type : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
stCharacterSortStruct* GetCharacterOrder(int type)
{
	if (s_pCharacterStruct == NULL)
		return NULL;
	for(int i=0;i<s_nStructCount;i++)
	{
		if(s_pCharacterStruct[i].dwTypeID == type)
		{
			return &s_pCharacterStruct[i];
		}
	}
	return NULL;
}
