/**
 * \file      ZipAggregate.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     遍历ZIP包中文件信息
 * 
 *	      遍历ZIP包中文件信息
 */

// ZipAggregate.h: interface for the ZipAggregate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZIPAGGREGATE_H__6AABA90F_A540_4A68_97C9_E8FD2C00A837__INCLUDED_)
#define AFX_ZIPAGGREGATE_H__6AABA90F_A540_4A68_97C9_E8FD2C00A837__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ZipDirFileHeader;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class ZipAggregate
{
public:
	struct FileEntry {
		enum {
			Uncompressed = 0,
			Compressed   = (1 << 0)
		};

		//char pPath[256];
		char pFileName[MAX_PATH];
		DWORD fileOffset;
		DWORD fileSize;
		DWORD compressedFileSize;
		DWORD flags;
	};

	//-------------------------------------- Instance scope members and decls.
private:
	char             m_szZipFileName[MAX_PATH];
	std::vector<FileEntry> m_fileList;

	void enterZipDirRecord(const ZipDirFileHeader& in_rHeader);
	bool createZipDirectory(Stream*);
	void destroyZipDirectory();

	ZipAggregate(const ZipAggregate&);   // disallowed 
public:
	ZipAggregate();
	~ZipAggregate();

	// Opening/Manipulation interface...
public:
	bool openAggregate(const char* in_pFileName);
	void closeAggregate();
	bool refreshAggregate();

	// Entry iteration interface...
public:
	typedef std::vector<FileEntry>::const_iterator iterator;

	DWORD numEntries() const                           { return m_fileList.size(); }
	const FileEntry& operator[](const DWORD idx) const { return m_fileList[idx]; }
	iterator begin() const                           { return m_fileList.begin(); }
	iterator end() const                             { return m_fileList.end(); }
};

#endif // !defined(AFX_ZIPAGGREGATE_H__6AABA90F_A540_4A68_97C9_E8FD2C00A837__INCLUDED_)
