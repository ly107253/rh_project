#ifndef _EL_COMMON_H__
#define _EL_COMMON_H__


#include "libxml/tree.h"

typedef int BOOL;

#define TRUE    1
#define FALSE   0


/**
 *  @brief      : �����ַ���ת��Ϊʮ������
 *  @param[in]  : pStr���ַ���
 *  @param[in]  : nLen���ַ�������
 *  @param[out] : ��
 *  @return     : ת�����ʮ������
*/
extern BOOL str2dec(char *pStr, int *pDst);

/**
 *  @brief      : ���ַ���ת��Ϊ16����,��"11"->0x11
 *  @param[in]  : pStr���ַ���
 *  @param[in]  : nLen, �ַ�������
 *  @param[out] : pDst��ת�����ֽ�����
 *  @return     : �ɹ�TRUE��ʧ��FLASE
*/
extern int str2hex(const char *pStr, unsigned char *pBuff, int nLen);

/**
 *  @brief      : ���򿽱�
 *  @param[out]  : dest,Ŀ������
 *  @param[in]  : src,Դ����
 *  @param[in] : len,����
*/
extern void reversecpy(unsigned char * dest, const unsigned char* src,  int len);

/**
 *  @brief      : С���ڴ濽��
 *  @param[in]  : pSrc��Դ����
 *  @param[in]  : len����������
 *  @param[out] : pDst��Ŀ������
 *  @return     :
*/
extern void smallcpy(void *pDst, void *pSrc, int len);

/**
 ******************************************************************************
 * @brief		    ��ȡ�ڵ��num����
 * @param[in]       node     xml�ڵ�
 * @return                   ʧ�ܷ��� -1 �ɹ����� num
 * @details
 ******************************************************************************
 */
extern int get_cfg_num(xmlNodePtr node);


#endif
