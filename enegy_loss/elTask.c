#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include "elTask.h"
#include "elMeter.h"
#include "elDbase.h"
#include "elCommon.h"
#include "rh_msg_channel.h"

static LNLS_STATE_T  elStatis[METER_LEV_MAX];


#ifndef utime_t
#define utime_t				unsigned int			// ��2000.1.1 0:0 ��ʼ������
#endif

#ifndef SECS1970TO2000
#define SECS1970TO2000		0x386D4380		// 1970.1.1 0:0:0��2000.1.1 0:0:0������946684800((30*365+7)*24*3600)
#endif

#define TIMEZONESECS 28800

/**
*********************************************************************
* @brief       ��ȡϵͳ����
* @return      ϵͳ����
* @note        ��
* @warning     ��
*********************************************************************
*/
utime_t sys_time_get(void)
{
	time_t curtime = time(NULL);
	curtime -= SECS1970TO2000;
	curtime += TIMEZONESECS;
	return (utime_t)curtime;
}


static void el_statis_proc( void )
{
	unsigned char level = 0;
	int          elT;
	unsigned int enesal;
	unsigned int enesup;

	for(level= 0; level < METER_LEV_MAX;level++)
	{
		enesal = el_enesales_get(level);
		if(enesal <= 0)
		{
			return;
		}

		enesup = el_enesupply_get(level);
		if(enesup <= 0)
		{
			return;
		}

		elT = ((float)( enesup - enesal )/enesup)*100;

		elStatis[level].nRatio = elT;
		elStatis[level].eneSal = enesal;
		elStatis[level].eneSup = enesup;
	}
	
}

void el_statis_save( void )
{
	//����
}

void el_statis_init( void )
{
	memset(&elStatis,0x00,sizeof(elStatis));
}

static void* elTaskProc(void *arg)
{
	utime_t now  = 0;
	utime_t last = 0;

	BOOL bMin    = FALSE;	
	BOOL b15Min  = FALSE;
	BOOL bHour	 = FALSE;
	BOOL bDay    = FALSE;

	last = sys_time_get();

	while(1)
	{		
		now = sys_time_get();	

		/**>ʱ��ص������¼���*/
		if( now < last )
		{
			last = now;
		}
		else if( (last/(3600*24)) != (now/(3600*24)) )
		{
			bDay   = TRUE;
			
			bMin   = TRUE;				
			b15Min = TRUE;
			bHour  = TRUE;
			
		}
		else if((last/(60*60)) != (now/(60*60)))
		{
			bHour = TRUE;
			b15Min = TRUE;
			bMin   = TRUE;
		}
		else if((last/(60*15)) != (now/(60*15)))
		{
			b15Min = TRUE;
			bMin   = TRUE;
		}
		else if( (last/60) != (now/60) )
		{
			bMin   = TRUE;	
		}

		if (b15Min)
		{
			//���ݸ��� 
			meter_data_update();

			//����ͳ��
			el_statis_proc();
		}

		
	}

	return NULL;
}


void el_task_init()
{
	int nRet = -1;
	pthread_t id;

	nRet = pthread_create(&id,NULL,(void *) elTaskProc,NULL);

	if( nRet != 0 )
	{
		printf("Create eltask error\n");
	}
	else 
	{
		printf("Create eltask sucess\n");
	}
}


void el_init( void )
{
	//���ݽ��ճ�ʼ��
	el_dbase_init();

	//�����ʼ��
	el_task_init();
}
