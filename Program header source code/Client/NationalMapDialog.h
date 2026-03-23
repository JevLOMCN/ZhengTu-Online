#pragma once
#include "..\gui\include\guidialog.h"
#include "../xml_parse/XmlParseEx.h"

#include <vector>

#define pszImagePack "data\\interfaces.gl"
#define scGroupID 6

class CDistrict;
class CProvince;
class CMainlandMap;
class CWorldMap;

class CDistrict //区域
{
public:
	CDistrict()
	{
		memset(name,0,MAX_NAMESIZE);
		iTask = -1;
		iBtnImage = -1;
		bIsDo = false;
	}
	CDistrict(CProvince* parent)
		:m_pParent(parent)
	{
		memset(name,0,MAX_NAMESIZE);
		iTask = -1;
		iBtnImage = -1;
		bIsDo = false;
	}
	~CDistrict()
	{

	}
	bool Init(TiXmlElement *district)
	{
		if(!district) return false;

		strcpy(name,district->Attribute("name"));
		if( district->QueryIntAttribute("taskid",&iTask) != TIXML_SUCCESS )
		{
			iTask = -1;
		}
		if( district->QueryIntAttribute("image",&iBtnImage) != TIXML_SUCCESS )
		{
			iBtnImage = -1;
		}
		int ido = 0;
		if( district->QueryIntAttribute("do",&ido) != TIXML_SUCCESS )
		{
			ido = 0;
		}
		bIsDo = ( ido != 0 );
		return true;
	}
	stResourceLocation GetButtonImage() const;
	int GetButtonFrameNum() const { return 1; }
	bool bIsTaskDo() const { return bIsDo; }
	void TaskFinished() { bIsDo = true; }
	int GetTaskID() const { return iTask; }
	const char* GetName() { return name; }
	CProvince*	GetParentMap() { return m_pParent; }
private:
	char	name[MAX_NAMESIZE];	//区域名字
	int		iTask;				//任务id
	int		iBtnImage;			//按钮图片id
	bool	bIsDo;				//是否完成任务	
	CProvince*	m_pParent;		//所在州
};


class CProvince//地区
{
public:
	typedef std::vector<CDistrict *> tListDistrict;
	typedef tListDistrict::iterator DistrictIt;

	CProvince()
	{
		memset(name,0,MAX_NAMESIZE);
		memset(defaultDistrict,0,MAX_NAMESIZE);
		iBtnImage = -1;
		iBorderImage = -1;
		iBtnFrameNum = 3;
		iCoverImage = -1;
		bDo = false;
	}


	CProvince(CMainlandMap* parent)
		:m_pParent(parent)
	{
		memset(name,0,MAX_NAMESIZE);
		memset(defaultDistrict,0,MAX_NAMESIZE);
		iBtnImage = -1;
		iBorderImage = -1;
		iBtnImage = -1;
		iCoverImage = -1;
		iBtnFrameNum = 3;
		bDo = false;
	}

	~CProvince()
	{
		Release();
	}

	bool Init(TiXmlElement* province)
	{
		Release();
		if(!province) return false;
		strcpy(name,province->Attribute("name"));
		strcpy(defaultDistrict,province->Attribute("default"));
		if( province->QueryIntAttribute("background",&iBtnImage) != TIXML_SUCCESS )
		{
			return false;
		}
		int ido = 0;
		if( province->QueryIntAttribute("do",&ido) != TIXML_SUCCESS )
		{
			ido = 0;
		}
		if( province->QueryIntAttribute("background",&iBackImage) != TIXML_SUCCESS )
		{
			iBackImage = -1;
		}

		if( province->QueryIntAttribute("border",&iBorderImage) != TIXML_SUCCESS )
		{
			iBorderImage = -1;
		}
		if( province->QueryIntAttribute("image",&iBtnImage) != TIXML_SUCCESS )
		{
			iBtnImage = -1;
		}
		if( province->QueryIntAttribute("cover",&iCoverImage) != TIXML_SUCCESS )
		{
			iCoverImage = -1;
		}

		if( province->QueryIntAttribute("framenum",&iBtnFrameNum) != TIXML_SUCCESS )
		{
			iBtnFrameNum = -1;
		}
		if( province->QueryIntAttribute("taskid",&iTask) != TIXML_SUCCESS )
		{
			iTask = -1;
		}	

		bDo = (ido != 0);
		TiXmlElement* district = province->FirstChildElement("district");
		while(district)
		{
			CDistrict *pdis = new CDistrict(this);
			if(pdis && pdis->Init(district) ) 
			{
				listDistrict.push_back(pdis);
			}
			district = district->NextSiblingElement("district");
		}
		return true;
	}
	void Release()
	{
		for(DistrictIt it = listDistrict.begin(); it != listDistrict.end() ;++it)
		{
			delete (CDistrict *)(*it);
		}
		listDistrict.clear();
	}

	DistrictIt GetDistrictItByTaskID(DWORD dwTask)
	{
		for(DistrictIt it = listDistrict.begin(); it != listDistrict.end() ;++it)
		{
			if( (*it)->GetTaskID() == dwTask )
			{
				return it;
			}
		}
		return listDistrict.end();
	}

