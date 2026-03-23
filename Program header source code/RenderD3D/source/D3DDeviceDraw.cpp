#include "../../engine/include/engine.h"

#include "../include/D3DBitmap.h"
#include "../include/d3ddevice.h"
#include "../include/D3DBitmapList.h"
#include "../include/TextureManager.h"
#include "../include/D3DTextout.h"
#include "../include/D3DLightMap.h"
#include "../Include/RenderD3D.h"
#include "../Include/D3DMBitmap.h"
#include "../include/TextureCell.h"
#include "../include/TextureCache.h"
#include "../include/CustomTexture.h"
#include "../../Engine/include/MathEx.h"
#include "../../engine/include/Ini.h"
 
const DWORD D3DFVF_BITMAP = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

#define D3DFVF_MULTITEXBITMAP (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2)
const float c_error = 0.5f;
/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param * : ÃèÊö
* \param ri : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
inline void FillTriangleStrip(stVertexBitmap * v,stRenderInfoBase& ri)
{
	v[0].v = D3DXVECTOR4(ri.rect.left - c_error,ri.rect.top - c_error,0.0f,1.0f);
	v[0].uv = D3DXVECTOR2(ri.uv.left,ri.uv.top);
	v[0].color = ri.color;

	v[1].v = D3DXVECTOR4(ri.rect.left - c_error,ri.rect.bottom - c_error,0.0f,1.0f);
	v[1].uv = D3DXVECTOR2(ri.uv.left,ri.uv.bottom);
	v[1].color = ri.color;

	v[2].v = D3DXVECTOR4(ri.rect.right - c_error,ri.rect.top - c_error,0.0f,1.0f);
	v[2].uv = D3DXVECTOR2(ri.uv.right,ri.uv.top);
	v[2].color = ri.color;

	v[3].v = D3DXVECTOR4(ri.rect.right - c_error,ri.rect.bottom - c_error,0.0f,1.0f);
	v[3].uv = D3DXVECTOR2(ri.uv.right,ri.uv.bottom);
	v[3].color = ri.color;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param * : ÃèÊö
* \param ri : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
inline void FillTriangleList(stVertexBitmap * v,stRenderInfoBase& ri)
{
	v[0].v = D3DXVECTOR4(ri.rect.left - c_error,ri.rect.top - c_error,0.0f,1.0f);
	v[0].uv = D3DXVECTOR2(ri.uv.left,ri.uv.top);
	v[0].color = ri.color;

	v[1].v = D3DXVECTOR4(ri.rect.left - c_error,ri.rect.bottom - c_error,0.0f,1.0f);
	v[1].uv = D3DXVECTOR2(ri.uv.left,ri.uv.bottom);
	v[1].color = ri.color;

	v[2].v = D3DXVECTOR4(ri.rect.right - c_error,ri.rect.bottom - c_error,0.0f,1.0f);
	v[2].uv = D3DXVECTOR2(ri.uv.right,ri.uv.bottom);
	v[2].color = ri.color;

	v[3].v = D3DXVECTOR4(ri.rect.left - c_error,ri.rect.top - c_error,0.0f,1.0f);
	v[3].uv = D3DXVECTOR2(ri.uv.left,ri.uv.top);
	v[3].color = ri.color;

	v[4].v = D3DXVECTOR4(ri.rect.right - c_error,ri.rect.bottom - c_error,0.0f,1.0f);
	v[4].uv = D3DXVECTOR2(ri.uv.right,ri.uv.bottom);
	v[4].color = ri.color;

	v[5].v = D3DXVECTOR4(ri.rect.right - c_error,ri.rect.top - c_error,0.0f,1.0f);
	v[5].uv = D3DXVECTOR2(ri.uv.right,ri.uv.top);
	v[5].color = ri.color;
}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param * : ÃèÊö
* \param ri : ÃèÊö
* \param count : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
inline void FillTriangleList(stVertexBitmap * v,stRenderInfoBase* ri,size_t count)
{
	for(size_t i=0;i<count;++i)
	{
		FillTriangleList(v,ri[i]);
		v += 6;
	}
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param * : ÃèÊö
* \param pMat : ÃèÊö
* \param count : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
inline void TransformVectorArray(stVertexBitmap * pV,D3DXMATRIX* pMat,size_t count)
{
	for(size_t i=0;i<count;++i)
	{
		D3DXVec4Transform(&pV[i].v,&pV[i].v,pMat);
	}
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param * : ÃèÊö
* \param ri : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
inline void FillTriangleStrip(stMultiTexVertexBitmap * v,stMultiTexRenderInfoBase& ri)
{
	v[0].v = D3DXVECTOR4(ri.rect.left - c_error,ri.rect.top - c_error,0.0f,1.0f);
	v[0].uv = D3DXVECTOR2(ri.uv.left,ri.uv.top);
	v[0].uv2 = D3DXVECTOR2(ri.uv2.left,ri.uv2.top);
	v[0].color = ri.color;

	v[1].v = D3DXVECTOR4(ri.rect.left - c_error,ri.rect.bottom - c_error,0.0f,1.0f);
	v[1].uv = D3DXVECTOR2(ri.uv.left,ri.uv.bottom);
	v[1].uv2 = D3DXVECTOR2(ri.uv2.left,ri.uv2.bottom);
	v[1].color = ri.color;

	v[2].v = D3DXVECTOR4(ri.rect.right - c_error,ri.rect.top - c_error,0.0f,1.0f);
	v[2].uv = D3DXVECTOR2(ri.uv.right,ri.uv.top);
	v[2].uv2 = D3DXVECTOR2(ri.uv2.right,ri.uv2.top);
	v[2].color = ri.color;

	v[3].v = D3DXVECTOR4(ri.rect.right - c_error,ri.rect.bottom - c_error,0.0f,1.0f);
	v[3].uv = D3DXVECTOR2(ri.uv.right,ri.uv.bottom);
	v[3].uv2 = D3DXVECTOR2(ri.uv2.right,ri.uv2.bottom);
	v[3].color = ri.color;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param * : ÃèÊö
* \param ri : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
inline void FillTriangleList(stMultiTexVertexBitmap * v,stMultiTexRenderInfoBase& ri)
{
	v[0].v = D3DXVECTOR4(ri.rect.left - c_error,ri.rect.top - c_error,0.0f,1.0f);
	v[0].uv = D3DXVECTOR2(ri.uv.left,ri.uv.top);
	v[0].uv2 = D3DXVECTOR2(ri.uv2.left,ri.uv2.top);
	v[0].color = ri.color;

	v[1].v = D3DXVECTOR4(ri.rect.left - c_error,ri.rect.bottom - c_error,0.0f,1.0f);
	v[1].uv = D3DXVECTOR2(ri.uv.left,ri.uv.bottom);
	v[1].uv2 = D3DXVECTOR2(ri.uv2.left,ri.uv2.bottom);
	v[1].color = ri.color;

	v[2].v = D3DXVECTOR4(ri.rect.right - c_error,ri.rect.bottom - c_error,0.0f,1.0f);
	v[2].uv = D3DXVECTOR2(ri.uv.right,ri.uv.bottom);
	v[2].uv2 = D3DXVECTOR2(ri.uv2.right,ri.uv2.bottom);
	v[2].color = ri.color;

	v[3].v = D3DXVECTOR4(ri.rect.left - c_error,ri.rect.top - c_error,0.0f,1.0f);
	v[3].uv = D3DXVECTOR2(ri.uv.left,ri.uv.top);
	v[3].uv2 = D3DXVECTOR2(ri.uv2.left,ri.uv2.top);
	v[3].color = ri.color;

	v[4].v = D3DXVECTOR4(ri.rect.right - c_error,ri.rect.bottom - c_error,0.0f,1.0f);
	v[4].uv = D3DXVECTOR2(ri.uv.right,ri.uv.bottom);
	v[4].uv2 = D3DXVECTOR2(ri.uv2.right,ri.uv2.bottom);
	v[4].color = ri.color;

	v[5].v = D3DXVECTOR4(ri.rect.right - c_error,ri.rect.top - c_error,0.0f,1.0f);
	v[5].uv = D3DXVECTOR2(ri.uv.right,ri.uv.top);
	v[5].uv2 = D3DXVECTOR2(ri.uv2.right,ri.uv2.top);
	v[5].color = ri.color;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param * : ÃèÊö
* \param ri : ÃèÊö
* \param count : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
inline void FillTriangleList(stMultiTexVertexBitmap * v,stMultiTexRenderInfoBase* ri,size_t count)
{
	for(size_t i=0;i<count;++i)
	{
		FillTriangleList(v,ri[i]);
		v += 6;
	}
}

DWORD CD3DDevice::__GetRenderState(D3DRENDERSTATETYPE state)
{
	return mRenderStates[state];
}

DWORD CD3DDevice::__GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type)
{
	return mTextureStageState[Stage].states[Type];
}

void CD3DDevice::__SetRenderState(D3DRENDERSTATETYPE state,DWORD value)
{
	if(mRenderStates[state] != value){
		mRenderStates[state] = value;
		m_pD3DD->SetRenderState(state,value);
	}
}

void CD3DDevice::__SetTextureStageState( DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	if(mTextureStageState[Stage].states[Type] != Value){
		mTextureStageState[Stage].states[Type] = Value;
		m_pD3DD->SetTextureStageState(Stage,Type,Value);
	}
}

inline void CD3DDevice::SetDeviceStateNoTex()
{
	m_pD3DD->SetTexture(0,NULL);
	__SetTextureStageState( 0,D3DTSS_ALPHAOP,D3DTOP_DISABLE );
	__SetTextureStageState( 0,D3DTSS_COLOROP,D3DTOP_DISABLE );

	__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
}

void  CD3DDevice::SetDeviceDrawState(enumDrawState drawState,BlendType blend)
{
	if(drawState != mDrawState || blend != mBlendType){
		mDrawState = drawState;
		mBlendType = blend;
		switch(drawState){
		case DrawStateTex:
			SetDeviceState(blend);
			break;
		case DrawStateMultiTex:
			SetDeviceStateForMultiTex(blend);
			break;
		case DrawStateNoTex:
			SetDeviceStateNoTex();
			break;
		default:
			SetDeviceState(Blend_Null);
			break;
		}
	}
}

void CD3DDevice::SetDeviceState(BlendType blend)
{
	if(IsDoubleTexture()){
		m_pD3DD->SetTexture(1,NULL);
		__SetTextureStageState( 1,D3DTSS_ALPHAOP,D3DTOP_DISABLE );
		__SetTextureStageState( 1,D3DTSS_COLOROP,D3DTOP_DISABLE );
	}

	// FinalColor = TexelColor ¡Á SourceBlendFactor + PixelColor ¡Á DestBlendFactor
	// D3D ÑÕÉ«¼ÆËã¹«Ê½ Êä³ö = ÎÆÀíÑÕÉ« ¡Á SourceBlendFactor + ÏñËØÑÕÉ« ¡Á DestBlendFactor
	switch(blend)
	{
	case Blend_Null:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		break;
	case Blend_Add:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		break;
	case Blend_Add2:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	case Blend_AlphaAdd:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	case Blend_AlphaAdd2:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;
	case Blend_AlphaAdd3:
		__SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_MODULATE );
		__SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_SELECTARG1 );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		break;
	case Blend_MulAlpha:
		__SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG2 );
		__SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_MODULATE );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
		break;
	case Blend_MulInvAlpha:
		__SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG2 );
		__SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_MODULATE );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
	case Blend_MulInvColor:
		__SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_MODULATE );
		__SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_SELECTARG1 );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		break;
	case Blend_Bright:
		__SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
		__SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		break;
	case Blend_IgnoreTexelColor:	// ²»¼ÆËãÌùÍ¼µÄÑÕÉ«
		__SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
		__SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		break;
	case Blend_ColorAdd:
		__SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_ADD );
		__SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		break;
	case Blend_AlphaAdd4:
		__SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_ADD );

		__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		break;
	case Blend_AlphaSub:
		__SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SUBTRACT );

		__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		break;
	case Blend_Add3:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		break;
	case Blend_AlphaAdd5:
		__SetTextureStageState(0, D3DTSS_COLOROP,  D3DTOP_SELECTARG2 );
		__SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	case Blend_MulColor:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		break;
	default:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
	}
}

