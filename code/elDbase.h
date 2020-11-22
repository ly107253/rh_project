#ifndef _EL_DBASE_H__
#define _EL_DBASE_H__

#include "rh_msg_channel.h"

/** @brief 消息命令处理单元	*/
typedef struct tag_APP_MSG_T
{
	unsigned char  iop;								/**< 接口操作			*/
	int (*pfunc)(RH_MSG_BUFFER_T *pMsg);		/**< 处理函数			*/
}APP_MSG_T;


void el_dbase_init( void );


int el_write_data(unsigned short nPn,unsigned int id,unsigned char *buf,unsigned int len);

int el_read_data(unsigned short nPn,unsigned int id,unsigned char *outBuf,int nSize);

void el_msg_echo(int nErrNo,RH_MSG_BUFFER_T *pMsgRecv,unsigned char *buf,int len);

#endif
