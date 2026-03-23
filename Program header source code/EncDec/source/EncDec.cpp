#include "../../engine/include/engine.h"
#include "../include/Encdec.h"

CEncrypt::CEncrypt()
{
	m_fDES_set_key  = ::DES_set_key  ;
	m_fDES_encrypt1 = ::DES_encrypt1 ;
	m_fDES_encrypt3 = ::DES_encrypt3 ;
	m_fDES_decrypt3 = ::DES_decrypt3 ;

	m_fRC5_32_decrypt = ::RC5_32_decrypt ;
	m_fRC5_32_encrypt = ::RC5_32_encrypt ;
	m_fRC5_32_set_key = ::RC5_32_set_key ;

	m_fidea_set_encrypt_key = ::idea_set_encrypt_key  ;
	m_fidea_set_decrypt_key = ::idea_set_decrypt_key  ;
	m_fidea_encrypt = ::idea_encrypt;

	m_fCAST_set_key = ::CAST_set_key;
	m_fCAST_encrypt = ::CAST_encrypt;
	m_fCAST_decrypt = ::CAST_decrypt;
}

void CEncrypt::DES_set_key(const unsigned char *key, DES_key_schedule *schedule)
{
	m_fDES_set_key((const_DES_cblock*)key,schedule);
}

void CEncrypt::DES_encrypt1(unsigned char *data, DES_key_schedule *ks, int enc)
{
	m_fDES_encrypt1((DES_LONG*)data,ks,&MyDES_SPtrans,enc);
}

void CEncrypt::DES_encrypt3(unsigned char *data, DES_key_schedule *ks1,
			DES_key_schedule *ks2, DES_key_schedule *ks3)
{
	m_fDES_encrypt3((DES_LONG*)data,ks1,ks2,ks3,&MyDES_SPtrans);
}

void CEncrypt::DES_decrypt3(unsigned char *data, DES_key_schedule *ks1,
			DES_key_schedule *ks2, DES_key_schedule *ks3)
{
	m_fDES_decrypt3((DES_LONG*)data,ks1,ks2,ks3,&MyDES_SPtrans);
}

void CEncrypt::RC5_32_set_key(RC5_32_KEY *key, const unsigned char *data)
{
	m_fRC5_32_set_key(key,16,data,12);
}

void CEncrypt::RC5_32_encrypt(unsigned char *d, RC5_32_KEY *key)
{
	m_fRC5_32_encrypt((unsigned long*)d,key);
}

void CEncrypt::RC5_32_decrypt(unsigned char *d, RC5_32_KEY *key)
{
	m_fRC5_32_decrypt((unsigned long*)d,key);
}

void CEncrypt::idea_set_encrypt_key(const unsigned char *key, IDEA_KEY_SCHEDULE *ks)
{
	m_fidea_set_encrypt_key(key,ks);
}

void CEncrypt::idea_set_decrypt_key(IDEA_KEY_SCHEDULE *ek, IDEA_KEY_SCHEDULE *dk)
{
	m_fidea_set_decrypt_key(ek,dk);
}

void CEncrypt::idea_encrypt(unsigned long *d, IDEA_KEY_SCHEDULE *key)
{
	m_fidea_encrypt(d,key);
}

void CEncrypt::CAST_set_key(CAST_KEY *key, const unsigned char *data)
{
	m_fCAST_set_key(key,8,data);
}

void CEncrypt::CAST_encrypt(unsigned char *data, CAST_KEY *key)
{
	m_fCAST_encrypt((CAST_LONG*)data,key);
}

void CEncrypt::CAST_decrypt(unsigned char *data, CAST_KEY *key)
{
	m_fCAST_decrypt((CAST_LONG*)data,key);
}

