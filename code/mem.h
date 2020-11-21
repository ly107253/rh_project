#ifndef _MEM_H_
#define _MEM_H_


#include <pthread.h> 
#include "plat_include.h"



#define TRUE    1
#define FALSE   0

#define MAX_NAME_LEN       (64)

//��������  
typedef enum
{
	DATA_YC = 0,
	DATA_YX,
	DATA_YM,
}DATA_TYPE_E;
	
//����IED��do��tagName����Ψһ����
typedef struct{
	char iedName[MAX_NAME_LEN];                  // �豸ʵ������
	char doName[MAX_NAME_LEN];                   // ���ݶ�������
	char tagName[MAX_NAME_LEN];                  // ��������ֵ
}MODE_POINT_T;

// �ļ�����
typedef struct{
	char dotype_tag[MAX_NAME_LEN];                // ��������ֵ
	int   offset;                                 // �����ڴ�ƫ��
	int   dataLen;                                // ���ݳ���
	unsigned int id;                              // ���ݱ�ʶ
}DOTTYPE_DO_UNIT_T;

// ��������
typedef struct{
	char do_tag[MAX_NAME_LEN];             // ���ݶ�������
	unsigned int itemid;                   // ����ID
	unsigned int offset;                   // ���ݶ����ڴ�ƫ��
	unsigned int dotTypeLen;               // ���������ܳ���
	int nNum;                     // ��������do����
	DOTTYPE_DO_UNIT_T *pDotList_104;       // 104����������
	DOTTYPE_DO_UNIT_T *pDotList_698;       // 698����������
}IED_DO_LIST_T;

//��������
typedef struct{
	char ied_tag[MAX_NAME_LEN];     // ied_tag
	unsigned short nPn;             // �������
	pthread_mutex_t* lock;          // �豸���ݶ�дͬ����

	int nNum;                       // ���ݶ������
	IED_DO_LIST_T *pDoList;         // ���ݶ����б�
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

// һ������
typedef struct{
	int nNum;
	IED_LIST_T *pIedList;
}MEM_MAP_T;

//104���

typedef struct
{
	char ied_tag[MAX_NAME_LEN];     //�豸tag
	char do_tag[MAX_NAME_LEN];      //����tag
	char dotype_tag[MAX_NAME_LEN];  //��������tag
	int  addr;                      //���
}POINT_ITEM_T;

typedef struct{
	int nNum;
	POINT_ITEM_T  *point;	
	pthread_mutex_t* lock;          // �豸���ݶ�дͬ����
}POINT_LIST_T;

typedef struct{
	POINT_LIST_T ym_list;         //ң���б�
	POINT_LIST_T yx_list;         //ң���б�
	POINT_LIST_T yc_list;         //ң���б�
	ID_LIST_T    id_list;         //id�б�
}POINT_MAP_T;

int rh_mem_init( void );

int rh_mem_data_write_iec(unsigned char dataType,int addr_start,int nNum,void *data,int datalen);

int rh_mem_data_read_iec(unsigned char dataType,int addr_start,int nNum,void *data,int size);

int rh_mem_data_write_oop(unsigned short nPn,unsigned int nDi,void *data,int datalen);

int rh_mem_data_read_oop(unsigned short nPn,unsigned int nDi,void *data,int datalen);

#endif
