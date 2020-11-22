#ifndef __LDA_TASK_H__
#define __LDA_TASK_H__

#ifdef __cplusplus
extern "C"
{
#endif


#define DATA_PATH		"/data/app/package/data/"

/** @brief 6051(����2)-�����������*/
typedef struct tag_OOP_LNLS_UNIT
{
	unsigned int	eneSup;			/**< ������*/
	unsigned int	eneSal;			/**< �۵���*/
	unsigned int	eneDeliv;		/**< ���͵���*/
	short	        nRatio;			/**< ������*/
	unsigned char   abnorFlag;		/**< �쳣���(enum)*/
}LNLS_STATE_T;

/** �����̳߳�ʼ��*/
void lda_monitor_proc(void);

void el_init( void );


#ifdef __cplusplus
}
#endif

#endif
