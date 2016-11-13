#ifndef __WING_BASE64_H__
#define __WING_BASE64_H__
/* ---------------------------------------------------------- 
文件名称：wing_base64.h 
 
作者：秦建辉 
 
MSN：splashcn@msn.com 
 
当前版本：V1.1 
 
历史版本： 
    V1.1    2010年05月11日 
            修正BASE64解码的Bug。 
 
    V1.0    2010年05月07日 
            完成正式版本。 
 
功能描述： 
    BASE64编码和解码 
 
接口函数： 
    Base64_Encode 
    Base64_Decode 
 
说明： 
    1.  参考openssl-1.0.0。 
    2.  改进接口，以使其适应TCHAR字符串。 
    3.  修正EVP_DecodeBlock函数解码时未去掉填充字节的缺陷。 
 ------------------------------------------------------------ */  
#pragma once  
  
#include <windows.h>  
  
#ifdef  __cplusplus  
extern "C" {  
#endif  
  
/* 
功能：将二进制数据转换成BASE64编码字符串 
参数说明： 
    inputBuffer：要编码的二进制数据 
    inputCount：数据长度 
    outputBuffer：存储转换后的BASE64编码字符串 
返回值： 
     -1：参数错误 
    >=0：有效编码长度（字符数）,不包括字符串结束符。 
备注： 
    等效于openssl中EVP_EncodeBlock函数 
*/  
INT BASE64_Encode( const BYTE* inputBuffer, INT inputCount, TCHAR* outputBuffer );  
  
/* 
功能：将BASE64编码字符串转换为二进制数据 
参数说明： 
    inputBuffer：BASE64编码字符串 
    inputCount：编码长度（字符数）,应该为4的倍数。 
    outputBuffer：存储转换后的二进制数据 
返回值： 
     -1：参数错误 
     -2：数据错误 
    >=0：转换后的字节数 
备注： 
    等效于openssl中EVP_DecodeBlock函数 
*/  
INT BASE64_Decode( const TCHAR* inputBuffer, INT inputCount, BYTE* outputBuffer );  
  
#ifdef  __cplusplus  
}  
#endif  
#endif