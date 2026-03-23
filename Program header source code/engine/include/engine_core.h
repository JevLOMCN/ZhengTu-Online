/**
 * \file      engine_core.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#ifndef _ENGINE_CORE_H
#define _ENGINE_CORE_H

#include <string>
#include "stringex.h"
#include "Shlwapi.h"

struct stNumPercent{
	int num;
	int percent;
};

/**
 * \brief  按照出现机率（百分比）获取随机值
 *
 *	按照出现机率（百分比）获取随机值
 *
 * \param aNum : 随机值和百分比
 * \param count : 个数
 * \param ret : 返回的随机值
 *
 * \return  是否获取成功
 *
 */
extern bool GetRandNumByPercent(stNumPercent aNum[],size_t count,int & ret);

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param szPath : 描述
 * \return 返回值的描述
 */
inline void replaceBacklashPath(char *szPath)
{
	while(*szPath)
	{
		if(*szPath == '\\')
			*szPath = '/';
		szPath++;
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param szPath : 描述
 * \return 返回值的描述
 */
inline void replaceFrontlashPath(char *szPath)
{
	while(*szPath)
	{
		if(*szPath == '/')
		{
			*szPath = '\\';
		}
		szPath++;
	}
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param io_num : 描述
 * \return 返回值的描述
 */
inline DWORD GetPow2(DWORD io_num)
{
	Assert(io_num > 0);
	long oneCount   = 0;
	long shiftCount = -1;
	while (io_num) {
		if(io_num & 1)
			oneCount++;
		shiftCount++;
		io_num >>= 1;
	}
	if(oneCount > 1)
		shiftCount++;

	return DWORD(1 << shiftCount);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param in_num : 描述
 * \return 返回值的描述
 */
inline bool IsPow2(const DWORD in_num)
{
	return (in_num == GetPow2(in_num));
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param io_num : 描述
 * \return 返回值的描述
 */
inline DWORD GetBinLog2(DWORD io_num)
{
	AssertFatal(io_num != 0 && IsPow2(io_num),
		"Error, this only works on powers of 2 > 0");

	long shiftCount = 0;
	while (io_num) {
		shiftCount++;
		io_num >>= 1;
	}

	return DWORD(shiftCount - 1);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param io_num : 描述
 * \return 返回值的描述
 */
inline DWORD GetRangeBits(DWORD io_num)
{
	Assert(io_num > 0);

	long oneCount   = 0;
	long shiftCount = -1;
	while (io_num) {
		if(io_num & 1)
			oneCount++;
		shiftCount++;
		io_num >>= 1;
	}
	if(oneCount > 1)
		shiftCount++;

	return DWORD(shiftCount);
}

enum TextureUsageType
{
	Usage_Terrain,
	Usage_BlendTerrain,
	Usage_TerrainSmall,
	Usage_Building,
	Usage_Magic,
	Usage_Npc,
	Usage_Character,
	Usage_Interface,
	Usage_SmallObject,
	Usage_Shadow,
	Usage_NoAlpha,
	Usage_Num,
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pszFileName : 描述
 * \return 返回值的描述
 */
inline std::string GetFileDir(const char* pszFileName)
{
	const char* p =strrchr(pszFileName,'\\');
	if(!p) 
	{
		return std::string("");
	}
	else
	{
		int len = (int)(p - pszFileName);
		std::string str;
		str.resize(len);
		memcpy(&str[0],pszFileName,len);
		str[len] = 0;
		return str;
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pszPathName : 描述
 * \return 返回值的描述
 */
inline std::string GetPathFileName(const char* pszPathName)
{
	const char* p1 = strrchr(pszPathName,'\\');
	if(!p1) p1 = pszPathName;
	std::string str(p1);
	return str;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pszDir : 描述
 * \return 返回值的描述
 */
inline bool IsDirExist(const TCHAR* pszDir)
{
	DWORD dwFA = GetFileAttributes(pszDir);
	return ((dwFA != 0xffffffff) && (dwFA & FILE_ATTRIBUTE_DIRECTORY) );
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pszFileName : 描述
 * \return 返回值的描述
 */
inline bool IsFileExist(const TCHAR* pszFileName)
{
	DWORD dwFA = GetFileAttributes(pszFileName);
	return ( (dwFA != INVALID_FILE_ATTRIBUTES) && (dwFA & FILE_ATTRIBUTE_DIRECTORY)== 0 );
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pszFileName : 描述
 * \return 返回值的描述
 */
inline std::string GetFileNameExt(const char* pszFileName)
{
	const char* p = strrchr(pszFileName,'.');
	if(p && (*p)) return (p+1);
	return "";
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pszFileName : 描述
 * \return 返回值的描述
 */
inline std::string GetFileNameTitle(const char* pszFileName)
{
	const char* p2 = strrchr(pszFileName,'.');
	const char* p1 = strrchr(pszFileName,'\\');
	if(!p1) p1 = pszFileName;
	else p1++;
	if(!p2) p2 = pszFileName + strlen(pszFileName);
	int len = p2 - p1;
	std::string str;
	str.append(p1,len);
	return str;
}

inline std::string str_Replace(std::string &strText,const char *pszSrcText,const char *pszDestText)
//把字符串strText中的每一个pszSrcText都替换为pszDestText
{
	//注释的代码有一个bug，如果pszDestText = pszSrcText则进入死循环
	//size_t size = strText.find(pszSrcText);
	//if(size > strText.length()) 
	//	return strText;
	//else
	//{	
	//	size_t len = strlen(pszSrcText);
	//	strText.replace(size,len,pszDestText);
	//	return str_Replace(strText,pszSrcText,pszDestText);
	//}
	const char *p = strstr(strText.c_str(),pszSrcText);
	if(!p) return strText;
	size_t size = (size_t )(p - strText.c_str());
	while(size <= strText.length() - strlen(pszSrcText) || !p)
	{
		strText.replace(size,strlen(pszSrcText),pszDestText);
		if(size_t ( p - strText.c_str()) > strText.length() - 2) break;
		p = strstr(p+1,pszSrcText);
		if(!p) break;
		size = (size_t )(p - strText.c_str()) + 1;
	}
	return strText;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param path : 描述
 * \param strName : 描述
 * \return 返回值的描述
 */
inline bool CreateFile_Ex(const TCHAR *path,const TCHAR * strName)
{
	if(!IsDirExist(path))
	{
		CreateDirectory(path,NULL);
	}
	TCHAR buf[MAX_PATH];
	for(int i = 0;i < _tcslen(strName) && i < MAX_PATH;i++)
	{
		memset(buf,0,MAX_PATH);
		_tcscpy(buf,strName);
		if(strName[i] == '\\')
		{
			buf[i+1] = '\0';
			TRACE("%s\n",buf);
			if(!IsDirExist(buf))
			{
				CreateDirectory(buf,NULL);
			}
		}
	}
	return false;
	//while()
}

//秒换算成字符串形式的时间
inline const char* Second2String(int iSecond)
{
	static char szTemp[64] = {0};
	int second = iSecond % 60;
	int minute = iSecond / 60;
	int hour =   minute / 60;
	//int day =    hour / 24;

	if( iSecond <  0 )
	{
		return NULL;
	}
	else if( iSecond < 60 )
	{
		_snprintf(szTemp,63,"%d 秒",iSecond);
		szTemp[63] = 0;
	}
	else if( iSecond < 3600 )
	{		
		_snprintf(szTemp,63,"%d分%d秒",minute%60,second);
		szTemp[63] = 0;
	}
	else //if( iSecond < 60*60*24 )
	{
		_snprintf(szTemp,63,"%d小时%d分%d秒",hour,minute%60,second);
		szTemp[63] = 0;
	}
	//else
	//{
	//	_snprintf(szTemp,63,"%d天%d小时%d分%d秒",day,hour%24,minute%60,second);
	//	szTemp[63] = 0;
	//}
	return szTemp;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param path : 描述
 * \param strName : 描述
 * \return 返回值的描述
 */
inline bool CreateFile_Ex(const char *path,std::string strName)
{
	//unsigned char newName[MAX_PATH];
	//memcpy(newName,pszName,strlen(pszName));

	//char buf[MAX_PATH];

	//int index = 0;
	//for(int i = 0;i<strlen(pszName);i++)
	//{
	//	memset(buf,0,MAX_PATH);
	//	strcpy(buf,pszName);
	//	if(newName[i] == '\\')
	//	{
	//		index++;
	//		if(index > 1)
	//		{
	//			buf[i+1] = '\0';
	//			TRACE("%s\n",buf);
	//			if(::IsFileExist(buf))
	//			{
	//				CreateDirectory(buf,NULL);
	//			}
	//		}
	//	}
	//}	
	//FILE *pf = fopen(pszName,"w");
	//if(!pf) return false;
	//fclose(pf);
	return true;
}

struct stResourceLocation;
class CToolTips;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class IEngineCallBack
{
public:
	//virtual HRESULT InvalidateDeviceObjects() = 0;	//before call reset alt+tab 
	//virtual HRESULT RestoreDeviceObjects() = 0;		//after alt+tab
	virtual void OnDisplayModeChange() = 0;
	virtual void PlayUISound(DWORD dwSoundID) = 0;
	virtual std::string GetTaskVariable(DWORD taskID,const char* taskVarName) = 0;
	/**
	* \brief 获取物品对应的图片位置
	* 
	* 获取物品对应的图片位置
	* 
	* \param id : 物品ID
	* \param rl : 返回的图片位置
	* \return 成功返回TRUE，失败返回FALSE
	*/
	virtual bool GetObjectImageLocation(int id,stResourceLocation * rl) = 0;

	/**
	* \brief 获取物品对应的描述信息
	* 
	* 获取物品对应的描述信息
	* 
	* \param id : 物品ID
	* \param toolTips : 返回的描述信息
	* \return 无
	*/
	//soke 应该是物品标记
	virtual void SetObjectTooltips(int id,CToolTips& toolTips) = 0;
};

void Engine_SetCallBack(IEngineCallBack* p);
IEngineCallBack* Engine_GetCallBack();

struct stPointF{
	float x; 
	float y;
	stPointF(){}
	stPointF(const char* szPoint)
	{
		char* p;

		x = (float)strtod(szPoint,&p);

		if(p && *p)
		{
			++p;
			y = (float)strtod(p,NULL);
		}
		else y = 0;

	}
	stPointF& operator = (const POINT& pt)
	{
		x = (float)pt.x;
		y = (float)pt.y;
		return (*this);
	}
	stPointF(float fX,float fY)
	{
		x = fX;
		y = fY;
	}
	stPointF(const POINT & pt)
	{
		x = (float)pt.x;
		y = (float)pt.y;
	}
	stPointF(const stPointF & pt)
	{
		x = pt.x;
		y = pt.y;
	}
	stPointF& operator = (const stPointF & pt)
	{
		x = pt.x;
		y = pt.y;
		return (*this);
	}
	stPointF& operator += (const stPointF & pt)
	{
		x += pt.x;
		y += pt.y;
		return (*this);
	}
	stPointF operator + (const stPointF & pt)
	{
		stPointF pt1;
		pt1.x = x + pt.x;
		pt1.y = y + pt.y;
		return pt1;
	}

	stPointF& operator -= (const stPointF & pt)
	{
		x -= pt.x;
		y -= pt.y;
		return (*this);
	}
	stPointF operator - (const stPointF & pt)
	{
		stPointF pt1;
		pt1.x = x - pt.x;
		pt1.y = y - pt.y;
		return pt1;
	}
	operator POINT * ()
	{
		return (POINT*)this;
	}

	stPointF operator * (float value) const
	{
		stPointF pt1;
		pt1.x = x * value;
		pt1.y = y * value;
		return pt1;
	}

	stPointF& operator *= (float value)
	{
		x *= value;
		y *= value;
		return (*this);
	}
};

struct stPoint3F{
	float x; 
	float y;
	float z;
	stPoint3F(){}
	stPoint3F(const char* szPoint)
	{
		char* p;
		
		x = (float)strtod(szPoint,&p);

		if(p && *p)
		{
			++p;
			y = (float)strtod(p,&p);
		}
		else y = 0;

		if(p && *p)
		{
			++p;
			z = (float)strtod(p,NULL);
		}
		else z = 0;

	}
	stPoint3F& operator = (const POINT& pt)
	{
		x = (float)pt.x;
		y = (float)pt.y;
		z = 0.0f;
		return (*this);
	}

	stPoint3F(float fX,float fY,float fZ)
	{
		x = fX;
		y = fY;
		z = fZ;
	}

	stPoint3F(const POINT & pt)
	{
		x = (float)pt.x;
		y = (float)pt.y;
		z = 0;
	}

	stPoint3F(const stPoint3F & pt)
	{
		x = pt.x;
		y = pt.y;
		z = pt.z;
	}

	stPoint3F& operator = (const stPoint3F & pt)
	{
		x = pt.x;
		y = pt.y;
		z = pt.z;
		return (*this);
	}

	stPoint3F& operator += (const stPoint3F & pt)
	{
		x += pt.x;
		y += pt.y;
		z += pt.z;
		return (*this);
	}
	stPoint3F operator + (const stPoint3F & pt)
	{
		stPoint3F pt1;
		pt1.x = x + pt.x;
		pt1.y = y + pt.y;
		pt1.z = z + pt.z;
		return pt1;
	}

	stPoint3F& operator -= (const stPoint3F & pt)
	{
		x -= pt.x;
		y -= pt.y;
		z -= pt.z;
		return (*this);
	}

	stPoint3F operator - (const stPoint3F & pt)
	{
		stPoint3F pt1;
		pt1.x = x - pt.x;
		pt1.y = y - pt.y;
		pt1.z = z - pt.z;
		return pt1;
	}

	stPoint3F operator * (float value) const
	{
		stPoint3F pt1;
		pt1.x = x * value;
		pt1.y = y * value;
		pt1.z = z * value;
		return pt1;
	}

	stPoint3F& operator *= (float value)
	{
		x *= value;
		y *= value;
		z *= value;
		return (*this);
	}
};
struct stPointI : public tagPOINT{
	stPointI(){}
	
	stPointI(const char* szPoint)
	{
		char* p;
		x = strtol(szPoint,&p,10);
		if(p && *p)
		{
			++p;
			y = strtol(p,NULL,10);
		}
		else y = 0;
	}

	stPointI(long fX,long fY)
	{
		x = fX;
		y = fY;
	}
	stPointI(const POINT & pt)
	{
		x = (long)pt.x;
		y = (long)pt.y;
	}
	POINT& operator = (const POINT & pt)
	{
		x = pt.x;
		y = pt.y;
		return (*this);
	}

	POINT operator + (const POINT & pt) const
	{
		stPointI pt1;
		pt1.x = x + pt.x;
		pt1.y = y + pt.y;
		return pt1;
	}

	POINT operator - (const POINT & pt) const
	{
		stPointI pt1;
		pt1.x = x - pt.x;
		pt1.y = y -+ pt.y;
		return pt1;
	}

	POINT operator * (float value) const
	{
		stPointI pt1;
		pt1.x = (long)(x * value);
		pt1.y = (long)(y * value);
		return pt1;
	}

	POINT& operator -= (const POINT & pt)
	{
		x -= pt.x;
		y -= pt.y;
		return (*this);
	}

	POINT& operator += (const POINT & pt)
	{
		x += pt.x;
		y += pt.y;
		return (*this);
	}

	POINT& operator *= (float value)
	{
		x = (long)(x * value);
		y = (long)(y * value);
		return (*this);
	}
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt1 : 描述
 * \param pt2 : 描述
 * \return 返回值的描述
 */
inline POINT operator - (const POINT & pt1,const POINT& pt2)
{
	POINT pt={pt1.x-pt2.x,pt1.y-pt2.y};
	return pt;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt1 : 描述
 * \param pt2 : 描述
 * \return 返回值的描述
 */
inline POINT operator + (const POINT & pt1,const POINT& pt2)
{
	POINT pt={pt1.x+pt2.x,pt1.y+pt2.y};
	return pt;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt1 : 描述
 * \param pt2 : 描述
 * \return 返回值的描述
 */
inline POINT operator -= (POINT & pt1,const POINT& pt2)
{
	pt1.x -= pt2.x;
	pt1.y -= pt2.y;
	return pt1;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt1 : 描述
 * \param pt2 : 描述
 * \return 返回值的描述
 */
inline POINT operator += (POINT & pt1,const POINT& pt2)
{
	pt1.x += pt2.x;
	pt1.y += pt2.y;
	return pt1;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt1 : 描述
 * \param pt2 : 描述
 * \return 返回值的描述
 */
inline bool operator == (const POINT & pt1,const POINT& pt2)
{
	return (pt1.x == pt2.x && pt1.y == pt2.y);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt1 : 描述
 * \param pt2 : 描述
 * \return 返回值的描述
 */
inline bool operator != (const POINT & pt1,const POINT& pt2)
{
	return !(pt1.x == pt2.x && pt1.y == pt2.y);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param rc1 : 描述
 * \param rc2 : 描述
 * \return 返回值的描述
 */
inline bool operator != (const RECT & rc1,const RECT& rc2)
{
	return (rc1.left != rc2.left  ||
		rc1.top != rc2.top  ||
		rc1.right != rc2.right  ||
		rc1.bottom != rc2.bottom);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param rc1 : 描述
 * \param rc2 : 描述
 * \return 返回值的描述
 */
inline bool operator == (const RECT & rc1,const RECT& rc2)
{
	return (rc1.left == rc2.left  &&
		rc1.top == rc2.top  &&
		rc1.right == rc2.right  &&
		rc1.bottom == rc2.bottom);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param rc1 : 描述
 * \param rc2 : 描述
 * \return 返回值的描述
 */
inline RECT operator & (const RECT &rc1,const RECT & rc2)
{
	RECT rc3;
	IntersectRect(&rc3,&rc1,&rc2);
	return rc3;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param rc1 : 描述
 * \param rc2 : 描述
 * \return 返回值的描述
 */
inline const RECT& operator &= ( RECT &rc1,const RECT & rc2)
{
	rc1 = (rc1 & rc2);
	return rc1;
}

struct stRectF{
	float left;
	float top;
	float right;
	float bottom;
	stRectF(){}
	stRectF(float l,float t,float r,float b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}
	stRectF(const RECT& rc)
	{
		left = (float)rc.left;
		top = (float)rc.top;
		right = (float)rc.right;
		bottom = (float)rc.bottom;
	}

	void SetRect(float l,float t ,float r,float b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}
	void SetRectEmpty()
	{
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	}
	bool IsRectEmpty()
	{
		return (left >= right || top >= bottom);
	}
	bool operator == (const stRectF & rc)
	{
		return (left == rc.left && top == rc.top && right == rc.right && bottom == rc.bottom);
	}
	
	bool PtInRect(POINT  pt)const
	{
		return (pt.x >= (long)left && pt.x < (long)right && pt.y >= (long)top && pt.y < (long)bottom);
	}

	bool IsIntersect(const RECT & rc)const
	{
		return PtInRect(stPointI(rc.left,rc.top)) || PtInRect(stPointI(rc.right,rc.top)) || PtInRect(stPointI(rc.left,rc.bottom)) || PtInRect(stPointI(rc.right,rc.bottom));
	}

	stRectF & operator = (const RECT & rc)
	{
		left = (float)rc.left;
		top = (float)rc.top;
		right = (float)rc.right;
		bottom = (float)rc.bottom;
		return (*this);
	}
	stRectF operator&(const stRectF& rect2) const
	{
		stRectF rect;
		rect.left = (left > rect2.left ? left : rect2.left);
		rect.top = (top > rect2.top ? top : rect2.top);
		rect.right = (right < rect2.right ? right : rect2.right);
		rect.bottom = (bottom < rect2.bottom ? bottom : rect2.bottom);
		return rect;
	}
	
	stRectF & operator &= (const RECT& rect2) 
	{
		left = (left > rect2.left ? left : rect2.left);
		top = (top > rect2.top ? top : rect2.top);
		right = (right < rect2.right ? right : rect2.right);
		bottom = (bottom < rect2.bottom ? bottom : rect2.bottom);
		return (*this);
	}

	stRectF & operator &= (const stRectF& rect2) 
	{
		left = (left > rect2.left ? left : rect2.left);
		top = (top > rect2.top ? top : rect2.top);
		right = (right < rect2.right ? right : rect2.right);
		bottom = (bottom < rect2.bottom ? bottom : rect2.bottom);
		return (*this);
	}
	
	void OffsetRect(stPointF pt)
	{
		left += pt.x;
		right += pt.x;
		top += pt.y;
		bottom += pt.y;
	}

	void OffsetRect(float x,float y)
	{
		left += x;
		right += x;
		top += y;
		bottom += y;
	}
	float Width() const
	{
		return right - left;
	}
	float Height() const
	{
		return bottom - top;
	}

	void InflateRect(float x,float y)
	{
		left -= x;
		top -= y;
		right += x;
		bottom += y;
	}
};

struct stRectI : public tagRECT
{
	void ToString(char* pszBuffer,size_t size)
	{
		_snprintf(pszBuffer,size,"%d,%d,%d,%d",left,top,right-left,bottom-top);
		if(size) pszBuffer[size-1] = 0;
	}

	void FromString(const char* pszRect)
	{
		char* p;
		left = str_getnextl(pszRect,&p,10);
		top = str_getnextl(p,&p,10);
		right = str_getnextl(p,&p,10) + left;
		bottom = str_getnextl(p,&p,10) + top;
	}

	stRectI(){}
	
	stRectI(long l,long t,long r,long b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}
	
	stRectI(const RECT& rc)
	{
		left = (long)rc.left;
		top = (long)rc.top;
		right = (long)rc.right;
		bottom = (long)rc.bottom;
	}
	
	void SetRect(long l,long t ,long r,long b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}
	
	bool PtInRect(POINT  pt)const
	{
		return (pt.x >= left && pt.x < right && pt.y >= top && pt.y < bottom);
	}
	
	void SetRectEmpty()
	{
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	}

	bool IsRectEmpty()const
	{
		return (left >= right || top >= bottom);
	}
	
	bool operator == (const RECT & rc)
	{
		return (left == rc.left && top == rc.top && right == rc.right && bottom == rc.bottom);
	}
	
	POINT TopLeft()const
	{
		return *((POINT*)this);
	}
	POINT CenterPoint() const
	{
		return stPointI((right + left)/2,(bottom + top)/2);
	}
	
	POINT BottomRight()const
	{
		return *(((POINT*)this) + 1);
	}

	bool IsIntersect(const RECT & rc)const
	{
		return PtInRect(stPointI(rc.left,rc.top)) || PtInRect(stPointI(rc.right,rc.top)) || PtInRect(stPointI(rc.left,rc.bottom)) || PtInRect(stPointI(rc.right,rc.bottom));
	}

	RECT & operator = (const RECT & rc)
	{
		left = (long)rc.left;
		top = (long)rc.top;
		right = (long)rc.right;
		bottom = (long)rc.bottom;
		return (*this);
	}
	
	RECT operator&(const RECT& rect2) const
	{
		stRectI rect;
		rect.left = (left > rect2.left ? left : rect2.left);
		rect.top = (top > rect2.top ? top : rect2.top);
		rect.right = (right < rect2.right ? right : rect2.right);
		rect.bottom = (bottom < rect2.bottom ? bottom : rect2.bottom);
		return rect;
	}

	RECT & operator &= (const RECT& rect2)
	{
		left = (left > rect2.left ? left : rect2.left);
		top = (top > rect2.top ? top : rect2.top);
		right = (right < rect2.right ? right : rect2.right);
		bottom = (bottom < rect2.bottom ? bottom : rect2.bottom);
		return (*this);
	}

	//*
	RECT operator | (const RECT& rect2) const
	{
		stRectI rect;
		if(this->IsRectEmpty()) return rect2;
		if(((stRectI*)&rect2)->IsRectEmpty()) return *this;
		rect.left = (left < rect2.left ? left : rect2.left);
		rect.top = (top < rect2.top ? top : rect2.top);
		rect.right = (right > rect2.right ? right : rect2.right);
		rect.bottom = (bottom > rect2.bottom ? bottom : rect2.bottom);
		return rect;
	}

	RECT & operator |= (const RECT& rect2)
	{
		if(this->IsRectEmpty())
		{
			*this = rect2;
			return (*this);
		}

		if(((stRectI*)&rect2)->IsRectEmpty()) return (*this);

		left = (left < rect2.left ? left : rect2.left);
		top = (top < rect2.top ? top : rect2.top);
		right = (right > rect2.right ? right : rect2.right);
		bottom = (bottom > rect2.bottom ? bottom : rect2.bottom);
		return (*this);
	}
	//*/

	void OffsetRect(POINT pt)
	{
		left += pt.x;
		right += pt.x;
		top += pt.y;
		bottom += pt.y;
	}

	void OffsetRect(long x,long y)
	{
		left += x;
		right += x;
		top += y;
		bottom += y;
	}
	
	void InflateRect(long x,long y)
	{
		left -= x;
		top -= y;
		right += x;
		bottom += y;
	}

	long Width() const
	{
		return right - left;
	}
	
	long Height() const
	{
		return bottom - top;
	}
};
#endif