	CDistrict* GetDistrictByTaskID(DWORD dwTask) 
	{
		DistrictIt it = GetDistrictItByTaskID(dwTask);
		if( it != listDistrict.end() ) return (CDistrict*)(*it);
		return NULL;
	}

	bool bIsDo() const { return bDo; }
	void TaskFinished(DWORD taskid) { bDo = true; }

	tListDistrict* GetDistrictList() { return &listDistrict; }
	const char* GetName() { return name; }

	CDistrict* GetProvinceByName(const char *pszName) 
	{
		for(DistrictIt it = listDistrict.begin(); it != listDistrict.end() ;++it)
		{
			if(0 == strcmp((*it)->GetName(),pszName)) return (*it);
		}
		return NULL;
	}
	stResourceLocation GetBackGroundImage() const;
	stResourceLocation GetBorderImage() const;
	stResourceLocation GetButtonImage() const;
	stResourceLocation GetCoverImage() const;
	int GetButtonFrameNum() const { return iBtnFrameNum; }
	const char* GetDefaultDistrict() const { return defaultDistrict; }
	CMainlandMap*	GetParentMap() { return m_pParent; }
	int			GetTaskID() { return iTask; } 
private:
	char			defaultDistrict[MAX_NAMESIZE];//缺省地区
	char			name[MAX_NAMESIZE];	//州名
	tListDistrict	listDistrict;		//区域列表
	int				iBackImage;			//背景地图
	int				iBorderImage;		//边框图片
	int				iBtnImage;			//按钮图片
	int				iCoverImage;		//遮盖图片
	int				iBtnFrameNum;		//按钮帧数
	bool			bDo;				//是否完成(只在combox显示完成的)
	int				iTask;
	CMainlandMap*	m_pParent;			//所在国家
};


class CMainlandMap //国家
{
public:
	typedef std::vector<CProvince *> tListProvince;
	typedef tListProvince::iterator ProvinceIt;

	CMainlandMap()
	{
		memset(name,0,MAX_NAMESIZE);
		memset(defaultProvince,0,MAX_NAMESIZE);
		iBtnImage = -1;
		iBorderImage = -1;
		bDo = false;
		iBtnFrameNum = 3;
		m_pParent = NULL;
	}

	CMainlandMap(CWorldMap* parent)
		:m_pParent(parent)
	{
		memset(name,0,MAX_NAMESIZE);
		memset(defaultProvince,0,MAX_NAMESIZE);
		iBtnImage = -1;
		iBorderImage = -1;
		iBtnFrameNum = 3;
		bDo = false;
	}
	~CMainlandMap()
	{
		Release();
	}

	bool Init(TiXmlElement* Mainland)
	{
		Release();
		if(!Mainland) return false;

		strcpy(name,Mainland->Attribute("name"));
		strcpy(defaultProvince,Mainland->Attribute("default"));		
		int ido = 0;
		if( Mainland->QueryIntAttribute("do",&ido) != TIXML_SUCCESS )
		{
			return false;
		}
		if( Mainland->QueryIntAttribute("background",&iBackImage) != TIXML_SUCCESS )
		{
			iBackImage = -1;
		}

		if( Mainland->QueryIntAttribute("border",&iBorderImage) != TIXML_SUCCESS )
		{
			iBorderImage = -1;
		}
		if( Mainland->QueryIntAttribute("image",&iBtnImage) != TIXML_SUCCESS )
		{
			iBtnImage = -1;
		}
		if( Mainland->QueryIntAttribute("framenum",&iBtnFrameNum) != TIXML_SUCCESS )
		{
			iBtnFrameNum = -1;
		}
		if( Mainland->QueryIntAttribute("taskid",&iTask) != TIXML_SUCCESS )
		{
			iTask = -1;
		}		

		bDo = (ido != 0);
		TiXmlElement* province = Mainland->FirstChildElement("province");
		while(province)
		{
			CProvince *ppro = new CProvince(this);
			if(ppro && ppro->Init(province) ) 
			{
				listProvince.push_back(ppro);
			}
			province = province->NextSiblingElement("province");
		}
		return true;
	}

	void Release()
	{
		for(ProvinceIt it = listProvince.begin(); it != listProvince.end() ;++it)
		{
			delete (CProvince *)(*it);
		}
		listProvince.clear();
	}

	stResourceLocation GetBackGroundImage() const;
	stResourceLocation GetBorderImage() const;
	stResourceLocation GetButtonImage() const;
	int GetButtonFrameNum() const { return iBtnFrameNum; }

	tListProvince* GetProvinceList() { return &listProvince; }

	const char* GetName() { return name; }

	CProvince* GetProvinceByName(const char *pszName) 
	{
		for(ProvinceIt it = listProvince.begin(); it != listProvince.end() ;++it)
		{
			if(0 == strcmp((*it)->GetName(),pszName)) return (*it);
		}
		return NULL;
	}
	const char* GetDefaultProvince() const { return defaultProvince; }

