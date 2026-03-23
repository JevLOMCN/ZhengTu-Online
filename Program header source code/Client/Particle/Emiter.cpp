/**
 *\file		Emiter.cpp
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:18:00 中国标准时间
 *\brief	粒子发射器
 *
 * 
 *
*/

#include "public.h"
#include "./Emiter.h"


/**
 * \brief 构造函数
 * 
 * 
 * 
 * \param  : 起始角度
 * \param  : 终止角度
 * \return 
 */
//--------------------------Point Emiter--------------------------------------------------
CEmiterPoint::CEmiterPoint( float startAngle /* = 0 */, float endAngle /* = 360 */ )
{
	FUNCTION_BEGIN;

	m_type = PARTICLE_ET_POINT;
	
	//Get angle
	while( startAngle > 360.0f )
		startAngle -= 360.0f;
	while ( startAngle < 0.0f )
		startAngle += 360.0f;

	//Get angle
	while( endAngle > 360.0f )
		endAngle -= 360.0f;
	while ( endAngle < 0.0f )
		endAngle += 360.0f;

	m_fStartAngle = startAngle / 180.0f * D3DX_PI;
	m_fEndAngle = endAngle / 180.0f * D3DX_PI;

	FUNCTION_END;
}

/**
 * \brief 获取起始角度
 * 
 * 
 * 
 * \return 起始角度
 */
float CEmiterPoint::GetStartAngle()
{
	FUNCTION_BEGIN;

	return m_fStartAngle;

	FUNCTION_END;
}

/**
 * \brief 获取终止角度
 * 
 * 
 * 
 * \return 终止角度
 */
float CEmiterPoint::GetEndAngle()
{
	FUNCTION_BEGIN;

	return m_fEndAngle;

	FUNCTION_END;
}

/**
 * \brief 构造函数
 * 
 * 
 * 
 * \param width : 宽度
 * \param angle : 角度
 * \param dir	 : 方向
 * \return 
 */
//--------------------------Line Emiter-----------------------------------------------------
CEmiterLine::CEmiterLine( float width, float angle, int dir	)
{
	FUNCTION_BEGIN;

	m_type = PARTICLE_ET_LINE;

	m_width = width;
	//Get angle
	while( angle >= 180.0f )
		angle -= 180.0f;
	while ( angle < 0.0f )
		angle += 180.0f;

	m_angle = angle / 180.0f * D3DX_PI;

	m_direction = dir;

	FUNCTION_END;
}

/**
 * \brief 构造函数
 * 
 * 
 * 
 * \param  : 半径
 * \param  : 方向
 * \return 
 */
//--------------------------Circle Emiter---------------------------------------------------
CEmiterCircle::CEmiterCircle( float radio /* = 200.0f */, int dir /* = 0 */ )
{
	FUNCTION_BEGIN;

	m_type = PARTICLE_ET_CIRCLE;

	m_radio = radio;
	m_direction = dir;

	FUNCTION_END;
}

/**
 * \brief 构造函数
 * 
 * 
 * 
 * \param  : 半径
 * \param  : 方向
 * \return 
 */
//-------------------------Sphere Emiter-----------------------------------------------------
CEmiterSphere::CEmiterSphere( float radio /* = 200.0f */, int dir /* = 0 */ )
{
	FUNCTION_BEGIN;

	m_type = PARTICLE_ET_SPHERE;

	m_radio = radio;
	m_direction = dir;

	FUNCTION_END;
}

/**
 * \brief 构造函数
 * 
 * 
 * 
 * \param width : 宽度
 * \param height : 高度
 * \return 
 */
//------------------------Plane Emiter--------------------------------------------------------
CEmiterPlane::CEmiterPlane( float width, float height )
{
	FUNCTION_BEGIN;

	m_type = PARTICLE_ET_PLANE;

	m_width = width;
	m_height = height;

	FUNCTION_END;
}

//------------------------------Point List Emiter---------------------------------------------------
CEmiterPointList::CEmiterPointList(void)
: m_size(0)
, m_ptList(NULL)
{
	FUNCTION_BEGIN;
	
	m_type = PARTICLE_ET_POINT_LIST;

	FUNCTION_END;
}

