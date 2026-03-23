#include "../engine/include/engine.h"
#include ".\mapsurf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CMapSurfManager* GetMapSurfManager()
{
	static CMapSurfManager sm;
	return &sm;
}

///////////////////////////////////////////
// 海浪对象
///////////////////////////////////////////

#define radian2angle(X) ( (X) * 180.f/ PI )
#define angle2radian(X) ( (X) * PI / 180.f)

struct stRandFloat{
	float minNum;
	float maxNum;
	
	stRandFloat(float i_min,float i_max)
		: minNum(i_min) , maxNum(i_max)
	{
	}

	float GetValue() const{
		if(minNum == maxNum) return minNum;
		return minNum + rand() * (maxNum - minNum) / RAND_MAX;
	}
};

struct stRandNum{
	long minNum;
	long maxNum;

	stRandNum(long i_min,long i_max)
		: minNum(i_min) , maxNum(i_max)
	{
	}

	float GetValue() const{
		if(minNum == maxNum) return (float)minNum;
		return (float)(minNum + rand() % (maxNum - minNum));
	}
};

struct stSurfMoveData{
	stRandFloat initSpeed;
	stRandFloat accel;

	stRandFloat initScaleX;
	stRandFloat initCxSpeed;
	stRandFloat cxAccel;

	stRandFloat initAlpha;
	stRandFloat initAlphaSpeed;
	stRandFloat alphaAccel;
};


      
const stSurfMoveData s_surfData[] = {
	{	stRandFloat(35,75),stRandFloat(-8,-12), 
		stRandFloat(1.0f,1.0f), stRandFloat(0.3f,0.3f), stRandFloat(-0.01f,-0.01f), 
		stRandFloat(0.2f,0.2f), stRandFloat(0.5f,0.5f), stRandFloat(-0.2f,-0.2f)
		//stRandFloat(1,1), stRandFloat(0,0), stRandFloat(0,0)
	},	// 近海

	{	stRandFloat(100,115),stRandFloat(-16,-20), 
		stRandFloat(1.0f,1.0f),	stRandFloat(0.3f,0.3f),	stRandFloat(-0.01f,-0.01f), 
		stRandFloat(0.1f,0.1f), stRandFloat(0.8f,0.8f), stRandFloat(-0.2f,-0.2f)  //远海: 先从无快速渐变到亮, 马上开始淡,然后消失 
	},	// 远海

};

//const  stRandFloat c_distance = stRandFloat(100,200);  // 近海的滩距离
const   double c_distance = 150;
const   double c_pi = 3.1415926535;
extern  BOOL   g_bPosEnable;         
/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	描述: 		  刷新海浪状态
*/
void	CSurfObject::Run(float fTimeElasped)
{
	float oldSpeed = m_speed;
	float oldPos = m_pos;
	float oldAlpha = m_alpha;
                                                    
	m_pos += m_speed * fTimeElasped;                
	m_speed += m_accel * fTimeElasped;              
	m_alpha += m_alphaSpeed * fTimeElasped;         
	m_alphaSpeed += m_alphaAccel * fTimeElasped;      
	m_xScale += m_cxSpeed * fTimeElasped;              
	m_cxSpeed += m_cxAccel * fTimeElasped;              

	
	if( m_type == MapSurfType_Offing  && oldPos < c_distance &&  m_pos >= c_distance) // 近海碰到滩了  [上滩]
	{
		if( m_accel < 0.0f)
			m_accel -= 9.8000f ;
		if( m_speed > 0.0f ) 
			m_speed = m_speed * 1/2 ;    
		double time = (m_speed - 10.0f) / m_accel;

		m_cxSpeed = 0.0f;    //范围不再扩大
	}
	if( m_type == MapSurfType_Offing  && m_pos >= c_distance && m_speed < 10.0f && oldSpeed >= 10.0f)   // 下滩
	{
        if( m_accel < 0.0f && m_speed > 0.0f)
		{
			m_accel += 9.8000f;
			m_speed = - m_speed;
		}
          
		m_alphaAccel = 0.0f;
		m_alphaSpeed = 0.0f;

		m_cxSpeed  = 0.0f;
	}
	
	if( m_type == MapSurfType_Offing && m_pos < c_distance && oldPos >= c_distance)  // 离开滩了
	{
		m_cxSpeed = 0.0f;   // 水波范围从此不变的退下去，逐渐消失
		m_alphaSpeed = - s_surfData[m_type].initAlphaSpeed.GetValue() * 1/2  ;
	}

	if( m_type == MapSurfType_HighSeas && m_pos >= 3  * GRID_HEIGHT &&  oldPos < 3 * GRID_HEIGHT) 
	{
		if( m_alphaSpeed > 0.0f ) m_alphaSpeed = 0.0f;
	}
}

