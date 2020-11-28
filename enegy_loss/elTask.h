#ifndef __LDA_TASK_H__
#define __LDA_TASK_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define EL_MAX_PHASES   4


#define DATA_PATH		"/data/app/package/data/"

/** @brief 6051(����2)-�����������*/
typedef struct tag_OOP_LNLS_UNIT
{
	float	eneSup;			        /**< ������*/
	float	eneSal;			        /**< �۵���*/
	float	eneDeliv;		        /**< ���͵���*/
	float	eneDistrpwrnet;         /**< �ֲ�ʽ��Դ��������*/
	float	nRatio;			        /**< ������*/
	unsigned char   abnorFlag;		/**< �쳣���(enum)*/
}EL_UNIT_T;

typedef struct tag_EL_STATES
{
	unsigned int nNum;
	unsigned int rsv;
	EL_UNIT_T nVal[EL_MAX_PHASES];
}EL_STATES_T;


void el_init( void );


#ifdef __cplusplus
}
#endif

#endif
