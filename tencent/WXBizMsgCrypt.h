
#pragma once

#include <string>
#include <stdint.h>
#include "tinyxml2.h" 
namespace EncryptAndDecrypt {

static const unsigned int kAesKeySize = 32;
static const unsigned int kAesIVSize = 16;
static const unsigned int kEncodingKeySize = 43;
static const unsigned int kRandEncryptStrLen = 16;
static const unsigned int kMsgLen = 4;
static const unsigned int kMaxBase64Size = 1000000000;
enum  WXBizMsgCryptErrorCode
{
    WXBizMsgCrypt_OK = 0,
    WXBizMsgCrypt_ValidateSignature_Error = -40001,
    WXBizMsgCrypt_ParseXml_Error = -40002,
    WXBizMsgCrypt_ComputeSignature_Error = -40003,
    WXBizMsgCrypt_IllegalAesKey = -40004,
    WXBizMsgCrypt_ValidateAppid_Error = -40005,
    WXBizMsgCrypt_EncryptAES_Error = -40006,
    WXBizMsgCrypt_DecryptAES_Error = -40007,
    WXBizMsgCrypt_IllegalBuffer = -40008,
    WXBizMsgCrypt_EncodeBase64_Error = -40009,
    WXBizMsgCrypt_DecodeBase64_Error = -40010,
    WXBizMsgCrypt_GenReturnXml_Error = -40011,
};

class WXBizMsgCrypt
{
public:
    //���캯��
    // @param sToken: ����ƽ̨�ϣ����������õ�Token
    // @param sEncodingAESKey: ����ƽ̨�ϣ����������õ�EncodingAESKey
    // @param sAppid: ���ںŵ�appid
    WXBizMsgCrypt(const std::string &sToken, 
                    const std::string &sEncodingAESKey, 
                    const std::string &sAppid)
                    :m_sToken(sToken), m_sEncodingAESKey(sEncodingAESKey),m_sAppid(sAppid)
                    {   }
    
    
    
    // ������Ϣ����ʵ�ԣ����һ�ȡ���ܺ������
    // @param sMsgSignature: ǩ��������ӦURL������msg_signature
    // @param sTimeStamp: ʱ�������ӦURL������timestamp
    // @param sNonce: ���������ӦURL������nonce
    // @param sPostData: ���ģ���ӦPOST���������
    // @param sMsg: ���ܺ��ԭ�ģ���return����0ʱ��Ч
    // @return: �ɹ�0��ʧ�ܷ��ض�Ӧ�Ĵ�����
    int DecryptMsg(const std::string &sMsgSignature,
                    const std::string &sTimeStamp,
                    const std::string &sNonce,
                    const std::string &sPostData,
                    std::string &sMsg);
            
            
    //�����ںŻظ��û�����Ϣ���ܴ��
    // @param sReplyMsg:���ںŴ��ظ��û�����Ϣ��xml��ʽ���ַ���
    // @param sTimeStamp: ʱ����������Լ����ɣ�Ҳ������URL������timestamp
    // @param sNonce: ������������Լ����ɣ�Ҳ������URL������nonce
    // @param sEncryptMsg: ���ܺ�Ŀ���ֱ�ӻظ��û������ģ�����msg_signature, timestamp, nonce, encrypt��xml��ʽ���ַ���,
    //                      ��return����0ʱ��Ч
    // return���ɹ�0��ʧ�ܷ��ض�Ӧ�Ĵ�����
    int EncryptMsg(const std::string &sReplyMsg,
                    const std::string &sTimeStamp,
                    const std::string &sNonce,
                    std::string &sEncryptMsg);
private:
    std::string m_sToken;
    std::string m_sEncodingAESKey;
    std::string m_sAppid;

private:
    // AES CBC
    int AES_CBCEncrypt( const char * sSource, const uint32_t iSize,
            const char * sKey, unsigned int iKeySize, std::string * poResult );
    
    int AES_CBCEncrypt( const std::string & objSource,
            const std::string & objKey, std::string * poResult );
    
    int AES_CBCDecrypt( const char * sSource, const uint32_t iSize,
            const char * sKey, uint32_t iKeySize, std::string * poResult );
    
    int AES_CBCDecrypt( const std::string & objSource,
            const std::string & objKey, std::string * poResult );
    
    //base64
    int EncodeBase64(const std::string sSrc, std::string & sTarget);
    
    int DecodeBase64(const std::string sSrc, std::string & sTarget);
    
    //genkey
    int GenAesKeyFromEncodingKey( const std::string & sEncodingKey, std::string & sAesKey);
    
    //signature
    int ComputeSignature(const std::string sToken, const std::string sTimeStamp, const std::string & sNonce,
        const std::string & sMessage, std::string & sSignature);
    
    int ValidateSignature(const std::string &sMsgSignature, const std::string &sTimeStamp, 
        const std::string &sNonce, const std::string & sEncryptMsg);  
 
    //get , set data
    void GenRandStr(std::string & sRandStr, uint32_t len);

    void GenNeedEncryptData(const std::string &sReplyMsg,std::string & sNeedEncrypt );

    int GetXmlField(const std::string & sPostData, const std::string & sField,std::string &sEncryptMsg);

    int SetOneFieldToXml(tinyxml2::XMLDocument * pDoc, tinyxml2::XMLNode* pXmlNode, const char * pcFieldName, 
        const std::string & value, bool bIsCdata);

    int GenReturnXml(const std::string & sEncryptMsg, const std::string & sSignature, const std::string & sTimeStamp, 
        const std::string & sNonce, std::string & sResult);


};

}

