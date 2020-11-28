#ifndef __LDA_TASK_H__
#define __LDA_TASK_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define EL_MAX_PHASES   4


#define DATA_PATH		"/data/app/package/data/"

/** @brief 6051(属性2)-线损计算数据*/
typedef struct tag_OOP_LNLS_UNIT
{
	float	eneSup;			        /**< 供电量*/
	float	eneSal;			        /**< 售电量*/
	float	eneDeliv;		        /**< 倒送电量*/
	float	eneDistrpwrnet;         /**< 分布式电源上网电量*/
	float	nRatio;			        /**< 线损率*/
	unsigned char   abnorFlag;		/**< 异常标记(enum)*/
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
