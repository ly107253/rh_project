#include <stdio.h>
#include "rh_msg_channel.h"
#include "elDbase.h"

static int seq = 0;

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
static int xdr_add_variantlen(unsigned char *pBuf, unsigned int nSize)
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
			pBuf[nPos++] = ((uint8*)&nSize)[i];
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
static int xdr_get_variantlen(unsigned char *pBuf, unsigned int *nLen)
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

static int xdr_add_OCTET_STRING(unsigned char *pBuf, unsigned char *pData, unsigned int nSize)
{
	int nPos = 0;
	nPos += xdr_add_variantlen(&pBuf[nPos], nSize);
	memcpy(&pBuf[nPos], pData, nSize);
	nPos += nSize;
	return nPos;
}

static int xdr_get_OCTET_STRING(unsigned char *pBuf, unsigned char *pData, unsigned int len, unsigned int * nSize)
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

static int xdr_add_uint32(unsigned char *pBuf, unsigned int val)
{
	pBuf[0] = ((unsigned char)((val)>>24));
	pBuf[1] = ((unsigned char)((val)>>16));
	pBuf[2] = ((unsigned char)((val)>>8));
	pBuf[3] = ((unsigned char)((val)));
	
	return 4;
}


void el_dbase_init( void )
{
	SmMsgInit(EL_KEY_ID);
}

int el_read_data(unsigned short nPn,unsigned char *inBuf,int len,unsigned char *outBuf,int nSize)
{
	int nPos = 0;
	int nRet = -1;
	unsigned int datalen = 0;
	
	RH_MSG_BUFFER_T pMsgSend;
	RH_MSG_BUFFER_T pMsgRet;
	
	memset(&pMsgSend,0x00,sizeof(pMsgSend));
	memset(&pMsgRet,0x00,sizeof(pMsgRet));

	pMsgSend.head.prm  = MSG_REQ;
	pMsgSend.head.sync = 1;
	pMsgSend.head.index = seq++;
	strcpy(pMsgSend.head.szSrce,EL_APP_NAME);
	strcpy(pMsgSend.head.szDest,RH_APP_NAME);
	pMsgSend.msg.tag.iid = IID_EL_DBASE;
	pMsgSend.msg.tag.iop = IOP_DBASE_READ;
	
	pMsgSend.msg.nSize   = len;
	memcpy(pMsgSend.msg.pValue,&inBuf,len);
		
	nRet = SmMsgSendWait(EL_APP_NAME,EL_KEY_ID,&pMsgSend,&pMsgRet,WAITE_TIMEOUT);
	if( nRet > 0 )
	{
		if(pMsgRet.ack.nSize <= nSize)
		{
			smallcpy(outBuf,pMsgRet.ack.pValue,pMsgRet.ack.nSize);
		}		
	}

	return nRet;
}

int el_write_data(unsigned short nPn,unsigned char *buf,int nSize)
{
	RH_MSG_BUFFER_T pMsgSend;
	RH_MSG_BUFFER_T pMsgRet;
	
	memset(&pMsgSend,0x00,sizeof(pMsgSend));
	memset(&pMsgRet,0x00,sizeof(pMsgRet));

	pMsgSend.head.prm   = MSG_REQ;
	pMsgSend.head.sync  = 1;
	pMsgSend.head.index = seq++;
	strcpy(pMsgSend.head.szSrce,EL_APP_NAME);

	pMsgSend.msg.tag.iid = IID_EL_DBASE;
	pMsgSend.msg.tag.iop = IOP_DBASE_WRITE;

	smallcpy(&pMsgSend.msg.pValue,buf,nSize);
	
	pMsgSend.msg.nSize   = nSize;

	return SmMsgSendWait(EL_APP_NAME,EL_KEY_ID,&pMsgSend,&pMsgRet,WAITE_TIMEOUT);
}

void el_msg_echo(int nErrNo,RH_MSG_BUFFER_T *pMsgRecv,uint8 *buf,int len)
{	
	RH_MSG_BUFFER_T pMsgSend;

	memset(&pMsgSend,0x00,sizeof(RH_MSG_BUFFER_T));

	pMsgSend.head.prm   = MSG_ACK;
	pMsgSend.head.sync  = 0;
	pMsgSend.head.index = pMsgRecv->head.index;
	
	strcpy(pMsgSend.head.szSrce,EL_APP_NAME);
	strcpy(pMsgSend.head.szDest,pMsgRecv->head.szSrce);
	
	pMsgSend.ack.tag.iid = IID_EL_DBASE;
	pMsgSend.ack.tag.iop = pMsgRecv->msg.tag.iid;

	
	pMsgSend.ack.errNo = nErrNo;
	pMsgSend.ack.nSize = len;	
	smallcpy(pMsgSend.ack.pValue,buf,len);

	SmMsgSendEcho(EL_KEY_ID,&pMsgSend);
}
/** @brief ��Ϣ������	*/
const APP_MSG_T s_dbaseMsgMap[] = 
{
	{IOP_DBASE_READ,		NULL	},
};

static void* elDbaseProc(void *arg)
{
	uint32 now  = 0;
	uint32 last = 0;	
	RH_MSG_BUFFER_T  RecvMsg;

	while(1)
	{		
		memset(&RecvMsg,0x00,sizeof(RH_MSG_BUFFER_T));
		
		if(!SmMsgRecv(EL_APP_NAME,EL_KEY_ID,&RecvMsg))
		{
			printf("recv a msg IOP = %02x IID = %02x\n",RecvMsg.msg.tag.iop,RecvMsg.msg.tag.iid);
			for(int32 i = 0; i<(int32)(sizeof(s_dbaseMsgMap)/sizeof(s_dbaseMsgMap[0])); i++)
			{
				if( (RecvMsg->msg.tag.iop == s_dbaseMsgMap[i].iop) &&
					(NULL != s_dbaseMsgMap[i].pfunc) )
				{				
					s_dbaseMsgMap[i].pfunc(&RecvMsg);

					return 0;
				}
			}
		}
	}

}

void el_dbase_init( void )
{
	int nRet = -1;
	pthread_t id;

	nRet = pthread_create(&id,NULL,(void *) elDbaseProc,NULL);

	if( nRet != 0 )
	{
		printf("Create eltask error\n");
	}
	else 
	{
		printf("Create eltask sucess\n");
	}
}

