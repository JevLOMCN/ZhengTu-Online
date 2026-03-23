/**
 *\file		EffectCloud.h
 *\version	$Id$
 *\author	$$$02公司
 *\			$$$02公司@###.com
 *\date		2005-7-5   17:17:00 中国标准时间
 *\brief	云彩效果
 *
 * 详细描述
 *
*/

#pragma once

class CRenderTargetTexture;
class CCustomTexture;

#define SOURCE_IMG_NUM		4
#define SOURCE_IMG_SIZE		32

//#define USE_CLOUD_EFFECT

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CEffectCloud
{
public:
	CEffectCloud(void);
	~CEffectCloud();

	bool InitDevice();
	void UnInitDevice();

	void SetColor(const DWORD& color) { m_color = color; }
	void SetPosition(const stPointI& pt) { m_position = pt; }
	void SetPosition(int x, int y) { m_position.x = x; m_position.y = y; }
	void SetSize(int width, int height) { m_width = width; m_height = height; }
	void SetMoveSpeed(float speed) { m_moveSpeed = speed; }
	void SetUpdateFrequency(float frequency) { m_updateFrequency = frequency; }
	void SetDensity(int density);
	void SetCloudVolume(int volume) { m_cloudVolume = volume; }
	void SetScaleRate(float scale) { m_scaleRate = scale; }
    
	DWORD GetColor() { return m_color; }
	stPointI GetPosition() { return m_position; }
	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }
	float GetMoveSpeed() { return m_moveSpeed; }
	float GetFrequency() { return m_updateFrequency; }
	int GetDensity() { return m_density; }
	int GetCloudVolume() { return m_cloudVolume; }
	float GetScaleRate() { return m_scaleRate; }
	
	//
	HRESULT Update();
	HRESULT Render(float fElapsedTime);

	void Stop() { m_bPlaying = false; }
	void Play() { m_bPlaying = true; }
	//
	void ReleaseAll();
private:
	///噪音图
	CCustomTexture*			m_pSourceImg[SOURCE_IMG_NUM][SOURCE_IMG_NUM];		//Our source noise img
	///临时纹理
	CRenderTargetTexture*	m_pRenderBmp[SOURCE_IMG_NUM+1];		//for render target use
	///云彩的颜色
	DWORD					m_color;	//this time just for white cloud
	///云彩位置
	stPointI				m_position;
	///云彩宽度
	int						m_width;
	///云彩高度
	int						m_height;
	///云彩移动速度
	float					m_moveSpeed;		//the bigger the quicker
	///云彩浓度
	int						m_density;			//0~255	the larger the thick
	///云彩刷新频率
	float					m_updateFrequency;			//ms
	///刷新指示
	bool					m_bNeedUpdate;
	///是否播放云彩移动效果
	bool					m_bPlaying;
	///0~10
	int						m_cloudVolume;
	///缩放比例		1~8
	float					m_scaleRate;
	//////////////////////////////////////////////////////////////////////////
	bool					m_bReadyDraw;
};

