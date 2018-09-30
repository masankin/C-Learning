#include "BlowFishCrypt.h"
#include "cocos2d.h"

USING_NS_CC;


Blowfish::Blowfish() {

	initIV((unsigned char*)"12345678", BF_BLOCK);
}

Blowfish::~Blowfish(){
}



void Blowfish::initIV(unsigned char* iv, int size)
{
	if (size < 0 || iv == nullptr )
	{
		CCLOGERROR("[trace]Blowfish init initTV error in Blowfish\n");
		return;
	}	
	for (int i = 0; i < BF_BLOCK; ++i) {
		m_initIV[i] = iv[i];
	}
	return;
}


bool Blowfish::initKey(unsigned char* szKey, int size)
{
	if (size < 0 || szKey == nullptr)
	{
		CCLOGERROR("[trace]Blowfish init parametere error in Blowfish\n");
		return false;
	}
	BF_set_key(&m_key, size, szKey);

	return true;
}

int Blowfish::getEncryptSize(int len)
{
	return ((len - 1) / 8 + 1) * 8;
}



int Blowfish::encrypt(int flen, const unsigned char *plain, unsigned char * encrypt)
{
	unsigned char iv[BF_BLOCK];
	memcpy(iv, m_initIV, BF_BLOCK);
	BF_cbc_encrypt(plain, encrypt, flen, &m_key, iv, BF_ENCRYPT);
	return 0;
}

int Blowfish::decrypt(int flen,const unsigned char *encrypt, unsigned char *decrypted)
{
	unsigned char iv[BF_BLOCK];
	memcpy(iv, m_initIV, BF_BLOCK);
	BF_cbc_encrypt(encrypt, decrypted, flen, &m_key, iv, BF_DECRYPT);
	return 0;
}


