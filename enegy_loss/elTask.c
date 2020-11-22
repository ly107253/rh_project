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
	int          elT;
	unsigned int enesal;
	unsigned int enesup;

	for(level= 0; level < METER_LEV_MAX;level++)
	{
		for(nPhase = 0; nPhase < EL_MAX_PHASES;nPhase++)
		{
			//获取下一层级分相电能
			enesup = el_ene_get(level,nPhase);
			if(enesup < 0)
			{
				return;
			}

			//获取当前层级分相电能
			enesal = el_ene_get(level+1,nPhase);
			if(enesal < 0)
			{
				return;
			}
			
			//计算当前层级线损率			
			elT = ((float)( enesup - enesal )/enesup)*100;

			printf("level = %d phase = %d enesal = %d enesup = %d elT= %d\n",level,nPhase,enesal,enesup,elT);
			
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

			//查找设备level和当前level的第一个设备
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
			//写供电量
			el_write_data(metid,ID_IBJ_60510201,(unsigned char*)&elStatis[level].nVal[nPhase].eneSup, 4);
			//写售电量
			el_write_data(metid,ID_IBJ_60510202,(unsigned char*)&elStatis[level].nVal[nPhase].eneSal, 4);
			//写倒送电量
			el_write_data(metid,ID_IBJ_60510203,(unsigned char*)&elStatis[level].nVal[nPhase].eneDeliv, 4);
			//写分布式电源上网电量
			el_write_data(metid,ID_IBJ_60510204,(unsigned char*)&elStatis[level].nVal[nPhase].eneDistrpwrnet, 4);
			//写线损率
			el_write_data(metid,ID_IBJ_60510205,(unsigned char*)&elStatis[level].nVal[nPhase].nRatio, 2);
			//写异常标志
			el_write_data(metid,ID_IBJ_60510206,(unsigned char*)&elStatis[level].nVal[nPhase].abnorFlag, 1);
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
			printf("statis begin................................................\n");
			//数据更新 
			meter_data_update();

			//线损统计
			el_statis_proc();

			//保存线损数据
			el_statis_save();
			
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
	//初始化
	meter_data_init();
		
	//档案参数加载
	meter_para_load("/opt/app/meter.xml");
	
	//数据接收初始化
	el_dbase_init();

	//线损数据初始化
	el_statis_init();
	
	//任务初始化
	el_task_init();
}
