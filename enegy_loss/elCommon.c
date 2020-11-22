#include <stdio.h>
#include <string.h>
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "elCommon.h"


/**
 *  @brief      : 数字字符串转换为十进制数
 *  @param[in]  : pStr，字符串
 *  @param[in]  : nLen，字符串长度
 *  @param[out] : 无
 *  @return     : 转换后的十进制数
*/
BOOL str2dec(char *pStr, int *pDst)
{
    unsigned int data = 0;
    BOOL prefix = FALSE;
    int i = 0;
    int nLen = strlen((const char*)pStr);
    for(i=0; i<nLen; i++)
    {
        if(pStr[i]<'0'||pStr[i]>'9')
        {
            if((i == 0)&&(pStr[i] == '-'))
            {
                prefix = TRUE;
                continue;
            }
            break;
        }

        data *= 10;
        data += pStr[i]-'0';
		
    }

    if(prefix)
    {
        *pDst = data * (-1);
    }
    else
    {
        *pDst = data;
    }

    return TRUE;
}

/**
 *  @brief      : 将字符串转换为16进制,如"11"->0x11
 *  @param[in]  : pStr，字符串
 *  @param[in]  : nLen, 字符串长度
 *  @param[out] : pDst，转换后字节数组
 *  @return     : 成功TRUE，失败FLASE
*/
int str2hex(const char *pStr, unsigned char *pBuff, int nLen)
{
	char c = 0;
	unsigned char uc = 0;
	unsigned char number = 0;
	BOOL bUpper = FALSE;
	int curLen = 0;
	while(0 != *pStr && curLen < nLen)
	{
		c = *pStr++;
		if(c >= '0' && c <= '9')
		{
			uc = (unsigned char)(c-'0');
		}
		else if((c >= 'A') && (c <= 'F'))
		{
			uc = (unsigned char)(c-'A') + 10;
		}
		else if((c >= 'a') && (c <= 'f'))
		{
			uc = (unsigned char)(c-'a') + 10;
		}
		else
		{
			continue;
		}

		if(bUpper)
		{
			number = (number << 4) + uc;
			*pBuff++ = number;
			bUpper = FALSE;
			curLen++;
		}
		else
		{
			number = uc;
			bUpper = TRUE;
		}
	}

	return curLen;	
}
/**
 *  @brief      : 倒序拷贝
 *  @param[out]  : dest,目的数据
 *  @param[in]  : src,源数据
 *  @param[in] : len,长度
*/
void reversecpy(unsigned char * dest, const unsigned char* src,  int len)
{
    int i = 0, j = 0;

    for(i=0,j=len-1; i<len; i++,j--)
    {
        dest[j] = src[i];
    }
}

/**
 *  @brief      : 小段内存拷贝
 *  @param[in]  : pSrc，源数据
 *  @param[in]  : len，拷贝长度
 *  @param[out] : pDst，目的数据
 *  @return     :
*/
void smallcpy(void *pDst, void *pSrc, int len)
{
    int i = 0;
    unsigned char  *des = (unsigned char*)pDst;
    unsigned char  *src = (unsigned char*)pSrc;

    for(i=0; i<len; i++)
    {
        des[i] = src[i];
    }
}

/**
 ******************************************************************************
 * @brief		    获取节点的num属性
 * @param[in]       node     xml节点
 * @return                   失败返回 -1 成功返回 num
 * @details
 ******************************************************************************
 */
int get_cfg_num(xmlNodePtr node)
{
	//未找到IED_CFG节点
	if(node == NULL) return -1;

	int Num = 0;
	xmlChar* szAttr = xmlGetProp(node,BAD_CAST "num"); 	
	if(szAttr != NULL)
	{
		str2dec((char*)szAttr,&Num);
		xmlFree(szAttr);
	}


	return Num;
}

/**
 ******************************************************************************
 * @brief		可变长度添加
 * @param[out]  pBuf	数据缓冲
 * @param[in]	nLen	长度
 * @return		函数执行结果
 * @details
 * 1、函数执行成功时，函数返回长度字节数
 * 2、函数执行失败时，函数返回错误码
 ******************************************************************************
 */
int xdr_add_variantlen(unsigned char *pBuf, unsigned int nSize)
{
	int nPos = 0;
	unsigned char nBytes = 1;
	int i = 0;

	if( nSize <= 127 )
	{
		pBuf[nPos++] = (unsigned char)nSize;
	}
	else
	{
		if( nSize > 0xFF )
			nBytes++;
		if( nSize > 0xFFFF )
			nBytes++;
		if( nSize > 0xFFFFFF )
			nBytes++;
		
		pBuf[nPos++] = nBytes|0x80;
		
		for( i=nBytes-1;i>=0;i-- )
		{
			pBuf[nPos++] = ((unsigned char*)&nSize)[i];
		}
	}

	return nPos;
}

/**
 ******************************************************************************
 * @brief		可变长度获取
 * @param[in]   pBuf	数据缓冲
 * @param[out]	nLen	长度
 * @return		函数执行结果
 * @details
 * 1、函数执行成功时，函数返回长度字节数
 * 2、函数执行失败时，函数返回错误码
 ******************************************************************************
 */
int xdr_get_variantlen(unsigned char *pBuf, unsigned int *nLen)
{
	int nPos = 0;
	int i = 0;
	unsigned char nBytes = pBuf[nPos++];

	*nLen = 0;
	
	if( (nBytes&0x80) == 0 )
	{
		*nLen = nBytes;
	}
	else
	{
		nBytes&=0x7F;
		
		for( i=nBytes-1;i>=0;i-- )
		{
			((unsigned char*)nLen)[i] = pBuf[nPos++];
		}
	}

	return nPos;
}

int xdr_add_OCTET_STRING(unsigned char *pBuf, unsigned char *pData, unsigned int nSize)
{
	int nPos = 0;
	nPos += xdr_add_variantlen(&pBuf[nPos], nSize);
	memcpy(&pBuf[nPos], pData, nSize);
	nPos += nSize;
	return nPos;
}

int xdr_get_OCTET_STRING(unsigned char *pBuf, unsigned char *pData, unsigned int len, unsigned int * nSize)
{
	int nPos = 0;
	int nRet = 0;

	nRet = xdr_get_variantlen(&pBuf[nPos], nSize);
	if( nRet < 0 )
	{
		*nSize = 0;
		return -1;
	}

	if( (*nSize) > len )
	{
		*nSize = 0;
		return -1;
	}
	
	nPos += nRet;
	memcpy(pData, &pBuf[nPos], *nSize);
	nPos += *nSize;
	return nPos;
}

int xdr_add_uint32(unsigned char *pBuf, unsigned int val)
{
	pBuf[0] = ((unsigned char)((val)>>24));
	pBuf[1] = ((unsigned char)((val)>>16));
	pBuf[2] = ((unsigned char)((val)>>8));
	pBuf[3] = ((unsigned char)((val)));
	
	return 4;
}


