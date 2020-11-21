#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>
#include "rh_msg_channel.h"

static pthread_mutex_t* s_lock;			// �豸���ݶ�дͬ����

void SmMutexInit( void )
{
	s_lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(s_lock,NULL);	
}

void SmMutexLock( void )
{	
	pthread_mutex_lock(s_lock);
}

void SmMutexUnlock( void )
{	
	pthread_mutex_unlock(s_lock);
}

HANDLE SmGetMsgQue(unsigned int keyid) 
{
	int shmid = 0;
	
	if((shmid = shmget(keyid,sizeof(RH_MSG_BUFFER_T),0666|IPC_CREAT)) < 0)
	{
		perror("shmget failed");
		return -1;
	}
	
	return shmid;
}

void SmMsgInit(unsigned int keyid)
{
	unsigned int shmId;
	RH_MSG_BUFFER_T *pMem = NULL;

	//��ȡ����
	shmId = SmGetMsgQue(keyid);
	if(shmId >= 0)
	{
		//��ȡ�ڴ�
		pMem = (RH_MSG_BUFFER_T*)shmat(shmId, NULL, 0);
		if(pMem != NULL)
		{	
			pMem->head.state = MSG_IDLE;
		}
	}	

	//ͬ������ʼ��
	SmMutexInit();
}

BOOL SmDestroyMsgQue(HANDLE handle)
{	
	if(shmctl(handle,IPC_RMID,NULL) < 0)
	{
		perror("shmctl");
		return FALSE;
	}
	
	return TRUE;
}

int SmMsgSendEcho(unsigned int keyid,RH_MSG_BUFFER_T *pMsg)
{
	int shmId = 0;
	RH_MSG_BUFFER_T *pMem = NULL;

	SmMutexLock();
	//��ȡ����
	shmId = SmGetMsgQue(keyid);
	if(shmId < 0)
	{
		SmMutexUnlock();
		return -1;
	}

	//��ȡ�ڴ�
	pMem = (RH_MSG_BUFFER_T*)shmat(shmId, NULL, 0);
	if(pMem == NULL)
	{	
		SmMutexUnlock();
		return -2;
	}	

	//����Ƿ���Է���Ϣ
	if( pMem->head.state != MSG_IDLE )
	{
		SmMutexUnlock();
		return -3;
	}	

	memcpy(pMem,pMsg,sizeof(RH_MSG_BUFFER_T));
	
	SmMutexUnlock();

	return 0;

}

int SmMsgSendWait(const char * szSrc,unsigned int keyid,RH_MSG_BUFFER_T *pMsg, RH_MSG_BUFFER_T *pRet,unsigned int nWaittime)
{
	int shmId = 0;
	unsigned int time = 0;
	RH_MSG_BUFFER_T *pMem = NULL;

	//��ȡ����
	shmId = SmGetMsgQue(keyid);
	if(shmId < 0)
	{
		return -1;
	}
	
	SmMutexLock();

	//��ȡ�ڴ�
	pMem = (RH_MSG_BUFFER_T*)shmat(shmId, NULL, 0);
	if(pMem == NULL)
	{
		SmMutexUnlock();
		return -2;
	}	

	if(nWaittime <= 0 || nWaittime > MAX_MSG_TIMEOUT)
	{
		nWaittime = MAX_MSG_TIMEOUT;
	}
	
	//����Ƿ���Է���Ϣ

	printf("nWaittime = %d\n",nWaittime);
	
	for(time = 0; time < nWaittime;time++)
	{		
		printf("waiting sending.....(%02x)\n",pMem->head.state);
		
		sleep(1);
		
		if( pMem->head.state != MSG_IDLE ) continue;

		
		memcpy(pMem,pMsg,sizeof(RH_MSG_BUFFER_T));

		break;
	}

	//��ʱ
	if(time >= nWaittime) 
	{
		pMem->head.state = MSG_IDLE;
		
		SmMutexUnlock();
		return -3;
	}

	printf("time = %d nWaittime = %d\n",time,nWaittime);
	
	//�����Ϣ�Ƿ񷵻�
	for(;time < nWaittime;time++)
	{
		sleep(1);

		printf("waint recv state = %02x sync = %d %s %s\n",pMem->head.state,pMem->head.sync,pMem->head.szDest,szSrc);
		
		if( pMem->head.state != MSG_ACK ) continue;
		if( !pMem->head.sync ) continue;	
		if( pMem->head.index != pMsg->head.index ) continue;		
		if( strcmp(pMem->head.szDest,szSrc)) continue;
		
		memcpy(pRet,pMem,sizeof(RH_MSG_BUFFER_T)); 

		//�ڴ��ÿ���
		pMem->head.state = MSG_IDLE;
		
		SmMutexUnlock();

		return 0;
	}
	
	//�ڴ��ÿ���
	pMem->head.state = MSG_IDLE;

	SmMutexUnlock();

	return -4;
	
}

int SmMsgRecv(const char *szName,unsigned int keyid, RH_MSG_BUFFER_T *pMsg)
{
	int shmId = 0;
	RH_MSG_BUFFER_T *pMem = NULL;

	//��ȡ����	
	shmId = SmGetMsgQue(keyid);
	if(shmId < 0)
	{
		printf("get msg que faile %s\n",__func__);
		return -1;
	}

	//��ȡ�ڴ�
	pMem = (RH_MSG_BUFFER_T*)shmat(shmId, NULL, 0);
	if(pMem == NULL)
	{
		printf("get mem faile %s\n",__func__);
		return -2;
	}	

	//����Ƿ�����Ϣ����
	if( pMem->head.state != MSG_REQ ) 
	{		
		return -3;	
	}	

	//��ȡ��Ϣ
	
	printf("pMem->head.state = %02x\n",pMem->head.state);
	printf("szDest = %s szSrc = %s\n",pMem->head.szDest,szName);
	
	if(strcmp(pMem->head.szDest,szName) == 0)
	{		
		memcpy(pMsg,pMem,sizeof(RH_MSG_BUFFER_T)); 

		//�ڴ��ÿ���
		pMem->head.state = MSG_IDLE;
		
		return 0;
	}
	

	return -4;
}
