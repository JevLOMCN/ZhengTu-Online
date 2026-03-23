/**
 *\file		EffectRain.cpp
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:17:00 中国标准时间
 *\brief	下雨的效果
 *
 * 
 *
*/

#include "public.h"
#include "./EffectRain.h"
#include "./GameScene.h"
#include "../RenderD3D/include/RenderD3D.h"
#include "../RenderD3D/include/D3DDevice.h"
#include "../RenderD3D/include/TextureManager.h"
#include "../RenderD3D/include/CustomTexture.h"
#include "./GameAppation.h"


/**
 * \brief 构造函数
 * 
 * 
 * 
 * \return 
 */
CEffectRain::CEffectRain()
{
	FUNCTION_BEGIN;

	for( int i=0;i<RAIN_IMG_NUM;i++ )
	{
		m_pSourceImg[i] = NULL;
	}

	m_pRenderBmp = NULL;

	m_moveSpeed = 1000;
	m_heavyGrade = 1;
	m_color = 0xFFFF0000;

	m_updateFrequency = 600;
	m_bNeedUpdate = true;

	FUNCTION_END;
}

/**
 * \brief 析构函数
 * 
 * 
 * 
 * \return 
 */
CEffectRain::~CEffectRain()
{
	FUNCTION_BEGIN;

	ReleaseAll();

	FUNCTION_END;
}

/**
 * \brief 初始化设备
 * 
 * 
 * 
 * \return 
 */
bool CEffectRain::InitDevice()
{
	FUNCTION_BEGIN;

	//Create source texture
	for( int i=0;i<RAIN_IMG_NUM;i++ )
	{
		m_pSourceImg[i] = GetTextureManager()->CreateCurstomTexture( RAIN_IMG_SIZE, RAIN_IMG_SIZE, D3DFMT_R5G6B5 );
		if ( NULL == m_pSourceImg[i] )
			return false;
		if ( !m_pSourceImg[i]->Init() )
			return false;

		//Load from artist texture
		char imgFile[256];
		sprintf( imgFile, "%s%d.TGA", "datas\\image\\Effect\\Rain\\sourceImg", i ); //soke 下雨图片文件
		Stream* pStream = OpenPackFileForRead( "data\\datas.pak", imgFile );		//"data\\datas.pak"
		if(pStream)
		{
			m_pSourceImg[i]->LoadFromTgaStream( pStream );
			ClosePackFileForRead(pStream);
		}
		if ( !m_pSourceImg[i]->IsCreated() )
			return false;
	}

	//Create render target texture
	m_pRenderBmp = GetTextureManager()->CreateRenderTargetTexture( RAIN_IMG_SIZE, RAIN_IMG_SIZE, D3DFMT_R5G6B5 );
	if ( NULL == m_pRenderBmp )
		return false;
	if ( !m_pRenderBmp->Init() )
		return false;	
	//Finish create device
	return true;

	FUNCTION_END;
}

/**
 * \brief 释放设备
 * 
 * 
 * 
 * \return 
 */
void CEffectRain::UnInitDevice()
{
	FUNCTION_BEGIN;

	for( int i=0;i<RAIN_IMG_NUM;i++ )
	{
		if ( m_pSourceImg )
		{
			GetTextureManager()->DeleteCustomeTexture( m_pSourceImg[i] );
			m_pSourceImg[i] = NULL;
		}
	}

	if ( m_pRenderBmp )
	{
		GetTextureManager()->DeleteCustomeTexture( m_pRenderBmp );
		m_pRenderBmp = NULL	;
	}
	

	FUNCTION_END;
}

/**
 * \brief 释放资源
 * 
 * 
 * 
 * \return 
 */
void CEffectRain::ReleaseAll()
{
	FUNCTION_BEGIN;

	UnInitDevice();

	FUNCTION_END;
}

/**
 * \brief 获取总的时间
 * 
 * 详细描述
 * 
 * \param frame : 截至第几桢
 * \param time[RAIN_IMG_NUM] : 各桢的时间列表
 * \return 总的时间
 */
int GetTotalTime(int frame, int time[RAIN_IMG_NUM])
{
	FUNCTION_BEGIN;

	int total = 0;
	for( int i=0;i<frame;i++ )
	{
		total += time[i];
	}

	return total;

	FUNCTION_END;
}

/**
 * \brief 更新
 * 
 * 
 * 
 * \return 
 */
