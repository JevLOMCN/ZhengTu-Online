#include "../include/engine.h"
#include <string>
#include <vector>
#include "../../xml_parse/XmlParseEx.h"

#define _USE_MY_MBSSTR_

struct stSearchList
{
	unsigned char * pPos;
	bool            bChinese;
};


//#ifdef _USE_MY_MBSSTR_
//
//unsigned char * My_Mbsstr(const unsigned char *pdest,const unsigned char * ps)
//{
//         const unsigned char * pSub = ps;
//	 const unsigned char * pdest
//}
//#define _MY_MBSSTR_(x,y)  My_Mbsstr(x,y)
//       
//#else 
//	#define _MY_MBSSTR_(x,y)  _mbsstr(x,y)
//#endif

//pdest = _mbsstr((const unsigned char *)pdest,(const unsigned char *)ps);

enum TYPE_FILETER
{
	TYPE_CHAT,
	TYPE_NAME,
};
class CPhraseFilter
{
public:
	// constructor 
	explicit CPhraseFilter(){
		//m_Singleton = 0 ;
		m_bLoadOK = false;
		m_bLoaded = false;
	};

	// destructor 
	~CPhraseFilter(){};
private:
	struct stFilter
	{
		std::string strWord;
	};
	//stdext::hash_map<DWORD,stFilter> m_MapFilter;	
	std::vector<stFilter>  m_vecFilterChat;
	std::vector<stFilter>  m_vecFilterName;

	bool __LoadFromFile(TYPE_FILETER type);

public:
	bool PhraseFilter(const char * pword,char * pOutter,size_t nlen,TYPE_FILETER  type);

	/// get instance pointer
	static CPhraseFilter* Instance();

	static void SafeRelease();

protected:
	//static CPhraseFilter* m_Singleton;

	static bool m_bLoadOK ;
	static bool m_bLoaded ;
};

//CPhraseFilter* CPhraseFilter::m_Singleton = 0; 
bool           CPhraseFilter::m_bLoadOK = false;
bool           CPhraseFilter::m_bLoaded = false;

static CPhraseFilter g_filter;
CPhraseFilter* CPhraseFilter::Instance()
{
	return &g_filter;
	/*if( m_Singleton == 0)
	m_Singleton = new CPhraseFilter;
	Assert(m_Singleton != 0);
	return m_Singleton;*/
}

void CPhraseFilter::SafeRelease()
{
	/*if( m_Singleton ){
	delete m_Singleton;
	m_Singleton = 0;
	}*/
	m_bLoadOK = false;
	m_bLoaded = false;
}

bool CPhraseFilter::__LoadFromFile(TYPE_FILETER type)
{
	static bool m_bLoadedChat = false;
	static bool m_bLoadOKChat = false;

	static bool m_bLoadedName = false;
	static bool m_bLoadOKName = false;

	std :: vector<stFilter> * pVector = NULL;
	std::string strFileName;
	if(type == TYPE_CHAT)
	{
		if (m_bLoadedChat)
			return m_bLoadOKChat;

		m_bLoadedChat = true;
	
		pVector = &m_vecFilterChat;

		strFileName = "datas\\filters1.xml";
	}
	else  // name
	{
		if (m_bLoadedName)
			return m_bLoadOKName;

		m_bLoadedName = true;

		pVector = &m_vecFilterName;

		strFileName = "datas\\filters.xml";
	}

	pVector->clear();

	TiXmlDocument doc;
	Stream* pStream = OpenPackFileForRead("data\\datas.pak",strFileName.c_str());
	if(!pStream)
	{
		Engine_WriteLog("装载 filters.xml 失败\n");
		return FALSE;
	}
	//pStream->write("e:\\filters.xml");
	if(!LoadXMLFromStream(pStream,doc))
	{
		ClosePackFileForRead(pStream);
		Engine_WriteLog("解析 filters.xml 失败\n");
		return FALSE;
	}
	TiXmlElement* filter = doc.FirstChildElement("filter");
	if(filter)
	{
		TiXmlElement* Text = filter->FirstChildElement("Text");
		while(Text)
		{
			const char* pword = Text->Attribute("word");
			if( pword  )
			{
				//DWORD dwCRC = calculateCRC(pword,strlen(pword));
				//DWORD dwCRC = 1;
				//CPhraseFilter::stFilter & fiter = m_MapFilter[dwCRC];
				CPhraseFilter::stFilter fiter;
				fiter.strWord = pword;
				pVector->push_back(fiter);

			}
			Text = Text->NextSiblingElement("Text");
		}
	}
	ClosePackFileForRead(pStream);

	if(type == TYPE_CHAT)
	{	
		m_bLoadOKChat = true;
	}
	else  // name
	{
		m_bLoadOKName = true;
	}
	
	return true;
}