void CD3DDevice::SetDeviceStateForMultiTex(BlendType blend)
{
	__SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
	__SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
	// FinalColor = TexelColor ¡Á SourceBlendFactor + PixelColor ¡Á DestBlendFactor
	// D3D ÑÕÉ«¼ÆËã¹«Ê½ Êä³ö = ÎÆÀíÑÕÉ« ¡Á SourceBlendFactor + ÏñËØÑÕÉ« ¡Á DestBlendFactor
	switch(blend)
	{
	case Blend_Null:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		break;
	case Blend_Add:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		break;
	case Blend_Add2:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	case Blend_AlphaAdd:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	case Blend_AlphaAdd2:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;
	case Blend_AlphaAdd3:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		break;
	case Blend_MulAlpha:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
		__SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
		break;
	case Blend_MulInvAlpha:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
		__SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
	case Blend_MulInvColor:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		break;
	case Blend_Bright:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
		__SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		break;
	case Blend_IgnoreTexelColor:	// ²»¼ÆËãÌùÍ¼µÄÑÕÉ«
		__SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
		__SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		break;
	case Blend_ColorAdd:
		__SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_ADD );
		__SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		break;
	case Blend_AlphaAdd4:
		__SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_ADD );

		__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		break;
	case Blend_AlphaSub:
		__SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SUBTRACT );

		__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		break;
	case Blend_Add3:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		break;
	case Blend_AlphaAdd5:
		__SetTextureStageState( 0, D3DTSS_COLOROP,  D3DTOP_SELECTARG2 );
		__SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	case Blend_MulColor:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		__SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		__SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		break;
	default:
		__SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		__SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		__SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		__SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
	}
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
void CD3DDevice::SetupStates()
{
//	HRESULT hr;
	__SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	
	__SetTextureStageState( 0,D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	__SetTextureStageState( 0,D3DTSS_MINFILTER, D3DTEXF_LINEAR );

	__SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	__SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	__SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	__SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	if(IsDoubleTexture())
	{
		__SetTextureStageState( 1,D3DTSS_TEXCOORDINDEX, 1 );

		__SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_TEXTURE );
		__SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT );

		__SetTextureStageState(1,D3DTSS_ALPHAARG1,D3DTA_TEXTURE );
		__SetTextureStageState(1,D3DTSS_ALPHAARG2,D3DTA_CURRENT );

		__SetTextureStageState( 1,D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		__SetTextureStageState( 1,D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	}

	__SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );		// ½ûÖ¹ ZBuffer
	__SetRenderState( D3DRS_ZENABLE,		FALSE );		// ½ûÖ¹ ZBuffer
	__SetRenderState( D3DRS_LIGHTING,		FALSE );		// ½ûÖ¹¹âÕÕ
	__SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
	__SetRenderState( D3DRS_ALPHATESTENABLE ,	TRUE );

	__SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE );
	__SetRenderState( D3DRS_SHADEMODE,  D3DSHADE_FLAT );

	mDrawState = DrawStateTex;
	mBlendType = Blend_Null;
	SetDeviceState(Blend_Null);

	m_bTexMagLinear = true;
	m_bTexMinLinear = true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param dwVertexNum : ÃèÊö
