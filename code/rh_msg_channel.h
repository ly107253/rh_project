#ifndef _RH_MSG_CHANNEL_H
#define _RH_MSG_CHANNEL_H

#define APP_NAME_LEN     64

#define MAX_MSG_BUF_LEN  2048
#define MAX_MSG_TIMEOUT  30

#define HANDLE  int




enum Msg_Flag
{
	MSG_REQ  = 0x55,            //共享内存中有请求消息
	MSG_ACK  = 0xAA,            //共享内存中有确认消息
	MSG_IDLE = 0xFF,            //共享内存空闲 
};
	
/** @brief 消息头结构	*/
typedef struct
{
	unsigned char       state;                    /**< 启动标志 55 发送 AA 接收 FF 空闲*/
	unsigned char       sync;                   /**< 同步标记 0 否 1 是*/
	unsigned short		index;					/**< 帧序号				*/ 	
	unsigned short		resv;					/**< 保留					*/
	char 		szSrce[APP_NAME_LEN]; 	/**< 发送方消息app名字				*/
	char		szDest[APP_NAME_LEN];	/**< 接收方消息app名字				*/
} MSG_HEAD_T;

typedef union 
{
	unsigned int 			value; 			/**< 标识							*/
	struct 
	{
		unsigned int 		iop  : 16; 		/**< 接口操作						*/
		unsigned int 		iid  : 16; 		/**< 接口ID							*/
	};
} TAG_U;

/** @brief 消息结构	*/
typedef struct 
{
	TAG_U 			tag; 			                /**< 消息标识						*/
	unsigned short			nSize;			                /**< 消息内容长度					*/
	unsigned short			resv;			                /**< 保留							*/
	unsigned char 			pValue[MAX_MSG_BUF_LEN]; 		/**< 消息内容						*/
} MSG_T;

/** @brief 返回结果结构	*/
typedef struct 
{
	TAG_U			        tag;			                /**< 请求消息标识					*/
	unsigned short			nSize;			                /**< 消息内容长度					*/
	int			            errNo;			                /**< 错误码(0为OK, 其他值为错误)	*/
	unsigned char 			pValue[MAX_MSG_BUF_LEN]; 		/**< 返回内容						*/
} ACK_T;

// 消息缓存
typedef struct 
{
	MSG_HEAD_T     head;
	union
	{
		MSG_T 		msg; 			// 消息
		ACK_T		ack;			// 应答
	};
} RH_MSG_BUFFER_T;


void SmMsgInit(unsigned int keyid);

int SmMsgRecv(const char *szName,unsigned int keyid, RH_MSG_BUFFER_T *pMsg);

int SmMsgSendEcho(unsigned int keyid,RH_MSG_BUFFER_T *pMsg);

int SmMsgSendWait(const char * szSrc,unsigned int keyid,RH_MSG_BUFFER_T *pMsg, RH_MSG_BUFFER_T *pRet,unsigned int nWaittime);




#endif

