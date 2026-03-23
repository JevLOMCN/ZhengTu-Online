#include "public.h"
#include "../gui/include/guiTypes.h"
#include ".\gamecursor.h"
#include "./GameAppation.h"
#include "./resource.h"
#include "../engine/include/IGraphicCursor.h"

CGameCursor theGameCursor;

struct stCursorInfo{
	stPointI ptHot;
	int		 pic;
	HCURSOR  hCursor;
};

class cCursorHandleSet{
	std::map<int,HCURSOR> Handles;
public:
	cCursorHandleSet(){
		/*Handles[0] = LoadCursor(GetModuleHandle(NULL),MAKEINTRESOURCE(IDC_NORMAL));
		Handles[2] = LoadCursor(GetModuleHandle(NULL),MAKEINTRESOURCE(IDC_PICK_UP));
		Handles[3] = LoadCursor(GetModuleHandle(NULL),MAKEINTRESOURCE(IDC_ATTACK));
		Handles[5] = LoadCursor(GetModuleHandle(NULL),MAKEINTRESOURCE(IDC_PACKAGE));
		Handles[12] = LoadCursor(GetModuleHandle(NULL),MAKEINTRESOURCE(IDC_ATTACK_EX));
		Handles[13] = LoadCursor(GetModuleHandle(NULL),MAKEINTRESOURCE(IDC_GARROW));*/
	}
	~cCursorHandleSet(){
		for(std::map<int,HCURSOR>::iterator it = Handles.begin(); it != Handles.end(); ++it){
			DestroyIcon(it->second);
		}
		Handles.clear();
	}
	HCURSOR getCursorHandle(int pic){
		std::map<int,HCURSOR>::iterator it = Handles.find(pic);
		if(it == Handles.end()){
			return NULL;
		}
		return it->second;
	}

};

//static cCursorHandleSet curHandles;
static HCURSOR getCursorHandle(int pic){
	//return curHandles.getCursorHandle(pic);
	return NULL;
}