HRESULT CEffectRain::Update()
{
	FUNCTION_BEGIN;

	for( int i=0;i<RAIN_IMG_NUM;i++ )
	{
		if ( !m_pSourceImg[i]->IsCreated() )
			InitDevice();
	}
	
	if ( !m_pRenderBmp->IsCreated() )
		InitDevice();

	//See if we need update
	static float timeLast = m_updateFrequency;
	timeLast += Engine_GetElapsedTime()*1000.0f;

	if ( timeLast < m_updateFrequency )
	{
		m_bNeedUpdate = false;
		return S_OK;
	}
	else
	{
		m_bNeedUpdate = true;
		timeLast = 0;
	}

	//draw to render target texture
	if( !GetD3DDevice()->TestCooperativeLevel() )
		return E_FAIL;

	DWORD c;
	DWORD dwTimePast_ = GetTickCount();
	int time[RAIN_IMG_NUM] = {2000,2000};
	int frame1, frame2;
	D3DRECT d3dRectBack = {0,0,RAIN_IMG_SIZE,RAIN_IMG_SIZE};
	//Set texture filter state
	GetD3DDevice()->__SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR );
	GetD3DDevice()->__SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR );

	//get inter
	//this time we just use two source img
	frame1 = ( dwTimePast_ % GetTotalTime(RAIN_IMG_NUM, time) ) / time[0];
	int inter = dwTimePast_ % GetTotalTime(RAIN_IMG_NUM, time);
	inter -= GetTotalTime(frame1, time);
	inter = inter % time[frame1];
	inter = (float)inter / time[frame1] * 255;
	frame2 = (frame1+1) % RAIN_IMG_NUM;

	c = 0xFF000000 | ((255-inter)<<16) | ((255-inter)<<8) | (255-inter);

	if ( !m_pRenderBmp->BeginRenderTarget() )
		return E_FAIL;

	if ( !GetD3DDevice()->BeginScene() )
	{
		m_pRenderBmp->EndRenderTarget();
		return E_FAIL;
	}

	GetD3DDevice()->__Clear(1,&d3dRectBack,D3DCLEAR_TARGET,0xff000000,1.0f,0);

	//render cur two key frame
	if ( !m_pSourceImg[frame1]->Render( 0, 0, NULL, NULL, c, Blend_Null ) )
	{
		GetD3DDevice()->EndScene();
		m_pRenderBmp->EndRenderTarget();
		return E_FAIL;
	}

	c = 0xFF000000 | (inter<<16) | (inter<<8) | inter;
	if ( !m_pSourceImg[frame2]->Render( 0, 0, NULL, NULL, c, Blend_Add2 ) )
	{
		GetD3DDevice()->EndScene();
		m_pRenderBmp->EndRenderTarget();
		return E_FAIL;
	}

	if ( !GetD3DDevice()->EndScene() )
	{
		m_pRenderBmp->EndRenderTarget();
		return E_FAIL;
	}

	if ( !m_pRenderBmp->UpdateRenderTarget() )
	{
		m_pRenderBmp->EndRenderTarget();
		return E_FAIL;
	}

	m_pRenderBmp->EndRenderTarget();

	//Restore texture stage state
	GetD3DDevice()->__SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	GetD3DDevice()->__SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

	return S_OK;	

	FUNCTION_END;
}

/**
 * \brief 绘制
 * 
 * 
 * 
 * \param fElapsedTime : 持续时间
 * \return 
 */
HRESULT CEffectRain::Render(float fElapsedTime)
{
	FUNCTION_BEGIN;

	//Get render rect and scale
	stPointF scale( 4.0f, 4.0f );
	stRectF rcSrc;
	static float timeLast = 0;
	timeLast += fElapsedTime;
	int moveLength = timeLast * m_moveSpeed;

	//Consider map offset when main character is moving;
	static stPointI offsetOld = GetMapScene()->GetPixelOffset();
	static stPointI offset(0,0);

	offset += GetMapScene()->GetPixelOffset() - offsetOld;
	offsetOld = GetMapScene()->GetPixelOffset();

	static float frt[4] = {0.0f,0.0f,1.0f,1.0f};

	frt[0] = float(0+offset.x)/1024.0f;
	frt[1] = -float(moveLength-offset.y)/1024.0f;
	frt[2] = frt[0]+1.0f;
	frt[3] = frt[1]+1.0f;

	rcSrc.left = frt[0]*256;
	rcSrc.top = frt[1]*256;
	rcSrc.right = frt[2]*256;
	rcSrc.bottom = frt[3]*256;


	if ( m_pSourceImg )
	{
		if( !m_pRenderBmp->Render( 0, 0, &rcSrc, &scale, m_color, Blend_Add) )			//
			return E_FAIL;
	}
	return S_OK;

	FUNCTION_END;
}
