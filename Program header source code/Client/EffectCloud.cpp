/**
 *\file		EffectCloud.cpp
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:16:00 中国标准时间
 *\brief	云彩效果
 *
 * 
 *
*/

#include "public.h"
#include "./EffectCloud.h"
#include "./GameScene.h"
#include "../RenderD3D/include/RenderD3D.h"
#include "../RenderD3D/include/D3DDevice.h"
#include "../RenderD3D/include/TextureManager.h"
#include "../RenderD3D/include/CustomTexture.h"
#include "./GameAppation.h"
#include "./MainCharacter.h"

//#define USE_ARTIST_IMG

/**
 * \brief 噪音图生成算法
 * 
 * 详细描述
 * 
 * \param x : 位置
 * \param y : 位置
 * \return 
 */
//Noise function as found on Hugo Elias' website
inline float Noise2D(int x, int y)
{
	FUNCTION_BEGIN;

	int n = x + y * 57;
	n = (n<<13) ^ n;
	return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);    

	FUNCTION_END;
}

/**
 * \brief 3D噪音图生成算法
 * 
 * 详细描述
 * 
 * \param x : 位置
 * \param y : 位置
 * \param z : 位置
 * \return 
 */
//Noise function as found on Hugo Elias' website, modified to take a 3rd variable
inline float Noise3D(int x, int y, int z)
{
	FUNCTION_BEGIN;

	int n = x + y * 57 + z * 131;
	n = (n<<13) ^ n;
	return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) 
		* 0.000000000931322574615478515625f);    

	FUNCTION_END;
}

/**
 * \brief 构造函数
 * 
 * 
 * 
 * \return 
 */
CEffectCloud::CEffectCloud()
{
	FUNCTION_BEGIN;

	int i,j;
	for( i=0;i<SOURCE_IMG_NUM;i++ )
	{
		for( j=0;j<SOURCE_IMG_NUM;j++ )
		{
			m_pSourceImg[i][j] = NULL;
		}
	}

	for( i=0;i<=SOURCE_IMG_NUM;i++ )
		m_pRenderBmp[i] = NULL;

	//Init cloud factor
	m_color = 0xFFFFFFFF;
	m_position.x = -100;
	m_position.y = -100;
	m_width = 400;
	m_height = 400;
	m_moveSpeed = 10.0f;
	m_density = 150;
	m_cloudVolume = 0;
	m_updateFrequency = 200;
	m_bNeedUpdate = true;
	m_scaleRate = 8.0f;

	m_bReadyDraw = false;

	FUNCTION_END;
}

/**
 * \brief 析构函数
 * 
 * 
 * 
 * \return 
 */
CEffectCloud::~CEffectCloud()
{
	FUNCTION_BEGIN;

	ReleaseAll();

	FUNCTION_END;
}

/**
 * \brief 设置云彩浓度
 * 
 * 
 * 
 * \param density : 浓度
 * \return 
 */
void CEffectCloud::SetDensity( int density )
{
	FUNCTION_BEGIN;

	if ( (density >= 0) && (density <= 255) )
		m_density = density;

	FUNCTION_END;
}

/**
 * \brief 初始化设备
 * 
 * 
 * 
 * \return 成功则返回true, 否则返回false
 */
