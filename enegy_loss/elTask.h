#ifndef __LDA_TASK_H__
#define __LDA_TASK_H__

#ifdef __cplusplus
extern "C"
{
#endif


#define DATA_PATH		"/data/app/package/data/"

/** @brief 6051(属性2)-线损计算数据*/
typedef struct tag_OOP_LNLS_UNIT
{
	unsigned int	eneSup;			/**< 供电量*/
	unsigned int	eneSal;			/**< 售电量*/
	unsigned int	eneDeliv;		/**< 倒送电量*/
	short	        nRatio;			/**< 线损率*/
	unsigned char   abnorFlag;		/**< 异常标记(enum)*/
}LNLS_STATE_T;

/** 交采线程初始化*/
void lda_monitor_proc(void);

void el_init( void );


#ifdef __cplusplus
}
#endif

#endif