/*
*	函数:		
*	返回值：	
*	参数：		  ptMapOffset : 局部地图的起点的世界位置
*	
*	描述: 		  绘制海浪对象
*/
void	CSurfObject::Draw( POINT ptMapOffset,stPointF * scale)
{
	stPointI ptStart = m_startPos;  
	long pos = (long)m_pos;               
	if( scale )
	{
		ptStart.x = (long)(ptStart.x * scale->x);
		ptStart.y = (long)(ptStart.y * scale->y);
		ptMapOffset.x = (long)(ptMapOffset.x * scale->x);
		ptMapOffset.y = (long)(ptMapOffset.y * scale->y);
		pos = (long)(pos * scale->y);
	}

	GetDevice()->SetRotation(ptStart - ptMapOffset,m_angle);

	stPointI pt(ptStart.x,ptStart.y - pos);

	float fAlpha = m_alpha;             
	if(fAlpha > 1.0f) fAlpha = 1.0f;
	else if(fAlpha < 0.0f) fAlpha = 0.0f;
	int alpha = (int)(fAlpha * 255);

	stPointF scale1(m_xScale,m_xScale);  
	if(scale)
	{
		scale1.x *= scale->x;
		scale1.y *= scale->y;
	}

	pt -= ptMapOffset;

	if(scale)
	{
		pt.x -= (long)(m_bmp->GetBitmap(0)->GetWidth()/2 * m_xScale * scale->x);
		pt.y -= (long)((m_xScale - 1.0f) * m_bmp->GetBitmap(0)->GetHeight() * scale->y);
	}
	else
	{
		pt.x -= (long)(m_bmp->GetBitmap(0)->GetWidth()/2 * m_xScale);
		pt.y -= (long)((m_xScale - 1.0f) * m_bmp->GetBitmap(0)->GetHeight());
	} 
	m_bmp->RenderAni(pt.x,pt.y,xtimeGetTime(),NULL,&scale1,COLOR_ARGB(alpha,255,255,255),Blend_AlphaAdd);

	GetDevice()->IdentityTransform();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
///////////////////////////////////////////
// 海浪发生器
///////////////////////////////////////////
CMapSurfGenerator::CMapSurfGenerator(stSurfData  data,POINT gridPos)
: m_pos(gridPos)
, m_nextRunTime(0)
{
	m_data = data;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CMapSurfGenerator::~CMapSurfGenerator(void)
{
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	描述: 		  海浪发生器运行
*/
void	CMapSurfGenerator::Run(float fTimeElasped)
{
	if( xtimeGetTime() > m_nextRunTime )
	{
		CSurfObject* pObj = new CSurfObject;
		if(!Generate(pObj))
			delete pObj;
		else 
			GetMapSurfManager()->PushSurfObject(pObj);

		stRandNum times[4] ={
			stRandNum(2000,3000),
				stRandNum(3000,4000),
				stRandNum(4000,5000),
				stRandNum(5000,6000),
		};

		if( m_data.byFrequency == 4 )    // 随机
		{
			m_data.byFrequency  = rand() % 4 ;
		}
		DWORD time = (DWORD)times[m_data.byFrequency].GetValue();
		m_nextRunTime = xtimeGetTime() + time;
	}
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	描述: 		  海浪初始化
*/
bool	CMapSurfGenerator::Init(CSurfObject* pObj)
{
	pObj->m_startPos.x = m_pos.x * GRID_WIDTH + GRID_WIDTH/2;
	pObj->m_startPos.y = m_pos.y * GRID_HEIGHT + GRID_HEIGHT/2;
	pObj->m_angle = angle2radian((float)m_data.byAngle);

	pObj->m_type = (enumSurfType)m_data.bySurfType;

	pObj->m_pos = 0.0f;														// 注意这个m_pos 是海浪的对象
	pObj->m_bmp = GetMapSurfManager()->GetImage((enumSurfType)m_data.bySurfType);
	return pObj->m_bmp && pObj->m_bmp->GetFrameCount() != 0;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	描述: 		  产生一个海浪
*/

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pObj : 描述
 * \return 返回值的描述
 */
bool	CMapSurfGenerator::Generate(CSurfObject* pObj)
{
	if(!Init(pObj)) return false;
	if(m_data.bySurfType >= count_of(s_surfData)) return false;

	const stSurfMoveData * pData = &s_surfData[m_data.bySurfType];
	pObj->m_speed = pData->initSpeed.GetValue();
	pObj->m_accel = pData->accel.GetValue();

	pObj->m_alpha = pData->initAlpha.GetValue();
	pObj->m_alphaSpeed = pData->initAlphaSpeed.GetValue();
	pObj->m_alphaAccel = pData->alphaAccel.GetValue();

	pObj->m_xScale = pData->initScaleX.GetValue();
	pObj->m_cxSpeed = pData->initCxSpeed.GetValue();
	pObj->m_cxAccel = pData->cxAccel.GetValue();

	return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
///////////////////////////////////////////
// 海浪管理器
///////////////////////////////////////////
void CMapSurfManager::Init()
{
	m_pBmpList = GetDevice()->FindResource("data\\other.gl",0,Usage_TerrainSmall);
	/*static stSurfData data;
	data.byAngle = 45;
	data.bySurfType = MapSurfType_Offing;
	data.byFrequency = 2;
	AddSurf(stPointI(5,5),&data);*/
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CMapSurfManager::Release()
{
	tParent::Release();

	for(tSurfObjects::iterator it = m_surfObjects.begin();
		it != m_surfObjects.end(); ++it)
	{
		delete (*it);
	}
	m_surfObjects.resize(0);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CMapSurfManager::~CMapSurfManager()
{
	Release();
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param type : 描述
 * \return 返回值的描述
 */
IBitmaps*		CMapSurfManager::GetImage(enumSurfType type)
{
	//return m_pBmpList->GetBitmaps(2,4);
	//return m_pBmpList->GetBitmaps(0,0);
	return m_pBmpList->GetBitmaps(1,0);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptMapOffset : 描述
 * \param scale : 描述
 * \return 返回值的描述
 */
void CMapSurfManager::Draw(POINT ptMapOffset,stPointF * scale)
{
	for(tSurfObjects::iterator it = m_surfObjects.begin();
		it != m_surfObjects.end(); ++it)
	{
		(*it)->Draw(ptMapOffset,scale);
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param time_elasped : 描述
 * \return 返回值的描述
 */
void CMapSurfManager::Run( float time_elasped )
{
	tParent::Run(time_elasped);

	for( size_t i=0;i<m_surfObjects.size(); )
	{
		m_surfObjects[i]->Run(time_elasped);
		if(m_surfObjects[i]->IsDeleted())
		{
			delete m_surfObjects[i];
			SimpleRemoveFromArray(m_surfObjects,i);
		}
		else ++i;
	}
}