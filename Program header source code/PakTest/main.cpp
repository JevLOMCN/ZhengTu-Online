#include <iostream>
#include <string>
#include <io.h>
#include <direct.h>

#include "../Engine/include/engine.h"
#include "../engine/source/FilePackHelper.cpp"

void paktest1()
{
	PackAggregate pakaggregate;

	byte des_key[3][8];
	PasswordToDesKey( PackAggregate::GetDefaultPassword(),des_key[0],des_key[1],des_key[2] );
	pakaggregate.SetDesKey((const byte*)des_key);

	pakaggregate.openAggregate( "data\\datas.pak" );

	PackAggregate::iterator it = pakaggregate.begin();
	PackAggregate::iterator end = pakaggregate.end();
	for(;it< end;it++)
		std::cout << it->szName << std::endl;
}

int main()
{
	std::cout << "输入文件名:";
	char pakFileName[200];
	std::cin >> pakFileName;

	bool bFullMap = false;
	//const TCHAR* pszZipPack = "data\\datas.pak";
	stZipPackInfo * pZipPackInfo = new stZipPackInfo;
	pZipPackInfo->dwFlags = 0;
	pZipPackInfo->strPackFileName = pakFileName;
	pZipPackInfo->bFullMap = bFullMap;

	PackAggregate packAggregate;
	ZipAggregate zipAggregate;
	
	//char pszPassword[] = "xuzhao20041108@_@buhuigaole"; //新
	char pszPassword[] = "xuzhao20041108"; //旧
	//PasswordToDesKey( PackAggregate::GetDefaultPassword(),pZipPackInfo->des_key[0],pZipPackInfo->des_key[1],pZipPackInfo->des_key[2] );
	PasswordToDesKey( pszPassword,pZipPackInfo->des_key[0],pZipPackInfo->des_key[1],pZipPackInfo->des_key[2] );
	packAggregate.SetDesKey((const BYTE*)pZipPackInfo->des_key);
	
	
	if( packAggregate.openAggregate(pakFileName))
	{
		pZipPackInfo->packType = stZipPackInfo::eMyPack;
		pZipPackInfo->dwFlags = packAggregate.getFlags();
		pZipPackInfo->dwVersion = packAggregate.getVersion();
		if(pZipPackInfo->dwFlags & PackAggregate::flagEncrypt || pZipPackInfo->dwFlags & PackAggregate::flagEncryptHeaderOnly)
		{
			//Assert(pszPassword);
		}

		PackAggregate::iterator itr;
		for(itr = packAggregate.begin();itr != packAggregate.end();++itr)
		{
			const PackAggregate::stFileBaseInfo & fileInfo = *itr;
			TCHAR szFileName[MAX_PATH];
			sprintf(szFileName,"%s",fileInfo.szName);
			strlwr(szFileName);
			replaceFrontlashPath(szFileName);
			//TRACE("发现资源文件: %s\n",szFileName);
			//std::cout << "发现资源文件:" << szFileName << std::endl;
			stZipFileInfo& ZipFileInfo = pZipPackInfo->mapZipPackInfo[calculateCRC(szFileName,strlen(szFileName))];
			ZipFileInfo.dwFileIndex = itr - packAggregate.begin();
			ZipFileInfo.bGetDetailInfo = false;
			ZipFileInfo.nFileOffset = fileInfo.fileOffset;
			ZipFileInfo.nCompressedFileSize = fileInfo.size;
			ZipFileInfo.dwCompressedCRC = fileInfo.dwCRC;

			ZipFileInfo.name = szFileName;

		}
		packAggregate.closeAggregate();
		FrameAllocator::init(20 * 1024 * 1024);
		stZipPackInfo::tMapZipPackInfo::iterator it = pZipPackInfo->mapZipPackInfo.begin();
		for (;it != pZipPackInfo->mapZipPackInfo.end();++it)
		{
			std::cout << it->second.name << std::endl;

			int pos = it->second.name.rfind("\\");
			if (pos < it->second.name.size())
			{
				std::string s = it->second.name.substr(0, pos+1);
				int len = s.length();
				char tmpDirPath[256] = { 0 };
				for (int i = 0; i < len; i++)
				{
					tmpDirPath[i] = s[i];
					if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
					{
						if (_access(tmpDirPath, 0) == -1)
						{
							_mkdir(tmpDirPath);
						}
					}
				}
			}
			Stream* pStream;
			pStream = OpenZipFileForRead(pZipPackInfo,&(it->second),pZipPackInfo->bFullMap);
			BYTE* pData = (BYTE*)FrameAllocator::alloc(pStream->getSize());
			if(!pData)
				continue;
			pStream->read(pData,pStream->getSize());
			std::ofstream fout;
			fout.open(it->second.name.data(),std::ios::binary);
			if (!fout.is_open())
				std::cout << "打开文件失败" << std::endl;
			fout.write((char*)pData,pStream->getSize());
			fout.close();

		}

	}

	std::cout << std::endl;
	system("pause");
	return 0;
}