* \param pData : ÃèÊö
* \param & : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
HRESULT CD3DDevice::LockVertexBuffer(DWORD dwVertexNum,void** pData,DWORD & dwStartVertex)
{
	DWORD dwStartOffset;
	HRESULT hr = _LockVertexBuffer(dwVertexNum * sizeof(stVertexBitmap),pData,dwStartOffset,sizeof(stVertexBitmap));
	if(FAILED(hr)) return hr;
	dwStartVertex = dwStartOffset / sizeof(stVertexBitmap);
	return hr;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param dwVertexNum : ÃèÊö
* \param pData : ÃèÊö
* \param & : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
inline HRESULT CD3DDevice::LockMultiTexVertexBuffer(DWORD dwVertexNum,void** pData,DWORD & dwStartVertex)
{
	DWORD dwStartOffset;
	HRESULT hr = _LockVertexBuffer(dwVertexNum * sizeof(stMultiTexVertexBitmap),pData,dwStartOffset,sizeof(stMultiTexVertexBitmap));
	if(FAILED(hr)) return hr;
	dwStartVertex = dwStartOffset / sizeof(stMultiTexVertexBitmap);
	return hr;
}

inline HRESULT CD3DDevice::LockVertexBufferAlign(DWORD dwVertexNum,void ** pData,DWORD & dwStartVertex,size_t stride)
{
	DWORD dwStartOffset;
	HRESULT hr = _LockVertexBuffer(dwVertexNum * stride,pData,dwStartOffset,stride);
	if(FAILED(hr)) return hr;
	dwStartVertex = dwStartOffset/stride;
	return hr;
}

inline HRESULT CD3DDevice::_LockVertexBuffer(DWORD dwSize,void** pData,DWORD & dwStartOffset,DWORD dwStride)
{
	HRESULT hr;

#ifdef USE_DYNAMIC_BUFFER
	DWORD dwLockFlags;

	if(dwStride)
	{
		m_nVBOffset = (m_nVBOffset + dwStride-1)/dwStride * dwStride;
		if(m_nVBOffset > m_nVBSize)  m_nVBOffset = m_nVBSize;
	}

	if( m_nVBOffset + dwSize <= m_nVBSize )
	{
		dwLockFlags = m_dwVertexLockFlags[0];
		dwStartOffset = m_nVBOffset;
	}
	else
	{
		dwLockFlags = m_dwVertexLockFlags[1];
		dwStartOffset = 0;
		m_nVBOffset = 0;
	}

	hr = GetVB()->Lock(m_nVBOffset,dwSize,(BYTE**)pData,dwLockFlags);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
	}

	m_nVBOffset += dwSize;
#else
	dwStartOffset = 0;
	return GetVB()->Lock(0,dwSize,(BYTE**)pData,0);
#endif
	return hr;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param dwIndexNum : ÃèÊö
* \param pData : ÃèÊö
* \param & : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
HRESULT CD3DDevice::LockIndexBuffer(DWORD dwIndexNum,void** pData,DWORD & dwStartIndex)
{
	
	HRESULT hr;

#ifdef USE_DYNAMIC_BUFFER
	DWORD dwLockFlags;

	if( m_nIBOffset + dwIndexNum <= c_maxIndexNum )
	{
		dwLockFlags = m_dwVertexLockFlags[0];
		dwStartIndex = m_nIBOffset;
	}
	else
	{
		dwLockFlags = m_dwVertexLockFlags[1];
		dwStartIndex = 0;
		m_nIBOffset = 0;
	}

	hr = GetIB()->Lock(m_nIBOffset * GetD3DIndexSize(),GetD3DIndexSize() * dwIndexNum,(BYTE**)pData,dwLockFlags);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
	}

	m_nIBOffset += dwIndexNum;
#else
	dwStartIndex = 0;
	return GetIB()->Lock(0,GetD3DIndexSize()*dwIndexNum,(BYTE**)pData,0);
#endif
	return hr;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
HRESULT CD3DDevice::UnlockVertexBuffer()
{
	return GetVB()->Unlock();
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
HRESULT CD3DDevice::UnlockIndexBuffer()
{
	return GetIB()->Unlock();
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
inline HRESULT CD3DDevice::UnlockMultiTexVertexBuffer()
{
	return GetMultiTexVB()->Unlock();
}

inline HRESULT CD3DDevice::RenderTriangleStrip(IDirect3DTexture8 * pTex,stDevRenderInfo * pRenderInfo,BlendType blend)
{

	stVertexBitmap* v;
//	DWORD * pIndex;
	HRESULT hr;
	DWORD nStartVertex;
//	DWORD nStartIndex;

	hr = LockVertexBuffer(4,(void**)&v,nStartVertex);

	if(FAILED(hr))
	{
		return hr;
	}

	memcpy( v,pRenderInfo->vec,4 * sizeof(stVertexBitmap) );

	UnlockVertexBuffer();

	hr = SetStreamSource(sizeof(stVertexBitmap),D3DFVF_BITMAP);
	if(FAILED(hr)) 
	{
		TRACE_DXERROR(hr);
		return hr;
	}

	hr = GetDevice()->SetTexture(0,pTex);
	if(FAILED(hr)) 
	{
		TRACE_DXERROR(hr);
		return hr;
	} 

	SetDeviceDrawState(DrawStateTex,blend);

	hr = GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP,nStartVertex,2);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
	}

	return hr;
}

inline HRESULT CD3DDevice::RenderTriangleStrip(IDirect3DTexture8 * pTex,IDirect3DTexture8 * pTex2,stDevMultiTexRenderInfo * pRenderInfo,BlendType blend)
{
	stMultiTexVertexBitmap* v;
//	DWORD * pIndex;
	HRESULT hr;
	DWORD nStartVertex;
//	DWORD nStartIndex;

	hr = LockMultiTexVertexBuffer(4,(void**)&v,nStartVertex);
	if(FAILED(hr)) return hr;

	memcpy(v,pRenderInfo->vec,4 * sizeof(stMultiTexVertexBitmap));

	UnlockMultiTexVertexBuffer();

	hr = SetStreamSource(sizeof(stMultiTexVertexBitmap),D3DFVF_MULTITEXBITMAP);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return hr;
	}

	hr = GetDevice()->SetTexture(0,pTex);	// ÑÕÉ«

	hr = GetDevice()->SetTexture(1,pTex2);	// Alpha
	
	SetDeviceDrawState(DrawStateMultiTex,blend);

	hr = GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP,nStartVertex,2);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
	}

//_draw_final:

	return hr;
}

inline HRESULT CD3DDevice::RenderTriangleList(IDirect3DTexture8 * pTex,stDevRenderInfo * pRenderInfo,size_t nRenderNum,BlendType blend)
{
	stVertexBitmap* v;
	DWORD * pIndex;
	HRESULT hr;
	DWORD nStartVertex;
	DWORD nStartIndex;
	
#ifdef USE_INDEX_BUFFER

	hr = LockVertexBuffer(nRenderNum * 4,(void**)&v,nStartVertex);
	if(FAILED(hr))
	{
		return hr;
	}

	memcpy( v,pRenderInfo->vec,nRenderNum * 4 * sizeof(stVertexBitmap) );

	UnlockVertexBuffer();

#else

	hr = LockVertexBuffer(nRenderNum * 6,(void**)&v,nStartVertex);
	if(FAILED(hr))
	{
		return hr;
	}

	memcpy( v,pRenderInfo->vec,nRenderNum * 6 * sizeof(stVertexBitmap) );

	UnlockVertexBuffer();

#endif
	

	hr = SetStreamSource(sizeof(stVertexBitmap),D3DFVF_BITMAP);
	if(FAILED(hr)) 
	{
		TRACE_DXERROR(hr);
		return hr;
	}

#ifdef USE_INDEX_BUFFER

	hr = LockIndexBuffer(nRenderNum * 6,(void**)&pIndex,nStartIndex);
	if(FAILED(hr))
	{
		UnlockVertexBuffer();
		return hr;
	}

	FillIndexBuffer(pIndex,GetD3DIndexSize(),0,nRenderNum);
	UnlockIndexBuffer();
	
	hr = GetDevice()->SetIndices(GetIB(),nStartVertex);
	if(FAILED(hr)) 
	{
		TRACE_DXERROR(hr);
		return hr;
	}

#endif

	hr = GetDevice()->SetTexture(0,pTex);
	if(FAILED(hr)) 
	{
		TRACE_DXERROR(hr);
		return hr;
	} 

	SetDeviceDrawState(DrawStateTex,blend);

	//GetDevice()->SetTexture(0,NULL);
#ifdef USE_INDEX_BUFFER
	hr = GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,nRenderNum * 4 , nStartIndex,2 * nRenderNum);
#else
	hr = GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST,nStartVertex,2 * nRenderNum);
#endif
	//GetDevice()->SetTexture(0,NULL);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
	}

	return hr;
}

inline HRESULT CD3DDevice::RenderTriangleList(IDirect3DTexture8 * pTex,IDirect3DTexture8 * pTex2,stDevMultiTexRenderInfo * pRenderInfo,size_t nRenderNum,BlendType blend)
{
	stMultiTexVertexBitmap* v;
	DWORD * pIndex;
	HRESULT hr;
	DWORD nStartVertex;
	DWORD nStartIndex;

#ifdef USE_INDEX_BUFFER

	hr = LockMultiTexVertexBuffer(nRenderNum * 4,(void**)&v,nStartVertex);
	if(FAILED(hr)) return hr;

	memcpy(v,pRenderInfo->vec,nRenderNum * 4 * sizeof(stMultiTexVertexBitmap));

	UnlockMultiTexVertexBuffer();

#else

	hr = LockMultiTexVertexBuffer(nRenderNum * 6,(void**)&v,nStartVertex);
	if(FAILED(hr)) return hr;

	memcpy(v,pRenderInfo->vec,nRenderNum * 6 * sizeof(stMultiTexVertexBitmap));

	UnlockMultiTexVertexBuffer();

#endif

	hr = SetStreamSource(sizeof(stMultiTexVertexBitmap),D3DFVF_MULTITEXBITMAP);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return hr;
	}

