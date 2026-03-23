#include "../engine/include/engine.h"
#include "canimation.h"
#include "iMagic.h"
#include "iCharacter.h"
#include "math.h"
#include "MapScene.h"
#include "../engine/include/lookaside_alloc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


float fAlpha = 11.25f*PI/180.f;
const float tg1 = (float)tan(fAlpha*1);
const float tg2 = (float)tan(fAlpha*2);
const float tg3 = (float)tan(fAlpha*3);
const float tg4 = (float)tan(fAlpha*4);
const float tg5 = (float)tan(fAlpha*5);
const float tg6 = (float)tan(fAlpha*6);
const float tg7 = (float)tan(fAlpha*7);


//std::lookaside_allocator<cAnimation> gAllocMagicAnimation;

//////////////////////////////////////////////////////////
// 将cr的alpha值减少c
//////////////////////////////////////////////////////////
void DecColor(DWORD& cr,int c)
{
	int a = cr >> 24;
	if (a < c)
	{
		cr = 0;
		return ;
	}
	a -= c;
	cr = (cr&0x000ffffff)|((DWORD)a << 24);
}

cAnimation::cAnimation()
{
	m_ulCount = 1;                 // 引用计数
	m_nDir = 0;                    // 动画方向

	m_nTimeMS = 0;                 // 动画消耗时间 
	m_nOverTime = 0;

	m_crColor = 0xffffffff;
	
	m_bLooped = false;

	m_nPack = c_nInvalidPack;      //图包属于哪个角色

	m_pTexture = NULL;             
	m_nFrameOfDir = 0;             //一个方向多少帧
	m_nFrame = 0;                  //当前帧
	m_nFrame0 = 0;                 //当前方向的起始帧

	m_bMirror = false;             
	m_nAni = 0;
	m_bBack = false;
	m_pRes = NULL;
	m_strType[0] = 0;
	m_strName[0] = 0;
}

cAnimation::~cAnimation(void)
{
	Clear();
}

iAnimation* cAnimation::Clone()
{
	cAnimation* p = new cAnimation();
	//cAnimation* p = LOOKASIDE_ALLOC(gAllocMagicAnimation);
	p->m_bMirror = m_bMirror;
	p->m_pTexture = m_pTexture;
	p->m_pRes = m_pRes;
	p->m_crColor = m_crColor;
	p->m_nPack = m_nPack;
	p->m_eUsage = m_eUsage;
	return p;
}

void cAnimation::Clear()
{
	m_pTexture = NULL;
	m_pRes = NULL;
}

void cAnimation::UpdateAnimation()
{
	LoadFrame();
}


class cPackInfo{
	typedef std::vector<IBitmapList*> tPackInfo;
	tPackInfo info;
public:
	void init(const char* filename,TextureUsageType usage){
		for( int i=1; i<32; ++i )
		{
			char szRes[MAX_PATH];
			if(i == 1)
				sprintf(szRes,"data\\%s.gl",filename);
			else
				sprintf(szRes,"data\\%s%d.gl",filename,i);

			IBitmapList* bmpList = (IBitmapList*)GetDevice()->FindResource( szRes,c_nMagicResourceFlags,usage );

			if(bmpList)
				info.push_back(bmpList);
			else 
				break;
		}
	}

	IBitmapList* getGroupPack(DWORD groupID){
		for(size_t i=0; i<info.size();++i){
			if(info[i]->IsExistGroup(groupID))
				return info[i];
		}
		return NULL;
	}
};

class cPackInfos{
	typedef stdext::hash_map<std::string,cPackInfo*> tMapPackInfo;
	tMapPackInfo infos;
public:
	cPackInfos()
	{
	}
	~cPackInfos()
	{
		for(tMapPackInfo::iterator it = infos.begin(); it != infos.end(); ++it){
			delete it->second;
		}
		infos.clear();
	}
	IBitmapList* getGroupPack(const char* filename,DWORD groupID,TextureUsageType usage)
	{
		std::pair<tMapPackInfo::iterator,bool> pr = infos.insert(tMapPackInfo::value_type(filename,NULL));
		cPackInfo* pPackInfo;
		if(pr.second){
			pr.first->second = new cPackInfo;
			pr.first->second->init(filename,usage);
		}

		pPackInfo = pr.first->second;

		return pPackInfo->getGroupPack(groupID);
	}
};

