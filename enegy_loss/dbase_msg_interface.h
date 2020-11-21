#ifndef _EL_DBASE_H__
#define _EL_DBASE_H__

//消息ID
#define EL_KEY_ID      1024

//线损app名字
#define EL_APP_NAME    "elAPP"


//消息超时默认时间
#define WAITE_TIMEOUT     30


/**
 ******************************************************************************
 @brief 消息读数据
 @verbatim
		REQ ∷= SEQUENCE
		{
			信息点号		     long-unsigned，
			数据类型标识           double-long-unsigned，
		}
		ACK ∷= SEQUENCE
		{
			信息点号		     long-unsigned，
			数据类型标识           double-long-unsigned，
			数据内容             oct-string;
		}
 @endverbatim
 ******************************************************************************
 */
#define IOP_DBASE_READ     0x10  

/**
 ******************************************************************************
 @brief 消息写数据
 @verbatim
		REQ ∷= SEQUENCE
		{
			信息点号		     long-unsigned，
			数据类型标识           double-long-unsigned，
			数据内容             oct-string;
		}
		ACK ∷= BOOL

 @endverbatim
 ******************************************************************************
 */
#define IOP_DBASE_WRITE    0x11








#endif
