#include <stdio.h>
#include <string.h>
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "elCommon.h"


/**
 *  @brief      : �����ַ���ת��Ϊʮ������
 *  @param[in]  : pStr���ַ���
 *  @param[in]  : nLen���ַ�������
 *  @param[out] : ��
 *  @return     : ת�����ʮ������
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
 *  @brief      : ���ַ���ת��Ϊ16����,��"11"->0x11
 *  @param[in]  : pStr���ַ���
 *  @param[in]  : nLen, �ַ�������
 *  @param[out] : pDst��ת�����ֽ�����
 *  @return     : �ɹ�TRUE��ʧ��FLASE
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
 *  @brief      : ���򿽱�
 *  @param[out]  : dest,Ŀ������
 *  @param[in]  : src,Դ����
 *  @param[in] : len,����
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
 *  @brief      : С���ڴ濽��
 *  @param[in]  : pSrc��Դ����
 *  @param[in]  : len����������
 *  @param[out] : pDst��Ŀ������
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
 * @brief		    ��ȡ�ڵ��num����
 * @param[in]       node     xml�ڵ�
 * @return                   ʧ�ܷ��� -1 �ɹ����� num
 * @details
 ******************************************************************************
 */
int get_cfg_num(xmlNodePtr node)
{
	//δ�ҵ�IED_CFG�ڵ�
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
 * @brief		�ɱ䳤�����
 * @param[out]  pBuf	���ݻ���
 * @param[in]	nLen	����
 * @return		����ִ�н��
 * @details
 * 1������ִ�гɹ�ʱ���������س����ֽ���
 * 2������ִ��ʧ��ʱ���������ش�����
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
 * @brief		�ɱ䳤�Ȼ�ȡ
 * @param[in]   pBuf	���ݻ���
 * @param[out]	nLen	����
 * @return		����ִ�н��
 * @details
 * 1������ִ�гɹ�ʱ���������س����ֽ���
 * 2������ִ��ʧ��ʱ���������ش�����
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