//0000：4,4
//0001：15,15
//0002：8,8
//0003：2,2
//0004：15,15
//0005：15,15
//0006：3,11
//0007：2,2
//0010：15,15
//0011：9,10
//0012：2,2
//0013：0,0
static stCursorInfo s_CursorInfos[eCursorNum]=
{
	{stPointI(4,4),0,getCursorHandle(0)},// 正常    
	{stPointI(15,17),5,getCursorHandle(5)},// 交易状态
	{stPointI(3,3),3,getCursorHandle(3)}, // eCursorAttack          
	{stPointI(4,4),0,getCursorHandle(0)}, // eCursorWorkDig
	{stPointI(4,4),0,getCursorHandle(0)}, // eCursorWorkLumber
	{stPointI(4,4),0,getCursorHandle(0)}, // eCursorWorkGather
	{stPointI(4,4),0,getCursorHandle(0)}, // eCursorSetAccess
	{stPointI(4,4),0,getCursorHandle(0)}, // eCursorSetAccessInEmotion
	{stPointI(4,4),0,getCursorHandle(0)},// 情感    
	{stPointI(4,4),0,getCursorHandle(0)},// 任命    
	{stPointI(4,4),0,getCursorHandle(0)},// 好友    
	{stPointI(4,4),0,getCursorHandle(0)},// 招收队员
	{stPointI(16,16),2,getCursorHandle(2)},// 拾取物体
	{stPointI(1,3),0,getCursorHandle(0)},// 出口    
	{stPointI(1,3),1,getCursorHandle(1)},// 说话    
	{stPointI(27,22),0,getCursorHandle(0)},// 传送点  
	{stPointI(8,16),4,getCursorHandle(4)},	// 修理
	{stPointI(10,10),11,getCursorHandle(11)},// 查看  
	{stPointI(20,20),10,getCursorHandle(10)},// 跟随
	{stPointI(16,16),6,getCursorHandle(6)}, // Use access
	{stPointI(16,16),6,getCursorHandle(6)}, // Find access
	{stPointI(3,3),12,getCursorHandle(12)},	// attack type==21 special NPC
	{stPointI(1,3),13,getCursorHandle(13)}	//Move while mouse move
	
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CGameCursor* GetGameCursor()
{
	FUNCTION_BEGIN;

	return &theGameCursor;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CGameCursor::CGameCursor(void)
: m_imageType(eCursorNone)
{
	FUNCTION_BEGIN;

	CIni ini;
	ini.Open("data\\hcursor\\hot.ini");
	for(size_t i=0; i<ini.GetKeyCount("hot");++i){
		std::string str = ini.GetKeyValue(0,i);
		std::string strNum = ini.GetKeyName(0,i);
		mHots[atol(strNum.c_str())] = stPointI(str.c_str());
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CGameCursor::~CGameCursor(void)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param type : 描述
 * \param image : 描述
 * \return 返回值的描述
 */
void CGameCursor::SetCursor(enumCursorType type,enumCursor image)
{
	FUNCTION_BEGIN;

	if(type == m_eType && m_imageType == image)
		return;
	if(image >= count_of(s_CursorInfos))
		return;
	m_eType = type;
	m_imageType = image;
	stResourceLocation rl;
	rl.SetFileName("data\\cursor.gl");
	rl.group = 0;
	rl.frame = s_CursorInfos[image].pic;

	//Engine_WriteLog("SetCursorImage ........\n");
	std::map<int,stPointI>::iterator it = mHots.find(s_CursorInfos[image].pic);
	stPointI ptHot(0,0);
	if(it != mHots.end())
		ptHot = it->second;

	SetImage(&rl,ptHot);

	FUNCTION_END;
}

void CGameCursor::SetCursorTypeEx(const enumCursor& type)
{
	m_CursorType = type;
	if( type == eCursorNormal )
	{
		stResourceLocation rl;
		rl.SetFileName("data\\cursor.gl");
		rl.group = 0;
		rl.frame = s_CursorInfos[0].pic;

		std::map<int,stPointI>::iterator it = mHots.find(s_CursorInfos[0].pic);
		stPointI ptHot(0,0);
		if(it != mHots.end())
			ptHot = it->second;

		SetImage(&rl,ptHot);
	}
}

HCURSOR CGameCursor::GetCursorHandle(const stResourceLocation * rl)
{
	/*if(GetGameApplication()->GetConfig()->bHardwareCursor){
		char szFileName[MAX_PATH];

		strcpy(szFileName,rl->szFileName);
		strlwr(szFileName);

		if(strstr(szFileName,"data\\cursor."))
			return curHandles.getCursorHandle(rl->frame);
		return NULL;
	}*/
	return NULL;
}

HCURSOR CGameCursor::GetNormalCursorHandle()
{
	/*if(GetGameApplication()->GetConfig()->bHardwareCursor){
		return curHandles.getCursorHandle(0);
	}*/
	return NULL;
}

void CGameCursor::OnSetHardwareCursor(const stResourceLocation* rl,stPointI ptHot)
{
	m_bHardwareCursor = false;

	if(GetGameApplication()->GetConfig()->bHardwareCursor && GetDevice()->IsSupportHardwareCursor()){
		IBitmaps * pBmps = GetDevice()->FindBitmaps(rl);
		if(pBmps){
			m_bHardwareCursor = GetGraphicCursor()->loadCursor(pBmps,ptHot);
		}
		/*char szFileName[MAX_PATH];
		strcpy(szFileName,rl->szFileName);
		strlwr(szFileName);
		if(strstr(szFileName,"data\\cursor.")){
			sprintf(szFileName,"data\\hcursor\\%04d.tga",rl->frame);
			m_bHardwareCursor = GetGraphicCursor()->loadCursor(szFileName,ptHot);
		}*/
	}

	//ShowCursor(m_bHardwareCursor);
}