static cPackInfos* getPackInfos(){
	static cPackInfos infos;
	return &infos;
}

/**
 * \brief 设置动画所属的角色为n,并加载好对应的图片(该角色所有动作的图片)
 */
bool cAnimation::SetAnimationID(size_t n)
{
	Assert(m_eUsage >=0 && m_eUsage < Usage_Num);
	m_nPack = n;		// 动画所属角色
	if (IsAnimationInvalid())
	{
		Clear();
		return true;
	}
	strlwr(m_strType);
	m_pRes = getPackInfos()->getGroupPack(m_strType,m_nPack,m_eUsage);

	if(m_pRes == NULL)
	{
		static bool s_bMessage = false;
		if(!s_bMessage)
		{
			//GetMapScene()->MessageBox("警告：您的图库文件不全。请确保运行最新的客户端！");
			s_bMessage = true;
		}
	}

	return true;
}

/**
 * \brief 该动画所属的角色
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
size_t cAnimation::GetGroup()
{
	return m_nGroup;
	//return m_stAnimation.nGroup;// character.mdb 中　“动画group”字段
}

/**
 * \brief 加载当前动作的当前帧图片,之后，只要等显示就可以了
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void cAnimation::LoadFrame()
{
	if(m_bStopped && GetType() == stAnimationStruct::Animation_NoDisplay)
		return;

	if (IsAnimationInvalid())
		return;
	size_t nGroup = GetGroup();// 得到该动作所属角色

	if(m_pRes == NULL)
		return;

	int nFrame;
	if(m_stAnimation.eOrder == stAnimationDataStruct::AniPlayOrder_Desc)// "播放顺序"
		//  每个方向上有m_nFrameOfDir-1帧，m_nFrame当前帧,m_nFrame0为当前方向上的起始帧
		//  所以m_nFrameOfDir - 1 - (m_nFrame - m_nFrame0)代表当前方向的反向顺序
		nFrame = m_nFrameOfDir - 1 - (m_nFrame - m_nFrame0) + m_nFrame0;
	else
		nFrame = m_nFrame;

	m_bMirror = false;
	m_pTexture = m_pRes->LoadFrame(m_nPack,nGroup,nFrame);
}

/**
 * \brief 得到group（代表一个动作）上每个方向的frame数
 * 
 * 详细描述
 * 
 * \param nGroup : 描述
 * \param iDirNum : 方向数
 * \return 返回值的描述
 */
size_t cAnimation::GetFrameOfDir(size_t nGroup,int iDirNum)
{
	if(!m_pRes) return 0;
	return m_pRes->GetFrameFrameCount(m_nPack,nGroup)/iDirNum;
}

/*
 *  \brief:     在pt处画一张图
 *  \param:     当然是相对于屏幕左上角的象素点
 */
///////////////////////////////////////////////////////////////////////////
void cAnimation::Draw(POINT pt)
{
	stPointI ptCenter(0,0);
	Draw(pt,ptCenter);
}

/*
 * \brief :             倘若在pt处显示这个动画(人物重心落在pt处),图片裁减区所覆盖的范围
 * 
 * \param pt :		人物重心落在pt处
 * \param ptCenter :    美工图片中心(本程序处理为重心点的负值)
 * \param rc :         ［输出］人物中心pt处图片裁减区
 */
bool cAnimation::GetDrawRect(POINT pt,POINT ptCenter,RECT & rc)
{
	if (IsAnimationInvalid())
		return false;
	if (m_pTexture)
	{
		if (!m_bMirror)
		{
			pt += ptCenter;
		}
		else 
		{
			pt.y += ptCenter.y;
			pt.x -= ptCenter.x;
		}

		pt += m_pTexture->GetClipOffset();
		rc.left = pt.x;
		rc.top = pt.y;
		rc.right = pt.x + m_pTexture->GetClipWidth();
		rc.bottom = pt.y + m_pTexture->GetClipHeight();
		return true;
	}
	else 
	{
		LoadFrame();
		if (m_pTexture)
			return GetDrawRect(pt,ptCenter,rc);
	}
	return false;
}

/** 
 * \param pt : 相对于屏幕左上角的像素偏移
 * \param ptCenter : 可能是人物重心位置，也可能传(0,0)仅仅代表在pt处画一幅图
 * \param scale : 缩放系数
 */
