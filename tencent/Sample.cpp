#include <iostream>
#include <string>
#include "WXBizMsgCrypt.h"


using namespace EncryptAndDecrypt;
using namespace std;
using namespace tinyxml2;

/*
*read me
*��ʾ������Ӧ���˿�Դ��openssl��tinyxml2�⣬�������32λ��64λ�ľ�̬������,
*ʾ��Makefile������������64λ�汾�ģ������Ի���Ϊ32λ�����Makefile������
*windows�����£���Ҫ�Լ���װ��

*openssl�İ汾����openssl-1.0.1h��http://www.openssl.org/
*tinyxml2�İ汾����tinyxml2-2.1.0,    https://github.com/leethomason/tinyxml2
*/




int main()
{
    cout<<"start to test"<<endl<<endl<<endl;
    string sEncodingAesKey = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFG";
    string sToken = "spamtest";
    string sAppid = "wx2c2769f8efd9abc2";    
    WXBizMsgCrypt oWXBizMsgCrypt(sToken,sEncodingAesKey,sAppid);
    
    //decrypt
    string sMsgSignature = "003fee52ecc56afb46c00b5c7721be87860ce785";
    string sTimestamp = "1410349438";
    string sNonce = "298025754";
    string sEncryptBase64 = "mfBCs65c67CeJw22u4VT2TD73q5H06+ocrAIxswCaeZ/d/Lw"
                            "0msSZFHY0teqgSYiI1zR2gD2DKrB3TIrmX/liNSDrGqS8jSI/"
                            "WPeKB5VPr7Ezr7gomZAyGCwJSgT1TRFWPfONGJMxuj2nk4faTu"
                            "spAuVIFQ6SHwZuJBZC7mcJp7Cgr9cUhATQWDbOPaE7ukZBTV2Yq"
                            "yzH+UI2AK+J1S47cE79k1RX8t0hcTz/O0hlK8DGXKnvYv88qKQcI"
                            "7z4iaajqHfRVZKBNyOODabs+It+ZfM3dWTeFcPgDbGtIEnpt/EDtu"
                            "uA/zMvtkaKdHdswPnVZQ+xdwbYr3ldGvfT8HlEYEgkgKaThxTFobVl"
                            "wzu2ZkXCjicbP3xdr15Iq48ObgzPpqYuZ3IEoyggZDKClquk0u0orMck4GTF/XyE8yGzc4=";
    
    cout<<"sEncryptBase64 size:"<<sEncryptBase64.size()<<endl;
    string sPostData= "<xml><ToUserName><![CDATA[toUser]]></ToUserName><Encrypt><![CDATA[" 
               + sEncryptBase64
               + "]]></Encrypt></xml>";
    string sMsg;
    int ret = oWXBizMsgCrypt.DecryptMsg(sMsgSignature,sTimestamp,sNonce,sPostData,sMsg);
    cout<<"DecryptMsg ret:"<<ret<<" size:"<<sMsg.size()<<" msg:"<<endl;
    cout<<sMsg<<endl<<endl<<endl;


    //encrypt
    string sToXml = "<xml><ToUserName><![CDATA[oia2TjjewbmiOUlr6X-1crbLOvLw]]></ToUserName>"
                    "<FromUserName><![CDATA[gh_7f083739789a]]></FromUserName>"
                    "<CreateTime>1407743423</CreateTime>"
                    "<MsgType><![CDATA[video]]></MsgType>"
                    "<Video>"
                    "<MediaId><![CDATA[eYJ1MbwPRJtOvIEabaxHs7TX2D-HV71s79GUxqdUkjm6Gs2Ed1KF3ulAOA9H1xG0]]></MediaId>"
                    "<Title><![CDATA[testCallBackReplyVideo]]></Title>"
                    "<Description><![CDATA[testCallBackReplyVideo]]></Description>"
                    "</Video></xml>";

    string sEncryptMsg;
    ret = oWXBizMsgCrypt.EncryptMsg(sToXml,sTimestamp,sNonce,sEncryptMsg);
    cout<<"EncryptMsg ret:"<<ret<<" size:"<<sEncryptMsg.size()<<" msg:"<<endl;
    cout<<sEncryptMsg<<endl;

    return 0;
}
