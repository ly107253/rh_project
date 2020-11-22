#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>
#include "elDbase.h"
#include "elCommon.h"
#include "mem.h"
#include "dbase_msg_interface.h"

static int seq = 0;


int el_read_data(unsigned short nPn,unsigned int id,unsigned char *outBuf,int nSize)
{
	int nPos = 0;
	int nRet = -1;
	unsigned int len = 0;
	RH_MSG_BUFFER_T pMsgSend;
	RH_MSG_BUFFER_T pMsgRet;
	
	memset(&pMsgSend,0x00,sizeof(pMsgSend));
	memset(&pMsgRet,0x00,sizeof(pMsgRet));

	pMsgSend.head.state  = MSG_REQ;
	pMsgSend.head.sync   = 1;
	pMsgSend.head.index  = seq++;
	strcpy(pMsgSend.head.szSrce,RH_APP_NAME);
	strcpy(pMsgSend.head.szDest,EL_APP_NAME);
	pMsgSend.msg.tag.iid = IID_EL;
	pMsgSend.msg.tag.iop = IOP_DBASE_READ;

	nPos += xdr_add_uint16(&pMsgSend.msg.pValue[nPos],nPn);
	nPos += xdr_add_uint32(&pMsgSend.msg.pValue[nPos],id);
	
	pMsgSend.msg.nSize   = nPos;
		
	nRet = SmMsgSendWait(EL_APP_NAME,EL_KEY_ID,&pMsgSend,&pMsgRet,WAITE_TIMEOUT);
	if( nRet == 0 )
	{
		nPos = 0;
		printf("pMsgRet.ack.nSize = %d\n",pMsgRet.ack.nSize);
		printf("%02x %02x %02x\n",pMsgRet.ack.pValue[0],pMsgRet.ack.pValue[1],pMsgRet.ack.pValue[2]);
		if(pMsgRet.ack.nSize <= nSize)
		{
			//解析消息
			nPos += xdr_get_OCTET_STRING(pMsgRet.ack.pValue, outBuf,nSize,&len);

			return len;
		}		
	}

	return nRet;
}

int el_write_data(unsigned short nPn,unsigned int id,unsigned char *buf,unsigned int len)
{
	int nPos = 0;
	RH_MSG_BUFFER_T pMsgSend;
	RH_MSG_BUFFER_T pMsgRet;
	
	memset(&pMsgSend,0x00,sizeof(pMsgSend));
	memset(&pMsgRet,0x00,sizeof(pMsgRet));

	pMsgSend.head.state = MSG_REQ;
	pMsgSend.head.sync  = 1;
	pMsgSend.head.index = seq++;
	strcpy(pMsgSend.head.szSrce,RH_APP_NAME);
	strcpy(pMsgSend.head.szDest,EL_APP_NAME);

	pMsgSend.msg.tag.iid = IID_EL;
	pMsgSend.msg.tag.iop = IOP_DBASE_WRITE;

	nPos += xdr_add_uint16(&pMsgSend.msg.pValue[nPos],nPn);
	nPos += xdr_add_uint32(&pMsgSend.msg.pValue[nPos],id);
	nPos += xdr_add_OCTET_STRING(&pMsgSend.msg.pValue[nPos],buf,len);
	
	pMsgSend.msg.nSize   = nPos;

	return SmMsgSendWait(EL_APP_NAME,EL_KEY_ID,&pMsgSend,&pMsgRet,WAITE_TIMEOUT);
}

