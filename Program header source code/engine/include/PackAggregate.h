/**
 * \file      PackAggregate.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     Pak文件包支持，支持加密压缩
 * 
 *	      Pak文件包支持，支持加密压缩
 */

#pragma once

#include <vector>


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class PackAggregate
{
public:

	enum enumFlags{
		flagCompress = 0x1,
		flagEncrypt = 0x2,
		flagEncryptHeaderOnly = 0x4,
	};

	struct stPackHeader
    {
		DWORD magic;
		DWORD version;
        int filenum;
		int headerSize;
        DWORD flag;
    };

	struct stFileInfo1{
		DWORD dwFileID;
		DWORD fileOffset;
		DWORD size; // 压缩加密后的大小 
		DWORD interspaceSize;	// 所有空间大小，包括未用的空间
		DWORD dwCRC;// 压缩加密后的CRC
	};

	struct stFileInfo
	{
		union{
			struct{
				DWORD dwFileID;
				DWORD fileOffset;
				DWORD size;				// 压缩加密后的大小 
				DWORD interspaceSize;	// 所有空间大小，包括未用的空间
				DWORD dwCRC;			// 压缩加密后的CRC
			};
			stFileInfo1 st1;
		};
		char szName[MAX_PATH];
	};

	struct stFileSizeInfo
	{
		DWORD version;
		DWORD flags;
		DWORD uncompressedSize; // 未压缩加密的大小
		DWORD dwCRC;			// 未压缩加密的CRC
	};


	// 在子文件内部的信息
	struct stFileBaseInfo
	{
		char szName[MAX_PATH];
		DWORD fileOffset;
		DWORD size;				// 文件整个大小（包括头）
		DWORD dwCRC;			// 压缩加密后的CRC
	};

	struct stFileDetailInfo
	{
		DWORD fileOffset;
		DWORD uncompressedSize;	// 文件实际大小
		DWORD dwUnprocessCRC;	// 未压缩加密的CRC
		DWORD flags;
	};

private:
	char	m_szFileName[MAX_PATH];
	std::vector<stFileBaseInfo> m_aFileInfo;
	DWORD	m_dwFlags;
	DWORD   m_dwVersion;
	const	BYTE* m_pDesKey;
	
public:
	typedef std::vector<stFileBaseInfo>::const_iterator iterator;

	PackAggregate(void);
	~PackAggregate(void);

	bool getFileInfo(DWORD dwFileIndex,Stream* pStream,DWORD dwVersion,DWORD flags,DWORD offset,stFileDetailInfo * result);

	DWORD getFlags(){ return m_dwFlags; }
	DWORD getVersion(){ return m_dwVersion;}
	bool refreshAggregate();
	bool openAggregate(const char* szFileName);
	
	static bool getFileInfo(const char* szFileName,std::vector<stFileInfo> & files);
	static bool resortFilePack(const char* pack,std::vector<PackAggregate::stFileInfo> & files);

	void closeAggregate();
	iterator begin(){ return m_aFileInfo.begin();}
	iterator end(){return m_aFileInfo.end();}
	void SetDesKey(const BYTE* pKey)
	{
		m_pDesKey = pKey;
	}
	static const char* GetDefaultPassword();
	typedef void ( *tOutputFunc)(LPCTSTR lpszFmt,...);
	static bool MakePackDir(LPCSTR szDir,LPCSTR szPackName,bool bZip,int nEncrypt,int zipLevel,const char* szPassword ,std::vector<String> & fileFilter,tOutputFunc  pOutoutFunc,BOOL & bStop,bool (* fnOnMakeSubFile)(const char* fileName,DWORD dwCRC) = NULL);
};
