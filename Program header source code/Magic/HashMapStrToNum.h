#ifdef MAGIC_EDITOR

#pragma once

#include <hash_map>
#include <string>

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param key : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline unsigned int HashStrToNum(const char* key)
{
	unsigned int nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class hash_compare_str
{
public:
	enum
	{	// parameters for hash table
		bucket_size = 4,	// 0 < bucket_size
		min_buckets = 8		// min_buckets = 2 ^^ N, 0 < N
	};	

	size_t operator()(const std::string& _Keyval) const
	{	// hash _Keyval to size_t value
		return ((size_t)HashStrToNum(_Keyval.c_str()));
	}
	size_t operator()(const char* _Keyval) const
	{	// hash _Keyval to size_t value
		return ((size_t)HashStrToNum(_Keyval));
	}

	//	size_t operator()(const _Kty& _Keyval) const
	//		{	// hash _Keyval to size_t value by pseudorandomizing transform
	//		ldiv_t _Qrem = ldiv((size_t)_Keyval, 127773);
	//		_Qrem.rem = 16807 * _Qrem.rem - 2836 * _Qrem.quot;
	//		if (_Qrem.rem < 0)
	//			_Qrem.rem += 2147483647;
	//		return ((size_t)_Qrem.rem); }

	BOOL operator()(const std::string & _Keyval1, const std::string & _Keyval2) const
	{	// test if _Keyval1 ordered before _Keyval2
		return (_Keyval1 < _Keyval2);
	}
	BOOL operator()(const char* _Keyval1, const char* _Keyval2) const
	{	// test if _Keyval1 ordered before _Keyval2
		return (strcmp(_Keyval1 , _Keyval2) < 0);
	}
};

//typedef std::hash_map<std::string,UINT,hash_compare_str> HashMapStrToNum;
typedef stdext::hash_map<std::string,UINT> HashMapStrToNum;

#endif