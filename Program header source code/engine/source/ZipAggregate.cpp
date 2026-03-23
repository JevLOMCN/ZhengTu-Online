/**
* \file      ZipAggregate.cpp
* \version   $Id$
* \author    $$$03¹«Ë¾
*            $$$03¹«Ë¾@###.com
* \date      2006-7-19  21:11:00
* \brief     ±éÀúZIP°üÖĞÎÄ¼şĞÅÏ¢
* 
*	      ±éÀúZIP°üÖĞÎÄ¼şĞÅÏ¢
*/

// ZipAggregate.cpp: implementation of the ZipAggregate class.
//
//////////////////////////////////////////////////////////////////////
#include "../include/engine.h"
#include "../include/streamtypes.h"
#include <vector>
#include "../include/ZipAggregate.h"
#include "../include/FileStream.h"
#include "../include/ZipHeader.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
ZipAggregate::ZipAggregate()
{
	m_szZipFileName[0]=0;
	//VECTOR_SET_ASSOCIATION(m_fileList);
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
ZipAggregate::~ZipAggregate()
{
	closeAggregate();
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
bool
ZipAggregate::refreshAggregate()
{
	AssertFatal(m_szZipFileName[0] != 0, "No filename?  Must not be open.  Disallowed");

	char tmpBuff[512];
	strcpy(tmpBuff, m_szZipFileName);

	return openAggregate(tmpBuff);
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param in_pFileName : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool ZipAggregate::openAggregate(const char* in_pFileName)
{
	closeAggregate();

	AssertFatal(in_pFileName != NULL, "No filename to open!");

	strcpy(m_szZipFileName ,in_pFileName);

	FileStream stream ;
	if (stream.open(m_szZipFileName, FileStream::Read) == false ||
		createZipDirectory(&stream)   == false) {
			// Failure, abort the open...
			//

			m_szZipFileName[0]=0;
			return false;
		}

		// Finished!  Open for business
		return true;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
void ZipAggregate::closeAggregate()
{
	destroyZipDirectory();

	m_szZipFileName[0]=0;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
void ZipAggregate::destroyZipDirectory()
{
	m_fileList.clear();
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param io_pStream : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
bool ZipAggregate::createZipDirectory(Stream* io_pStream)
{
	AssertFatal(io_pStream != NULL,      "Error, stream not open.");

	DWORD streamSize      = io_pStream->getSize();
	DWORD initialPosition = io_pStream->getPosition();

	// We assume that the CD is 22 bytes from the end.  This will be invalid
	//  in the case that the zip file has comments.  Perhaps test the quick
	//  way, then degrade to seaching the final 64k+22b (!) of the stream?
	//
	bool posSuccess = io_pStream->setPosition(streamSize - sizeof(ZipEOCDRecord::EOCDRecord));
	if (posSuccess == false) {
		AssertFatal(false, "Unable to position stream to start of EOCDRecord");
		return false;
	}

	ZipEOCDRecord* pEOCDRecord = new ZipEOCDRecord;
	if (pEOCDRecord->readFromStream(*io_pStream) == false) {
		// This is where we would try to degrade to general case...
		//
		AssertFatal(false, "Unable to locate central directory.  "
			"Zip File might have comments");
		delete pEOCDRecord;
		return false;
	}

	// Check the consistency of the zipFile.
	//
	if ((pEOCDRecord->m_record.diskNumber       != pEOCDRecord->m_record.eocdDiskNumber)    ||
		(pEOCDRecord->m_record.numCDEntriesDisk != pEOCDRecord->m_record.numCDEntriesTotal)) 
	{
		AssertFatal(false, "Zipfile appears to be part of a "
			"multi-zip disk span set, unsupported");
		delete pEOCDRecord;
		return false;
	}

	// If we're here, we're good!  Scan to the start of the CDirectory, and
	//  start scanning the entries into our directory structure...
	//
	DWORD startCDPosition = pEOCDRecord->m_record.cdOffset;
	DWORD endCDPosition   = pEOCDRecord->m_record.cdOffset +
		pEOCDRecord->m_record.cdSize;

	posSuccess = io_pStream->setPosition(startCDPosition);
	if (posSuccess == false) {
		AssertFatal(false, "Unable to position to CD entries.");
		delete pEOCDRecord;
		return false;
	}

	m_fileList.reserve(pEOCDRecord->m_record.numCDEntriesTotal);
	bool dirReadSuccess = true;
	BYTE* buf = new BYTE[pEOCDRecord->m_record.cdSize];
	BYTE* pBuf = buf;
	io_pStream->read(buf,pEOCDRecord->m_record.cdSize);
	for (DWORD i = 0; i < pEOCDRecord->m_record.numCDEntriesTotal; i++) {

		/*
		ZipDirFileHeader zdfHeader;

		bool hrSuccess = zdfHeader.readFromStream(*io_pStream);
		if (hrSuccess == false) {
		AssertFatal(false, "Error reading a CD Entry in zip aggregate");
		dirReadSuccess = false;
		break;
		}

		enterZipDirRecord(zdfHeader);
		//*/
		ZipDirFileHeader::DirFileHeader *hdr = (ZipDirFileHeader::DirFileHeader*)pBuf;
		pBuf+=sizeof(*hdr);
		char* pszFileName = (char*)pBuf;
		pBuf += (hdr->fileNameLength + hdr->extraFieldLength);
		if(pszFileName[hdr->fileNameLength-1] != '/' && 
			(hdr->compressionMethod == ZipDirFileHeader::Deflated ||
			hdr->compressionMethod == ZipDirFileHeader::Stored) )
		{
			m_fileList.resize(m_fileList.size()+1);

			FileEntry & ent = m_fileList.back();

			strncpy(ent.pFileName,pszFileName,hdr->fileNameLength);
			ent.pFileName[hdr->fileNameLength] = 0;
			ent.fileSize = hdr->uncompressedSize;
			ent.compressedFileSize = hdr->compressedSize;
			ent.fileOffset = hdr->relativeOffsetOfLocalHeader;

			if(hdr->compressionMethod == ZipDirFileHeader::Deflated)
				ent.flags = FileEntry::Compressed;
			else if(hdr->compressionMethod == ZipDirFileHeader::Stored)
			{
				ent.flags = FileEntry::Uncompressed;
			}
		}
	}
	delete [] buf;
	delete pEOCDRecord;

	if (dirReadSuccess == true) {
		// Every thing went well, we're done, position the stream to the end of the
		//  CD...
		//
		io_pStream->setPosition(endCDPosition);
		return true;
	} else {
		// Oh, crap.
		io_pStream->setPosition(initialPosition);
		destroyZipDirectory();
		return false;
	}
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param in_rHeader : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void
ZipAggregate::enterZipDirRecord(const ZipDirFileHeader& in_rHeader)
{
	// First figure out whether we are looking at a directory
	// or a file. Directories have a trailing / in the file name
	// and a filelength of 0
	if (in_rHeader.m_pFileName[strlen(in_rHeader.m_pFileName) - 1] == '/' &&
		(in_rHeader.m_header.compressedSize   == 0 &&
		in_rHeader.m_header.uncompressedSize == 0))
		return;

	// We have a file if we are here, so enter it
	// into the directory
	m_fileList.push_back(FileEntry());
	FileEntry& rEntry = m_fileList.back();

	// Copy the path to a file within a zip to tempString
	strcpy(rEntry.pFileName, in_rHeader.m_pFileName);

	// Tell ResourceManger the appropriate file attributes
	rEntry.fileSize           = in_rHeader.m_header.uncompressedSize;
	rEntry.compressedFileSize = in_rHeader.m_header.compressedSize;
	rEntry.fileOffset         = in_rHeader.m_header.relativeOffsetOfLocalHeader;

	// Tell ResourceManager the appropriate file compressions used on the file
	if(in_rHeader.m_header.compressionMethod == ZipDirFileHeader::Deflated)
		rEntry.flags = FileEntry::Compressed;
	else if(in_rHeader.m_header.compressionMethod == ZipDirFileHeader::Stored)
	{
		rEntry.flags = FileEntry::Uncompressed;
	}
	else 
	{
		// We can't have anything other than Stored or Deflated zips
		AssertFatal(0, avar("Warning, non-stored or deflated resource in %s", m_szZipFileName));
		std::vector<FileEntry>::iterator it = m_fileList.end();
		it--;
		if(it != m_fileList.end())
			m_fileList.erase(it);
	}

}