#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "mem.h"


int main( void )
{	
	unsigned char buf1[4]={0x03,0x00,0x00,0x00};	
	unsigned char buf2[4]={0x02,0x00,0x00,0x00}; 

	
	rh_mem_init();

	rh_mem_data_write_oop(1,0x00110200,(void*)buf1,sizeof(buf1));
	
	rh_mem_data_write_oop(2,0x00110200,(void*)buf2,sizeof(buf2));

//	data = 0;
	
//	rh_mem_data_read_iec(DATA_YX,0,1,(void*)&data,sizeof(data));

	//rh_mem_data_write_oop(1,0x00010000,(void*)buf,4);

	//buf[0] = 0x55;
	//rh_mem_data_read_oop(1,0x00030200,(void*)buf,4);

	//int addr = rh_mem_get_yx_addr(3,0x09000001);
	
	//printf("addr = %d\n",addr);

	while(1)
	{
		sleep(1);
	}
	
	return 0;
}
