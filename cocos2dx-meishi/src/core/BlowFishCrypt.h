#ifndef __BLOWFISH_CRYPT_H
#define __BLOWFISH_CRYPT_H


#include <openssl/blowfish.h>

#include "Singleton.h"

class Blowfish
{
public:
	bool initKey(unsigned char* szKey, int size);

	int getEncryptSize(int len);
	int encrypt(int flen, const unsigned char *from, unsigned char *to);
	int decrypt(int flen, const unsigned char *from, unsigned char *to);

	Blowfish();
	~Blowfish();
private:

	void initIV(unsigned char* iv, int size);
	BF_KEY m_key;

	unsigned char m_initIV[BF_BLOCK];
};

#define sgBlowfishManager() Singleton<Blowfish>::Get()

#endif
