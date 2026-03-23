/**
 *\file		Particle.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:20:00 中国标准时间
 *\brief	粒子
 *
 * 
 *
*/

//--------------------------------------------------------------------------
//	This is the basic particle
//---------------------------------------------------------------------------

#ifndef __CPARTICLE_H_
#define __CPARTICLE_H_

#pragma once


//-----------------------------------------------------------------------------
// Custom vertex : a little same as billboard
//-----------------------------------------------------------------------------
struct COLORVERTEX
{
	D3DXVECTOR4	p;
	D3DXCOLOR	color;
	FLOAT       tu;
	FLOAT       tv;

	COLORVERTEX()
	{
		p.x = 0.0f;
		p.y = 0.0f;
		p.z = 0.0f;
		p.w = 1.0f;
		color = 0xffffffff;
	}
	COLORVERTEX( D3DXVECTOR4 vPos, D3DXCOLOR vColor, FLOAT tU, FLOAT tV )
	{
		p = vPos;
		p.z = 0.0f;
		p.w = 1.0f;
		color = vColor;
		tu = tU;
		tv = tV;
	}
};
#define cvFVF ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define FILL_COLOR_VERTEX(v, ax, ay, az, acolor, atu, atv) \
{\
	v.p.x = ax;	v.p.y = ay;	v.p.z = 0.0f; v.p.w = 1.0f;\
	v.color = acolor;\
	v.tu = atu;	v.tv = atv;\
}

//Use point sprite if hardware support
//But point sprite seemed not support particle scale
struct POINTVERTEX {
	D3DXVECTOR3 p;
	D3DCOLOR    color;

	POINTVERTEX()
	{
		p = D3DXVECTOR3(0.0f,0.0f,0.0f);
		color = 0xffffffff;
	}
	POINTVERTEX( D3DXVECTOR3 vPos, D3DCOLOR vColor )
	{
		p = vPos;
		p.z = 0.0f;
		color = vColor;
	}
};

#define psFVF ( D3DFVF_XYZ | D3DFVF_DIFFUSE )

//CParticle class
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CParticle
{
	friend class CParticleSystem;

public:

	// usual suspects, Constructor and Destructor
	CParticle();
	~CParticle();

	// Update method with TimeDelta for time-based movement
	bool Update( float fTimeDelta, stPointF offset ); //time lasted between two frame

	// Location of particle at last update
	///之前位置
	D3DXVECTOR3		m_d3dvPrevLocation;

	// Current location of particle
	///当前位置
	D3DXVECTOR3		m_d3dvLocation;

	// Current velocity of particle
	///速度
	D3DXVECTOR3		m_d3dvVelocity;

	// Current Color and how much color to add over time
	///颜色
	D3DCOLORVALUE	m_d3dcColor;
	///颜色分布
	D3DCOLORVALUE	m_d3dcColorDelta;

	// size of particle in world units and delta to add over time
	///尺寸
	float		m_fSize;
	///尺寸分布
	float		m_fSizeDelta;

	// translucency and delta to add over time
	// this may seem counter-intuitive, but 0.0f Alpha is transparent
	///透明度
	float		m_fAlpha;
	///透明度分布
	float		m_fAlphaDelta;

	// gravity ratio and delta to add over time
	// this is a proportion of normal gravity and can go negative
	///重力
	float		m_fGravity;
	///重力分布
	float		m_fGravityDelta;

	// Age of particle in seconds
	///年龄
	float		m_fAge;
	// Age at which particle dies
	//This time we do not consider those event dies or live for ever
	///生命
	float		m_fLifetime;	

	// Wether this is a spark
	///是否Spark
	bool		m_bSpark;
	///产生的Spark数目
	int			m_sparkNum;

	// Offset cause of character move
	///位置偏移
	stPointF	m_offset;

	// Set ParticleSystem as parent of particle
	void		SetParent(CParticleSystem* Parent){ m_pParent = Parent;}
	
	///链表中下个粒子指针
	CParticle*	m_pNext;		//Next particle at list
	
	bool		IsInLiveRange();

private:
	// our parent
	///所属粒子系统指针
	CParticleSystem* m_pParent;
};

#endif