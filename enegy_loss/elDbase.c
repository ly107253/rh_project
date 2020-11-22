#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>
#include "elDbase.h"
#include "dbase_msg_interface.h"

static int seq = 0;


int el_read_data(unsigned short nPn,unsigned char *inBuf,int len,unsigned char *outBuf,int nSize)
{
	int nRet = -1;
	
	RH_MSG_BUFFER_T pMsgSend;
	RH_MSG_BUFFER_T pMsgRet;
	
	memset(&pMsgSend,0x00,sizeof(pMsgSend));
	memset(&pMsgRet,0x00,sizeof(pMsgRet));

	pMsgSend.head.state  = MSG_REQ;
	pMsgSend.head.sync = 1;
	pMsgSend.head.index = seq++;
	strcpy(pMsgSend.head.szSrce,EL_APP_NAME);
	strcpy(pMsgSend.head.szDest,RH_APP_NAME);
	pMsgSend.msg.tag.iid = IID_EL;
	pMsgSend.msg.tag.iop = IOP_DBASE_READ;
	
	pMsgSend.msg.nSize   = len;
	memcpy(pMsgSend.msg.pValue,&inBuf,len);
		
	nRet = SmMsgSendWait(EL_APP_NAME,EL_KEY_ID,&pMsgSend,&pMsgRet,WAITE_TIMEOUT);
	if( nRet == 0 )
	{
		printf("pMsgRet.ack.nSize = %d\n",pMsgRet.ack.nSize);
		printf("%02x %02x %02x\n",pMsgRet.ack.pValue[0],pMsgRet.ack.pValue[1],pMsgRet.ack.pValue[2]);
		if(pMsgRet.ack.nSize <= nSize)
		{
			memcpy(outBuf,pMsgRet.ack.pValue,pMsgRet.ack.nSize);
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

	pMsgSend.head.state = MSG_REQ;
	pMsgSend.head.sync  = 1;
	pMsgSend.head.index = seq++;
	strcpy(pMsgSend.head.szSrce,EL_APP_NAME);
	strcpy(pMsgSend.head.szDest,RH_APP_NAME);

	pMsgSend.msg.tag.iid = IID_EL;
	pMsgSend.msg.tag.iop = IOP_DBASE_WRITE;

	memcpy(&pMsgSend.msg.pValue,buf,nSize);
	
	pMsgSend.msg.nSize   = nSize;

	return SmMsgSendWait(EL_APP_NAME,EL_KEY_ID,&pMsgSend,&pMsgRet,WAITE_TIMEOUT);
}

void el_msg_echo(int nErrNo,RH_MSG_BUFFER_T *pMsgRecv,unsigned char *buf,int len)
{	
	RH_MSG_BUFFER_T pMsgSend;

	memset(&pMsgSend,0x00,sizeof(RH_MSG_BUFFER_T));

	pMsgSend.head.state   = MSG_ACK;
	pMsgSend.head.sync  = pMsgRecv->head.sync;
	pMsgSend.head.index = pMsgRecv->head.index;
	
	strcpy(pMsgSend.head.szSrce,EL_APP_NAME);
	strcpy(pMsgSend.head.szDest,pMsgRecv->head.szSrce);
	
	pMsgSend.ack.tag.iid = IID_EL;
	pMsgSend.ack.tag.iop = pMsgRecv->msg.tag.iid;

	
	pMsgSend.ack.errNo = nErrNo;
	pMsgSend.ack.nSize = len;	
	memcpy(pMsgSend.ack.pValue,buf,len);

	SmMsgSendEcho(EL_KEY_ID,&pMsgSend);
}

int read_test(RH_MSG_BUFFER_T*pMsg)
{
	unsigned char buf[4] = {0x12,0x34,0x56,0x78};
	
	el_msg_echo(0,pMsg,buf,sizeof(buf));

	return 0;
}

/** @brief 消息命令处理表	*/
const APP_MSG_T s_dbaseMsgMap[] = 
{
	{IOP_DBASE_READ,		read_test	},
};

static void* elDbaseProc(void *arg)
{
	int i = 0;
	RH_MSG_BUFFER_T  RecvMsg;

	while(1)
	{		
		memset(&RecvMsg,0x00,sizeof(RH_MSG_BUFFER_T));
		
		if(!SmMsgRecv(EL_APP_NAME,EL_KEY_ID,&RecvMsg))
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

	while(1)
	{
		sleep(1);
	}
}

