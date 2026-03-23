/**
 *\file		EffectRain.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:17:00 中国标准时间
 *\brief	下雨的效果
 *
 * 
 *
*/

#pragma		once

class CCustomTexture;
class CRenderTargetTexture;

#define RAIN_IMG_NUM		2  //soke 下雨图片数量
#define RAIN_IMG_SIZE		256

//#define USE_RAIN_EFFECT

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CEffectRain
{
	///源纹理
	CCustomTexture*		m_pSourceImg[RAIN_IMG_NUM];
	///临时纹理
	CRenderTargetTexture*	m_pRenderBmp;
	///下雨速度
	float				m_moveSpeed;
	///雨颜色
	DWORD				m_color;
	///雨的大小
	int					m_heavyGrade;
	///刷新频率
	float				m_updateFrequency;
	///是否需要刷新
	bool				m_bNeedUpdate;
public:
	CEffectRain(void);
	~CEffectRain();

	//
	void SetMoveSpeed(float speed) { m_moveSpeed = speed; }
	void SetColor(DWORD color) { m_color = color; }
	void SetHeavyGrade(int heavy) { m_heavyGrade = heavy; }

	bool InitDevice();
	void UnInitDevice();
	//
    HRESULT Update();
	HRESULT Render(float fElapsedTime);
	//
	void ReleaseAll();

};