void CEmiterPointList::ReleaseAll()
{
	if ( m_ptList )
	{
		delete [] m_ptList;
		m_ptList = NULL;
	}
}

//Allocate memory
bool CEmiterPointList::AllocateMem( int size )
{
	FUNCTION_BEGIN;
	
	if ( size <=0 )
	{
		m_size = 0;
		m_ptList = NULL;
		return false;
	}

	m_ptList = new stPointI[size];

	if ( m_ptList )
	{
		m_size = size;
		return true;
	}

	m_size = 0;
	return false;

	FUNCTION_END;
}

//Init
 void CEmiterPointList::Init( stPointI* ptList, int size )
{
	FUNCTION_BEGIN;
	
	if ( size <=0 )
	{
		m_ptList = NULL;
		return;
	}

	//delete old point list
	if ( m_size > 0 )
	{
		ReleaseAll();
		m_size = 0;
	}

	m_ptList = new stPointI[size];
	
	//Get geo info
	//Get bounding box
	for( int i=0;i<size;i++ )
	{
		m_ptList[i] = ptList[i];
		if ( i == 0 )
		{
			m_rcBoundingBox.left = m_ptList->x;
			m_rcBoundingBox.right = m_ptList->x;
			m_rcBoundingBox.top = m_ptList->y;
			m_rcBoundingBox.bottom = m_ptList->y;
		}
		else
		{
			if ( m_ptList[i].x < m_rcBoundingBox.left )
				m_rcBoundingBox.left = m_ptList[i].x;
			if ( m_ptList[i].x > m_rcBoundingBox.right )
                m_rcBoundingBox.right = m_ptList[i].x;
			if ( m_ptList[i].y < m_rcBoundingBox.top )
				m_rcBoundingBox.top = m_ptList[i].y;
			if ( m_ptList[i].y > m_rcBoundingBox.bottom )
				m_rcBoundingBox.bottom = m_ptList[i].y;
		}		
	}

	//Get center
	m_ptCenter.x = ( m_rcBoundingBox.left + m_rcBoundingBox.right ) / 2;
	m_ptCenter.y = ( m_rcBoundingBox.top + m_rcBoundingBox.bottom ) / 2;

	m_size = size;

	FUNCTION_END;
}

void CEmiterPointList::InitFromString( const String& pStr )
{
	return;
	/*int nCharNum = pStr.length();
	char cStr[MAX_PATH];
	sprintf( cStr, pStr.c_str() );

	const int cFontMaxPoint = 65535;
	stPointI ptTList[cFontMaxPoint];
	int nPtNum = 0;
	int i = 0;

	while ( i<nCharNum && pStr[i] )
	{
		stPointI ptList[1000];
		int size;

		WORD wd = 0;
		bool bChinese = false;
		if ( pStr[i] & 0x80 )
		{
			wd = ((pStr[i]+1) << 8) + pStr[i+1];
			i += 2;
			bChinese = true;
		}
		else
			wd = pStr[i++];

		//if ( ::GetFontPointList( wd, ptList, size ) )
		{
			int xOff = 0;
			if ( bChinese )
				xOff = ((float)i-1.0f-(float)nCharNum/2.0f) * 10;
			else
				xOff = ((float)i-0.5f-(float)nCharNum/2.0f) * 10;

			for( int k=0;k<size;k++ )
			{
				ptList[k].x += xOff;
				ptTList[nPtNum++] = ptList[k];
			}
		}
	}

	Init( ptTList, nPtNum );*/
}

stPointI CEmiterPointList::RandomGetOnePoint()
{
	FUNCTION_BEGIN;
	
	if ( m_size <= 0 )
		return stPointI(0,0);
    
	return m_ptList[rand() % m_size];

	FUNCTION_END;
}

stPointI CEmiterPointList::HScaneGetOnePoint()
{
	FUNCTION_BEGIN;
	
	static int lastIndex = -1;
	if ( ++lastIndex >= m_size )
		lastIndex = 0;

    return m_ptList[lastIndex];

	FUNCTION_END;
}