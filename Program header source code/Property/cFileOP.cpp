#ifdef _GAME_TOOL
#include <afxdao.h>
#endif
#include "../engine/include/engine.h"
#include "cfileop.h"
#include <fstream>
#include <map>

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param e : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
using namespace std;
#ifdef _GAME_TOOL
const char* GetErrorMessage(CDaoException* e)
{
	static char result[2048];
	e->GetErrorMessage(result,2048);
	return result;
}
const char* GetFieldText(VARIANT & v)
{
	static char result[2048];

	result[0] = 0;
	switch(v.vt)
	{
	case VT_NULL:
	case VT_EMPTY:
		break;
	case VT_I8:
		sprintf(result,"%I64d",v.llVal);
		break;
	case VT_I4:
		sprintf(result,"%d",v.lVal);
		break;
	case VT_UI1:
		sprintf(result,"%u",(DWORD)v.bVal);
		break;
	case VT_I2:
		sprintf(result,"%hd",v.iVal);
		break;
	case VT_R4:
		sprintf(result,"%g",v.fltVal);
		break;
	case VT_R8:
		sprintf(result,"%g",v.dblVal);
		break;
	case VT_BOOL:
		sprintf(result,"%u",v.boolVal);
		break;
	/*case VT_BSTR:
		{
			CString str(v);
			sprintf(result,"%s",str);
		}
		break;*/
	case VT_I1:
		sprintf(result,"%d",(int)v.cVal);
		break;
	case VT_UI2:
		sprintf(result,"%u",(DWORD)v.uiVal);
		break;
	case VT_UI4:
		sprintf(result,"%u",v.ulVal);
		break;
	case VT_UI8:
		sprintf(result,"%I64u",v.ullVal);
		break;
	case VT_INT:
		sprintf(result,"%d",v.intVal);
		break;
	case VT_UINT:
		sprintf(result,"%u",v.uintVal);
		break;
	case VT_BSTR:
	case  VT_BYREF|VT_I1:
		strcpy(result,v.pcVal);
		break;
	}
	return result;
}

#endif

