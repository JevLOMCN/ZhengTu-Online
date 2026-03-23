/**
 * \file      zipHeader.h
 * \version   $Id$
 * \author    $$$03π´Àæ
 *            $$$03π´Àæ@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#ifndef _ZIPHEADER_H
#define _ZIPHEADER_H

#pragma pack(push,1)

//-------------------------------------- Structure designed to fit exactly 256 bytes.
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class ZipLocalFileHeader
{
	// NB: Extra field in the header is ignored, but the stream read seeks
	//      past it...
	//
private:
	static const DWORD csm_localFileHeaderSig;

public:
	enum {
		MaxFileNameLength = 211
	};
	enum CompressionMethod {
		Stored            = 0,
		Shrunk            = 1,
		ReducedL1         = 2,
		ReducedL2         = 3,
		ReducedL3         = 4,
		ReducedL4         = 5,
		Imploded          = 6,
		ReservedTokenized = 7,
		Deflated          = 8,
		EnhDefalted       = 9,
		DateCompression   = 10
	};

	struct LocalFileHeader {
		DWORD headerSig;
		WORD versionToDecompress;
		WORD bitFlags;
		WORD compressionMethod;
		WORD lastModTime;
		WORD lastModDate;
		DWORD crc32;
		DWORD compressedSize;
		DWORD uncompressedSize;

		WORD fileNameLength;
		WORD extraFieldLength;
	};

	LocalFileHeader m_header;         // Fixed size header
	char            m_pFileName[226]; // Variable size: FileName.  Note that the
	//  number of chars here is more than the
	//  getMax allowed filename for alignment
	//  purposes

	// Stream read routines
public:
	bool readFromStream(Stream& io_rStream);
};


//-------------------------------------- Also designed to fit into 256 bytes, note
//                                        that we ignore the extra and file comment
//                                        fields.
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */


class ZipDirFileHeader
{
private:
	static const DWORD csm_dirFileHeaderSig;

public:
	enum {
		MaxFileNameLength = 211
	};
	enum CompressionMethod {
		Stored            = 0,
		Shrunk            = 1,
		ReducedL1         = 2,
		ReducedL2         = 3,
		ReducedL3         = 4,
		ReducedL4         = 5,
		Imploded          = 6,
		ReservedTokenized = 7,
		Deflated          = 8,
		EnhDefalted       = 9,
		DateCompression   = 10
	};

	struct DirFileHeader {
		DWORD   headerSig;
		WORD   versionMadeBy;
		WORD   versionToDecompress;
		WORD   bitFlags;
		WORD   compressionMethod;
		WORD   lastModTime;
		WORD   lastModDate;
		DWORD   crc32;
		DWORD   compressedSize;
		DWORD   uncompressedSize;
		WORD   fileNameLength;
		WORD   extraFieldLength;
		WORD   fileCommentLength;
		WORD   diskNumberStart;
		WORD   internalFileAttributes;
		DWORD   externalFileAttributes;
		DWORD   relativeOffsetOfLocalHeader;
	};

	DirFileHeader  m_header;
	char           m_pFileName[212];

	// Stream read routines
public:
	bool readFromStream(Stream& io_rStream);
};


//-------------------------------------- Padded to 32 bytes.  Note that we completely
//                                        ignore any zip file comments.
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class ZipEOCDRecord
{
private:
	static const DWORD csm_eocdRecordSig;

public:
	enum {
		ProperRecordSize = 22
	};

	struct EOCDRecord {
		DWORD   eocdSig;
		WORD   diskNumber;
		WORD   eocdDiskNumber;
		WORD   numCDEntriesDisk;
		WORD   numCDEntriesTotal;
		DWORD   cdSize;
		DWORD   cdOffset;
		WORD   zipFileCommentLength;
	};

	EOCDRecord m_record;
	char       __padding[10];
	// Stream read routines
public:
	bool readFromStream(Stream& io_rStream);
};


#pragma pack(pop)

#endif