#include "RSACrypt.h"

#include <openssl/pem.h>
#include "cocos2d.h"


USING_NS_CC;

RSACrypt::RSACrypt():m_rsaPublic(nullptr), m_rsaPrivate(nullptr) {}

RSACrypt::~RSACrypt()
{
	if (m_rsaPrivate) delete m_rsaPrivate;
	m_rsaPrivate = nullptr;
	if (m_rsaPublic) delete m_rsaPublic;
	m_rsaPublic = nullptr;
}

bool RSACrypt::initPublicKey(unsigned char* szKey, int keyLneght)
{
	BIO *bio = NULL;
	unsigned char *chPublicKey = const_cast<unsigned char *>(szKey);
	if ((bio = BIO_new_mem_buf(chPublicKey, -1)) == NULL)       //从字符串读取RSA公钥
	{
		CCLOG("[trace]RSACrypt BIO new mem buf false");
		return false;
	}
	// 从文件中读取公钥
	//rsa3 = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
	m_rsaPublic = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
	if (m_rsaPublic == NULL){
		CCLOG("[trace]RSACrypt unable to read public key!\n");
		return false;
	}
	return true;
}


bool RSACrypt::initPrivateKey(unsigned char* szKey, int keyLength)
{
	BIO *bio = NULL;
	unsigned char *szPrivateKey = const_cast<unsigned char *>(szKey);
	if ((bio = BIO_new_mem_buf(szPrivateKey, -1)) == NULL)       //从字符串读取RSA公钥
	{
		CCLOG("[trace]RSACrypt BIO new mem buf false");
		return false;
	}
	// 从文件中读取公钥
	//rsa3 = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
	m_rsaPrivate = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);
	if (m_rsaPublic == NULL){
		CCLOG("[trace]RSACrypt unable to read private key!\n");
		return false;
	}
	return true;
}

int RSACrypt::getEncryptSize(bool bPublic)
{
	RSA* rsa = bPublic ? m_rsaPublic : m_rsaPrivate;
	if (rsa == nullptr)
	{
		CCLOG("[trace]RSACrypt failed to init rsa\n");
		return -1;
	}
	return RSA_size(rsa);
}

int RSACrypt::encrypt(int flen, const unsigned char *plain, unsigned char *encrypted, bool bPublic)
{
	RSA* rsa = bPublic ? m_rsaPublic:m_rsaPrivate;
	if (rsa == nullptr)
	{
		CCLOG("[trace]RSACrypt failed to init rsa\n");
		return -1;
	}
	int len = RSA_public_encrypt(flen, (unsigned char*)plain, (unsigned char*)encrypted, rsa, RSA_PKCS1_OAEP_PADDING);
	if (len == -1){
		CCLOG("[trace]RSACrypt failed to encrypt\n");
		return -1;
	}
	return len;
}

int RSACrypt::decrypt(int flen, const unsigned char *encrypt, unsigned char *decrypted, bool bPublic)
{
	RSA* rsa = bPublic ? m_rsaPublic : m_rsaPrivate;
	if (rsa == nullptr)
	{
		CCLOG("[trace]RSACrypt failed to init rsa\n");
		return -1;
	}
	int len = RSA_private_decrypt(flen, (unsigned char*)encrypt, (unsigned char*)decrypted, rsa, RSA_PKCS1_OAEP_PADDING);
	if (len == -1){
		CCLOG("[trace]RSACrypt failed to encrypt\n");
		return -1;
	}
	return len;
}

