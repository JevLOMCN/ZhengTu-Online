#ifndef _ENCDEC_H
#define _ENCDEC_H

#include "./myrc5.h"
#include "./mydes.h"
#include "./mycast.h"
#include "./myidea.h"
#include "./md5ex.h"
#include "./RLEEncDec.h"

extern void DES_set_key(const_DES_cblock *key, DES_key_schedule *schedule);
extern void DES_encrypt1(DES_LONG *data, DES_key_schedule *ks,t_DES_SPtrans * sp, int enc);
extern void DES_encrypt3(DES_LONG *data, DES_key_schedule *ks1,
			DES_key_schedule *ks2, DES_key_schedule *ks3,t_DES_SPtrans * sp);
extern void DES_decrypt3(DES_LONG *data, DES_key_schedule *ks1,
			DES_key_schedule *ks2, DES_key_schedule *ks3,t_DES_SPtrans * sp);

extern void RC5_32_set_key(RC5_32_KEY *key, int len, const unsigned char *data,
	int rounds);
extern void RC5_32_encrypt(unsigned long *d, RC5_32_KEY *key);
extern void RC5_32_decrypt(unsigned long *d, RC5_32_KEY *key);

extern void idea_set_encrypt_key(const unsigned char *key, IDEA_KEY_SCHEDULE *ks);
extern void idea_set_decrypt_key(IDEA_KEY_SCHEDULE *ek, IDEA_KEY_SCHEDULE *dk);
extern void idea_encrypt(unsigned long *d, IDEA_KEY_SCHEDULE *key);

extern void CAST_set_key(CAST_KEY *key, int len, const unsigned char *data);
extern void CAST_encrypt(CAST_LONG *data, CAST_KEY *key);
extern void CAST_decrypt(CAST_LONG *data, CAST_KEY *key);

// 加密解密接口。之所以定义一个接口是为了把加密解密换成虚函数调用，增加破解者跟踪难度
class IEncrypt{
public:
	//--------------------------------------
	// DES: 密钥长度 8 Byte, 加密数据单位 8 Byte
	//--------------------------------------
	virtual void DES_set_key(const unsigned char *key, DES_key_schedule *schedule) = 0;
	virtual void DES_encrypt1(unsigned char *data, DES_key_schedule *ks, int enc) = 0;
	virtual void DES_encrypt3(unsigned char *data, DES_key_schedule *ks1,
				  DES_key_schedule *ks2, DES_key_schedule *ks3) = 0;
	virtual void DES_decrypt3(unsigned char *data, DES_key_schedule *ks1,
				  DES_key_schedule *ks2, DES_key_schedule *ks3) = 0;

	//--------------------------------------
	// RC5: 密钥长度 16 Byte, 加密数据单位 8 Byte
	//--------------------------------------
	virtual void RC5_32_set_key(RC5_32_KEY *key,const unsigned char *data) = 0;
	virtual void RC5_32_encrypt(unsigned char *d, RC5_32_KEY *key) = 0;
	virtual void RC5_32_decrypt(unsigned char *d, RC5_32_KEY *key) = 0;

	//--------------------------------------
	// idea: 
	//--------------------------------------
	virtual void idea_set_encrypt_key(const unsigned char *key, IDEA_KEY_SCHEDULE *ks) = 0;
	virtual void idea_set_decrypt_key(IDEA_KEY_SCHEDULE *ek, IDEA_KEY_SCHEDULE *dk) = 0;
	virtual void idea_encrypt(unsigned long *d, IDEA_KEY_SCHEDULE *key) = 0;

	//--------------------------------------
	// CAST: 密钥长度 8 Byte, 加密数据单位 8 Byte
	//--------------------------------------
	virtual void CAST_set_key(CAST_KEY *key, const unsigned char *data) = 0;
	virtual void CAST_encrypt(unsigned char *data, CAST_KEY *key)=0;
	virtual void CAST_decrypt(unsigned char *data, CAST_KEY *key)= 0;
};

class CEncrypt : public IEncrypt{

	f_DES_set_key m_fDES_set_key;
	f_DES_encrypt1 m_fDES_encrypt1;
	f_DES_encrypt3 m_fDES_encrypt3;
	f_DES_decrypt3 m_fDES_decrypt3;

	f_RC5_32_decrypt m_fRC5_32_decrypt;
	f_RC5_32_encrypt m_fRC5_32_encrypt;
	f_RC5_32_set_key m_fRC5_32_set_key;

	f_idea_set_encrypt_key m_fidea_set_encrypt_key;
	f_idea_set_decrypt_key m_fidea_set_decrypt_key;
	f_idea_encrypt			m_fidea_encrypt;

	f_CAST_set_key			m_fCAST_set_key;
	f_CAST_encrypt			m_fCAST_encrypt;
	f_CAST_decrypt			m_fCAST_decrypt;

	void DES_set_key(const unsigned char *key, DES_key_schedule *schedule);
	void DES_encrypt1(unsigned char *data, DES_key_schedule *ks, int enc);
	void DES_encrypt3(unsigned char *data, DES_key_schedule *ks1,
		DES_key_schedule *ks2, DES_key_schedule *ks3);
	void DES_decrypt3(unsigned char *data, DES_key_schedule *ks1,
		DES_key_schedule *ks2, DES_key_schedule *ks3);

	void RC5_32_set_key(RC5_32_KEY *key, const unsigned char *data);
	void RC5_32_encrypt(unsigned char *d, RC5_32_KEY *key);
	void RC5_32_decrypt(unsigned char *d, RC5_32_KEY *key);

	void idea_set_encrypt_key(const unsigned char *key, IDEA_KEY_SCHEDULE *ks);
	void idea_set_decrypt_key(IDEA_KEY_SCHEDULE *ek, IDEA_KEY_SCHEDULE *dk);
	void idea_encrypt(unsigned long *d, IDEA_KEY_SCHEDULE *key);

	void CAST_set_key(CAST_KEY *key, const unsigned char *data);
	void CAST_encrypt(unsigned char *data, CAST_KEY *key);
	void CAST_decrypt(unsigned char *data, CAST_KEY *key);
public:
	CEncrypt();

};
#endif