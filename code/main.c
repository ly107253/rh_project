#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "mem.h"


int main( void )
{	
	//unsigned char buf1[4]={0x03,0x00,0x00,0x00};	
	//unsigned char buf2[4]={0x02,0x00,0x00,0x00}; 

	float data1 = 3.3;
	float data2 = 2.2;
	
	rh_mem_init();

	rh_mem_data_write_oop(1,0x00010000,(void*)&data1,sizeof(data1));
	
	rh_mem_data_write_oop(2,0x00010000,(void*)&data2,sizeof(data2));

	rh_mem_data_write_oop(1,0x00150000,(void*)&data1,sizeof(data1));
	
	rh_mem_data_write_oop(2,0x00150000,(void*)&data2,sizeof(data2));

	rh_mem_data_write_oop(1,0x00290000,(void*)&data1,sizeof(data1));
	
	rh_mem_data_write_oop(2,0x00290000,(void*)&data2,sizeof(data2));

	
	

//	data = 0;
	
//	rh_mem_data_read_iec(DATA_YX,0,1,(void*)&data,sizeof(data));

	//rh_mem_data_write_oop(1,0x00010000,(void*)buf,4);

	//buf[0] = 0x55;
	//rh_mem_data_read_oop(1,0x00030200,(void*)buf,4);

	//int addr = rh_mem_get_yx_addr(3,0x09000001);
	
	//printf("addr = %d\n",addr);

	int nTick = 0;

	while(1)
	{
		rh_mem_data_read_oop(1,0x60510205,(void*)&data1,sizeof(data1));

		printf("ÏßËðÂÊ = %f\n",data1);

		if(nTick > 70)
		{
			float data1 = 4.4;
			float data2 = 3.0;

			rh_mem_data_write_oop(1,0x00010000,(void*)&data1,sizeof(data1));
	
			rh_mem_data_write_oop(2,0x00010000,(void*)&data2,sizeof(data2));

			nTick = 0;
		}

		nTick++;
		sleep(1);
	}
	
	return 0;
}
