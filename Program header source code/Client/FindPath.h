#pragma once

#include <string>
#include <list>
#include <map>

struct PATH_STRUCT
{
	std::string sFrom;
	std::string sTo;
	POINT From;
	POINT To;

	bool Set( const char* pFrom, const char* pTo, int xFrom, int yFrom, int xTo, int yTo )
	{
		sFrom = pFrom;
		sTo = pTo;
		From.x = xFrom;
		From.y = yFrom;
		To.x = xTo;
		To.y = yTo;

		return true;
	}
};

class CFindPath
{
public:
	CFindPath(void);
	~CFindPath(void);

	bool GetPath( const char* pFrom, const char* pTo, std::list< std::string >& Path );
	bool GetPathPoint( const char* pFrom, const char* pTo, POINT& p );
private:
	std::list< PATH_STRUCT > m_PathList;
	std::map< std::string, bool > m_Map;

	// Temp
	std::list< std::list< std::string > > m_AllPath;
	std::list< std::string > m_OldPath;

	bool LoadPath( const char* pPath );
	bool LoadPath();
	bool InitMap();
	bool CheckMap( const char* pMap );
	bool SetMap( const char* pMap );

	bool CheckOldPath( const char* pFrom, const char* pTo, std::list< std::string >& Path );
	bool CheckShortPath( std::list< std::string >& PathList );

	bool GetAllMap( const char* pFrom, std::list< std::string >& Path );
	bool GetAllPath( const char* pFrom );

	bool FindInList( const char* pMap, std::list< std::string >& List, int& iIndex );
};

//bool SetPath( PATH_STRUCT& ps, const char* pFrom, const char* pTo, int x, int y );