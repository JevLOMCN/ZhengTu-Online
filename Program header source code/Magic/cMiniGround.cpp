#include "../engine/engine.h"

#include ".\cminiground.h"
#include <iostream>
#include "public.h"

#define OPEN_FILE(f,file_name) (f.Create(file_name,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NORMAL))

using namespace std;
#define appDump cout

const int c_xScale = 32;
const int c_yScale = 32;

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param b : √Ë ˆ
 * \param g : √Ë ˆ
 * \param r : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
inline WORD COLOR_B5G6R5(BYTE b, BYTE g,BYTE r )
{
	//	return  (WORD)( ( ((b) & 0xf8) >> 3 ) | ( ((g) & 0xfc) << 3 ) |  ( ((r) & 0xf8) << 8 )) ;
	return  (WORD)( ( ((r) & 0xf8) >> 3 ) | ( ((g) & 0xfc) << 3 ) |  ( ((b ) & 0xf8) << 8 )) ;
	//	return  (WORD)( ( ((r) & 0xf8) >> 3 ) | ( ((r) & 0xfc) << 3 ) |  ( ((r ) & 0xf8) << 8 )) ;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
cMiniGround::cMiniGround(void)
{
	m_bHide = false;

	m_szRender.cx = 512;
	m_szRender.cy = 512;

	m_size.cx = 256;
	m_size.cy = 256;

#ifdef USE_JPGTEXTURE
	m_pJpgTexture = NULL;
#else
	ZeroMemory(&m_image,sizeof(m_image));
#endif
	m_pRenderTexture = NULL;

	m_format = D3DFMT_R5G6B5;
	m_nPixelBytes = 2;
	
	DWORD dw = 0x00112233;
	BYTE* p = (BYTE* )&dw;
	WORD color = COLOR_B5G6R5(*p++,*p++,*p++);
	if (color != 4358)
		TRACE("error, the color not the same of debug and release.\n");
		//AfxMessageBox("error");
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
cMiniGround::~cMiniGround(void)
{
	FinalRelease();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void cMiniGround::Draw()
{
	if (!IsValid())
		return;
	RenderTerrain();
	RenderObj();
	//draw lines...
	RECT rc ={0,0,Cache_GetDevice()->GetWidth(),Cache_GetDevice()->GetHeight()};
	OffsetRect(&rc,Ground_GetGround()->GetPos().x,Ground_GetGround()->GetPos().y);
	POINT p0;
	WorldToScreen(*((POINT*)&rc),p0);
	POINT p1;
	WorldToScreen(*(((POINT*)&rc) + 1),p1);
	RECT r ={p0.x,p0.y,p1.x,p1.y};
	Cache_GetDevice()->DrawLineEx(r.left,rc.top,r.right,r.bottom,0xffffffff,0xffffffff);
	for (int x = -1; x <= 2; x++)
	for (int y = -1; y <= 2; y++)
	{

	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param ptScreen : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cMiniGround::MoveTo(POINT ptScreen)
{
	m_ptScreen = ptScreen;
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param sz : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cMiniGround::SetSize(SIZE sz)
{
	if (sz.cx <= m_szRender.cx && sz.cy <= m_szRender.cy)
	{
		m_size = sz;
		Update();
		return true;
	}
	return false;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void cMiniGround::RenderTerrain()
{
	RECT rc ={m_ptScreen.x,m_ptScreen.y,m_ptScreen.x +  m_size.cx,m_ptScreen.y + m_size.cy};
	if (rc!= m_rcScreen)
		//ªÊ÷∆±≥æ∞
		Cache_GetDevice()->DrawRectEx(rc.left,rc.top,rc.right,rc.bottom,0xff000000,0xff000000,0xff000000,0xff000000);

	if (IsRectEmpty(&m_rcScreen))
		return;
	RECT& rc0 = m_rcTexture;
	stRectF st;
	st.left = (float)rc0.left/m_szRender.cx;
	st.right = (float)rc0.right/m_szRender.cx;
	st.top = (float)rc0.top/m_szRender.cy;
	st.bottom = (float)rc0.bottom/m_szRender.cy;

	Cache_GetDevice()->DrawTexture(m_pRenderTexture,m_rcScreen,st,0xffffffff);
	return;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void cMiniGround::RenderObj()
{
	//begin draw simple obj... not use texture...not lightmap...
	const int c_aSize[] = {2,4,8,};	
	for (int i=0; i<PointSize_Num; i++)
	{
		//Cache_GetDevice()->SetPointSize(c_aSize[i]);
		//Cache_GetDevice()->DrawPointArray(m_aPoint[i]);
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param size : √Ë ˆ
 * \param stA : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void cMiniGround::AddObject(PointSizeEnum size,const stDrawPoint& stA)
{
	if (!IsValid())
		return;
	stDrawPoint st;
	st.color = stA.color;
	POINT pt;
	if (!WorldToScreen(stA.pt,pt))
		return;
	st.pt = pt;
	if (size >= 0 && size < PointSize_Num)
		m_aPoint[size].push_back(st);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void cMiniGround::ClearObject()
{
	for (int i=0; i<PointSize_Num; i++)
		m_aPoint[i].clear();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cMiniGround::CreateMiniGround()
{
	m_pRenderTexture = 
		Cache_GetCacheManager()->CacheCreateTexture(
		m_szRender.cx,m_szRender.cy,
		0,
		m_format,
		D3DPOOL_MANAGED);
	return m_pRenderTexture != NULL;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void cMiniGround::FinalRelease()
{
	SAFE_RELEASE(m_pRenderTexture);
#ifdef USE_JPGTEXTURE
	SAFE_RELEASE(m_pJpgTexture);
#else
	if (m_image.DIBBytes)
	{
		free(m_image.DIBBytes);
		m_image.DIBBytes = NULL;
	}
#endif
}

#define JPG_ENC_LEN 1024
#define JPG_ENC 0xEF

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param szMap : √Ë ˆ
 * \param width : √Ë ˆ
 * \param height : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cMiniGround::Load(LPCSTR szMap,int width,int height)
{
	m_ptRenderCenter.x = 0;
	m_ptRenderCenter.y = 0;
	
	m_rcWorldClip.left = 0;
	m_rcWorldClip.top = 0;
	m_rcWorldClip.right = width/c_xScale;
	m_rcWorldClip.bottom = height/c_yScale;

#ifdef USE_JPGTEXTURE
	SAFE_RELEASE(m_pJpgTexture);
	char str[MAX_PATH] = "map\\";
	strcat(str , szMap);
	strcat(str , ".jpg");
	HRESULT hr = D3DXCreateTextureFromFileEx(Cache_GetDevice()->GetDevice9(),
		str,0,0,1,
		0,
		m_format,
		D3DPOOL_SYSTEMMEM,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		&m_info,NULL,
		&m_pJpgTexture);
	//¥À∫Ø ˝∞—Texture∑≈¥Û¡À
	if (FAILED(hr))
	{
#ifdef _DEBUG
		afxDump << "error! Load minimap : " << DXGetErrorDescription9(hr) << "\n";
#endif
	}
	if (m_info.Format != m_format)
	{
#ifdef _DEBUG
		afxDump << "error! minimap is not 24 depth: \n" ;
#endif
		SAFE_RELEASE(m_pJpgTexture);
	}
	return m_pJpgTexture != NULL;
#else
	char str[MAX_PATH] = "graphics\\map\\";
	strcat(str , szMap);
	strcat(str , ".ini");
	LPCSTR szFile = str;
	CAtlFile file;
	OPEN_FILE(file,szFile);
	ULONGLONG ulLen;
	file.GetSize(ulLen);
	int len = ulLen;
	int enc_len = JPG_ENC_LEN;
	if(enc_len > len) enc_len = len;
//		fseek(fp,0,SEEK_SET);
	BYTE* pJPG = (BYTE*)malloc(len);
//		fread(pJPG,len,1,fp);
	file.Read(pJPG,len);
//		fclose(fp);
	for(int i=0;i<enc_len;i++)
		pJPG[i] ^= JPG_ENC;
	bool b = ReadJpgTo565(pJPG,len,m_image);
	free(pJPG);
	return b;
	return true;
#endif
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param ptPos : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void cMiniGround::OnGroundMoveTo(POINT ptPos)
{
	//not finished
	POINT ptCenter = {Cache_GetDevice()->GetWidth()/2,Cache_GetDevice()->GetHeight()/2};
	m_ptGroundCenter = ptCenter + ptPos;

	m_ptGroundCenter.x /= c_xScale;
	m_ptGroundCenter.y /= c_yScale;
	Update();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cMiniGround::Update()
{
	if (!IsValid())
		return false;
	if (!Validate())
		return Build();
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
HRESULT cMiniGround::InvalidateDeviceObjects()
{
	return S_OK;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
HRESULT cMiniGround::RestoreDeviceObjects() 
{
	return S_OK;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cMiniGround::IsValid()
{
#ifdef USE_JPGTEXTURE
	return (m_pJpgTexture && m_pRenderTexture && !m_bHide);
#else
	return (m_image.DIBBytes && m_pRenderTexture && !m_bHide);
#endif
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cMiniGround::Validate()
{
	if (m_ptRenderCenter.x == 0 && m_ptRenderCenter.y == 0)
		return false;
	//rc on screen...
	int offx = m_size.cx / 2; 
	int offy = m_size.cy / 2;
	RECT rcOffset ={-offx,-offy,offx,offy};
	RECT rcClip = m_rcWorldClip;
	OffsetRect(&rcClip,-m_ptGroundCenter.x,-m_ptGroundCenter.y);
	rcOffset &= rcClip;
	m_rcScreen = rcOffset;
	OffsetRect(&m_rcScreen,offx,offy);
	//»Áπ˚±Ì æµƒ«¯”Ú¥Û”⁄map«¯”Ú,≤√ºı

	//rc on texture
	RECT rc0 = rcOffset;
	POINT pt1 = m_ptGroundCenter-m_ptRenderCenter;
	OffsetRect(&rc0,pt1.x,pt1.y);
	OffsetRect(&rc0,m_szRender.cx/2,m_szRender.cy/2);

	//rc on jpg
	if (rc0.left < 0 || rc0.right > m_szRender.cx || rc0.top < 0 || rc0.bottom > m_szRender.cy)
		return false;

	m_rcTexture = rc0;
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cMiniGround::Build()
{
	//lock then fill
	m_ptRenderCenter = m_ptGroundCenter;
	POINT ptOffset = m_ptGroundCenter;
	ptOffset.x -= m_szRender.cx/2;
	ptOffset.y -= m_szRender.cy/2;
	RECT rcJpg ={ptOffset.x,ptOffset.y,ptOffset.x + m_szRender.cx,ptOffset.y + m_szRender.cy};

	int xoff = 0;
	int yoff = 0;
	if (rcJpg.left < 0)
	{
		xoff = -rcJpg.left;
		rcJpg.left = 0;
	}
	if (rcJpg.right > m_rcWorldClip.right)
	{
		rcJpg.right = m_rcWorldClip.right;
	}
	if (rcJpg.top < 0)
	{
		yoff = -rcJpg.top;
		rcJpg.top = 0;
	}
	if (rcJpg.bottom > m_rcWorldClip.bottom)
	{
		rcJpg.bottom = m_rcWorldClip.bottom;
	}
	//load rect jpg to render 
	HRESULT hr;
	D3DLOCKED_RECT lrSrc,lrDes;
#ifdef USE_JPGTEXTURE
	if (FAILED(hr = m_pJpgTexture->LockRect(0,&lrSrc,NULL,0)))
	{
		ATLASSERT(0);
		return false;
	}
#else
	lrSrc.pBits = m_image.DIBBytes;
	lrSrc.Pitch = m_image.DIBWidth * 2;
#endif
	if (FAILED(hr = m_pRenderTexture->LockRect(0,&lrDes,NULL,0)))
	{
		ATLASSERT(0);
		return false;
	}

	BYTE* pDes = (BYTE* )lrDes.pBits;
	BYTE* pSrc = (BYTE* )lrSrc.pBits;
	pSrc += rcJpg.top * lrSrc.Pitch + rcJpg.left*m_nPixelBytes;
	pDes += yoff * lrDes.Pitch + xoff*m_nPixelBytes;

	for (int y=0; y<rcJpg.bottom - rcJpg.top; y++)
	{
		memcpy(pDes,pSrc,(rcJpg.right-rcJpg.left)*m_nPixelBytes);
		pDes += lrDes.Pitch;
		pSrc += lrSrc.Pitch;
	}

#ifdef USE_JPGTEXTURE
	if (FAILED(hr = m_pJpgTexture->UnlockRect(0)))
	{
		//error
	}
#endif
	if (FAILED(hr = m_pRenderTexture->UnlockRect(0)))
	{
		//error
	}

	if (0)
	{
		hr = D3DXSaveTextureToFile("c:\\minimap.bmp",D3DXIFF_BMP,m_pRenderTexture,NULL);
	}
	return Validate();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param ptOff : √Ë ˆ
 * \param ptOffset : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//Ω´ ¿ΩÁµƒ◊¯±Í£¨◊™Œ™∆¡ƒªµƒ◊¯±Í
bool cMiniGround::WorldToOffset(POINT ptOff,POINT& ptOffset)
{
	ptOff.x /= c_xScale;
	ptOff.y /= c_yScale;
	ptOff -= m_ptGroundCenter;
	if (abs(ptOff.x) >= m_size.cx /2|| abs(ptOff.y) >= m_size.cy/2)
		return false;
	POINT ptCenter = {m_size.cx/2,m_size.cy/2};
	ptOffset = ptCenter + ptOff;
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pt : √Ë ˆ
 * \param ptScreen : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cMiniGround::WorldToScreen(POINT pt,POINT& ptScreen)
{
	//not finished
	if (WorldToOffset(pt,ptScreen))
	{
		ptScreen += m_ptScreen;
		return true;
	}
	return false;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
EXPORT_API iMiniGround* Ground_GetMiniGround()
{
	static cMiniGround s_miniGround;
	return &s_miniGround;
}

//jpg

#define JPG_ENC 0xEF
#define JPG_ENC_LEN 1024

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pJPG : √Ë ˆ
 * \param nJpgLen : √Ë ˆ
 * \param image : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
static bool ReadJpgTo565(const BYTE* pJPG,int nJpgLen,JPEG_CORE_PROPERTIES& image)
{
    memset( &image,0, sizeof( JPEG_CORE_PROPERTIES ) );
    BYTE* imageData;

	if( ijlInit( &image ) != IJL_OK )
	{
		TRACE( "Cannot initialize Intel JPEG library\n" );
		return false;
	}
	image.JPGBytes = (BYTE*)pJPG;
	image.JPGSizeBytes = nJpgLen;
    if( ijlRead( &image, /*IJL_JFILE_READPARAMS*/IJL_JBUFF_READPARAMS ) != IJL_OK )
    {
        TRACE( "Cannot read JPEG file header from %s file\n",
            image.JPGFile );
		return false;
    }

	// !dudnik: to fix bug case 584680, [OT:287A305B]
    // Set the JPG color space ... this will always be
    // somewhat of an educated guess at best because JPEG
    // is "color blind" (i.e., nothing in the bit stream
    // tells you what color space the data was encoded from).
    // However, in this example we assume that we are
    // reading JFIF files which means that 3 channel images
    // are in the YCbCr color space and 1 channel images are
    // in the Y color space.
    switch(image.JPGChannels)
    {
    case 1:
        image.JPGColor    = IJL_G;
        image.DIBChannels = 3;
        image.DIBColor    = IJL_BGR;
        break;

    case 3:
        image.JPGColor    = IJL_YCBCR;
        image.DIBChannels = 3;
        image.DIBColor    = IJL_BGR;
        break;

    case 4:
        image.JPGColor    = IJL_YCBCRA_FPX;
        image.DIBChannels = 4;
        image.DIBColor    = IJL_RGBA_FPX;
        break;

    default:
        // This catches everything else, but no
        // color twist will be performed by the IJL.
        image.DIBColor = (IJL_COLOR)IJL_OTHER;
        image.JPGColor = (IJL_COLOR)IJL_OTHER;
        image.DIBChannels = image.JPGChannels;
        break;
    }


	image.DIBWidth    = image.JPGWidth;
    image.DIBHeight   = image.JPGHeight;
    image.DIBPadBytes = IJL_DIB_PAD_BYTES(image.DIBWidth,image.DIBChannels);

    int imageSize = (image.DIBWidth * image.DIBChannels + image.DIBPadBytes) *
        image.DIBHeight;

    imageData = (BYTE*)malloc(imageSize);
    if( imageData == NULL )
    {
        TRACE( "Cannot allocate memory for image\n" );
        return false;
    }
    image.DIBBytes = imageData;

    if( ijlRead( &image, /*IJL_JFILE_READWHOLEIMAGE*/IJL_JBUFF_READWHOLEIMAGE ) != IJL_OK )
    {
        TRACE( "Cannot read image data from %s file\n", image.JPGFile );
        free(imageData);
		image.DIBBytes = NULL;
        return false;
    }
    if( ijlFree( &image ) != IJL_OK )
    {
        TRACE( "Cannot free Intel(R) JPEG library" );
		free(imageData);
		image.DIBBytes = NULL;
		return false;
    }

	//888 to 565...
	image.DIBBytes = (BYTE*)malloc(imageSize * 2 / 3);
	WORD* pDes = (WORD*)image.DIBBytes;
	BYTE* pSrc = imageData;
	for (int i=0;i<imageSize/3; i++)
		*pDes++=COLOR_B5G6R5(*pSrc++,*pSrc++,*pSrc++);
    //if(image.DIBColor == IJL_RGBA_FPX)
    //{
    //    RGBA_FPX_to_BGRA(imageData,image.DIBWidth,image.DIBHeight);
    //}
	free(imageData);
	return true;
}