void cAnimation::Draw(POINT pt,POINT ptCenter,stPointF scale)
{
	if (IsAnimationInvalid())
		return;
	if (m_pTexture)
	{
		if (!m_bMirror)
		{
			pt += ptCenter;
		}
		else 
		{
			pt.y += ptCenter.y;
			pt.x -= ptCenter.x;
		}

		if(scale.x != 1.0f || scale.y != 1.0f)
		{
			//醉梦 小人!!!!!!!突破瓶颈 处理部分特效歪的
			int w = m_pTexture->GetWidth();
			int h = m_pTexture->GetHeight();
			if(w ==256 && h==256)
			{
				m_pTexture->Render(pt.x-64,pt.y-64,NULL,&scale,m_crColor,(BlendType)m_stAnimation.eBlend);
			}
			else if(w == 300 && h==300)
			{
				m_pTexture->Render(pt.x-52,pt.y+52,NULL,&scale,m_crColor,(BlendType)m_stAnimation.eBlend);
			}
			else if(w == 330 && h==330)
			{
				m_pTexture->Render(pt.x-45,pt.y-45,NULL,&scale,m_crColor,(BlendType)m_stAnimation.eBlend);
			}
			else if(w == 768 && h==768)
			{
				m_pTexture->Render(pt.x+196,pt.y+196,NULL,&scale,m_crColor,(BlendType)m_stAnimation.eBlend);
			}
			else if(w == 800 && h==800)
			{
				m_pTexture->Render(pt.x+72,pt.y+72,NULL,&scale,m_crColor,(BlendType)m_stAnimation.eBlend);
			}
			else if(w ==1024 && h==1024)
			{
				m_pTexture->Render(pt.x+128,pt.y+128,NULL,&scale,m_crColor,(BlendType)m_stAnimation.eBlend);
			}
			else{
				m_pTexture->Render(pt.x,pt.y,NULL,&scale,m_crColor,(BlendType)m_stAnimation.eBlend);
			}
		}
		else
		{
			m_pTexture->Render(pt.x,pt.y,m_crColor,(BlendType)m_stAnimation.eBlend);
		}
	}
	else 
	{
		LoadFrame();
		if (m_pTexture)
			Draw(pt,ptCenter);
	}
}


/*
 * \brief 判断是否是动作的开始
 */
bool cAnimation::IsActionStart()
{
	return m_nFrame == m_nFrame0;// m_nFrame0代表的是当前方向的第一帧(0就有此意)
}

/*
 * \brief 判断是否是动作的最后一帧
 */
bool cAnimation::IsActionEnd()
{
	return m_nFrame == m_nFrame0 + m_nFrameOfDir - 1;// m_nFrameOfDir代表的是一个方向上的帧数
}

/**
 * \brief 判断动画片是否结束
 */
bool cAnimation::IsDie()
{
	switch( GetType() ) 
	{                                               // 对应character.mdb中的"动画类型"字段:
	case stAnimationStruct::Animation_OverLifeOver: // Animation_OverLifeOver代表 动画结束生命结束
	case stAnimationStruct::Animation_OverLifeOver2:// Animation_OverLifeOver2:动画结束保持在最后一帧一段时间生命结束 动画淡出
	case stAnimationStruct::Animation_Stop:
		return m_bStopped;
	default:
		;
	}
	return false;
}

/**
 * \brief 简短描述
 */
bool cAnimation::IsStoped()
{  // 动画类型== 动画结束静止 ||动画类型＝＝动画直接静止在第一帧
	/*if (GetType() == stAnimationStruct::Animation_Stop || GetType() == stAnimationStruct::Animation_Stop0)
		return m_bStopped;
	return false;*/
	return m_bStopped;
}

/**
 * \brief  设置当前方向的当前帧为当前方向的nFrame帧［如:假设一个方向最大有8帧,nFrame则可能为0,1,2,3,4,5,6,7］
 */
void cAnimation::SetDirFrame(size_t nFrame)
{
	if(nFrame >= m_nFrameOfDir)
		nFrame = m_nFrameOfDir-1;
	m_nFrame = nFrame + m_nFrame0;
	LoadFrame();
	m_nTimeMS = 0;
	m_nOverTime = 0;
}


