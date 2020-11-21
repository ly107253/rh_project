#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "mem.h"


int main( void )
{	
	unsigned  char data = 1;
	unsigned char buf[20]={0};	

	buf[0] = 0xAA;
	
	rh_mem_init();

	int nRet = rh_mem_data_write_iec(DATA_YX,0,1,(void*)&data,sizeof(data));

	printf("222222222222 NrET = %d\n",nRet);
	data = 0;
	
	rh_mem_data_read_iec(DATA_YX,0,1,(void*)&data,sizeof(data));

	printf("data = %d\n",data);
	//rh_mem_data_write_oop(1,0x00010000,(void*)buf,4);

	//buf[0] = 0x55;
	//rh_mem_data_read_oop(1,0x00030200,(void*)buf,4);

	//int addr = rh_mem_get_yx_addr(3,0x09000001);
	
	//printf("addr = %d\n",addr);
	return 0;
}