bool CEffectCloud::InitDevice()
{
	FUNCTION_BEGIN;

	int i,j;
	HRESULT hr;

	//Load source texture
	for( i=0;i<SOURCE_IMG_NUM;i++ )
	{
		for( j=0;j<SOURCE_IMG_NUM;j++ )
		{
			m_pSourceImg[i][j] = GetTextureManager()->CreateCurstomTexture( pow(2,i)*SOURCE_IMG_SIZE, pow(2,i)*SOURCE_IMG_SIZE, D3DFMT_R5G6B5, 0 );
			if ( NULL == m_pSourceImg[i][j] )
				return false;
			if( !(m_pSourceImg[i][j]->Init()) )
				return false;

#ifdef USE_ARTIST_IMG
			char szImageName[256];
			sprintf( szImageName, "%s%d%d.tga", "datas\\image\\Effect\\cloud\\source", i, j );
			Stream* pStream = OpenPackFileForRead( "data\\datas.pak",szImageName );
			if(pStream)
			{
				m_pSourceImg[i][j]->LoadFromTgaStream( pStream );
				ClosePackFileForRead(pStream);
			}
			if ( !(m_pSourceImg[i][j]->IsCreated()) )
				return false;
#else
			//Fill the soure img with noise
			//First we create a texture in sysmem pool
			IDirect3DTexture8* pMemTex = NULL;
			pMemTex = GetD3DDevice()->CreateTexture(pow(2,i)*SOURCE_IMG_SIZE,pow(2,i)*SOURCE_IMG_SIZE,D3DFMT_R5G6B5,D3DPOOL_SYSTEMMEM);
			if(pMemTex == NULL) 
			{
				return false;
			}
			if(pMemTex)
				pMemTex->AddRef();

			//Then we fill sysmem texture with noise

			D3DLOCKED_RECT d3dlr;
			hr = pMemTex->LockRect( 0, &d3dlr, 0, 0 );
			if ( hr != D3D_OK )
			{
				pMemTex->UnlockRect(0);
				SAFE_RELEASE(pMemTex);
				return false;
			}
			WORD* pPixel = (WORD*)d3dlr.pBits;

			// Fill each pixel
			for( DWORD k=0; k<pow(2,i)*SOURCE_IMG_SIZE; k++  )
			{
				for( DWORD m=0; m<pow(2,i)*SOURCE_IMG_SIZE; m++ )
				{
					//Get the color info from the bitmap
					unsigned char intensity = (char)((1.0f+Noise3D(m,k,pow(2,i)+j)) * 128);
					WORD r = intensity;
					WORD g = intensity; 
					WORD b = intensity;
					WORD a = 0x000000ff;
					//WORD a = intensity;

					WORD dr = ( (r>>3) & 0x1f ) << 11;
					WORD dg = ( (g>>2) & 0x3f ) << 5;
					WORD db = ( (b>>3) & 0x1f ) << 0;	

					/*WORD dr = ( (r>>3) & 0x1f ) << 11;
					WORD dg = ( (g>>3) & 0x1f ) << 5;
					WORD db = ( (b>>3) & 0x1f ) << 0; */

					//Just a test
					WORD tmp = dr + dg + db;
					WORD newR = (tmp>>11) & 0x1f;
					WORD newG = (tmp>>5) & 0x3f;
					WORD newB = (tmp>>0) & 0x1f;
					if ( newR-newG+newG-newB+newB-newR )
					{
						int tmp = 0;
					}
					//End test

					*pPixel++ = dr + dg + db;
				}
			}
			pMemTex->UnlockRect(0);

			//Update sysmem texture to our custom texture
			hr = GetD3DDevice()->__UpdateTexture( pMemTex,m_pSourceImg[i][j]->GetTex() );

			//We save cur texture just for temp use
			//char fileName[256];
			//sprintf( fileName, "%s\\data\\YangziTest\\CloudSourceImg%u%u.BMP", GetGameApplication()->m_szAppPath, i, j );
			//hr = D3DXSaveTextureToFile( fileName, D3DXIFF_BMP, m_pSourceImg[i][j]->GetTex(), NULL );	//
			//End save

			SAFE_RELEASE(pMemTex);

			if( hr != D3D_OK )
			{
				TRACE_DXERROR(hr);
				return false;
			}
#endif
			
			
		}
	}
	//Create render target texture
	for( i=0;i<=SOURCE_IMG_NUM;i++ )
	{
		m_pRenderBmp[i] = GetTextureManager()->CreateRenderTargetTexture( pow(2,SOURCE_IMG_NUM-1)*SOURCE_IMG_SIZE, pow(2,SOURCE_IMG_NUM-1)*SOURCE_IMG_SIZE, D3DFMT_R5G6B5 );
		if ( NULL == m_pRenderBmp[i] )
			return false;
		if ( !(m_pRenderBmp[i]->Init()) )
			return false;
	}

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
void CEffectCloud::UnInitDevice()
{
	FUNCTION_BEGIN;

	int i,j;
	for( i=0;i<SOURCE_IMG_NUM;i++ )
	{
		for( j=0;j<SOURCE_IMG_NUM;j++ )
		{
			if ( m_pSourceImg[i][j] )
			{
				GetTextureManager()->DeleteCustomeTexture( m_pSourceImg[i][j] );
				m_pSourceImg[i][j] = NULL;
			}
		}
	}
	for( int i=0;i<=SOURCE_IMG_NUM;i++ )
	{
		if ( m_pRenderBmp[i] )
		{
            GetTextureManager()->DeleteCustomeTexture( m_pRenderBmp[i] );
			m_pRenderBmp[i] = NULL;
		}
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
void CEffectCloud::ReleaseAll()
{
	FUNCTION_BEGIN;

	UnInitDevice();	

	FUNCTION_END;
}

/**
 * \brief 更新
 * 
 * 
 * 
 * \return 
 */
HRESULT CEffectCloud::Update()
{
	FUNCTION_BEGIN;

	//See if we need update
	static float timeLast = m_updateFrequency;
	timeLast += Engine_GetElapsedTime()*1000.0f;

	if ( !m_bPlaying )
		return S_OK;

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
	
	int i,j;
	HRESULT hr;
	//Make sure we got source img
	for( i=0;i<SOURCE_IMG_NUM;i++ )
	{
		for( j=0;j<SOURCE_IMG_NUM;j++ )
		{
			if ( !(m_pSourceImg[i][j]->IsCreated()) )
				InitDevice();
		}
	}
	//Make sure we get render target texture
	for( i=0;i<=SOURCE_IMG_NUM;i++ )
	{
		if ( !(m_pRenderBmp[i]->IsCreated() ) )
			InitDevice();
	}

	if( !GetD3DDevice()->TestCooperativeLevel() )
		return E_FAIL;

	DWORD dwTimePast_ = GetTickCount();

	int inter;
	DWORD c;
	int frame1,frame2;
	int size[4] = {32,64,128,256};
	int scale[4] = {8,4,2,1};
	int time[4] = {8000,8000,4000,3000};			//8000,8000,4000,3000
	//int weight[4] = {2,4,8,16};
	int weight[4] = {2,4,8,16};
	
	D3DRECT d3dRectBack = {0,0,256,256};
	//Set texture filter state
	GetD3DDevice()->__SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR );
	GetD3DDevice()->__SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR );


	//Get frame1 and frame2 for all source img
	for( i=0;i<SOURCE_IMG_NUM;i++ )
	{
		frame1 = (dwTimePast_%(time[i]+time[i]+time[i]+time[i]))/time[i];
		inter = (dwTimePast_%time[i])*255/time[i];
		frame2 = (frame1+1)%SOURCE_IMG_NUM;
		//compute frame1
		c = 0xFF000000 | ((255-inter)<<16) | ((255-inter)<<8) | (255-inter);
		//Render frame1
		if( !m_pRenderBmp[i]->BeginRenderTarget() )
			return E_FAIL;


		if(!GetD3DDevice()->BeginScene())
		{
			m_pRenderBmp[i]->EndRenderTarget();
			return E_FAIL;
		}

		GetD3DDevice()->__Clear(1,&d3dRectBack,D3DCLEAR_TARGET,0xff000000,1.0f,0);
		
		stPointF scaleImg( scale[i], scale[i] );
		if( !m_pSourceImg[i][frame1]->Render( 0, 0, NULL, &scaleImg, c, Blend_Null ) )
		{
			GetD3DDevice()->EndScene();
			m_pRenderBmp[i]->EndRenderTarget();
			return E_FAIL;
		}

		//compute frame2
		c = 0xFF000000 | (inter<<16) | (inter<<8) | inter;
		if( !m_pSourceImg[i][frame2]->Render( 0, 0, NULL, &scaleImg, c, Blend_Add2 ) )
		{
			GetD3DDevice()->EndScene();
			m_pRenderBmp[i]->EndRenderTarget();
			return E_FAIL;
		}

		if(!GetD3DDevice()->EndScene())
		{
			m_pRenderBmp[i]->EndRenderTarget();
			return E_FAIL;
		}

		if ( !m_pRenderBmp[i]->UpdateRenderTarget() )
		{
			m_pRenderBmp[i]->EndRenderTarget();
			return E_FAIL;
		}

		m_pRenderBmp[i]->EndRenderTarget();
	}

	//Render all combination
	if ( !m_pRenderBmp[SOURCE_IMG_NUM]->BeginRenderTarget() )
		return E_FAIL;


	if(!GetD3DDevice()->BeginScene())
	{
		m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();
		return E_FAIL;
	}

	GetD3DDevice()->__Clear(1,&d3dRectBack,D3DCLEAR_TARGET,0xff000000,1.0f,0);
	
	for ( i=0;i<SOURCE_IMG_NUM;i++ )
	{
		//compute combination frame
		inter = 255/weight[i];
		c = 0xFF000000 | (inter<<16) | (inter<<8) | inter;
		
		//render combination frame
		if( !m_pRenderBmp[i]->Render( 0, 0, NULL, NULL, c, Blend_Add2 ) )
		{
			GetD3DDevice()->EndScene();
			m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();
			return E_FAIL;
		}
	}

	if(!GetD3DDevice()->EndScene())
	{
		m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();
		return E_FAIL;
	}

	if ( !m_pRenderBmp[SOURCE_IMG_NUM]->UpdateRenderTarget() )
	{
		m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();
		return E_FAIL;
	}

	m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();

	//return S_OK;
	
	{
		//compute square + 1-B
		if( !m_pRenderBmp[SOURCE_IMG_NUM-1]->BeginRenderTarget() )
			return E_FAIL;

		c = 0xFFFFFFFF;
		inter = 255-m_density;
		DWORD cBG = 0xFF000000|(inter<<16)|(inter<<8)|inter;
		//DWORD cBG = 0xFF000000|(0x00FF0000)|(inter<<8)|inter;


		if(!GetD3DDevice()->BeginScene())
		{
			m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();
			return E_FAIL;
		}

		hr = GetD3DDevice()->__Clear(1,&d3dRectBack,D3DCLEAR_TARGET,cBG,1.0f,0);

		if ( hr != D3D_OK )
		{
			m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();
			return E_FAIL;
		}
 
        if ( !m_pRenderBmp[SOURCE_IMG_NUM]->Render( 0, 0, NULL, NULL, c, Blend_Add2 ) )
		{
			GetD3DDevice()->EndScene();
			m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();
			return E_FAIL;
		}

		if(!GetD3DDevice()->EndScene())
		{
			 m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();
			 return E_FAIL;
		}

		if ( !m_pRenderBmp[SOURCE_IMG_NUM-1]->UpdateRenderTarget() )
		{
			m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();
			return E_FAIL;
		}

		m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();


		//compute B-square
		if ( !m_pRenderBmp[SOURCE_IMG_NUM]->BeginRenderTarget() )
			return E_FAIL;

		if(!GetD3DDevice()->BeginScene())
		{
			m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();
			return E_FAIL;
		}

		hr = GetD3DDevice()->__Clear(1,&d3dRectBack,D3DCLEAR_TARGET,0xFFFFFFFF,1.0f,0);

		if( !m_pRenderBmp[SOURCE_IMG_NUM-1]->Render( 0, 0, NULL, NULL, c, Blend_MulInvColor ) )
		{
			GetD3DDevice()->EndScene();
			m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();
			return E_FAIL;
		}

		if(!GetD3DDevice()->EndScene())
		{
			m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();
			return E_FAIL;
		}

		if ( !m_pRenderBmp[SOURCE_IMG_NUM]->UpdateRenderTarget() )
		{
			m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();
			return E_FAIL;
		}

		m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();


		//get energy back
		for( int m=0;m<m_cloudVolume;m++ )
		{
			if ( !m_pRenderBmp[SOURCE_IMG_NUM-1]->BeginRenderTarget() )
				return E_FAIL;


			if(!GetD3DDevice()->BeginScene())
			{
				m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();
				return E_FAIL;
			}

			hr = GetD3DDevice()->__Clear(1,&d3dRectBack,D3DCLEAR_TARGET,0xFFFFFFFF,1.0f,0);

			if( !m_pRenderBmp[SOURCE_IMG_NUM]->Render( 0, 0, NULL, NULL, c, Blend_Add3 ) )
			{
				GetD3DDevice()->EndScene();
				m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();
				return E_FAIL;
			}

			if(!GetD3DDevice()->EndScene())
			{
				m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();
				return E_FAIL;
			}

			if ( !m_pRenderBmp[SOURCE_IMG_NUM-1]->UpdateRenderTarget() )
			{
				m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();
				return E_FAIL;
			}

			m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();

			//===============
			if ( !m_pRenderBmp[SOURCE_IMG_NUM]->BeginRenderTarget() )
				return E_FAIL;

			if(!GetD3DDevice()->BeginScene())
			{
				m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();
				return E_FAIL;
			}

			hr = GetD3DDevice()->__Clear(1,&d3dRectBack,D3DCLEAR_TARGET,0xFFFFFFFF,1.0f,0);

			if ( !m_pRenderBmp[SOURCE_IMG_NUM-1]->Render( 0, 0, NULL, NULL, c, Blend_Add3 ) )
			{
				GetD3DDevice()->EndScene();
				m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();
				return E_FAIL;
			}

			if(!GetD3DDevice()->EndScene())
			{
				m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();
				return E_FAIL;
			}

			if ( !m_pRenderBmp[SOURCE_IMG_NUM]->UpdateRenderTarget() )
			{
				m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();
				return E_FAIL;
			}

			m_pRenderBmp[SOURCE_IMG_NUM]->EndRenderTarget();
		}
		

		//==================================
		if ( !m_pRenderBmp[SOURCE_IMG_NUM-1]->BeginRenderTarget() )
			return E_FAIL;

		//hr = GetD3DDevice()->GetDevice()->Clear( 0xFFFFFFFF );

		if(!GetD3DDevice()->BeginScene())
		{
			m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();
			return E_FAIL;
		}

		if( !m_pRenderBmp[SOURCE_IMG_NUM]->Render( 0, 0, NULL, NULL, c, Blend_Null ) )
		{
			GetD3DDevice()->EndScene();
			m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();
			return E_FAIL;
		}

		if(!GetD3DDevice()->EndScene())
		{
			m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();
			return E_FAIL;
		}

		if ( !m_pRenderBmp[SOURCE_IMG_NUM-1]->UpdateRenderTarget() )
		{
			m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();
			return E_FAIL;
		}

		m_pRenderBmp[SOURCE_IMG_NUM-1]->EndRenderTarget();
	}
	
	//Restore texture stage state
	GetD3DDevice()->__SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	GetD3DDevice()->__SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	
	m_bReadyDraw = true;

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
HRESULT CEffectCloud::Render( float fElapsedTime )
{
	FUNCTION_BEGIN;

	if ( !m_bPlaying )
		return S_OK;

	if ( !m_bReadyDraw )
		return S_OK;

	//Get render rect and scale
	stPointF scale( m_scaleRate, m_scaleRate );
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
	frt[0] = float(moveLength)/2048.0f;
	frt[2] = frt[0]+0.015625f;
	frt[1] = frt[0];
	frt[3] = frt[2];

	frt[0] = float(moveLength+offset.x)/2048.0f+float((m_position.x*4)%128)*0.0078125f;
	frt[1] = float(moveLength+offset.y)/2048.0f+float((m_position.y*2)%128)*0.0078125f;
	frt[2] = frt[0]+1.0f;
	frt[3] = frt[1]+1.0f;

	rcSrc.left = frt[0]*SOURCE_IMG_SIZE*pow(2,SOURCE_IMG_NUM-1);
	rcSrc.top = frt[1]*SOURCE_IMG_SIZE*pow(2,SOURCE_IMG_NUM-1);
	rcSrc.right = frt[2]*SOURCE_IMG_SIZE*pow(2,SOURCE_IMG_NUM-1);
	rcSrc.bottom = frt[3]*SOURCE_IMG_SIZE*pow(2,SOURCE_IMG_NUM-1);

	//TRACE( "==============================Cloud==========================\n" );
	//TRACE( "====Left=%d====Right=%d====Top=%d====Bottom=%d====\n", rcSrc.left, rcSrc.right, rcSrc.top, rcSrc.bottom );
	//TRACE( "==============================Cloud==========================\n" );
	
	if ( NULL != m_pRenderBmp[SOURCE_IMG_NUM-1] )
	{
		if( !(m_pRenderBmp[SOURCE_IMG_NUM-1]->Render( m_position.x, m_position.y, &rcSrc, &scale, m_color, Blend_Add )) )			//
			return E_FAIL;
	}
	
	return S_OK;

	FUNCTION_END;
}