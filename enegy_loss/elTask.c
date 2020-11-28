#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "elTask.h"
#include "elMeter.h"
#include "elDbase.h"
#include "elCommon.h"
#include "rh_msg_channel.h"
#include "dbase_msg_interface.h"

static EL_STATES_T  elStatis[METER_LEV_MAX];


static void el_statis_proc( void )
{
	unsigned char nPhase = 0;
	unsigned char level  = 0;	
	unsigned char met_level = 0;
	unsigned short metid= 0;
	float elT = 0.0;
	float enesal = 0.0;
	float enesup = 0.0;

	for(level= 0; level < METER_LEV_MAX;level++)
	{
		for(nPhase = 0; nPhase < EL_MAX_PHASES;nPhase++)
		{
			//�жϵ�ǰ�㼶�Ƿ�����һ�㼶�豸
			for(metid = 0; metid < MAX_MET_NUM;metid++)
			{
				if(EMPTY_METER(metid)) continue;

				//���Ҳ㼶Ϊlevel+1���豸
				met_level = meter_level_check(metid);

				//�ҵ��Ͳ㼶�豸
				if((level+1) == met_level) break;
			}

			if(metid >= MAX_MET_NUM)
			{
				continue;
			}
			
			//��ȡ��ǰ�㼶�������			
			enesup = el_ene_get(level,nPhase);
			if(enesup <= 0)
			{
				continue;
			}

			//��ȡ��һ�㼶�������
			enesal = el_ene_get(level+1,nPhase);
			if(enesal < 0)
			{
				continue;
			}
			
			//���㵱ǰ�㼶������			
			elT = (( enesup - enesal )/enesup)*100;

			printf("level = %d phase = %d enesal = %f enesup = %f elT= %f\n",level,nPhase,enesal,enesup,elT);
			
			elStatis[level].nVal[nPhase].nRatio = elT;
			elStatis[level].nVal[nPhase].eneSal = enesal;
			elStatis[level].nVal[nPhase].eneSup = enesup;
		}

	}

	
}

static void el_statis_save( void )
{	
	unsigned short metid = 0;
	unsigned char level  = 0;
	unsigned char met_level = 0;
	unsigned char  nPhase = 0;
		
	for(level = 0; level < METER_LEV_MAX;level++)
	{
		for(metid = 0; metid < MAX_MET_NUM;metid++)
		{
			if(EMPTY_METER(metid)) continue;

			//�����豸level�͵�ǰlevel�ĵ�һ���豸
			met_level = meter_level_check(metid);
			if(met_level >= METER_LEV_MAX) continue;

			if(level == met_level) break;
		}

		if(metid >= MAX_MET_NUM)
		{
			continue;
		}
		
		for(nPhase = 0; nPhase < EL_MAX_PHASES;nPhase++)
		{
			//д������
			el_write_data(metid,ID_IBJ_60510201|(nPhase << 24),(unsigned char*)&elStatis[level].nVal[nPhase].eneSup, 4);
			//д�۵���
			el_write_data(metid,ID_IBJ_60510202|(nPhase << 24),(unsigned char*)&elStatis[level].nVal[nPhase].eneSal, 4);
			//д���͵���
			el_write_data(metid,ID_IBJ_60510203|(nPhase << 24),(unsigned char*)&elStatis[level].nVal[nPhase].eneDeliv, 4);
			//д�ֲ�ʽ��Դ��������
			el_write_data(metid,ID_IBJ_60510204|(nPhase << 24),(unsigned char*)&elStatis[level].nVal[nPhase].eneDistrpwrnet, 4);
			//д������
			el_write_data(metid,ID_IBJ_60510205|(nPhase << 24),(unsigned char*)&elStatis[level].nVal[nPhase].nRatio, 4);
			//д�쳣��־
			el_write_data(metid,ID_IBJ_60510206|(nPhase << 24),(unsigned char*)&elStatis[level].nVal[nPhase].abnorFlag, 1);
		}

		
	}

}

void el_statis_init( void )
{
	memset(&elStatis,0x00,sizeof(elStatis));
}

static void* elTaskProc(void *arg)
{
	static int nTicks = 0;

	while(1)
	{		
	
		if ( nTicks > 60 )
		{
			printf("meter_data_update begin...........................\n");
			//���ݸ��� 
			meter_data_update();
			
			printf("meter_data_update end...........................\n");

			//����ͳ��
			printf("el_statis_proc begin...........................\n");

			el_statis_proc();
			
			printf("el_statis_proc end...........................\n");

			//������������
			printf("el_statis_save begin...........................\n");
			
			el_statis_save();
			
			printf("el_statis_save end...........................\n");
			
			nTicks = 0;
		}
		
		printf("eltask runing........ticks = %d\n",nTicks);

		nTicks++;
		
		sleep(1);
		
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
	//��ʼ��
	meter_data_init();
		
	//������������
	meter_para_load("/data/app/meter.xml");
	
	//���ݽ��ճ�ʼ��
	el_dbase_init();

	//�������ݳ�ʼ��
	el_statis_init();
	
	//�����ʼ��
	el_task_init();
}