const int c_nMaxPath = 256;

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param szDes : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
inline bool AppBuildPath(LPCSTR szDes)
{
	return true;

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param szDes : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
}
static const char* AppGetFullPath(LPCTSTR szDes)
{
	static char sDes[MAX_PATH];
	strcpy(sDes,szDes);
	return sDes;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param sz1 : √Ë ˆ
 * \param sz2 : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool IsTimeLate(LPCSTR sz1, LPCSTR sz2)
{
	WIN32_FIND_DATA fd1,fd2;
	memset(&fd1,0,sizeof(fd1));
	memset(&fd2,0,sizeof(fd2));

	HANDLE hFd1,hFd2;
	
	hFd1 = FindFirstFile(sz1,&fd1);
	hFd2 = FindFirstFile(sz2,&fd2);

	if(hFd1 != INVALID_HANDLE_VALUE && hFd2 != INVALID_HANDLE_VALUE)
	{
		FindClose(hFd1);
		FindClose(hFd2);
		return CompareFileTime(&fd1.ftLastWriteTime , &fd2.ftLastWriteTime) < 0;
	}
	if(hFd1 != INVALID_HANDLE_VALUE)
		FindClose(hFd1);
	if(hFd2 != INVALID_HANDLE_VALUE)
		FindClose(hFd2);

	return false;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pTable : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
cFileOP::cFileOP(iPropertyTable* pTable)
{
	m_strFileBin[0] =0;
	m_strFileIni[0] =0;
	m_strFilePack[0]=0;
	m_pTable = pTable;
	m_eFile = FILE_NULL;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
cFileOP::~cFileOP(void)
{
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cFileOP::Load()
{
#ifdef _GAME_TOOL
	return LoadFromDatabase();
#else
	return LoadBinFromZip();
#endif
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cFileOP::Save()
{
#ifdef _GAME_TOOL
	bool b = SaveToDatabase();
	if (SaveBin())
	{
		return true;
	}
	return b;
#else
	return false;
#endif

}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param bBuildPath : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
#include "../magic/public.h"
#include "../magic/icharacter.h"
#include "cPropertyTable.h"
bool cFileOP::SaveIni(bool bBuildPath)
{
	if (GetFile() == FILE_NULL)
		return false;
	if (m_strFileIni[0]==0)
		return false;

	if(bBuildPath)
		AppBuildPath(m_strFileIni);

	switch(m_eFile) 
	{
	case FILE_DATA:
		{
			ofstream f(m_strFileIni);
			if (!f.is_open())
				return false;

			string s;
			int n = 16;
			int n2 = 128;
			f.width(n);
			
			const std::vector<stItem>& aItem = GetTable()->GetItemVector();
			for (size_t i=0; i<aItem.size() ;i++)
			{
				if (aItem.at(i).IsArray())
					f.width(n2);
				else
					f.width(n);
				f << aItem[i].strName;
			}

			std::vector<std::string> array;
			array.reserve(aItem.size());
			for (size_t i=0; i<GetTable()->GetPropertyCount();i++)
			{ 
				f << "\n";
				array.resize(0);
				GetTable()->Property2String(GetTable()->GetRawProperty(i),array);

				for (size_t j=0; j<array.size(); j++)
				{
					if (aItem.at(j).IsArray())
						f.width(n2);
					else
						f.width(n);

					f << array[j];
				}
			}
		}
		break;
	case FILE_HELP:
		{
			std::vector<stGroup>& aGroup = GetTable()->GetGroupVector();
			std::vector<stItem>& aItem = GetTable()->GetItemVector();
			size_t i;
			size_t j;
			j = 0;
			FILE* fp = fopen(m_strFileIni,"w");
			if(!fp) return false;
			for(i=0;i<aItem.size();i++)
			{
				stGroup & group = aGroup[j];
				if( i == group.nStart)
				{
					fprintf(fp,"//%s\n",group.strGroupName);
					
					j++;
					if(j >= aGroup.size()) j = aGroup.size()-1;
				}
				stItem & item = aItem[i];
				fprintf(fp,"[%s]\n",item.strName);
				fprintf(fp,"(%s)\n",item.strValue);
				size_t k;
				for(k=0;k<item.aStrHelp.size();k++)
				{
					std::string & str = item.aStrHelp[k];
					const char* pp = str.c_str();
					fprintf(fp,"%s\n",item.aStrHelp[k].c_str());
				}
			}
			fclose(fp);
		}
		break;
	default:
		return false;
	}
	return true;
}
//static void _parse_str(std::vector<std::string> & vRet,const char* pszString,char ch)
//{
//	vRet.resize(0);
//	const char* p1,*p2;
//	p1 = pszString;
//	for(;;)
//	{
//		while(*p1 != 0 && *p1==ch)
//		{
//			p1++;
//		}
//
//		if(*p1 == 0) break;
//
//		p2 = p1;
//
//		while(*p2 != 0 && *p2!=ch)
//		{
//			p2++;
//		}
//
//		char szTmp[1024];
//		memcpy(szTmp,p1,p2-p1);
//		szTmp[p2-p1] = 0;
//
//		vRet.push_back(szTmp);
//		if(*p2 == 0) break;
//
//		p1 = p2+1;
//	}
//}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cFileOP::LoadIni()
{
 	if (GetFile() == FILE_NULL)
		return false;
	if (m_strFileIni[0] == 0)
		return false;

	static const string::size_type npos = -1;
	try
	{
		switch(m_eFile)
		{
		case FILE_HELP:
			{
				ifstream f;
				f.open(m_strFileIni);
				if (!f.is_open())
					return false;

				GetTable()->ClearItem();
				GetTable()->ClearGroup();
				std::string s;
				stItem item;
				while(!f.eof())
				{
					f >> s;
					if (s.empty())
						break;
					if (s.find('/') == 0)
					{
						stGroup group;
						group.nStart = GetTable()->GetItemVector().size();
						if (group.nStart != 0)
							group.nStart++;
						strcpy(group.strGroupName , s.substr(2).c_str());
						GetTable()->AddGroup(group);
						continue;
					}
					string::size_type left;
					string::size_type right;
					left = s.find('[');
					right = s.find(']');
					if (left != npos && right != npos)
					{
						if (item.strName[0])
							GetTable()->AddItem(item);
						item.aStrHelp.clear();
						strcpy(item.strName , s.substr(left+1,right-left-1).c_str());
					}
					else 
					{
						left = s.find('(');
						right = s.find(')');
						if (left != -1 && right != -1)
						{
							string strValue;
							strValue = s.substr(left+1,right-left-1);
							int pos = strValue.find(',');
							if (pos < 0)
							{
								item.CalculateSize();
							}
							else
							{
								strcpy(item.strValue , strValue.substr(0,pos).c_str());
								strValue = strValue.substr(pos+1);
								item.size = atoi(strValue.c_str());
							}
						}
						else
							item.aStrHelp.push_back(s);
					}
				}
				GetTable()->AddItem(item);
			}
			return true;
		case FILE_DATA:
			{
				GetTable()->ClearProperty();

				std::ifstream file(m_strFileIni);
				if(!file)return false;
				
				char strTmp[1024*10];
				std::vector<std::string> cols;
				cols.reserve(256);

				std::vector<std::string> array;
				array.resize(GetTable()->GetItemVector().size());

				std::vector<std::string> aStrValue;
				aStrValue.reserve(256);

				std::map<std::string,std::string> mapFieldValue;

				file.getline(strTmp,sizeof(strTmp));
				_parse_str(cols,strTmp,' ');
				
				Assert(cols.size() <= GetTable()->GetItemVector().size());

				int iFilePos = 0;
				while(!file.eof())
				{
					file.getline(strTmp,sizeof(strTmp));
					aStrValue.resize(0);
					_parse_str(aStrValue,strTmp,' ');

					Assert(aStrValue.size() == cols.size() );

					for(size_t i=0;i<cols.size();i++)
						mapFieldValue[cols[i]] = aStrValue[i];
					
					for (size_t i=0; i<cols.size(); i++)
					{
						std::string strValue;

						stItem& item = GetTable()->GetItemVector()[i];

						std::map<std::string,std::string>::iterator it = mapFieldValue.find(item.strName);

						if(it != mapFieldValue.end())
							strValue = it->second;
						else
							strValue = item.GetDefaultValue();
						array[i] = strValue;
					}
					GetTable()->AddProperty(array);
				}
				file.close();
			}
			return true;
		default:
			return false;
		}
	}
	catch (...) 
	{
		return false;
	}
	

	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param bBuildPath : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cFileOP::SaveBin(bool bBuildPath)
{
	if (GetFile() == FILE_NULL)
		return false;

	if (m_strFileBin[0]==0)
		return false;

	if(bBuildPath)
		AppBuildPath(m_strFileBin);

	FileStream f;
	const char* sPath = AppGetFullPath(m_strFileBin);
	
	if(!f.open(sPath,FileStream::Write))
		return false;
	try
	{
		switch(m_eFile)
		{
		case FILE_DATA:
			{
				size_t n = GetTable()->GetPropertyCount();
				f.write(&n,sizeof(n));
				for (size_t  i=0; i<n; i++)
				{
					if(i == 11)
					{
						int iiiiiiii=0;
					}
					f.write(GetTable()->GetRawProperty(i),GetTable()->GetPropertySize());
				}
			}
			break;
		case FILE_HELP:
			{
				const std::vector<stItem>& aItem = GetTable()->GetItemVector();
				size_t n = aItem.size();
				f.write(&n,sizeof(n));
				for (size_t i=0; i<n; i++)
				{
					const stItem& item = aItem[i];
					f.write(&item.strName,sizeof(item.strName));
					f.write(&item.strValue,sizeof(item.strValue));
					f.write(&item.size,sizeof(item.size));

					size_t  m = item.aStrHelp.size();
					f.write( &m,sizeof(m) );
					for ( size_t  j=0; j<m; j++ )
					{
						char str[64];
						strcpy(str,item.aStrHelp[j].c_str());
						f.write(str,sizeof(str));
					}
				}

				const std::vector<stGroup>& aGroup= GetTable()->GetGroupVector();
				size_t k = aGroup.size();
				f.write(&k,sizeof(k));
				f.write(&aGroup[0],sizeof(stGroup) * aGroup.size());

				for(size_t i=0;i<aGroup.size();++i)
				{
					//TRACE("%d ,%s\n",aGroup[i].nStart,aGroup[i].strGroupName);
				}
			}
			break;
		default:
			return false;
		}
	}	
	catch (...)
	{
		return false;
	}
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param file : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cFileOP::LoadBinFromStream(Stream* file)
{
	bool bLoad = true;
	switch(m_eFile)
	{
	case FILE_HELP:
		{
			GetTable()->ClearItem();
			GetTable()->ClearGroup();
			std::vector<stItem>& aItem = GetTable()->GetItemVector();
			int n;
			file->read(&n,sizeof(n));
			aItem.resize(n);
			for ( int i=0; i<n; i++ )
			{
				stItem & item = aItem[i];
				char str[64];
				file->read(str,sizeof(str));
				strcpy(item.strName , str);
				file->read(str,sizeof(str));
				strcpy(item.strValue , str);
				file->read(&item.size,sizeof(item.size));
				
				int m;
				file->read(&m,sizeof(m));
				item.aStrHelp.resize(m);
				
				for (int j=0; j<m; j++)
				{
					file->read(str,sizeof(str));
					item.aStrHelp[j] = str;
				}
			}

			std::vector<stGroup>& aGroup = GetTable()->GetGroupVector();
			int k;
			file->read(&k,sizeof(k));
			aGroup.resize(k);
			file->read(&aGroup[0],k * sizeof(stGroup));

			for(size_t i=0;i<aGroup.size();++i)
			{
				//TRACE("%d ,%s\n",aGroup[i].nStart,aGroup[i].strGroupName);
			}
		}
		break;
	case FILE_DATA:
		{
			GetTable()->ClearProperty();

			int n;
			file->read(&n,sizeof(n));
			char buffer[10240];
			Assert(GetTable()->GetPropertySize() <= sizeof(buffer));
			for (int i=0; i<n; i++)
			{
				file->read(buffer,GetTable()->GetPropertySize());
				GetTable()->AddProperty(buffer);
			}
		}
		break;
	default:
		bLoad = false;
	}
	return bLoad;
}
 
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cFileOP::LoadBinFromZip()
{
	if (GetFile() == FILE_NULL)
		return false;

	if (m_strFileBin[0]==0)
		return false;

	int i = 0;
	
	Stream* file = OpenPackFileForRead(m_strFilePack,m_strFileBin);
	if(!file) return false;

	bool bLoad = LoadBinFromStream(file);

	ClosePackFileForRead(file);
	return bLoad;
}

#ifdef _GAME_TOOL


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cFileOP::LoadBin()
{
	if (GetFile() == FILE_NULL)
		return false;

	if (m_strFileBin[0]==0)
		return false;

	int i = 0;
	FileStream file0;
	FileStream * file = &file0;

	if(!file->open(m_strFileBin,FileStream::Read))
		return false;

	bool bLoad = LoadBinFromStream(file);

	return bLoad;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cFileOP::LoadFromDatabase()
{
	CDaoDatabase conn;
	CDaoRecordset rs(&conn);
	
	HRESULT hr;
	bool bLoad = true;
	try
	{
		conn.Open(m_strDatabase);
		
		char wszSQL[256];

		switch(m_eFile)
		{
		case FILE_HELP:
			{
				sprintf(wszSQL,"select * from %s order by [group],[index]","Help");
				rs.Open(dbOpenSnapshot,wszSQL,dbReadOnly);

				CDaoRecordset rsGroup(&conn);

				GetTable()->ClearItem();
				GetTable()->ClearGroup();
				
				std::vector<stItem>& aItem = GetTable()->GetItemVector();
				std::vector<stGroup>& aGroup = GetTable()->GetGroupVector();
				aItem.resize(rs.GetRecordCount());
				
				stGroup prevGroup;
				prevGroup.strGroupName[0] = 0;
				prevGroup.nStart = -1;

				int i;
				for(i=0;!rs.IsEOF();rs.MoveNext(),++i)
				{
					stItem& item = aItem[i];
					strcpy(item.strName , GetFieldText(rs.GetFieldValue("name")));
					strcpy(item.strValue , GetFieldText(rs.GetFieldValue("type")));
					item.size = atol(GetFieldText(rs.GetFieldValue("size")));
					if(item.size <= 0)
						item.CalculateSize();

					item.aStrHelp.resize(0);
					_parse_str(item.aStrHelp,GetFieldText(rs.GetFieldValue("help")),';');
					
					stGroup group;
					
					long nGroup = atol(GetFieldText(rs.GetFieldValue("group")));

					sprintf(wszSQL,"select * from [%s] where [index] = %d","Group",nGroup);

					rsGroup.Open(dbOpenSnapshot,wszSQL,dbReadOnly);
					strcpy(group.strGroupName,GetFieldText(rsGroup.GetFieldValue("name")));
					rsGroup.Close();

					if(i == 0)
					{
						strcpy(prevGroup.strGroupName,group.strGroupName);
						prevGroup.nStart = i;
						aGroup.push_back(prevGroup);
						//TRACE("LoadFromDB %d,%s\n",i,prevGroup.strGroupName);
					}
					else
					{
						if( strcmp(group.strGroupName,prevGroup.strGroupName) )
						{
							group.nStart = i;
							aGroup.push_back(group);
							prevGroup = group;
							//TRACE("LoadFromDB %d,%s\n",i,group.strGroupName);
						}
					}
				}
			}
			break;
		case FILE_DATA:
			{
				sprintf(wszSQL,"select * from %s order by IndexID",::GetFileNameTitle(m_strFileBin).c_str());
				
				try
				{
					rs.Open(dbOpenSnapshot,wszSQL,dbReadOnly);
				}
				catch(CDaoException * e)
				{
					TRACE("%s\n",(const char*)GetErrorMessage(e));
					sprintf(wszSQL,"select * from %s",::GetFileNameTitle(m_strFileBin).c_str());
					rs.Open(dbOpenSnapshot,wszSQL,dbReadOnly);
					e->Delete();
				}

				int i;
				std::vector<stItem>& aItem = GetTable()->GetItemVector();
				std::vector<std::string> aStrValue;
				aStrValue.resize(aItem.size());
				for(i=0;!rs.IsEOF();rs.MoveNext(),++i)
				{
					int j;
					for(j=0;j<aItem.size();++j)
					{
						char szValue[2048];
						try
						{
							COleVariant var = rs.GetFieldValue(aItem[j].strName);
							if(var.vt == VT_EMPTY || var.vt == VT_NULL)
							{
								strcpy(szValue,aItem[j].GetDefaultValue().c_str());
							}
							else
							{
								if(aItem[j].IsInt())
									sprintf(szValue,"%d",atol(GetFieldText(var)));
								/*
								else if(aItem[j].IsEnum())
								{
									Assert((long)var < aItem[j].aStrHelp.size());
									sprintf(szValue,"%s",aItem[j].aStrHelp[(long)var].c_str());
								}
								//*/
								else if(aItem[j].IsFloat())
									sprintf(szValue,"%g",(float)atof(GetFieldText(var)));
								else 
									strcpy(szValue,(const char*)GetFieldText(var));
							}
						}
						catch(CDaoException * e)
						{
							TRACE("** %s\n",(const char*)GetErrorMessage(e));
							strcpy(szValue,aItem[j].GetDefaultValue().c_str());
							e->Delete();
						}
						aStrValue[j] = szValue;
					}
					GetTable()->AddProperty(aStrValue);
				}
			}
			break;
		default:
			bLoad = false;
			break;
		}
	}
	catch(CDaoException * e)
	{
		e->ReportError();
		bLoad = false;
		e->Delete();
	}
	return bLoad;
}

#ifdef _GAME_TOOL
struct stFieldInfo{
	char szName[64];
	short type;
	long size;
	
	stFieldInfo(){}
	
	stFieldInfo(const char* name,short t,long sz)
	{
		strcpy(szName,name);
		type = t;
		size = sz;
	}
	
	stFieldInfo(const stFieldInfo& f)
	{
		strcpy(szName,f.szName);
		type = f.type;
		size = f.size;
	}

	bool operator == (const stFieldInfo & f) const
	{
		return stricmp(f.szName,szName) == 0 && type == f.type && (size == 0 || f.size == 0 || size == f.size);
	}
	
	stFieldInfo & operator = (const stFieldInfo & f)
	{
		strcpy(szName,f.szName);
		type = f.type;
		size = f.size;
		return (*this);
	}
};
#endif

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param db : √Ë ˆ
 * \param pszTable : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cFileOP::InitTable(CDaoDatabase& db,const char * pszTable)
{
	try
	{
		CDaoTableDef tab(&db);
		bool bAddTable = false;
		try
		{
			tab.Open(pszTable);
		}
		catch(CDaoException* e)
		{
			TRACE("*** InitTable %s\n",GetErrorMessage(e));
			tab.Create(pszTable);
			bAddTable = true;
			e->Delete();
		}

		std::vector<stFieldInfo> aFields;
		aFields.reserve(256);
		
		switch(m_eFile)
		{
		case FILE_HELP:
			if(stricmp(pszTable,"group") == 0)
			{
				aFields.push_back(stFieldInfo("index",dbLong,4));
				aFields.push_back(stFieldInfo("name",dbText,50));
			}
			else if(stricmp(pszTable,"help") == 0)
			{
				aFields.push_back(stFieldInfo("index",dbLong,4));
				aFields.push_back(stFieldInfo("name",dbText,50));
				aFields.push_back(stFieldInfo("type",dbText,50));
				aFields.push_back(stFieldInfo("group",dbLong,4));
				aFields.push_back(stFieldInfo("help",dbMemo,1024));
				aFields.push_back(stFieldInfo("size",dbLong,4));
			}
			else
			{
				Assert(0);
			}
			break;
		case FILE_DATA:
			{
				std::vector<stItem> & aItem = GetTable()->GetItemVector();
				aFields.push_back(stFieldInfo("IndexID",dbLong,4));
				for(size_t i=0;i<aItem.size();++i)
				{
					stItem& item = aItem[i];
					const char* fieldName = item.strName;
					if(item.IsInt() /*|| item.IsEnum()*/)
						aFields.push_back(stFieldInfo(fieldName,dbLong,4));
					else if(item.IsDword() || item.IsColor())
						aFields.push_back(stFieldInfo(fieldName,dbText,8));
					else if(item.IsFloat())
						aFields.push_back(stFieldInfo(fieldName,dbSingle,4));
					else if(item.IsName() || item.IsString() || item.IsEnum())
						aFields.push_back(stFieldInfo(fieldName,dbText,50));
					else if(item.IsArrayDword() || item.IsArrayColor() || item.IsArrayInt())
						aFields.push_back(stFieldInfo(fieldName,dbMemo,50));
					else
						aFields.push_back(stFieldInfo(fieldName,dbText,255));
				}
			}
			break;
		default:
			return false;
			break;
		}

		// …æ≥˝Œﬁ”√µƒ◊÷∂Œ
		for(int i = tab.GetFieldCount() - 1;i >= 0; --i)
		{
			bool bDeleted = true;
			CDaoFieldInfo fi;
			tab.GetFieldInfo(i,fi);
			for(size_t j=0; j< aFields.size(); ++j)
			{
				stFieldInfo & st = aFields[j];
				if(stricmp(st.szName,fi.m_strName) == 0)
				{
					if(st.type != fi.m_nType)
					{
					}
					else
					{
						bDeleted = false;
					}
					break;
				}
			}
			if(bDeleted)
				tab.DeleteField(i);
		}

		// ∏¸–¬◊÷∂Œ
		for( size_t j=0; j< aFields.size(); ++j )
		{
			stFieldInfo & st = aFields[j];
			bool bAdd = true;
			for( int i = tab.GetFieldCount() - 1; i >= 0; --i )
			{
				CDaoFieldInfo fi;
				tab.GetFieldInfo(i,fi);
				
				if(stricmp(st.szName,fi.m_strName) == 0)
				{
					if(st.type != fi.m_nType)
					{
					}
					else
					{
						bAdd = false;
					}
					break;
				}
			}
			if(bAdd)
			{
				//tab.CreateField(st.szName,st.type,st.size);
				//*
				CDaoFieldInfo fi;
				fi.m_strName = st.szName;
				fi.m_nType = st.type;
				fi.m_lSize = st.size;
				if(st.type == dbText || st.type == dbMemo)
					fi.m_bAllowZeroLength = TRUE;
				else
					fi.m_bAllowZeroLength = FALSE;
				fi.m_lAttributes = 0;
				fi.m_bRequired = FALSE;
				fi.m_lCollatingOrder = 0;
				fi.m_nOrdinalPosition = 0;

				/*fieldinfo.m_strName = lpszName;
				fieldinfo.m_nType = nType;
				fieldinfo.m_lSize = lSize;
				fieldinfo.m_lAttributes = lAttributes;
				fieldinfo.m_nOrdinalPosition = 0;
				fieldinfo.m_bRequired = FALSE;
				fieldinfo.m_bAllowZeroLength = FALSE;
				fieldinfo.m_lCollatingOrder = 0;*/
				tab.CreateField(fi);
				//*/
			}
		}
		if(bAddTable) tab.Append();
		else tab.Close();
		return true;
	}
	catch(CDaoException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param aTableID : √Ë ˆ
 * \param pszDatabase : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cFileOP::GetTableSet(std::vector<int> & aTableID,const char* pszDatabase)
{
	aTableID.resize(0);
	try
	{
		CDaoDatabase db;
		db.Open(pszDatabase);
		
		for(long i=0;i<db.GetTableDefCount();++i)
		{
			CDaoTableDefInfo ti;
			db.GetTableDefInfo(i,ti);
			char* p;
			long id = strtoul(ti.m_strName,&p,10);
			if(p != (const char*)ti.m_strName)
			{
				aTableID.push_back(id);
			}
		}
		return true;
	}
	catch(CDaoException * e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param db : √Ë ˆ
 * \param szSQL : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool ExecuteSQL(CDaoDatabase& db,const char* szSQL)
{
	try
	{
		db.Execute(szSQL);
		return true;
	}
	catch(CDaoException* e)
	{
		TRACE("ExecuteSQL %s\n",GetErrorMessage(e));
		e->Delete();
		return false;
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param db : √Ë ˆ
 * \param szUpdateSQL : √Ë ˆ
 * \param szInsertSQL : √Ë ˆ
 * \param szSelectSQL : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool UpdateTableData(CDaoDatabase& db,const char* szUpdateSQL,const char* szInsertSQL,const char* szSelectSQL)
{
	try
	{
		CDaoRecordset rs(&db);
		rs.Open(dbOpenSnapshot,szSelectSQL,dbReadOnly);
		if(rs.GetRecordCount() == 0)
		{
			return ExecuteSQL(db,szInsertSQL);
		}
	}
	catch(CDaoException * e)
	{
		e->ReportError();
		e->Delete();
	}
	if(!ExecuteSQL(db,szUpdateSQL))
		return ExecuteSQL(db,szInsertSQL);
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cFileOP::SaveToDatabase()
{
	CDaoDatabase conn;

	static char szUpdate[4096];
	static char szInsert[4096];
	char szSelect[1024];
	char szDelete[1024];
	try
	{
		if(IsFileExist(m_strDatabase))
		{
			conn.Open(m_strDatabase);
		}
		else
		{
			conn.Create(m_strDatabase);
		}
		switch(m_eFile)
		{
		case FILE_HELP:
			{
				InitTable(conn,"help");
				InitTable(conn,"group");

				std::vector<stGroup>& aGroup = GetTable()->GetGroupVector();
				std::vector<stItem>& aItem = GetTable()->GetItemVector();
				int nGroup = 0;				

				sprintf(szUpdate , "Update [Group] Set name='%s' where [index]=%d" , aGroup[nGroup].strGroupName , nGroup);
				sprintf(szInsert , "Insert Into [Group] (Name,Index) Values ('%s',%d)" , aGroup[nGroup].strGroupName , nGroup);
				sprintf(szSelect,"Select [index] From [Group] where [index]=%d",nGroup);
				UpdateTableData(conn,szUpdate,szInsert,szSelect);

				for(size_t i=0;i < aItem.size();++i)
				{
					if(nGroup + 1 < aGroup.size() && aGroup[nGroup+1].nStart == i)
					{
						nGroup++;

						sprintf(szUpdate , "Update [Group] Set name='%s' where [index]=%d" , aGroup[nGroup].strGroupName , nGroup);
						sprintf(szInsert , "Insert Into [Group] (Name,Index) Values ('%s',%d)" , aGroup[nGroup].strGroupName , nGroup);
						sprintf(szSelect,"Select [index] From [Group] where [index]=%d",nGroup);
						UpdateTableData(conn,szUpdate,szInsert,szSelect);
					}

					stGroup & group = aGroup[nGroup];
					stItem & item = aItem[i];

					sprintf(szUpdate,"Update Help Set name='%s' , type='%s' , [group] = %d, Help = '%s', size=%d where [index]=%d",item.strName,item.strValue,nGroup,item.GetHelpString().c_str(),item.size,i);
					sprintf(szInsert,"Insert Into Help ( name,type,[group],help,size,[index] ) Values('%s','%s', %d,'%s',%d ,%d)",item.strName,item.strValue,nGroup,item.GetHelpString().c_str(),item.size,i);
					sprintf(szSelect,"Select [index] from Help where [index]=%d",i);
					UpdateTableData(conn,szUpdate,szInsert,szSelect);
				}
				sprintf(szDelete,"Delete from [Group] where [index] > %d",nGroup);
				ExecuteSQL(conn,szDelete);

				sprintf(szDelete,"Delete from Help where [index] >= %d",aItem.size());
				ExecuteSQL(conn,szDelete);
				return true;
			}
			break;
		case FILE_DATA:
			{
				std::string strTableName = ::GetFileNameTitle(m_strFileBin);
				InitTable(conn,strTableName.c_str());

				std::vector<stItem>& aItem = GetTable()->GetItemVector();
				std::vector<std::string> array;
				array.reserve(aItem.size());

				std::string strFieldNames;
				for(size_t j = 0;j < aItem.size();++j)
				{
					stItem & item = aItem[j];
					strFieldNames += item.strName;
					strFieldNames += ",";
				}
				strFieldNames += "IndexID";

				
				for (size_t i=0; i<GetTable()->GetPropertyCount();i++)
				{
					array.resize(0);
					GetTable()->Property2String(GetTable()->GetRawProperty(i),array);
					
					sprintf(szUpdate,"Update %s set ", strTableName.c_str());
					sprintf(szInsert,"Insert Into %s (%s) Values (",strTableName.c_str(),strFieldNames.c_str());

					for(size_t j = 0;j < aItem.size();++j)
					{
						stItem & item = aItem[j];
						if(j != 0)
						{
							strcat(szUpdate,",");
							strcat(szInsert,",");
						}
						if(item.IsInt() || item.IsFloat())
						{
							sprintf(szUpdate + strlen(szUpdate),"%s=%s",item.strName,array[j].c_str());
							sprintf(szInsert + strlen(szInsert),"%s",array[j].c_str());
						}
						else
						{
							sprintf(szUpdate + strlen(szUpdate),"%s=\'%s\'",item.strName,array[j].c_str());
							sprintf(szInsert + strlen(szInsert),"\'%s\'",array[j].c_str());
						}
					}
					sprintf(szUpdate + strlen(szUpdate) ," Where IndexID = %d",i);
					sprintf(szInsert + strlen(szInsert), ", %d)",i);
					sprintf(szSelect,"Select IndexID From %s where IndexID=%d",strTableName.c_str(),i);
					//TRACE("%s\n",szUpdate);
					//TRACE("%s\n",szInsert);
					UpdateTableData(conn,szUpdate,szInsert,szSelect);
				}
				sprintf(szDelete,"Delete from %s where IndexID >= %d",strTableName.c_str(),i);
				ExecuteSQL(conn,szDelete);
			}
			break;
		default:
			return false;
			break;
		}
		return true;
	}
	catch(CDaoException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param bBin : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cFileOP::Load2(bool bBin)
{
	if(bBin)
		return LoadBin();
	else
		return LoadIni();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param bBin : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool cFileOP::Save2(bool bBin)
{
	if(bBin) return SaveBin(false);
	else return SaveIni(false);
}
#endif