void cAnimation::AddRunTime(long time)
{
	if(IsAnimationInvalid()) return;
	m_nTimeMS += time;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void cAnimation::ToAniEnd()
{
	m_nFrame = m_nFrame0 + m_nFrameOfDir - 1;
	LoadFrame();
}

void cAnimation::CopyState(const cAnimation* pAni)
{
	Assert(pAni);
	Assert(pAni->m_nFrame0 == pAni->m_nFrame0 && pAni->m_nFrameOfDir == pAni->m_nFrameOfDir);
	m_bStopped = pAni->m_bStopped;
	m_nTimeMS = pAni->m_nTimeMS;
	m_nOverTime = pAni->m_nOverTime;
	bool bLoadFrame = m_nFrame != pAni->m_nFrame;
	m_nFrame = pAni->m_nFrame;
	m_nFrameOfDir = pAni->m_nFrameOfDir;
	m_nFrame0 = pAni->m_nFrame0;
	m_bLooped = pAni->m_bLooped;
	m_bBack = pAni->m_bBack;
	m_nDir = pAni->m_nDir;

	switch (GetType())
	{
	case stAnimationStruct::Animation_OverLifeOver2:
		m_crColor = (pAni->m_crColor & 0xff000000) | (m_crColor & 0xffffff);
		break;
	}
	if(bLoadFrame)
		LoadFrame();
}

/**
* \brief 重新加载当前方向上的当前动作m_nAni，默认取第1帧
*/
void cAnimation::Reload()
{
	m_pTexture = NULL;
	Action(m_nAni,m_stAnimation,false);
}

/**
* \brief 设置动画所属角色为pack，并做动作nAni
* 
* 详细描述
* 
* \param szType : 对于NPC,szType= "npc",
*				   对于主角： szType = 	const char* aType[] = 
*					{"shadow\\weaponl","shadow\\weaponr","body",
*                                        "weaponl","weaponr","hair","pattern\\Body1","pattern\\Body2",
*					 "pattern\\weaponl","pattern\\weaponr","npc","pattern\\npc"};
* \param pack : 角色id（该动画将属于这个角色）
* \param nAni : 该角色所拥有动作中的某个动作nAni,注：一个动作包含8/16个方向的帧
* \param stAnimation :character.mdb中表id(000x,表示某个角色)中动作nAni的关于动画的描述
* \param eUsage : 标识
* \return 返回值的描述
*/
bool cAnimation::CreateAnimation(LPCSTR szType, size_t pack, int nAni, const stAnimationStruct& stAnimation,int eUsage)
{
	m_eUsage = (TextureUsageType )eUsage;
	Assert(m_eUsage >=0 && m_eUsage < Usage_Num);
	Assert(m_eUsage >= 0 && m_eUsage < Usage_Num);
	strcpy(m_strName , stAnimation.strAnimation);
	strcpy(m_strType , szType);
	if ( 1028 != pack )
	{
		if (SetAnimationID(pack))	    	    // 设置此cAnimation所属的角色为pack
		{
			if (Action(nAni,stAnimation,false)) // 做动作nAni 
				return true;
		}
	}

	return false;
}
/**
* \brief :	在当前方向上做动作nAni,取第一帧,实际就是加载好这幅图片，只要等待显示就可以了
*               同时计算好初始成员值 
* \param nAni : 要做的动作
* \param stAnimation : 该动作在character.mdb下的一些设置描述
* \param bServerLogic : 描述
*/
bool cAnimation::Action(int nAni,const stAnimationStruct& stAnimation,bool bServerLogic)
{
	if (IsAnimationInvalid())
		return false;
	//only wait will call and call and call!
	if (!bServerLogic && GetAction() == nAni && m_pTexture != NULL)
		return true;

	if(!m_pRes) return false;

	m_nAni = nAni;
	m_stAnimation = stAnimation;
	if(stAnimation.nGroupCount > 1)
		m_nGroup = stAnimation.nGroup + rand()%stAnimation.nGroupCount;
	else
		m_nGroup = stAnimation.nGroup;

	m_bBack = false;
	if (strcmp(m_stAnimation.strAnimation,"null") == 0)
	{
		m_pTexture = NULL;
		return false;
	}

	UINT nFrameOfGroup;
	if( ( nFrameOfGroup = m_pRes->GetFrameFrameCount(m_nPack,GetGroup()) ) == 0 )
	{
		m_pTexture = NULL;
		return false;
	}

	if (GetMaxDir() == 0)
	{
		m_stAnimation.nDir = 8;
	}

	if (nFrameOfGroup < GetMaxDir())
	{
		m_stAnimation.nDir = 1;
	}

	m_nFrameOfDir = nFrameOfGroup / GetMaxDir();

	Assert(m_nFrameOfDir < 256 && m_nFrameOfDir > 0);

	m_nFrame0 = m_nDir * m_nFrameOfDir;
	m_nFrame = m_nFrame0;
	Assert(m_nFrame < 512);

	LoadFrame();
	m_nTimeMS = 0;
	m_nOverTime = 0;
	m_bStopped = false;

	return true;
}

/**
 * \brief 相当于run(time_eslasped)
 * 
 * \param time_eslasped : 
 * \return 返回值的描述
 */
//#define DEBUG_NORMAL2			5	// 普通输出信息
//extern void _DEBUG_MESSAGE( HWND hWnd, int iErrorCode, const char * pMessage, ... );
void cAnimation::ActionNext(long time_eslasped)
{
	if (IsAnimationInvalid())
		return;

//special type!
	if (GetType() == stAnimationStruct::Animation_Stop0)
	{
		m_bStopped = true;
		return;
	}

	if (m_bStopped)
		return;

	m_nTimeMS += time_eslasped;
	if (m_nFrame >= (int)(m_nFrame0 + m_nFrameOfDir-1))
		m_nOverTime += time_eslasped;

	/*switch (GetType())
	{
	case stAnimationStruct::Animation_Loop:
		if(m_stAnimation.nTimeLoop)
		{
			m_stAnimation.nTimeLoop -= time_eslasped;
			if(m_stAnimation.nTimeLoop <= 0)
			{
				m_bStopped = true;
				m_stAnimation.nTimeLoop = 0;
			}
		}
		break;
	}*/

	long fAniSpeed = GetAnimationSpeed();
	//if(fAniSpeed < 1) fAniSpeed = 1;
	if (fAniSpeed > 0 && m_nTimeMS > fAniSpeed)
	{
		int nFrame = m_nFrame;

		if ( GetType() == stAnimationStruct::Animation_Back && m_bBack )
		{
			/*nFrame -= (m_nTimeMS / fAniSpeed);
			m_nTimeMS %= fAniSpeed;*/
			while(m_nTimeMS > fAniSpeed)
			{
				m_nTimeMS -= fAniSpeed;
				nFrame--;
			}
		}
		else
		{
			/*nFrame += (m_nTimeMS / fAniSpeed);
			m_nTimeMS %= fAniSpeed;*/
			while(m_nTimeMS > fAniSpeed)
			{
				m_nTimeMS -= fAniSpeed;
				nFrame++;
			}
		}

		if (m_bBack && nFrame < (int)m_nFrame0)
		{
			Assert(GetType() == stAnimationStruct::Animation_Back);
			if(m_nFrameOfDir >= 2)
				m_nFrame = m_nFrame0 + 1;
			else
				m_nFrame = m_nFrame0;
			m_bBack = false;
		}
		else if (nFrame >= (int)(m_nFrame0 + m_nFrameOfDir))
		{
			switch (GetType())
			{
			case stAnimationStruct::Animation_Back:
				if(m_nFrameOfDir >= 2)
					m_nFrame = m_nFrame0 + m_nFrameOfDir - 2;
				else
					m_nFrame = m_nFrame0;
				m_bBack = true;
				break;
			case stAnimationStruct::Animation_Stop:
				m_nFrame = m_nFrame0 + m_nFrameOfDir - 1;
				m_bStopped = true;
				break;
			case stAnimationStruct::Animation_Loop:
				m_nFrame = m_nFrame0;
				m_bStopped = false;
				break;
			case stAnimationStruct::Animation_OverLifeOver:
				m_nFrame = m_nFrame0 + m_nFrameOfDir - 1;
				m_bStopped = true;
				break;
			case stAnimationStruct::Animation_NoDisplay:
				//m_nFrame = -1;
				m_nFrame = m_nFrame0 + m_nFrameOfDir - 1;
				m_bStopped = true;
				m_pTexture = NULL;
				return;
				break;
			case stAnimationStruct::Animation_OverLifeOver2:	// 动画结束保持在最后一帧一段时间生命结束 动画淡出
				{
					m_nFrame = m_nFrame0 + m_nFrameOfDir - 1;
					//m_stAnimation.nTimeLoop -= time_eslasped;
					//m_nOverTime += time_eslasped;
					//if(m_stAnimation.nTimeLoop <= 0)
					DWORD dwOverTime = m_nOverTime;
					if ( dwOverTime >= (DWORD)m_stAnimation.nTimeLoop)
						m_bStopped = true;
					else
					{
						int alpha = ( (m_stAnimation.nTimeLoop - dwOverTime) * 255 / m_stAnimation.nTimeLoop);
						m_crColor = (m_crColor & 0xffffff) | ( alpha << 24);
						//char szTmp[256];
						//sprintf(szTmp,"****************** alpha : %u, time %u,time_eslasped:%g\n",alpha,dwOverTime,time_eslasped);
						//OutputDebugString(szTmp);
					}
				}
				break;
			case stAnimationStruct::Animation_WaitLoop:
				m_nFrame = m_nFrame0;
				m_bLooped = false;
				break;
			}
		}
		else
		{
			m_nFrame = nFrame;
		}
		Assert(m_nFrame < 512);
//		afxDump << "now frame is " << m_nFrame << "\n";
		LoadFrame();
		//special type!
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x : 描述
 * \param y : 描述
 * \param c_nDir : 描述
 * \param af[] : 描述
 * \return 返回值的描述
 */
int GetDir(float x, float y, int c_nDir, float af[])
{
	y = -y;
	y = y * c_nMagicTileWidth / c_nMagicTileHeight;
	int dir = 1;
	if (x == 0)
		x = 0.00001f;

	int c_nDir2 = c_nDir/2;

	float tga = (float)y / x;

	if (tga < af[0])
	{
		dir = c_nDir2;
	}
	else if (tga >= af[c_nDir2-1])
	{
		dir = 0;
	}
	else
	{
		for (int i=0; i<c_nDir2-1; i++)
		{
			if (tga >= af[i] && tga < af[i+1])
			{
				dir = c_nDir2-1-i;
				break;
			}
		}
	}
	if (x < 0)
		dir += c_nDir2;
	if (dir == c_nDir)
		dir = 0;
	return dir;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x : 描述
 * \param y : 描述
 * \return 返回值的描述
 */
EXPORT_API int GetDir8(int x, int y)
{
	static float af[] = {-tg6,-tg2,tg2,tg6};
	return GetDir(x,y,8,af);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param x : 描述
 * \param y : 描述
 * \return 返回值的描述
 */
EXPORT_API int GetDir16(int x, int y)
{
	static float af[] = {-tg7,-tg5,-tg3,-tg1,tg1,tg3,tg5,tg7};
	return GetDir(x,y,16,af);
}

/**
 * \brief 转到当前幀的背后方向的幀
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void cAnimation::TurnBack()
{
	if (GetMaxDir() == 1)
		return ;
	size_t dir = m_nDir;
	if (GetMaxDir() == 8)
	{
		dir += 4;
		if (dir >= 8)
			dir -= 8;
	}
	else
	{
		return;
	}
	TurnTo(dir);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param n : 描述
 * \return 返回值的描述
 */
void cAnimation::ChangeFrame(size_t n) 
{
	m_nFrame = n ;
	LoadFrame();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param n : 描述
 * \return 返回值的描述
 */
void cAnimation::ChangeFrameOfDir(size_t n) 
{
	m_nFrame = n + m_nFrame0;
	LoadFrame();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dir : 描述
 * \return 返回值的描述
 */
void cAnimation::TurnTo(size_t dir)
{
	if (dir >= GetMaxDir())
		dir = 0;
	if (m_nDir == dir)
		return;
	m_nDir = dir;
	size_t i = m_nFrame - m_nFrame0;
	m_nFrame0 = m_nDir * m_nFrameOfDir;
	m_nFrame = m_nFrame0 + i;
	m_nTimeMS = 0;
	m_nOverTime = 0;
	LoadFrame();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt : 描述
 * \return 返回值的描述
 */
void cAnimation::Turn(POINT pt)
{
	if (GetMaxDir() == 1)
		return ;
	if (pt.x == 0 && pt.y == 0)
		return;
	int dir = 0;
	if (GetMaxDir() == 8)
		dir = GetDir8(pt.x,pt.y);
	else if (GetMaxDir() == 16)
		dir = GetDir16(pt.x,pt.y);
	else
		return;
	TurnTo(dir);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
EXPORT_API iAnimation* Magic_CreateAnimation()
{
	//return LOOKASIDE_ALLOC(gAllocMagicAnimation);
	return new cAnimation;
}

void Magic_DestroyAnimation(iAnimation * p)
{
	delete p;
	//gAllocMagicAnimation.freePointer((cAnimation*)p);
}

/**
 * \brief 位置pt是否落在本动画图片范围内
 * 
 * 详细描述
 * 
 * \param pt : 要判断的位置（为相对于本动画位置的相对值）
 * \param ptCenter : 本动画的中心位置(相对于自己，为负值)
 * \param bJustInRect : 是否根据pt在ClipRect范围内判断，TRUE，是的，FALSE,只有alpha>128才认为
 * \return 返回值的描述
 */
bool cAnimation::IsAt(POINT pt,POINT ptCenter, bool bJustInRect)
{
	//点是否在图片上
	if (IsAnimationInvalid() || !m_pTexture)
		return false;

	POINT ptDraw = pt;
	if (!m_bMirror)
	{
		ptDraw -= ptCenter;
	}
	else 
	{
		ptDraw.y -= ptCenter.y;
		ptDraw.x += ptCenter.x;
	}
	return m_pTexture->IsAt(ptDraw,m_bMirror,bJustInRect);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param p : 描述
 * \param ptOffset : 描述
 * \param radius : 描述
 * \return 返回值的描述
 */
bool cAnimation::IsBreak(iAnimation* p, POINT ptOffset, int radius)
{
	//self is at 0,0 and p is at ptOffset..
	//cAnimation* pA = dynamic_cast<cAnimation*>(p);
	cAnimation* pA = (cAnimation*)(p);
	if (pA == NULL)
		return false;
	if (m_pTexture == NULL || pA->m_pTexture == NULL)
		return false;
	return m_pTexture->IsBreak(pA->m_pTexture,ptOffset,radius);
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
HRESULT cAnimation::InvalidateDeviceObjects()
{
	return S_OK;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
HRESULT cAnimation::RestoreDeviceObjects()
{
	return S_OK;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
stAnimationInfo cAnimation::GetAnimationInfo()
{
	stAnimationInfo info;
	info.bStop = m_bStopped;
	info.nDir = m_nDir;
	info.nFrame = m_nFrame;
	info.nTimeMS = m_nTimeMS;
	info.stAni = m_stAnimation;
	info.nFrame0 = m_nFrame0;
	info.nFrameOfDir = m_nFrameOfDir;
	info.bBack = m_bBack;
	return info;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param info : 描述
 * \return 返回值的描述
 */
void cAnimation::SetAnimationInfo(stAnimationInfo& info)
{
	m_bStopped = info.bStop ;
	m_nDir = info.nDir ;
	m_nFrame = info.nFrame ;
	m_nTimeMS = info.nTimeMS ;
	m_stAnimation = info.stAni;
	m_nFrame0 = info.nFrame0;   // current dir' frame 
	m_nFrameOfDir = info.nFrameOfDir;
	m_bBack = info.bBack;      // if back is true,will play back from the last frame
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param w : 描述
 * \param h : 描述
 * \return 返回值的描述
 */
bool cAnimation::GetWholeSize(int& w,int& h)
{
	if(!m_pTexture) 
	{
		LoadFrame();
		if(!m_pTexture) return false;
	}

	w = m_pTexture->GetWidth();
	h = m_pTexture->GetHeight();
	return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param prect : 描述
 * \return 返回值的描述
 */
bool cAnimation::GetClipRect(RECT* prect)
{
	if(!m_pTexture) 
	{
		LoadFrame();
		if(!m_pTexture) return false;
	}
	*prect = m_pTexture->GetClipRect();
	return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param w : 描述
 * \param h : 描述
 * \return 返回值的描述
 */
bool cAnimation::GetImageSize(int& w,int& h)
{
	if(!m_pTexture) 
	{
		LoadFrame();
		if(!m_pTexture) return false;
	}
	w = m_pTexture->GetClipWidth();
	h = m_pTexture->GetClipHeight();
	return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ani : 描述
 * \return 返回值的描述
 */
bool cAnimation::IsExistAction(int ani)
{
	return m_pRes && m_pRes->GetFrameFrameCount(m_nPack,ani) != 0;
}

ULONG cAnimation::Release() 
{
	int n = --m_ulCount;
	if (!m_ulCount) 
	{
		Magic_DestroyAnimation(this);
	}
	return n;
};