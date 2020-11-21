#ifndef _MEM_H_
#define _MEM_H_


#include <pthread.h> 
#include "plat_include.h"



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

int rh_mem_init( void );

int rh_mem_data_write_iec(unsigned char dataType,int addr_start,int nNum,void *data,int datalen);

int rh_mem_data_read_iec(unsigned char dataType,int addr_start,int nNum,void *data,int size);

int rh_mem_data_write_oop(unsigned short nPn,unsigned int nDi,void *data,int datalen);

int rh_mem_data_read_oop(unsigned short nPn,unsigned int nDi,void *data,int datalen);

#endif
