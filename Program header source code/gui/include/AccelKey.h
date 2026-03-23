#pragma once

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CAccelKey
{
	std::vector<BYTE> m_aKey;
	BYTE m_group;
public:

	CAccelKey()
	{}
	CAccelKey(const char* pszAccelKey);
	bool SetAccelKey(const char* pszAccelKey);
	bool IsKeyDown();
	bool IsAltDown();
	bool IsNumKey();
	std::string GetAccelKey();
	CAccelKey & operator = (const CAccelKey & v)
	{
		m_aKey.resize(v.m_aKey.size());
		memcpy( &m_aKey[0], &v.m_aKey[0], m_aKey.size() * sizeof(BYTE) );
		m_group = v.m_group;
		return *this;
	}
	void InsertKey(BYTE key){
		m_aKey.push_back(key);
	}
	void Clear(){ m_aKey.resize(0);}
	size_t GetKey(BYTE* pKey,size_t bufSize);

	bool IsKey(BYTE* pKey,size_t bufSize);
	bool IsSameGroup(BYTE group);
	void SetGroup(BYTE group) { m_group = group; }
	BYTE GetGroup() { return m_group; }
};
