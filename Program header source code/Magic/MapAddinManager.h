#ifndef _MAPADDINMANAGER_H
#define _MAPADDINMANAGER_H

template < class _Ty>
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class tMapAddinManager{

protected:
	typedef std::map<DWORD, _Ty* > tMapObject;
	tMapObject m_mapObject;

protected:
	DWORD GetKey(POINT pt)
	{
		return pt.y << 16 | pt.x;
	}

	POINT GetKeyPos(DWORD key)
	{
		return stPointI( key & 0xffff,(key & 0xffff0000) >> 16);
	}
public:

	void Release()
	{
		for( tMapObject::iterator it = m_mapObject.begin();
			it != m_mapObject.end(); ++it )
		{
			delete it->second;
		}
		m_mapObject.clear();
	}

	virtual _Ty* CreateObject(POINT pt,void* pData) { return NULL;}
	virtual void SetObjectData(_Ty* pObject,void* pData) {}

	void AddObject(POINT pt,void * pData)
	{
		DWORD key = GetKey(pt);
		tMapObject::iterator it = m_mapObject.find(key);

		if(it == m_mapObject.end())
		{
			_Ty* p = CreateObject(pt,pData);
			if(p) m_mapObject[key] = p;
		}
		else
		{
			SetObjectData(it->second,pData);
			//it->second->SetData(*pData);
		}
	}

	void DeleteObject(POINT pt)
	{
		DWORD key = GetKey(pt);
		tMapObject::iterator it = m_mapObject.find(key);
		//Assert(it != m_mapObject.end());
		if(it != m_mapObject.end())
		{
			delete it->second;
			m_mapObject.erase(it);
		}
	}

	void DeleteObject(const RECT * exception)
	{
		for(tMapObject::iterator it = m_mapObject.begin();
			it != m_mapObject.end(); )
		{
			POINT pt = GetKeyPos(it->first);
			if( !PtInRect(exception,pt) )
			{
				delete it->second;
				it = m_mapObject.erase(it);
			}
			else ++it;
		}
	}

	virtual void Run( float time_elasped )
	{
		for( tMapObject::iterator it = m_mapObject.begin();
			it != m_mapObject.end(); ++it )
		{
			it->second->Run(time_elasped);
		}
	}
};
#endif