#ifndef _EL_DBASE_H__
#define _EL_DBASE_H__



/** @brief ��Ϣ�����Ԫ	*/
typedef struct tag_APP_MSG_T
{
	uint8  iop;								/**< �ӿڲ���			*/
	int32 (*pfunc)(const RH_MSG_BUFFER_T *pMsg);		/**< ������			*/
}APP_MSG_T;


#define EL_KEY_ID      1024

#define EL_APP_NAME    "elAPP"

#define RH_APP_NAME    "rhAPP"

#define WAITE_TIMEOUT     180


#define IID_EL_DBASE       1


#define IOP_DBASE_READ     0x10  

#define IOP_DBASE_WRITE    0x11

void el_dbase_init( void );

#endif