#ifdef USE_INDEX_BUFFER
	hr = LockIndexBuffer(nRenderNum * 6,(void**)&pIndex,nStartIndex);
	if(FAILED(hr))
	{
		UnlockMultiTexVertexBuffer();
		return hr;
	}

	FillIndexBuffer(pIndex,GetD3DIndexSize(),0,nRenderNum);
	UnlockIndexBuffer();

	hr = GetDevice()->SetIndices(GetIB(),nStartVertex);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return hr;
	}
#endif

	hr = GetDevice()->SetTexture(0,pTex);	// ÑÕÉ«
	hr = GetDevice()->SetTexture(1,pTex2);	// Alpha

	SetDeviceDrawState(DrawStateMultiTex,blend);

#ifdef USE_INDEX_BUFFER
	hr = GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,nRenderNum * 4 , nStartIndex,2 * nRenderNum);
#else
	hr = GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST,nStartVertex,2 * nRenderNum);
#endif

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
	}

//_draw_final:

	return hr;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pRenderInfo : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
inline void CD3DDevice::AddMapRenderInfo(stRenderInfo* pRenderInfo)
{
	switch(m_FlushType)
	{
	case Flush_Cover:
		{
			if(!m_arrayRenderInfo.array.empty() && 
				(m_arrayRenderInfo.blend != pRenderInfo->blend || m_arrayRenderInfo.pTex != pRenderInfo->tex || m_arrayRenderInfo.array.size() == c_maxRectangleNum) )
			{
				RenderTriangleList(m_arrayRenderInfo.pTex,&m_arrayRenderInfo.array[0],m_arrayRenderInfo.array.size(),m_arrayRenderInfo.blend);
				m_arrayRenderInfo.array.resize(0);
			}
			if(m_arrayRenderInfo.array.empty())
			{
				m_arrayRenderInfo.pTex = pRenderInfo->tex;
				m_arrayRenderInfo.blend = pRenderInfo->blend;
			}
			m_arrayRenderInfo.array.resize(m_arrayRenderInfo.array.size() + 1);
			stDevRenderInfo & info = m_arrayRenderInfo.array.back();

#ifdef USE_INDEX_BUFFER
			FillTriangleStrip(info.vec,*pRenderInfo);
			if(m_bTransform)
			{
				TransformVectorArray(info.vec,&m_Transform,4);
			}
#else
			FillTriangleList(info.vec,*pRenderInfo);
			if(m_bTransform)
			{
				TransformVectorArray(info.vec,&m_Transform,6);
			}
#endif
		}
		break;
	case Flush_NoCover:
		{
			if(!m_mapMultiTexRenderInfo.empty())
			{
				for(tMapMultiTexRenderInfoIt it = m_mapMultiTexRenderInfo.begin(); it != m_mapMultiTexRenderInfo.end(); ++it)
				{
					IDirect3DTexture8* tex,*tex2;
					GetRenderKeyTex(it->first,tex,tex2);
					RenderTriangleList(tex,tex2,&it->second->array[0],it->second->array.size(),it->second->blend);
					m_allocMultiTexRenderInfos.freePointer( it->second );
				}
				m_mapMultiTexRenderInfo.clear();
			}

			DWORD key = (DWORD) pRenderInfo->tex;
			tMapRenderInfoIt it = m_mapRenderInfo.find(key);
			if(it == m_mapRenderInfo.end())
			{
				stRenderInfos* p = LOOKASIDE_ALLOC(m_allocRenderInfos);
				it = m_mapRenderInfo.insert(tMapRenderInfo::value_type(key, p )).first;
				it->second->array.reserve(4);
			}
			else
			{
				if( !it->second->array.empty() && (it->second->blend != pRenderInfo->blend || it->second->array.size() == c_maxRectangleNum) )
				{
					RenderTriangleList( (IDirect3DTexture8*)it->first,&it->second->array[0],it->second->array.size(),it->second->blend );
					it->second->array.resize( 0 );
				}
			}
			if(it->second->array.empty())
			{
				it->second->blend = pRenderInfo->blend;
			}

			it->second->array.resize( it->second->array.size() + 1 );
			stDevRenderInfo & info = it->second->array.back();

#ifdef USE_INDEX_BUFFER
			FillTriangleStrip(info.vec,*pRenderInfo);
			if( m_bTransform )
			{
				TransformVectorArray(info.vec,&m_Transform,6);
			}
#else
			FillTriangleList(info.vec,*pRenderInfo);
			if( m_bTransform )
			{
				TransformVectorArray(info.vec,&m_Transform,4);
			}
#endif

		}
		break;
	case Flush_Null:
		{
			stDevRenderInfo info;
			FillTriangleStrip(info.vec,*pRenderInfo);
			if(m_bTransform)
			{
				TransformVectorArray(info.vec,&m_Transform,4);
			}
			RenderTriangleStrip(pRenderInfo->tex,&info,pRenderInfo->blend);
		}
		break;
	}
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pRenderInfo : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
inline void CD3DDevice::AddMapRenderInfo(stMultiTexRenderInfo* pRenderInfo)
{
	switch(m_FlushType)
	{
	case Flush_NoCover:
		{
			if(!m_mapRenderInfo.empty())
			{
				for( tMapRenderInfoIt it = m_mapRenderInfo.begin(); it != m_mapRenderInfo.end(); ++it )
				{
					RenderTriangleList((IDirect3DTexture8*)it->first,&it->second->array[0],it->second->array.size(),it->second->blend);
					m_allocRenderInfos.freePointer(it->second);
				}
				m_mapRenderInfo.clear();
			}

			ULONGLONG key = (ULONGLONG) pRenderInfo->GetKey();
			tMapMultiTexRenderInfoIt it = m_mapMultiTexRenderInfo.find(key);
			if(it == m_mapMultiTexRenderInfo.end())
			{
				stMultiTexRenderInfos* p = LOOKASIDE_ALLOC(m_allocMultiTexRenderInfos);
				it = m_mapMultiTexRenderInfo.insert(tMapMultiTexRenderInfo::value_type(key,p)).first;
				it->second->array.reserve(4);
			}
			else
			{
				if(!it->second->array.empty() && (it->second->blend != pRenderInfo->blend || it->second->array.size() == c_maxRectangleNum))
				{
					IDirect3DTexture8* tex,*tex2;
					GetRenderKeyTex(it->first,tex,tex2);
					RenderTriangleList(tex,tex2,&it->second->array[0],it->second->array.size(),it->second->blend);
					it->second->array.resize(0);
				}
			}

			if( it->second->array.empty() )
			{
				it->second->blend = pRenderInfo->blend;
			}

			it->second->array.resize( it->second->array.size() + 1 );
			stDevMultiTexRenderInfo & info = it->second->array.back();
#ifdef USE_INDEX_BUFFER
			FillTriangleStrip( info.vec,*pRenderInfo );
			if( m_bTransform )
			{
				TransformVectorArray(info.vec,&m_Transform,4);
			}
#else
			FillTriangleList( info.vec,*pRenderInfo );
			if( m_bTransform )
			{
				TransformVectorArray(info.vec,&m_Transform,6);
			}
#endif
		}
		break;
	case Flush_Null:
		{
			stDevMultiTexRenderInfo info;
			FillTriangleStrip(info.vec,*pRenderInfo);
			if(m_bTransform)
			{
				TransformVectorArray(info.vec,&m_Transform,4);
			}
			RenderTriangleStrip(pRenderInfo->tex,pRenderInfo->tex2,&info,pRenderInfo->blend);
		}
		break;
	}

	GetDevice()->SetTexture(0,NULL);
	if(IsDoubleTexture())
		GetDevice()->SetTexture(1,NULL);
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param ri : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
HRESULT CD3DDevice::Render(stRenderInfo& ri)
{
	m_frameTriCount+=2;
	AddMapRenderInfo(&ri);
	return S_OK;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param ri : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
HRESULT CD3DDevice::Render(stMultiTexRenderInfo& ri)
{
	m_frameTriCount+=2;
	AddMapRenderInfo(&ri);
	return S_OK;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param RECT : ÃèÊö
* \param color : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CD3DDevice::FillRect(const RECT * lprc,DWORD color)
{
	RECT rc;
	if(!::IntersectRect(&rc,lprc,GetRenderRect()))
		return true;

	FinalFlush();

	const float c_error = 0.0f;
	//*
	stVertexBase* v;
	HRESULT hr;
	DWORD dwStartVertex;

	hr = LockVertexBufferAlign(4,(void**)&v,dwStartVertex,sizeof(stVertexBase));
	if(FAILED(hr)) return false;

	v[0].v = D3DXVECTOR4((float)rc.left - c_error,(float)rc.top - c_error,0.0f,1.0f);
	v[0].color = color;

	v[1].v = D3DXVECTOR4((float)rc.left - c_error,(float)rc.bottom - c_error,0.0f,1.0f);
	v[1].color = color;

	v[2].v = D3DXVECTOR4((float)rc.right- c_error,(float)rc.top - c_error,0.0f,1.0f);
	v[2].color = color;

	v[3].v = D3DXVECTOR4((float)rc.right- c_error,(float)rc.bottom- c_error,0.0f,1.0f);
	v[3].color = color;

	UnlockVertexBuffer();

	hr = SetStreamSource(sizeof(stVertexBase),D3DFVF_VERTEX_BASE );
	if(FAILED(hr)) 
	{
		return false;
	}

	SetDeviceDrawState(DrawStateNoTex,Blend_Null);
	hr = GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP,dwStartVertex,2);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param RECT : ÃèÊö
* \param colorLeftTop : ÃèÊö
* \param colorRightTop : ÃèÊö
* \param colorLeftBottom : ÃèÊö
* \param colorRightBottom : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CD3DDevice::FillRect(const RECT * lprc,DWORD colorLeftTop,DWORD colorRightTop,DWORD colorLeftBottom,DWORD colorRightBottom)
{

	RECT rc;
	if(!::IntersectRect(&rc,lprc,GetRenderRect()))
		return true;

	FinalFlush();

	const float c_error = 0.0f;
	//*
	stVertexBase* v;
	HRESULT hr;
	DWORD dwStartVertex;

	hr = LockVertexBufferAlign(4,(void**)&v,dwStartVertex,sizeof(stVertexBase));
	if(FAILED(hr)) return false;

	v[0].v = D3DXVECTOR4((float)rc.left - c_error,(float)rc.top - c_error,0.0f,1.0f);
	v[0].color = colorLeftTop;

	v[1].v = D3DXVECTOR4((float)rc.left - c_error,(float)rc.bottom - c_error,0.0f,1.0f);
	v[1].color = colorLeftBottom;

	v[2].v = D3DXVECTOR4((float)rc.right - c_error,(float)rc.top - c_error,0.0f,1.0f);
	v[2].color = colorRightTop;

	v[3].v = D3DXVECTOR4((float)rc.right - c_error,(float)rc.bottom - c_error,0.0f,1.0f);
	v[3].color = colorRightBottom;

	UnlockVertexBuffer();

	hr = SetStreamSource(sizeof(stVertexBase),D3DFVF_VERTEX_BASE );
	if(FAILED(hr)) 
	{
		return false;
	}

	SetDeviceDrawState(DrawStateNoTex,Blend_Null);
	bool bShadeGouraud = false;
	if(colorLeftTop != colorRightTop || colorLeftTop != colorLeftBottom || colorLeftTop != colorRightBottom){
		bShadeGouraud = true;
		__SetRenderState( D3DRS_SHADEMODE,     D3DSHADE_GOURAUD );
	}

	hr = GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP,dwStartVertex,2);

	if(bShadeGouraud)
		__SetRenderState( D3DRS_SHADEMODE,     D3DSHADE_FLAT );

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	return true;
	//*/
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param RECT : ÃèÊö
* \param color : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CD3DDevice::DrawRectangle(const RECT * lprc,DWORD color)
{
	stVertexBase* v;
	HRESULT hr;

	RECT rc;
	if(!::IntersectRect(&rc,lprc,GetRenderRect()))
		return true;
	const float c_error = 0.0f;
	FinalFlush();

	DWORD dwStartVertex;
	hr = LockVertexBufferAlign(5,(void**)&v,dwStartVertex,sizeof(stVertexBase));
	if(FAILED(hr)) return false;

	v[0].v = D3DXVECTOR4((float)lprc->left - c_error,(float)lprc->top - c_error,0.0f,1.0f);
	v[0].color = color;

	v[1].v = D3DXVECTOR4((float)lprc->right - c_error,(float)lprc->top - c_error,0.0f,1.0f);
	v[1].color = color;

	v[2].v = D3DXVECTOR4((float)lprc->right - c_error,(float)lprc->bottom - c_error,0.0f,1.0f);
	v[2].color = color;

	v[3].v = D3DXVECTOR4((float)lprc->left - c_error,(float)lprc->bottom - c_error,0.0f,1.0f);
	v[3].color = color;

	v[4].v = D3DXVECTOR4((float)lprc->left - c_error,(float)lprc->top - c_error,0.0f,1.0f);
	v[4].color = color;

	UnlockVertexBuffer();

	hr = SetStreamSource(sizeof(stVertexBase),D3DFVF_VERTEX_BASE);
	if(FAILED(hr)) 
	{
		return false;
	}

	SetDeviceDrawState(DrawStateNoTex,Blend_Null);

	hr = GetDevice()->DrawPrimitive(D3DPT_LINESTRIP,dwStartVertex,4);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param POINT : ÃèÊö
* \param triangleNum : ÃèÊö
* \param color : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CD3DDevice::DrawTriangleFan(const POINT * pt,DWORD triangleNum,DWORD color)
{
	FinalFlush();

	const float c_error = 0.0f;

	stVertexBase* v;
	HRESULT hr;
	DWORD dwStartVertex;

	if(triangleNum + 2 > c_maxVertexNum) triangleNum = c_maxVertexNum - 2;

	hr = LockVertexBufferAlign(triangleNum + 2,(void**)&v,dwStartVertex,sizeof(stVertexBase));
	if(FAILED(hr)) return false;

	for(size_t i = 0; i< triangleNum + 2 ; ++i)
	{
		v[i].v = D3DXVECTOR4((float)pt[i].x - c_error,(float)pt[i].y - c_error,0.0f,1.0f);
		v[i].color = color;
	}

	UnlockVertexBuffer();

	hr = SetStreamSource( sizeof(stVertexBase),D3DFVF_VERTEX_BASE );
	if(FAILED(hr)) 
	{
		return false;
	}

	SetDeviceDrawState(DrawStateNoTex,Blend_Null);

	hr = GetDevice()->DrawPrimitive(D3DPT_TRIANGLEFAN,dwStartVertex,triangleNum);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pt1 : ÃèÊö
* \param pt2 : ÃèÊö
* \param pt3 : ÃèÊö
* \param color1 : ÃèÊö
* \param color2 : ÃèÊö
* \param color3 : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CD3DDevice::DrawTriangle(POINT pt1,POINT pt2,POINT pt3,DWORD color1,DWORD color2,DWORD color3)
{
	FinalFlush();

	const float c_error = 0.0f;

	stVertexBase* v;
	HRESULT hr;
	DWORD dwStartVertex;

	hr = LockVertexBufferAlign(3,(void**)&v,dwStartVertex,sizeof(stVertexBase));
	if(FAILED(hr)) return false;

	v[0].v = D3DXVECTOR4((float)pt1.x - c_error,(float)pt1.y - c_error,0.0f,1.0f);
	v[0].color = color1;

	v[1].v = D3DXVECTOR4((float)pt2.x - c_error,(float)pt2.y  - c_error,0.0f,1.0f);
	v[1].color = color2;

	v[2].v = D3DXVECTOR4((float)pt3.x - c_error,(float)pt3.y - c_error,0.0f,1.0f);
	v[2].color = color3;

	UnlockVertexBuffer();

	hr = SetStreamSource( sizeof(stVertexBase),D3DFVF_VERTEX_BASE );
	if(FAILED(hr)) 
	{
		return false;
	}

	SetDeviceDrawState(DrawStateNoTex,Blend_Null);

	bool bShadeGouraud = false;
	if(color1 != color2 || color1 != color3){
		bShadeGouraud = true;
		__SetRenderState( D3DRS_SHADEMODE,     D3DSHADE_GOURAUD );
	}

	hr = GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST,dwStartVertex,1);

	if(bShadeGouraud)
		__SetRenderState( D3DRS_SHADEMODE,     D3DSHADE_FLAT );

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	return true;
}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param alpha : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
#define radian2angle(X) ( (X) * 180.f/ PI )
#define angle2radian(X) ( (X) * PI / 180.f)
// ×ª»»½Ç¶Èµ½ 0 ¡« 2PI Ö®¼ä
inline float standAngle(float alpha)
{
	if(alpha < 0) 
	{
		float f = -alpha;
		int i = (int)(f/(PI*2.0f));
		alpha = PI*2.0f - (f - i*PI*2.0f);
	}

	if(alpha > PI*2.0f) 
	{
		float f = alpha;
		int i = (int)(f/(PI*2.0f));
		alpha = f - i*PI*2.0f;
	}
	return alpha;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pt1 : ÃèÊö
* \param pt2 : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
inline float GetDis(POINT pt1,POINT pt2)
{
	return sqrtf( (float)((pt1.x - pt2.x) * (pt1.x - pt2.x) + (pt1.y - pt2.y) * (pt1.y - pt2.y)) );
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param RECT : ÃèÊö
* \param angle : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
inline POINT GetRectAnglePoint(const RECT * prect,float angle)
{
	int w = (prect->right - prect->left)/2;
	int h = (prect->top - prect->bottom)/2;

	stPointI pt0(prect->left + w,prect->bottom + h);
	stLine line1(pt0,angle);

	stPointI pt1,pt2;
	bool b= line1.Intersection(prect,pt1,pt2);
	Assert(b);

	if(pt1.y == pt2.y)
	{
		if( angle >= angle2radian(90) )
		{
			if(pt1.x < pt2.x) return pt1;
			return pt2;
		}
		else
		{
			if(pt1.x > pt2.x) return pt1;
			return pt2;
		}
	}

	if( angle <= angle2radian(180) )
	{
		if(pt1.y > pt2.y) return pt1;	
		return pt2;
	}

	if(pt1.y < pt2.y) return pt1;
	return pt2;
}
// ·½ÏòÄæÊ±Õë


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param RECT* : ÃèÊö
* \param fStartAngle : ÃèÊö
* \param fEndAngle : ÃèÊö
* \param color : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CD3DDevice::DrawRectPie( const RECT* prect,float fStartAngle,float fEndAngle,DWORD color )
{
	if( fabs(fStartAngle - fEndAngle) < 0.000001) return true;

	struct stAnglePoint{
		float a;
		stPointI pt;
	};

	int w = prect->right - prect->left;
	int h = prect->bottom - prect->top;
	stAnglePoint ap[] = {
		{angle2radian(0),stPointI(prect->right,prect->top + h/2)},
		{angle2radian(45),stPointI(prect->right,prect->top)},
		{angle2radian(90),stPointI(prect->left + w/2,prect->top)},
		{angle2radian(135),stPointI(prect->left,prect->top)},
		{angle2radian(180),stPointI(prect->left,prect->top + h/2)},
		{angle2radian(225),stPointI(prect->left,prect->bottom)},
		{angle2radian(270),stPointI(prect->left + w/2,prect->bottom)},
		{angle2radian(315),stPointI(prect->right,prect->bottom)},
	};

	stPointI ptS,ptE;
	float as,ae;
	as = standAngle(fStartAngle);	// ÆğÊ¼½Ç¶È
	ae = standAngle(fEndAngle);		// ½áÊø½Ç¶È

	stRectI rcPixel = *prect;
	rcPixel.top = GetHeight() - rcPixel.top;
	rcPixel.bottom = GetHeight() - rcPixel.bottom;

	ptS = GetRectAnglePoint( &rcPixel,as );
	ptE = GetRectAnglePoint( &rcPixel,ae );

	ptS.y = GetHeight() - ptS.y;
	ptE.y = GetHeight() - ptE.y;

	stPointI pt0(prect->left + w/2,prect->top + h/2);


	SetFrameAllocator fa;
	std::vector<stPointI,std::stack_allocator<stPointI> > vPoint;
	vPoint.reserve(32);
	vPoint.push_back(pt0);

	// ²éÕÒÆğÊ¼½Ç¶ÈµÄÇø¼ä
	int ix1,ix2;
	for( ix1 = 0; ix1 < count_of(ap);++ix1 )
	{
		if( ap[ix1].a > as )
		{
			break;
		}
	}
	if(ix1 == count_of(ap)) ix1 = 0;
	ix1--;
	if(ix1 < 0) ix1 = count_of(ap) - 1;

	for( ix2 = count_of(ap) - 1; ix2 >= 0; --ix2 )
	{
		if( ae > ap[ix2].a )
		{
			break;
		}
	}
	if(ix2 < 0) ix2 = count_of(ap) - 1;

	int ix = ix1;
	while(1)
	{
		stPointI pt1,pt2;
		if(ix == ix1) pt1 = ptS;
		else pt1 = ap[ix].pt;

		bool bEnd  = (ix == ix2);

		ix ++;
		if( ix >= count_of(ap)) ix = 0;

		if(bEnd) pt2 = ptE;
		else pt2 = ap[ix].pt;

		vPoint.push_back(pt1);
		vPoint.push_back(pt2);

		if(bEnd) break;
	}

	DrawTriangleFan(&vPoint[0],vPoint.size()-2,color);

	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param POINT* : ÃèÊö
* \param pointCount : ÃèÊö
* \param color : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CD3DDevice::DrawPolyLine(const POINT* points,size_t pointCount,DWORD color)
{
	Assert(pointCount <= c_maxVertexNum);

	stVertexBase* v;
	HRESULT hr;

	FinalFlush();
	
	DWORD dwStartVertex;

	hr = LockVertexBufferAlign(pointCount,(void**)&v,dwStartVertex,sizeof(stVertexBase));
	if(FAILED(hr)) return false;

	const float c_error = 0.0f;
	for(size_t i=0;i<pointCount;++i)
	{
		v[i].v = D3DXVECTOR4((float)points[i].x - c_error,(float)points[i].y - c_error,0.0f,1.0f);
		v[i].color = color;
	}

	UnlockVertexBuffer();

	hr = GetDevice()->SetTexture(0,NULL);

	if(FAILED(hr)) 
	{
		return false;
	}

	hr = SetStreamSource(sizeof(stVertexBase),D3DFVF_VERTEX_BASE);
	if(FAILED(hr)) 
	{
		return false;
	}

	SetDeviceDrawState(DrawStateNoTex,Blend_Null);

	hr = GetDevice()->DrawPrimitive(D3DPT_LINELIST,dwStartVertex,pointCount-1);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param POINT : ÃèÊö
* \param POINT : ÃèÊö
* \param color : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CD3DDevice::DrawLine(const POINT * pt1,const POINT *pt2,DWORD color)
{
	stVertexBase* v;
	HRESULT hr;

	RECT rc = {getMin(pt1->x,pt2->x),getMin(pt1->y,pt2->y),getMax(pt1->x,pt2->x)+1,getMax(pt1->y,pt2->y)+1};
	if(!::IntersectRect(&rc,&rc,GetRenderRect()))
		return false;
	const float c_error = 0.0f;
	FinalFlush();
	DWORD dwStartVertex;
	hr = LockVertexBufferAlign(2,(void**)&v,dwStartVertex,sizeof(stVertexBase));
	if(FAILED(hr)) return false;

	v[0].v = D3DXVECTOR4((float)pt1->x - c_error,(float)pt1->y - c_error,0.0f,1.0f);
	v[0].color = color;

	v[1].v = D3DXVECTOR4((float)pt2->x - c_error,(float)pt2->y - c_error,0.0f,1.0f);
	v[1].color = color;

	UnlockVertexBuffer();


	hr = SetStreamSource(sizeof(stVertexBase),D3DFVF_VERTEX_BASE);
	if(FAILED(hr)) 
	{
		return false;
	}

	SetDeviceDrawState(DrawStateNoTex,Blend_Null);

	hr = GetDevice()->DrawPrimitive(D3DPT_LINELIST,dwStartVertex,1);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	return true;
}

//-----------------------------------------------------------
//
//Add By David For 2006-3-9
bool CD3DDevice::DrawLine( int x1,int y1,int x2,int y2,DWORD color)
{
	stVertexBase* v;
	HRESULT hr;

	POINT pt1 = { x1,y1},pt2 = { x2, y2 };

	RECT rc = {getMin(pt1.x,pt2.x),getMin(pt1.y,pt2.y),getMax(pt1.x,pt2.x)+1,getMax(pt1.y,pt2.y)+1};
	if(!::IntersectRect(&rc,&rc,GetRenderRect()))
		return false;
	const float c_error = 0.0f;
	FinalFlush();
	DWORD dwStartVertex;
	hr = LockVertexBufferAlign(2,(void**)&v,dwStartVertex,sizeof(stVertexBase));
	if(FAILED(hr)) return false;

	v[0].v = D3DXVECTOR4((float)pt1.x - c_error,(float)pt1.y - c_error,0.0f,1.0f);
	v[0].color = color;

	v[1].v = D3DXVECTOR4((float)pt2.x - c_error,(float)pt2.y - c_error,0.0f,1.0f);
	v[1].color = color;

	UnlockVertexBuffer();


	hr = SetStreamSource(sizeof(stVertexBase),D3DFVF_VERTEX_BASE);
	if(FAILED(hr)) 
	{
		return false;
	}

	SetDeviceDrawState(DrawStateNoTex,Blend_Null);

	hr = GetDevice()->DrawPrimitive(D3DPT_LINELIST,dwStartVertex,1);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pTexture : ÃèÊö
* \param RECT* : ÃèÊö
* \param RECT* : ÃèÊö
* \param dwColor : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CD3DDevice::DrawTexture(IDirect3DTexture8* pTexture, const RECT* pRect, const RECT* pRectTexture, DWORD dwColor,BlendType blend)
{
	RECT rc;
	stRectI rcDst;

	HRESULT hr;

	D3DSURFACE_DESC desc;
	hr = pTexture->GetLevelDesc(0,&desc);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	if(pRect == NULL)
	{
		if(pRectTexture)
		{
			rcDst = stRectI(0,0,pRectTexture->right-pRectTexture->left,pRectTexture->bottom-pRectTexture->top);
		}
		else
		{
			rcDst = stRectI(0,0,desc.Width,desc.Height);
		}
		pRect = &rcDst;
	}

	if(!::IntersectRect(&rc,pRect,GetRenderRect()))
		return true;

	stRectI rcTexture;
	if( pRectTexture == NULL )
	{
		rcTexture = stRectI(0,0,desc.Width,desc.Height);

	}
	else
	{
		rcTexture = *pRectTexture;
	}

	rcTexture.left += rc.left - pRect->left;
	rcTexture.top += rc.top - pRect->top;

	rcTexture.right += rc.left - pRect->left;
	rcTexture.bottom += rc.top - pRect->top;

	FinalFlush();

	//*
	stVertexBitmap* v;

	DWORD dwStartVertex;
	hr = LockVertexBufferAlign(4,(void**)&v,dwStartVertex,sizeof(stVertexBitmap));
	if(FAILED(hr)) return false;

	v[0].v = D3DXVECTOR4((float)rc.left - c_error,(float)rc.top - c_error,0.0f,1.0f);
	v[0].uv = D3DXVECTOR2( (float)rcTexture.left/(float)desc.Width,(float)rcTexture.top/(float)desc.Height );
	v[0].color = dwColor;

	v[1].v = D3DXVECTOR4((float)rc.left - c_error,(float)rc.bottom - c_error,0.0f,1.0f);
	v[1].uv = D3DXVECTOR2( (float)rcTexture.left/(float)desc.Width,(float)rcTexture.bottom/(float)desc.Height );
	v[1].color = dwColor;

	v[2].v = D3DXVECTOR4((float)rc.right - c_error,(float)rc.top - c_error,0.0f,1.0f);
	v[2].uv = D3DXVECTOR2( (float)rcTexture.right/(float)desc.Width,(float)rcTexture.top/(float)desc.Height );
	v[2].color = dwColor;

	v[3].v = D3DXVECTOR4((float)rc.right - c_error,(float)rc.bottom - c_error,0.0f,1.0f);
	v[3].uv = D3DXVECTOR2( (float)rcTexture.right/(float)desc.Width,(float)rcTexture.bottom/(float)desc.Height );
	v[3].color = dwColor;

	UnlockVertexBuffer();

	hr = GetDevice()->SetTexture(0,pTexture);

	if(FAILED(hr)) 
	{
		return false;
	}

	hr = SetStreamSource(sizeof(stVertexBitmap),D3DFVF_BITMAP);
	if(FAILED(hr)) 
	{
		return false;
	}

	SetDeviceDrawState(DrawStateTex,blend);

	hr = GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP,dwStartVertex,2);

	hr = GetDevice()->SetTexture(0,NULL);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pTexture : ÃèÊö
* \param pAlpha : ÃèÊö
* \param stRectF* : ÃèÊö
* \param stRectF* : ÃèÊö
* \param stRectF* : ÃèÊö
* \param color : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CD3DDevice::RenderBlendTexture(CCustomTexture* pTexture,CCustomTexture* pAlpha,const stRectF* pRect, const stRectF* pRectTexture1,const stRectF* pRectTexture2,DWORD color)
{
	if( pTexture->GetTex() == NULL || pAlpha->GetTex() == NULL )
		return false;
	if( IsDoubleTexture())		//	
	{
		return RenderBlendTexture(pTexture->GetTex(),pAlpha->GetTex(),pRect,pRectTexture1,pRectTexture2,color);
	}
	else
	{
		//pTexture->Render(pRect->left,pRect->top,pRectTexture1,NULL,color);
		//*
		int alpha = 255 - COLOR_GET_ALPHA(color);
		pAlpha->Render(pRect->left,pRect->top,pRectTexture2,NULL,COLOR_ARGB(alpha,0,0,0) , Blend_MulInvAlpha);
		pTexture->Render(pRect->left,pRect->top,pRectTexture1,NULL,color,Blend_AlphaAdd);
		//*/
	}
	/*else
	{
	pAlpha->Render(pRect->left,pRect->top,pRectTexture2,NULL,0xff000000,Blend_MulInvAlpha);
	pTexture->Render(pRect->left,pRect->top,pRectTexture1,NULL,-1,Blend_Add2);
	}*/
	return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pTexture : ÃèÊö
* \param pAlpha : ÃèÊö
* \param stRectF* : ÃèÊö
* \param stRectF* : ÃèÊö
* \param stRectF* : ÃèÊö
* \param color : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CD3DDevice::RenderBlendTexture(IDirect3DTexture8* pTexture,IDirect3DTexture8* pAlpha,const stRectF* pRect, const stRectF* pRectTexture1,const stRectF* pRectTexture2,DWORD color)
{
	HRESULT hr;

	D3DSURFACE_DESC desc1,desc2;
	hr = pTexture->GetLevelDesc(0,&desc1);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	hr = pAlpha->GetLevelDesc(0,&desc2);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	stMultiTexRenderInfo ri;

	ri.blend = Blend_Null;
	ri.color = color;
	ri.rect = *pRect;
	ri.uv.SetRect(	(float)pRectTexture1->left/(float)desc1.Width, 
		(float)pRectTexture1->top/(float)desc1.Height, 
		(float)pRectTexture1->right/(float)desc1.Width, 
		(float)pRectTexture1->bottom/(float)desc1.Height );

	ri.uv2.SetRect(	(float)pRectTexture2->left/(float)desc2.Width, 
		(float)pRectTexture2->top/(float)desc2.Height, 
		(float)pRectTexture2->right/(float)desc2.Width, 
		(float)pRectTexture2->bottom/(float)desc2.Height );

	ri.tex = pTexture;
	ri.tex2 = pAlpha;

	FinalFlush();

	stDevMultiTexRenderInfo info;
	FillTriangleStrip(info.vec,ri);
	if(m_bTransform)
	{
		TransformVectorArray(info.vec,&m_Transform,4);
	}
	return SUCCEEDED(RenderTriangleStrip(pTexture,pAlpha,&info,Blend_Null));
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pBmp : ÃèÊö
* \param pAlpha : ÃèÊö
* \param x : ÃèÊö
* \param y : ÃèÊö
* \param color : ÃèÊö
* \param blend : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CD3DDevice::RenderBlendBitmap(IBitmap* pBmp,IBitmap* pAlpha,int x,int y,DWORD color,BlendType blend)
{
	stMultiTexRenderInfo ri;
	switch(pAlpha->m_block.byAlphaType)
	{
	case AlphaType_AllBlack:
		return true;
		break;
	case AlphaType_AllWhite:
		return pBmp->Render(x,y,color,blend);
		break;
	case AlphaType_OneOnly:
		{
			stRectI rect = pAlpha->GetClipRect();
			x += rect.left;
			y += rect.top;
			bool b = pBmp->Render(x,y,&rect,NULL,ColorBlend(color,COLOR_ARGB(pAlpha->m_block.byAlpha,255,255,255)));
			return b;
		}
		break;
	}
	/*CD3DBitmap* pD3DAlpha = (CD3DBitmap*)pAlpha;
	stRectI rectAlpha = pAlpha->GetClipRect();

	stRenderInfo ri1,ri2;

	bool bResult = false;
	enumBlockRenderInfo e = pD3DAlpha->GetBlockRenderInfo(stPointI(0,0),stPointF(x + rectAlpha.left,y + rectAlpha.top),NULL,NULL,color,blend,ri2);

	if( e == BlockRenderInfo_Error ) goto _render_ret;
	if( e != BlockRenderInfo_Render )
	{
		bResult = true;
		goto _render_ret;
	}

	pD3DAlpha->SetLastRenderTime(xtimeGetTime());

	bResult = SUCCEEDED(Render(ri2));
	return bResult;*/
	if(IsDoubleTexture())
	{
		CD3DBitmap* pD3DBmp = (CD3DBitmap*)pBmp;
		CD3DBitmap* pD3DAlpha = (CD3DBitmap*)pAlpha;

		bool bResult = false;

		stRectI rectAlpha = pAlpha->GetClipRect();

		stRenderInfo ri1,ri2;
		stMultiTexRenderInfo ri;
		enumBlockRenderInfo e = pD3DBmp->GetBlockRenderInfo(stPointI(0,0),stPointF((float)x,(float)y),&rectAlpha,NULL,color,blend,ri1);
		if( e == BlockRenderInfo_Error ) goto _render_ret;
		if( e != BlockRenderInfo_Render ) 
		{
			bResult = true;
			goto _render_ret;
		}

		e = pD3DAlpha->GetBlockRenderInfo(stPointI(0,0),stPointF((float)(x + rectAlpha.left),(float)(y + rectAlpha.top)),NULL,NULL,color,blend,ri2);

		if( e == BlockRenderInfo_Error ) goto _render_ret;
		if( e != BlockRenderInfo_Render )
		{
			bResult = true;
			goto _render_ret;
		}

		Assert( ri1.rect == ri2.rect );
		Assert(ri1.uv.Width() == ri2.uv.Width() && ri1.uv.Height() == ri2.uv.Height());

		pD3DBmp->SetLastRenderTime(xtimeGetTime());
		pD3DAlpha->SetLastRenderTime(xtimeGetTime());

		ri.blend = blend;
		ri.color = color;
		ri.rect = ri1.rect;
		ri.uv = ri1.uv;
		ri.uv2 = ri2.uv;
		ri.tex = pD3DBmp->GetBlockTexture(stPointI(0,0))->GetTexCache()->GetTex();
		ri.tex2 = pD3DAlpha->GetBlockTexture(stPointI(0,0))->GetTexCache()->GetTex();
		bResult = SUCCEEDED(Render(ri));;
_render_ret:
		return bResult;
	}
	else
	{
		pBmp->SetLastRenderTime(xtimeGetTime());
		pAlpha->SetLastRenderTime(xtimeGetTime());
		CD3DMBitmap* pMBmp = GetD3DMBitmapManager()->GetMBitmap((CD3DBitmap*)pBmp,(CD3DBitmap*)pAlpha);
		if(pMBmp) return pMBmp->Render(x,y,color,blend);
	}
	return false;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pBmp : ÃèÊö
* \param pAlpha : ÃèÊö
* \param x : ÃèÊö
* \param y : ÃèÊö
* \param RECT* : ÃèÊö
* \param stPointF : ÃèÊö
* \param color : ÃèÊö
* \param blend : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CD3DDevice::RenderBlendBitmap(IBitmap* pBmp,IBitmap* pAlpha,int x,int y,const RECT* rect,const stPointF * scale,DWORD color,BlendType blend )
{
	stMultiTexRenderInfo ri;
	switch(pAlpha->m_block.byAlphaType)
	{
	case AlphaType_AllBlack:
		return true;
		break;
	case AlphaType_AllWhite:
		return pBmp->Render(x,y,rect,scale,color,blend);
		break;
	case AlphaType_OneOnly:
		{
			stRectI alphaClip = pAlpha->GetClipRect();

			if(rect)	alphaClip &= *rect;

			if(scale)
			{
				x += alphaClip.left * (int)scale->x;
				y += alphaClip.top * (int)scale->y;
			}
			else
			{
				x += alphaClip.left;
				y += alphaClip.top;
			}

			bool b = pBmp->Render(x,y,&alphaClip,scale,ColorBlend(color,COLOR_ARGB(pAlpha->m_block.byAlpha,255,255,255)));

			return b;
		}
		break;
	}
	if(IsDoubleTexture())
	{
		CD3DBitmap* pD3DBmp = (CD3DBitmap*)pBmp;
		CD3DBitmap* pD3DAlpha = (CD3DBitmap*)pAlpha;

		stRectI rectAlpha = pAlpha->GetClipRect();

		POINT ptAlpha;
		if(scale)
		{
			ptAlpha.x = x + rectAlpha.left * (int)scale->x;
			ptAlpha.y = y + rectAlpha.top * (int)scale->y;
		}
		else
		{
			ptAlpha.x = x + rectAlpha.left;
			ptAlpha.y = y + rectAlpha.top;
		}

		if(rect) rectAlpha &= *rect;

		bool bResult = false;
		stRenderInfo ri1,ri2;
		stMultiTexRenderInfo ri;
		enumBlockRenderInfo e = pD3DBmp->GetBlockRenderInfo(stPointI(0,0),stPointF((float)x,(float)y),&rectAlpha,scale,color,blend,ri1);
		if( e == BlockRenderInfo_Error ) goto _render_ret;
		if( e != BlockRenderInfo_Render ) 
		{
			bResult = true;
			goto _render_ret;
		}

		e = pD3DAlpha->GetBlockRenderInfo(stPointI(0,0),ptAlpha,rect,scale,color,blend,ri2);

		if( e == BlockRenderInfo_Error ) goto _render_ret;
		if( e != BlockRenderInfo_Render )
		{
			bResult = true;
			goto _render_ret;
		}

		// Test
		//Assert( ri1.rect == ri2.rect );
		//Assert(ri1.uv.Width() == ri2.uv.Width() && ri1.uv.Height() == ri2.uv.Height());

		pD3DBmp->SetLastRenderTime(xtimeGetTime());
		pD3DAlpha->SetLastRenderTime(xtimeGetTime());

		ri.blend = blend;
		ri.color = color;
		ri.rect = ri1.rect;
		ri.uv = ri1.uv;
		ri.uv2 = ri2.uv;
		ri.tex = pD3DBmp->GetBlockTexture(stPointI(0,0))->GetTexCache()->GetTex();
		ri.tex2 = pD3DAlpha->GetBlockTexture(stPointI(0,0))->GetTexCache()->GetTex();
		bResult = SUCCEEDED(Render(ri));
_render_ret:
		return bResult;
	}
	else
	{
		pBmp->SetLastRenderTime(xtimeGetTime());
		pAlpha->SetLastRenderTime(xtimeGetTime());
		CD3DMBitmap* pMBmp = GetD3DMBitmapManager()->GetMBitmap((CD3DBitmap*)pBmp,(CD3DBitmap*)pAlpha);
		if(pMBmp) return pMBmp->Render(x,y,rect,scale,color,blend);
		return false;
	}
}

inline HRESULT	CD3DDevice::SetStreamSource(DWORD dwStride,DWORD dwFVF)
{
	HRESULT hr;
	/*hr = GetDevice()->SetStreamSource(0,GetVB(),dwStride);
	if(FAILED(hr)){
		return hr;
	}
	hr = GetDevice()->SetVertexShader(dwFVF);
	return hr;*/
	//*
	if(dwStride != m_dwStride || dwFVF != m_dwFVF)
	{
		hr = GetDevice()->SetStreamSource(0,GetVB(),dwStride);
		if(FAILED(hr))
			return hr;
		m_dwStride = dwStride;

		hr = GetDevice()->SetVertexShader(dwFVF);
		if(FAILED(hr))
		{
			return hr;
		}
		m_dwFVF = dwFVF;
	}
	//*/
	return true;
}