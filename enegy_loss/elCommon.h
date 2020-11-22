#ifndef _EL_COMMON_H__
#define _EL_COMMON_H__


#include "libxml/tree.h"

typedef int BOOL;

#define TRUE    1
#define FALSE   0


/**
 *  @brief      : 数字字符串转换为十进制数
 *  @param[in]  : pStr，字符串
 *  @param[in]  : nLen，字符串长度
 *  @param[out] : 无
 *  @return     : 转换后的十进制数
*/
extern BOOL str2dec(char *pStr, int *pDst);

/**
 *  @brief      : 将字符串转换为16进制,如"11"->0x11
 *  @param[in]  : pStr，字符串
 *  @param[in]  : nLen, 字符串长度
 *  @param[out] : pDst，转换后字节数组
 *  @return     : 成功TRUE，失败FLASE
*/
extern int str2hex(const char *pStr, unsigned char *pBuff, int nLen);

/**
 *  @brief      : 倒序拷贝
 *  @param[out]  : dest,目的数据
 *  @param[in]  : src,源数据
 *  @param[in] : len,长度
*/
extern void reversecpy(unsigned char * dest, const unsigned char* src,  int len);

/**
 *  @brief      : 小段内存拷贝
 *  @param[in]  : pSrc，源数据
 *  @param[in]  : len，拷贝长度
 *  @param[out] : pDst，目的数据
 *  @return     :
*/
extern void smallcpy(void *pDst, void *pSrc, int len);

/**
 ******************************************************************************
 * @brief		    获取节点的num属性
 * @param[in]       node     xml节点
 * @return                   失败返回 -1 成功返回 num
 * @details
 ******************************************************************************
 */
extern int get_cfg_num(xmlNodePtr node);

int xdr_add_uint8( unsigned char *pBuf, unsigned char val );

int xdr_get_uint8(unsigned char *pBuf, unsigned char *val );

int xdr_add_uint16(unsigned char *pBuf, unsigned short val);

int xdr_get_uint16(unsigned char *pBuf, unsigned short *val);

int xdr_add_uint32(unsigned char *pBuf, unsigned int val);

int xdr_get_uint32(unsigned char *pBuf, unsigned int * val);

int xdr_add_variantlen(unsigned char *pBuf, unsigned int nSize);

int xdr_get_variantlen(unsigned char *pBuf, unsigned int *nLen);

int xdr_add_OCTET_STRING(unsigned char *pBuf, unsigned char *pData, unsigned int nSize);

int xdr_get_OCTET_STRING(unsigned char *pBuf, unsigned char *pData, unsigned int len, unsigned int * nSize);

#endif
