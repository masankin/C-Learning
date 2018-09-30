#ifndef __RSA_CRYPT_H
#define __RSA_CRYPT_H

#include <openssl/rsa.h>

#include "Singleton.h"
//
//int RSA_public_encrypt(int flen, const unsigned char *from,
//	unsigned char *to, RSA *rsa, int padding);
//int RSA_private_encrypt(int flen, const unsigned char *from,
//	unsigned char *to, RSA *rsa, int padding);
//int RSA_public_decrypt(int flen, const unsigned char *from,
//	unsigned char *to, RSA *rsa, int padding);
//int RSA_private_decrypt(int flen, const unsigned char *from,
//	unsigned char *to, RSA *rsa, int padding);
class RSACrypt
{
public:
	bool initPublicKey(unsigned char* szKey,int keyLneght);
	bool initPrivateKey(unsigned char* szKey, int keyLength);
	int getEncryptSize(bool bPublic = true);
	int encrypt(int flen, const unsigned char *from, unsigned char *to, bool bPublic = true);
	int decrypt(int flen, const unsigned char *from, unsigned char *to, bool bPublic = false);

	RSACrypt();
	~RSACrypt();
private:

	RSA* m_rsaPrivate;
	RSA* m_rsaPublic;
};

#define sgRSACryptManager() Singleton<RSACrypt>::Get()

#endif