void el_msg_echo(int nErrNo,RH_MSG_BUFFER_T *pMsgRecv,unsigned char *buf,int len)
{	
	int nPos = 0;
	RH_MSG_BUFFER_T pMsgSend;

	memset(&pMsgSend,0x00,sizeof(RH_MSG_BUFFER_T));

	pMsgSend.head.state   = MSG_ACK;
	pMsgSend.head.sync  = pMsgRecv->head.sync;
	pMsgSend.head.index = pMsgRecv->head.index;
	
	strcpy(pMsgSend.head.szSrce,RH_APP_NAME);
	strcpy(pMsgSend.head.szDest,pMsgRecv->head.szSrce);
	
	pMsgSend.ack.tag.iid = IID_EL;
	pMsgSend.ack.tag.iop = pMsgRecv->msg.tag.iop;

	
	pMsgSend.ack.errNo = nErrNo;

	nPos += xdr_add_OCTET_STRING(&pMsgSend.ack.pValue[nPos],buf,len);
	
	pMsgSend.ack.nSize = nPos;	

	SmMsgSendEcho(EL_KEY_ID,&pMsgSend);
}

int read_mem_data(RH_MSG_BUFFER_T*pMsg)
{
	int nPos = 0;
	unsigned char nErrNo = 1;
	unsigned short nPn = 0;
	unsigned int id = 0;
	unsigned char dataBuf[128] = {0};
	
	nPos += xdr_get_uint16(&pMsg->msg.pValue[nPos],&nPn);
	nPos += xdr_get_uint32(&pMsg->msg.pValue[nPos],&id);

	printf("nPn = %d id = %08x\n",nPn,id);
	
	int nRet = rh_mem_data_read_oop(nPn,id,dataBuf,sizeof(dataBuf));
	if(nRet > 0)
	{
		nErrNo = 0;
	}
	
	el_msg_echo(nErrNo,pMsg,dataBuf,nRet);

	return 0;
}

int write_mem_data(RH_MSG_BUFFER_T*pMsg)
{
	int nPos = 0;
	unsigned char nErrNo = 1;
	unsigned short nPn = 0;
	unsigned int id = 0;
	unsigned int datalen;
	unsigned char dataBuf[128] = {0};
	
	nPos += xdr_get_uint16(&pMsg->msg.pValue[nPos],&nPn);
	nPos += xdr_get_uint32(&pMsg->msg.pValue[nPos],&id);
	nPos += xdr_get_OCTET_STRING(&pMsg->msg.pValue[nPos],dataBuf,sizeof(dataBuf),&datalen);
	
	printf("nPn = %d id = %08x\n",nPn,id);
	
	int nRet = rh_mem_data_write_oop(nPn,id,dataBuf,datalen);
	if(nRet > 0)
	{
		nErrNo = 0;
	}
	
	el_msg_echo(nErrNo,pMsg,dataBuf,nRet);

	return 0;

}

/** @brief 消息命令处理表	*/
const APP_MSG_T s_dbaseMsgMap[] = 
{
	{IOP_DBASE_READ,		read_mem_data	},
	{IOP_DBASE_WRITE,		write_mem_data	},
};

static void* elDbaseProc(void *arg)
{
	int i = 0;
	RH_MSG_BUFFER_T  RecvMsg;

	while(1)
	{		
		memset(&RecvMsg,0x00,sizeof(RH_MSG_BUFFER_T));
		
		if(!SmMsgRecv(RH_APP_NAME,EL_KEY_ID,&RecvMsg))
		{
			printf("recv a msg IOP = %02x IID = %02x\n",RecvMsg.msg.tag.iop,RecvMsg.msg.tag.iid);
			for(i = 0; i<(int)(sizeof(s_dbaseMsgMap)/sizeof(s_dbaseMsgMap[0])); i++)
			{
				if( (RecvMsg.msg.tag.iop == s_dbaseMsgMap[i].iop) &&
					(NULL != s_dbaseMsgMap[i].pfunc) )
				{				
					s_dbaseMsgMap[i].pfunc(&RecvMsg);

					break;
				}
			}
		}
		printf("rhDbase runing.....\n");
		sleep(1);
	}

	return NULL;
}

void el_dbase_init( void )
{
	int nRet = -1;
	pthread_t id;

	SmMsgInit(EL_KEY_ID);

	nRet = pthread_create(&id,NULL,elDbaseProc,NULL);

	if( nRet != 0 )
	{
		printf("Create eltask error\n");
	}
	else 
	{
		printf("Create eltask sucess\n");
	}
}