bool CPhraseFilter::PhraseFilter(const char * pword,char * pOutter,size_t nlen,TYPE_FILETER  type)
{
	if( !pword ) return false;

	assert(pOutter != NULL && nlen >= 0 );
	if( !pOutter  || nlen == 0 ) return false;

#ifdef _DEBUG
	assert( !IsBadWritePtr(pOutter,nlen));
#endif 

	// load from file 
	BOOL bLoadOK = __LoadFromFile(type);
	if( FALSE == bLoadOK  ) 
		return false;

	_snprintf(pOutter,nlen,"%s",pword); 
	pOutter[nlen-1]=0;

	bool bFilterd = false;
	////////////////////
	std :: vector<stFilter> * pVector = NULL;
	if(type == TYPE_CHAT)
	{
		pVector = &m_vecFilterChat;
	}
	else  // name
	{
		pVector = &m_vecFilterName;
	}
	////////////////////
	std::vector<stFilter>::iterator it = pVector->begin();
	int i = pVector->size();
	for(; it!= pVector->end(); ++it )
	{
		i--;
		const stFilter & f = *it;
		const unsigned char * ps =  (const unsigned char *)f.strWord.c_str() ;

		if(strlen((const char *)ps) == 0 )
			continue;

		//add by $$$03公司@###.com 如果过滤字符表有“”则退出，cup占100％就在这里处理有问题，看下
		//filters.xml里面最后面的过滤字符都是“”？？？
		if (strcmp(f.strWord.c_str(),"") == 0)
		{
			TRACE("－－－$$$03公司@###.com－－－过滤字符第%d行是空字符串－－\n",i);
			continue;
		}

		unsigned char *pdest = (unsigned char *)pOutter;

		//if( type == TYPE_NAME)
		//{
		//	///////////////////////////////////////////////////////////////////////////////////////
		//	// 这段词汇过滤代码的主要思想如下:
		//	//     对于每个关键字，如 key = "日你妈"; 分成　‘日’　‘你’　‘妈',放到listKey中
		//	//
		//	//     然后将要过滤的字符串中找出含有这些关键字的地方放到MgrKeyList中，
		//	//     MgrKeyList.size() = 3
		//	//     MgrKeyList[0] = *;*;*; // 日　系列
		//	//     MgrKeyList[1] = *;*;*; // 你　系列
		//	//     MgrKeyList[2] = *;*;*; // 妈　系列
		//	// 
		//	//     然后求出上述系列中个数最小的，
		//	//     从MgrKeyList的各个系列中分别抽取这么多个按照地址从前到后放到listTemp中，
		//	//     最后把listTemp中的元素相应位置用*号代替.
		//	///////////////////////////////////////////////////////////////////////////////////////
		//	// parse the keylist
		//	//std::list<stSearchList> listKey;
		//	//typedef std::list<stSearchList>::iterator Iterator;
		//	//typedef std::list<stSearchList> SubKeyList;
		//	//size_t i = 0;
		//	//bool bFullEnglish = true;
		//	//while( *(ps+i) )
		//	//{
		//	//	stSearchList key;
		//	//	key.pPos = (unsigned char *)(ps+i);
		//	//	if(*(ps+i) & 0x80)
		//	//	{
		//	//		if( *(ps+i+1) == 0) break;	
		//	//		key.bChinese = true;
		//	//		i+=2;
		//	//		bFullEnglish = false;
		//	//	}
		//	//	else
		//	//	{
		//	//		key.bChinese = false;
		//	//		i++;
		//	//	}
		//	//	listKey.push_back(key);
		//	//}

		//	//if( bFullEnglish)
		//	//{
		//	//	pdest = _mbsstr((const unsigned char *)pdest,(const unsigned char *)ps);
		//	//	while(pdest)
		//	//	{
		//	//		unsigned char * pAfter = pdest + strlen((const char *)ps) * sizeof(unsigned char);
		//	//		if( *pAfter == 0)
		//	//			break;
		//	//		if( !pAfter || *pAfter == ' ')
		//	//		{
		//	//			unsigned char * pCur = pdest;
		//	//			memset((void *)pCur,'*',_mbslen(ps));
		//	//			unsigned char * pInsert = pCur + _mbslen(ps) * sizeof(unsigned char);
		//	//			memmove(pInsert,pAfter,strlen((const char *)pAfter)*sizeof(unsigned char));
		//	//			*(pInsert + strlen((const char *)pAfter)*sizeof(unsigned char)) = 0;

		//	//			pdest = pInsert;

		//	//			bFilterd = true;
		//	//		}
		//	//		else 
		//	//		{
		//	//			pdest = pAfter+1;

		//	//		}
		//	//		pdest = _mbsstr((const unsigned char *)pdest,(const unsigned char *)ps);
		//	//	}
		//	//}
		//	//else 
		//	//{
		//	//	//parse the stringlist		
		//	//	std::vector<SubKeyList> MgrKeyList;
		//	//	MgrKeyList.resize(listKey.size());

		//	//	unsigned char *pTempDest = pdest;
		//	//	i = 0;
		//	//	Iterator iterKeyOrder = listKey.begin();
		//	//	while(*(pTempDest+i))
		//	//	{
		//	//		size_t k = 0 ;
		//	//		for(Iterator iter = listKey.begin(); iter != listKey.end();++iter,k++)
		//	//		{
		//	//			const stSearchList & key = *iter;
		//	//			if(*(pTempDest+i) & 0x80 )
		//	//			{						 
		//	//				if(!key.bChinese)
		//	//				{
		//	//					continue;						
		//	//				}					
		//	//				if( 0 == memcmp(key.pPos,pTempDest+i,sizeof(unsigned char) * 2) )
		//	//				{
		//	//					if(iterKeyOrder == iter)
		//	//					{
		//	//						iterKeyOrder++;    
		//	//					}
		//	//					stSearchList st;
		//	//					st.pPos = pTempDest+i;
		//	//					st.bChinese = true;
		//	//					MgrKeyList[k].push_back(st);
		//	//					i+=2;
		//	//					break;
		//	//				}
		//	//			}
		//	//			else 
		//	//			{
		//	//				if(key.bChinese)
		//	//				{
		//	//					continue;
		//	//				}
		//	//				if( 0 == memcmp(key.pPos,pTempDest+i,sizeof(unsigned char)) )
		//	//				{
		//	//					if(iterKeyOrder == iter)
		//	//					{
		//	//						iterKeyOrder++;    
		//	//					}
		//	//					stSearchList st;
		//	//					st.pPos = pTempDest+i;
		//	//					st.bChinese = false;
		//	//					MgrKeyList[k].push_back(st);
		//	//					i++;
		//	//					break;
		//	//				}
		//	//			}				
		//	//		}
		//	//		if(iter == listKey.end())
		//	//		{
		//	//			if(*(pTempDest+i) & 0x80)
		//	//			{
		//	//				i+=2;
		//	//			}
		//	//			else 
		//	//				i++;
		//	//		}
		//	//	}
		//	//	if(iterKeyOrder != listKey.end())
		//	//		continue;

		//	//	// if include ,then replace it 
		//	//	size_t min_group = 0xef;
		//	//	for(i = 0 ; i < MgrKeyList.size() ; ++i)
		//	//	{
		//	//		if(MgrKeyList[i].size() == 0)
		//	//			break;
		//	//		if( min_group > MgrKeyList[i].size())
		//	//			min_group = MgrKeyList[i].size();
		//	//	}
		//	//	if( i < MgrKeyList.size())
		//	//	{
		//	//		continue;
		//	//	}	

		//	//	//sort from small to big as the address,else the program will produce the memory error
		//	//	SubKeyList listTemp;
		//	//	for(i = 0 ;  i < MgrKeyList.size() ; ++i)
		//	//	{
		//	//		SubKeyList & keylist = MgrKeyList[i];
		//	//		size_t k = 0 ;
		//	//		for(Iterator SIter = keylist.begin(); k < min_group && SIter != keylist.end() ; ++SIter,++k)
		//	//		{

		//	//			stSearchList  & SElement = *SIter;
		//	//			// insert into the propery pos as the order of small-to-big as the address
		//	//			for(Iterator _iter = listTemp.begin(); _iter != listTemp.end(); ++_iter)
		//	//			{
		//	//				stSearchList & _SElement = * _iter;
		//	//				if(_SElement.pPos > SElement.pPos)
		//	//				{
		//	//					listTemp.insert(_iter,SElement);
		//	//					break;
		//	//				}							
		//	//			}
		//	//			if(_iter == listTemp.end())
		//	//				listTemp.push_back(SElement);
		//	//		}

		//	//	}

		//	//	// clear the temperatory data
		//	//	MgrKeyList.clear();

		//	//	char * pTempOutter  = (char *)_alloca(nlen);
		//	//	memset(pTempOutter,0,nlen);
		//	//	int nTempIndex = 0 ,nIndex =0;
		//	//	pTempOutter[nlen-1]=0;

		//	//	// fill the '*'    
		//	//	for(Iterator SIter = listTemp.begin(); SIter != listTemp.end() ; ++SIter)
		//	//	{
		//	//		stSearchList  & SElement = *SIter;
		//	//		if( SElement.pPos )
		//	//		{
		//	//			size_t pttrsize = SElement.pPos-(unsigned char *)(pOutter+nIndex);
		//	//			memcpy(pTempOutter+nTempIndex,pOutter+nIndex,pttrsize+1);
		//	//			*(pTempOutter + nTempIndex + pttrsize) = '*';
		//	//			nTempIndex += pttrsize + 1;
		//	//			nIndex += pttrsize + 1;
		//	//			if(SElement.bChinese)
		//	//				nIndex++;
		//	//			Assert(nIndex < nlen);
		//	//			Assert(nTempIndex < nlen);
		//	//			bFilterd = true;
		//	//		}
		//	//	}

		//	//	memcpy(pTempOutter+nTempIndex,pOutter+nIndex,nlen-1-nTempIndex);
		//	//	Assert(pTempOutter[nlen-1] == 0);
		//	//	pTempOutter[nlen-1] = 0;
		//	//	memcpy(pOutter,pTempOutter,nlen);
		//	//	pOutter[nlen-1] = 0;
		//	//}
		//}
		//else 
		//{//CHAT
		//	
		//}

		pdest = _mbsstr((const unsigned char *)pdest,(const unsigned char *)ps);
		while(pdest)
		{
			const unsigned char * pAfter = pdest + strlen((const char *)ps) * sizeof(unsigned char);
			unsigned char * pCur = pdest;
			memset((void *)pCur,'*',_mbslen(ps));
			unsigned char * pInsert = pCur + _mbslen(ps) * sizeof(unsigned char);
			memmove(pInsert,pAfter,strlen((const char *)pAfter)*sizeof(unsigned char));
			*(pInsert + strlen((const char *)pAfter)*sizeof(unsigned char)) = 0;

			pdest = pInsert;

			pdest = _mbsstr((const unsigned char *)pdest,(const unsigned char *)ps);
			bFilterd = true;
		}

	}

	// print 
#ifdef _DDEBUG
	stdext::hash_map<DWORD,stFilter>::iterator iter = m_MapFilter.begin();
	for( ;iter !=m_MapFilter.end();iter++)
	{
		stFilter & f = iter->second;
		TRACE("%d,%s\n",iter->first,f.strWord.c_str());
	}
#endif 
	return bFilterd;
}

///////////////////////////////////////////////////////////////////////////////
// param: 
//      @pword : phrase which will be filted 
//      @pOutter : the outer string buffer
//      @nlen:     the  length of  ' @pOutter ' 
////////////////////////////////////////////////////////////////////////////////
bool G_PhraseFilterName(const char * pword,char * pOutter,size_t nlen)
{
	CPhraseFilter * pFilter =  CPhraseFilter::Instance(); 
	return pFilter->PhraseFilter(pword,pOutter,nlen,TYPE_NAME); 
}


bool G_PhraseFilterChat(const char * pword,char * pOutter,size_t nlen)
{
	CPhraseFilter * pFilter =  CPhraseFilter::Instance(); 
	return pFilter->PhraseFilter(pword,pOutter,nlen,TYPE_CHAT); 
}
