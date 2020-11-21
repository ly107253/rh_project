#ifndef _MEM_H_
#define _MEM_H_


#include <pthread.h> 


typedef int BOOL;

#define    MAX_METER     512

#define TRUE    1
#define FALSE   0

#define MAX_NAME_LEN       (64)

//数据类型  
typedef enum
{
	DATA_YC = 0,
	DATA_YX,
	DATA_YM,
}DATA_TYPE_E;
	
//根据IED、do、tagName进行唯一索引
typedef struct{
	char iedName[MAX_NAME_LEN];                  // 设备实例名称
	char doName[MAX_NAME_LEN];                   // 数据对象名称
	char tagName[MAX_NAME_LEN];                  // 数据特征值
}MODE_POINT_T;

// 四级索引
typedef struct{
	char dotype_tag[MAX_NAME_LEN];                // 数据特征值
	int   offset;                                 // 数据内存偏移
	int   dataLen;                                // 数据长度
	unsigned int id;                              // 数据标识
}DOTTYPE_DO_UNIT_T;

// 三级索引
typedef struct{
	char do_tag[MAX_NAME_LEN];             // 数据对象名称
	unsigned int itemid;                   // 数据ID
	unsigned int offset;                   // 数据对象内存偏移
	unsigned int dotTypeLen;               // 数据类型总长度
	int nNum;                     // 数据类型do个数
	DOTTYPE_DO_UNIT_T *pDotList_104;       // 104数据索引表
	DOTTYPE_DO_UNIT_T *pDotList_698;       // 698数据索引表
}IED_DO_LIST_T;

//二级索引
typedef struct{
	char ied_tag[MAX_NAME_LEN];     // ied_tag
	unsigned short nPn;             // 测量点号
	pthread_mutex_t* lock;          // 设备数据读写同步锁

	int nNum;                       // 数据对象个数
	IED_DO_LIST_T *pDoList;         // 数据对象列表
}IED_LIST_T;

typedef struct{	
 	unsigned int id;	
	char do_tag[MAX_NAME_LEN];
	char dotype_tag[MAX_NAME_LEN];
}ID_ITEM_T;

typedef struct{
	int nNum;
	ID_ITEM_T *pItem;
}ID_LIST_T;

// 一级索引
typedef struct{
	int nNum;
	IED_LIST_T *pIedList;
}MEM_MAP_T;

//104点表

typedef struct
{
	char ied_tag[MAX_NAME_LEN];     //设备tag
	char do_tag[MAX_NAME_LEN];      //数据tag
	char dotype_tag[MAX_NAME_LEN];  //数据类型tag
	int  addr;                      //点号
	unsigned short pn;              //测量点号
	unsigned int   id;              //数据ID
}POINT_ITEM_T;

typedef struct{
	int nNum;
	POINT_ITEM_T  *point;	
	pthread_mutex_t* lock;          // 设备数据读写同步锁
}POINT_LIST_T;

typedef struct{
	POINT_LIST_T ym_list;         //遥脉列表
	POINT_LIST_T yx_list;         //遥信列表
	POINT_LIST_T yc_list;         //遥测列表
	ID_LIST_T    id_list;         //id列表
}POINT_MAP_T;

typedef struct Target_Server_Address
{
	union
	{
		unsigned char		af;
		struct
		{
			unsigned char	len	 	 : 4; /**< 地址长度取值范围：0…15，对应表示1…16个字节长度										*/
			unsigned char	oldvxd	 : 1; /**< 逻辑地址																*/
			unsigned char	flag 	 : 1; /**< 扩展逻辑地址标志															*/
			unsigned char	type 	 : 2; /**< 地址类型0表示单地址，1表示通配地址，2表示组地址，3表示广播地址								*/
		};
	};
	unsigned char vxd;						/**< 扩展逻辑地址			*/
	unsigned char add[16-1];		/**< 地址域			*/
}OOP_TSA_T;

typedef union Object_Attribute_Descriptor
{
	unsigned int			value;				/**< OOP_OAD_U值					*/
	struct
	{
		unsigned char		nIndex;				/**< 属性数据索引			*/
		struct
		{
			unsigned char	attID  : 5;			/**< 属性ID					*/
			unsigned char	attPro : 3;			/**< 属性标识(用于事件)		*/
		};
		unsigned short			nObjID;				/**< 对象标识				*/
	};
}OOP_OAD_U;

typedef struct tag_Meter_Basic
{
	OOP_TSA_T		tsa;						/**< 表计地址	*/
	unsigned char			baud;						/**< 波特率		*/
	unsigned char			protocol;					/**< 规约类型	*/
	OOP_OAD_U		port;						/**< 端口		*/
	char			password[12];		/**< 通信密码	*/
	unsigned char			pwdLen;						/**< 密码长度	*/
	unsigned char			rateNum;					/**< 费率个数	*/
	unsigned char			userType;					/**< 用户类型	*/
	unsigned char           level;                      /**< 设备类型 */
	unsigned char			pwrType;					/**< 接线方式	*/
	unsigned short 			stVolt;						/**< 额定电压	*/
	unsigned short			stAmp;						/**< 额定电流	*/
}OOP_METER_BASIC_T;

typedef struct tag_OOP_METER
{
	unsigned short			nIndex;		/**< 配置序号	*/
	OOP_METER_BASIC_T		basic;		/**< 基本信息	*/

}OOP_METER_T;




int rh_mem_init( void );

int rh_mem_data_write_iec(unsigned char dataType,int addr_start,int nNum,void *data,int datalen);

int rh_mem_data_read_iec(unsigned char dataType,int addr_start,int nNum,void *data,int size);

int rh_mem_data_write_oop(unsigned short nPn,unsigned int nDi,void *data,int datalen);

int rh_mem_data_read_oop(unsigned short nPn,unsigned int nDi,void *data,int datalen);

int rh_mem_get_yx_addr(unsigned short nPn,unsigned int id);

#endif
