#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "elDbase.h"
#include "rh_msg_channel.h"


int main( void )
{	
	unsigned char inBuf[128] = {0};
	unsigned char outBuf[128] = {0};
	
	el_dbase_init();

	inBuf[0] = 0x00;
	inBuf[1] = 0x10;
	inBuf[2] = 0x02;
	inBuf[3] = 0x00;
	
	int nRet = el_read_data(0,inBuf,4,outBuf,sizeof(outBuf));

	printf("nRet = %d\n",nRet);
	
	return 0;
}
