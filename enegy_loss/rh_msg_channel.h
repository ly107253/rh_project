#ifndef _RH_MSG_CHANNEL_H
#define _RH_MSG_CHANNEL_H

#define APP_NAME_LEN     64

#define MAX_MSG_BUF_LEN  2048
#define MAX_MSG_TIMEOUT  30

#define HANDLE  int




enum Msg_Flag
{
	MSG_REQ  = 0x55,            //�����ڴ�����������Ϣ
	MSG_ACK  = 0xAA,            //�����ڴ�����ȷ����Ϣ
	MSG_IDLE = 0xFF,            //�����ڴ���� 
};
	
/** @brief ��Ϣͷ�ṹ	*/
typedef struct
{
	unsigned char       state;                    /**< ������־ 55 ���� AA ���� FF ����*/
	unsigned char       sync;                   /**< ͬ����� 0 �� 1 ��*/
	unsigned short		index;					/**< ֡���				*/ 	
	unsigned short		resv;					/**< ����					*/
	char 		szSrce[APP_NAME_LEN]; 	/**< ���ͷ���Ϣapp����				*/
	char		szDest[APP_NAME_LEN];	/**< ���շ���Ϣapp����				*/
} MSG_HEAD_T;

typedef union 
{
	unsigned int 			value; 			/**< ��ʶ							*/
	struct 
	{
		unsigned int 		iop  : 16; 		/**< �ӿڲ���						*/
		unsigned int 		iid  : 16; 		/**< �ӿ�ID							*/
	};
} TAG_U;

/** @brief ��Ϣ�ṹ	*/
typedef struct 
{
	TAG_U 			tag; 			                /**< ��Ϣ��ʶ						*/
	unsigned short			nSize;			                /**< ��Ϣ���ݳ���					*/
	unsigned short			resv;			                /**< ����							*/
	unsigned char 			pValue[MAX_MSG_BUF_LEN]; 		/**< ��Ϣ����						*/
} MSG_T;

/** @brief ���ؽ���ṹ	*/
typedef struct 
{
	TAG_U			        tag;			                /**< ������Ϣ��ʶ					*/
	unsigned short			nSize;			                /**< ��Ϣ���ݳ���					*/
	int			            errNo;			                /**< ������(0ΪOK, ����ֵΪ����)	*/
	unsigned char 			pValue[MAX_MSG_BUF_LEN]; 		/**< ��������						*/
} ACK_T;

// ��Ϣ����
typedef struct 
{
	MSG_HEAD_T     head;
	union
	{
		MSG_T 		msg; 			// ��Ϣ
		ACK_T		ack;			// Ӧ��
	};
} RH_MSG_BUFFER_T;


void SmMsgInit(unsigned int keyid);

int SmMsgRecv(const char *szName,unsigned int keyid, RH_MSG_BUFFER_T *pMsg);

int SmMsgSendEcho(unsigned int keyid,RH_MSG_BUFFER_T *pMsg);

int SmMsgSendWait(const char * szSrc,unsigned int keyid,RH_MSG_BUFFER_T *pMsg, RH_MSG_BUFFER_T *pRet,unsigned int nWaittime);




#endif