	CWorldMap*	GetParentMap() { return m_pParent; }
	bool		bIsDo() const { return bDo; }
	int			GetTaskID() { return iTask; } 
private:
	char			defaultProvince[MAX_NAMESIZE];//缺省州
	char			name[MAX_NAMESIZE];	//国家名字
	tListProvince	listProvince;		//州列表
	int				iBackImage;			//背景地图
	int				iBorderImage;		//边框图片
	int				iBtnImage;			//按钮图片
	int				iBtnFrameNum;		//按钮帧数
	bool			bDo;				//是否完成(只在combox显示完成的)
	int				iTask;				//任务id
	CWorldMap*		m_pParent;			//所在世界
};

class CWorldMap //世界地图
{
public:
	typedef std::vector<CMainlandMap *> tListMainland;
	typedef tListMainland::iterator MainlandIt;

	CWorldMap()
	{
		strcpy(name,"世界地图");		
		iBtnImage = -1;
		iBorderImage = -1;
		memset(name,0,MAX_NAMESIZE);
		memset(defaultMainland,0,MAX_NAMESIZE);
	}
	~CWorldMap()
	{
		Release();
	}

	bool Init()
	{
		Release();

		Stream* pStream = OpenPackFileForRead("data\\datas.pak","datas\\worldmapinfo.xml");
		if(!pStream)
		{
			Engine_WriteLog("装载 worldmapinfo.xml 失败\n");
			return false;
		}
		TiXmlDocument doc;
		LoadXMLFromStream(pStream,doc);
		//if(!LoadXMLFromStream(pStream,doc))
		//{
		//	ClosePackFileForRead(pStream);
		//	Engine_WriteLog("解析 worldmapinfo.xml 失败\n");
		//	return false;
		//}

		TiXmlElement* world = doc.FirstChildElement("world");
		if(!world)
		{
			ClosePackFileForRead(pStream);
			Engine_WriteLog("解析 worldmapinfo.xml 失败\n");
			return false;
		}
		strcpy(defaultMainland,world->Attribute("default"));
		if( world->QueryIntAttribute("background",&iBtnImage) != TIXML_SUCCESS )
		{
			iBtnImage = -1;
		}

		if( world->QueryIntAttribute("border",&iBorderImage) != TIXML_SUCCESS )
		{
			iBorderImage = -1;
		}

		TiXmlElement* Mainland = world->FirstChildElement("mainland");

		while(Mainland)
		{
			CMainlandMap *p = new CMainlandMap(this);
			if(p && p->Init(Mainland) ) 
			{
				listMainland.push_back(p);
			}
			Mainland = Mainland->NextSiblingElement("mainland");
		}
		ClosePackFileForRead(pStream);
		return true;
	}

	void Release()
	{
		for(MainlandIt it = listMainland.begin(); it != listMainland.end() ;++it)
		{
			delete (CMainlandMap *)(*it);
		}
		listMainland.clear();
	}
	stResourceLocation GetBackGroundImage() const;
	stResourceLocation GetBorderImage() const;
	int GetButtonFrameNum() const { return 1; }

	tListMainland* GetMainlandList() { return &listMainland; }

	CMainlandMap* GetMainlandByName(const char *pszName) 
	{
		for(MainlandIt it = listMainland.begin(); it != listMainland.end() ;++it)
		{
			if(0 == strcmp((*it)->GetName(),pszName)) return (*it);
		}
		return NULL;
	}
	const char* GetDefaultMainland() const { return defaultMainland; }
private: 
	char			defaultMainland[MAX_NAMESIZE];//缺省国家
	char			name[MAX_NAMESIZE];	//地图名字
	tListMainland	listMainland;		//国家列表
	int				iBtnImage;				//大地图
	int				iBorderImage;		//边框图片
};


class CNationalMapDialog :
	public CGuiDialog
{
public:
	enum STATE
	{
		enum_show_world,		//显示世界地图
		enum_show_mainland,		//显示国家地图
		enum_show_province		//显示州地图
	};
	CNationalMapDialog(void);
	~CNationalMapDialog(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	void OnSetCloseLocation();
	HRESULT OnRender(float fElapsedTime);
	void OnResetScreenSize();

	void UpdateContents(STATE state);

	void UpdateProvinceMap();
private:
	void RemoveBitmapCtrl();

	void ShowWorldMap();
	void ShowMainlandMap();
	void ShowProvinceMap();

	bool OnBtnClick(UINT id);
	bool OnClickMapItem(UINT id);
	bool OnRButuonClick();
	bool OnComboBoxChange();

	inline void SetBackgroundImage(stResourceLocation dst);
	void AddMapImage(UINT id,stResourceLocation dst);
	void AddMapButton(UINT id,stResourceLocation dst,const char* text,int framenum = 3);

	bool OnClickSmallBtn();

	void InitRect();
private:
	CWorldMap m_worldMap;
	typedef std::vector<CGuiControl *> tListControl;
	typedef tListControl::iterator tListControlIt;
	tListControl m_listCtrl;

	CMainlandMap*	m_pCurMainland;	//当前国家
	CProvince*		m_pCurProvince;	//当前州
	STATE			m_curState;		//当前显示状态
};
