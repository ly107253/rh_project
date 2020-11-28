#ifndef _DBASE_MSG_INTERFACE_H__
#define _DBASE_MSG_INTERFACE_H__

//消息ID
#define EL_KEY_ID      1024

//线损app名字
#define EL_APP_NAME    "elAPP"
#define RH_APP_NAME    "rhApp"


#define IID_EL            1

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


//总
#define ID_IBJ_60510201           0x60510201           //供电量                 double-long-unsigned，  
#define ID_IBJ_60510202           0x60510202           //售电量                 double-long-unsigned，
#define ID_IBJ_60510203           0x60510203           //倒送电量                double-long-unsigned，
#define ID_IBJ_60510204           0x60510204           //分布式电源上网电量           double-long-unsigned
#define ID_IBJ_60510205           0x60510205           //线损率                 long（换算：-2，单位：%）
#define ID_IBJ_60510206           0x60510206           //异常标志                正常			（0）总表电量异常	（1）户表电量异常	（2）

//A相
#define ID_IBJ_61510201           0x61510201           //供电量                 double-long-unsigned，  
#define ID_IBJ_61510202           0x61510202           //售电量                 double-long-unsigned，
#define ID_IBJ_61510203           0x61510203           //倒送电量                double-long-unsigned，
#define ID_IBJ_61510204           0x61510204           //分布式电源上网电量           double-long-unsigned
#define ID_IBJ_61510205           0x61510205           //线损率                 long（换算：-2，单位：%）
#define ID_IBJ_61510206           0x61510206           //异常标志                正常			（0）总表电量异常	（1）户表电量异常	（2）

//B相
#define ID_IBJ_62510201           0x62510201           //供电量                 double-long-unsigned，  
#define ID_IBJ_62510202           0x62510202           //售电量                 double-long-unsigned，
#define ID_IBJ_62510203           0x62510203           //倒送电量                double-long-unsigned，
#define ID_IBJ_62510204           0x62510204           //分布式电源上网电量           double-long-unsigned
#define ID_IBJ_62510205           0x62510205           //线损率                 long（换算：-2，单位：%）
#define ID_IBJ_62510206           0x62510206           //异常标志                正常			（0）总表电量异常	（1）户表电量异常	（2）

//C相
#define ID_IBJ_63510201           0x63510201           //供电量                 double-long-unsigned，  
#define ID_IBJ_63510202           0x63510202           //售电量                 double-long-unsigned，
#define ID_IBJ_63510203           0x63510203           //倒送电量                double-long-unsigned，
#define ID_IBJ_63510204           0x63510204           //分布式电源上网电量           double-long-unsigned
#define ID_IBJ_63510205           0x63510205           //线损率                 long（换算：-2，单位：%）
#define ID_IBJ_63510206           0x63510206           //异常标志                正常			（0）总表电量异常	（1）户表电量异常	（2）


#endif
