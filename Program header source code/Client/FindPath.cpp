#include "public.h"
#include ".\findpath.h"

CFindPath::CFindPath(void)
{
	LoadPath();
}

bool CFindPath::LoadPath( const char* pPath )
{
	std::string s = pPath;
	int i = 0;
	int iEnd = 0;
	std::string s1 = "";
	std::string s2 = "";
	POINT p1;
	POINT p2;
	PATH_STRUCT ps;

	iEnd = ( int )s.find( ",", i );
	s1 = s.substr( i, iEnd - i );
	i = iEnd + 1;
	iEnd = ( int )s.find( ",", i );
	s2 = s.substr( i, iEnd - i );
	i = iEnd + 1;
	iEnd = ( int )s.find( ",", i );
	p1.x = atoi( s.substr( i, iEnd - i ).c_str() );
	i = iEnd + 1;
	iEnd = ( int )s.find( ",", i );
	p1.y = atoi( s.substr( i, iEnd - i ).c_str() );
	i = iEnd + 1;
	iEnd = ( int )s.find( ",", i );
	p2.x = atoi( s.substr( i, iEnd - i ).c_str() );
	i = iEnd + 1;
	iEnd = ( int )s.find( ",", i );
	p2.y = atoi( s.substr( i, iEnd - i ).c_str() );

	ps.Set( s1.c_str(), s2.c_str(), p1.x, p1.y, p2.x, p2.y );
	m_PathList.push_back( ps );

	return true;
}

bool CFindPath::LoadPath()
{
	m_PathList.clear();

	FILE* p = fopen( "Data/FindPath.txt", "r" );

	if( p == NULL )
	{
		return false;
	}

	std::string s = "";

	while( ! feof( p ) )
	{
		char c[ 999 ] = "";

		fread( c, 998, 1, p );
		s += c;
	}

	int i = 0;
	int iEnd = 0;

	while( 1 )
	{
		iEnd = s.find( 10, i );

		if( iEnd < 0 )
		{
			break;
		}

		LoadPath( s.substr( i, iEnd - i ).c_str() );
		i = iEnd + 1;
	}

	// Load Map
	m_Map.clear();

	std::list< PATH_STRUCT >::iterator it;

	for( it = m_PathList.begin(); it != m_PathList.end(); ++it )
	{
		m_Map.insert( std::map< std::string, bool >::value_type( it->sFrom, false ) );
		m_Map.insert( std::map< std::string, bool >::value_type( it->sTo, false ) );
	}

	return true;
}

bool CFindPath::GetPath( const char* pFrom, const char* pTo, std::list< std::string >& Path )
{
	Path.clear();

	//if( CheckOldPath( pFrom, pTo, Path ) )
	//{
	//	return true;
	//}

	InitMap();

	m_AllPath.clear();

	std::list< std::string > List;

	SetMap( pFrom );
	List.push_back( pFrom );
	m_AllPath.push_back( List );

	while( 1 )
	{
		if( ! GetAllPath( pFrom ) )
		{
			break;
		}
	}

	std::list< std::list< std::string > >::iterator it2;
	std::list< std::string > List4;
	std::list< std::string > List2;
	std::list< std::string > List3;
	std::list< std::string >::iterator it;
	int i = 0;
	int iMin = 999;

	for( it2 = m_AllPath.begin(); it2 != m_AllPath.end(); ++it2 )
	{
		if( FindInList( pTo, *it2, i ) )
		{
			if( iMin > i )
			{
				iMin = i;
				Path = *it2;
			}
		}
	}

	if( ( int )Path.size() > 0 )
	{
		m_OldPath = Path;
		return true;
	}
	else
	{
		return false;
	}
}

bool CFindPath::GetPathPoint( const char* pFrom, const char* pTo, POINT& p )
{
	std::list< PATH_STRUCT >::iterator it;

	for( it = m_PathList.begin(); it != m_PathList.end(); ++it )
	{
		if( it->sFrom.compare( pFrom ) == 0 && it->sTo.compare( pTo ) == 0 )
		{
			p = it->From;
			return true;
		}

		if( it->sFrom.compare( pTo ) == 0 && it->sTo.compare( pFrom ) == 0 )
		{
			p = it->To;
			return true;
		}
	}

	return false;
}

bool CFindPath::InitMap()
{
	std::map< std::string, bool >::iterator it;

	for( it = m_Map.begin(); it != m_Map.end(); ++it )
	{
		it->second = false;
	}

	return true;
}

bool CFindPath::CheckMap( const char* pMap )
{
	std::map< std::string, bool >::iterator it = m_Map.find( pMap );

	if( it != m_Map.end() )
	{
		return it->second;
	}

	return true;
}

bool CFindPath::SetMap( const char* pMap )
{
	std::map< std::string, bool >::iterator it = m_Map.find( pMap );

	if( it != m_Map.end() )
	{
		it->second = true;
		return true;
	}

	return false;
}

bool CFindPath::CheckOldPath( const char* pFrom, const char* pTo, std::list< std::string >& Path )
{
	m_OldPath = Path;

	return true;
}

bool CFindPath::CheckShortPath( std::list< std::string >& PathList )
{
	return true;
}

bool CFindPath::GetAllMap( const char* pFrom, std::list< std::string >& Path )
{
	Path.clear();

	std::list< PATH_STRUCT >::iterator it;

	for( it = m_PathList.begin(); it != m_PathList.end(); ++it )
	{
		if( it->sFrom.compare( pFrom ) == 0 )
		{
			if( ! CheckMap( it->sTo.c_str() ) )
			{
				SetMap( it->sTo.c_str() );
				Path.push_back( it->sTo );
			}
		}

		if( it->sTo.compare( pFrom ) == 0 )
		{
			if( ! CheckMap( it->sFrom.c_str() ) )
			{
				SetMap( it->sFrom.c_str() );
				Path.push_back( it->sFrom );
			}
		}
	}

	return true;
}

bool CFindPath::GetAllPath( const char* pFrom )
{
	std::string s = pFrom;
	std::list< std::list< std::string > >::iterator it2;
	std::list< std::string > List4;
	std::list< std::string > List2;
	std::list< std::string > List3;
	std::list< std::string >::iterator it;

	for( it2 = m_AllPath.begin(); it2 != m_AllPath.end(); ++it2 )
	{
		List2 = *it2;
		s = List2.back();

		GetAllMap( s.c_str(), List4 );

		for( it = List4.begin(); it != List4.end(); ++it )
		{
			List3 = List2;
			List3.push_back( *it );
			m_AllPath.push_back( List3 );
		}
		
		if( ( int )List4.size() > 0 )
		{
			m_AllPath.erase( it2 );
			return true;
		}
	}
	
	return false;
}

bool CFindPath::FindInList( const char* pMap, std::list< std::string >& List, int& iIndex )
{
	iIndex = 0;

	std::list< std::string >::iterator it;

	for( it = List.begin(); it != List.end(); ++it )
	{
		if( it->compare( pMap ) == 0 )
		{
			return true;
		}

		++iIndex;
	}

	return false;
}

CFindPath::~CFindPath(void)
{
}
