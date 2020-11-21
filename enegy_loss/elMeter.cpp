#include <string.h>
#include <assert.h>
#include "dbase_msg_interface.h"
#include <sys/un.h>
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "elMeter.h"

OOP_METER_T               g_ParaMet[MAX_MET_NUM];


MET_ENERGY_BLOCK_T	      s_UserMetData[METER_LEV_MAX];


static const unsigned int DI_ENERGY[] = 
{
	0x00100200, // 正有
	0x00200200, // 反有
	0x00300200, // 正无(组合无功1)
	0x00400200	// 反无(组合无功2)
};

void meter_para_load( const char *szFile )
{	
	int nMetNum = 0;
	int idx = 0;
	xmlDocPtr	   doc;  
	xmlNodePtr rootNode;
	xmlNodePtr iedPara;
	xmlNodePtr paramNode;

	memset(&g_ParaMet,0x00,sizeof(g_ParaMet));

	//忽略空白字符，忽略text节点
	xmlKeepBlanksDefault(0);

	printf("%s\n",szFile);
	//打开文件
    doc = xmlReadFile(szFile,"UTF-8",XML_PARSE_RECOVER);
    if (NULL == doc)
    { 
    	printf("xmlReadFile %s faild\n",szFile);
        return -1;
    }
	
	//获取xmlroot节点
	rootNode = xmlDocGetRootElement(doc);
    if (NULL == rootNode)
    {    	
		printf("xmlDocGetRootElement %s faild\n",szFile);
        return -1;
    }

	//获取IED_PARA节点
	iedPara = rootNode->children;	
	while(iedPara != NULL)
	{
		//查找获取IED_PARA节点节点
		 if (!xmlStrcmp(iedPara->name, BAD_CAST "IED_PARA"))
         {					
			break;
         } 		 

		 iedPara = iedPara->next;
	}
	
	if(iedPara == NULL) return -1;

	//获取设备个数
	nMetNum = get_cfg_num(iedPara);
	if(nMetNum <= 0) return -1;

	printf("nMetNum = %d\n",nMetNum);
	
	int nPn = 0;

	//获取设备参数节点
	paramNode = iedPara->children;	
	for(;paramNode != NULL;paramNode = paramNode->next)
	{
		if(idx > nMetNum) break;

		// 获取设备的测量点号
		xmlChar* szAttr = xmlGetProp(paramNode,BAD_CAST "index");	
		if(szAttr != NULL)
		{			
			str2dec((char*)szAttr,(int*)&nPn);
			if(nPn >= MAX_METER) break;
			
			g_ParaMet[nPn].nIndex = nPn;

			printf("nPn = %d\n",nPn);
			xmlFree(szAttr);
		}
		else 
		{
			printf("get pn failed\n");
			return -1;
		}

		//设备地址
		xmlChar* szAttrAddr = xmlGetProp(paramNode,BAD_CAST "addr");	
		if(szAttrAddr != NULL)
		{
			str2hex((char*)szAttrAddr,(unsigned char*)&g_ParaMet[nPn].basic.tsa.add,strlen((char*)szAttrAddr));
			//reversecpy((unsigned char*)&g_ParaMet[nPn].id,(unsigned char*)&id,4);
			
			printf("addr %02x %02x \n",g_ParaMet[nPn].basic.tsa.add[0],g_ParaMet[nPn].basic.tsa.add[1]);
			xmlFree(szAttrAddr);
		}

		//波特率
		xmlChar* szAttrBaud = xmlGetProp(paramNode,BAD_CAST "baud"); 	
		if(szAttrBaud != NULL)
		{						
			str2dec((char*)szAttrBaud,(int*)&g_ParaMet[nPn].basic.baud);
			printf("baud = %d\n",g_ParaMet[nPn].basic.baud);
			xmlFree(szAttrBaud);
		}
		else 
		{
			printf("get baud error\n");
			return -1;
		}

		//协议类型
		xmlChar* szAttrproto = xmlGetProp(paramNode,BAD_CAST "proto"); 	
		if(szAttrproto != NULL)
		{						
			str2dec((char*)szAttrproto,(int*)&g_ParaMet[nPn].basic.protocol);
			printf("proto = %d\n",g_ParaMet[nPn].basic.protocol);
			xmlFree(szAttrproto);
		}
		else 
		{
			printf("get proto error\n");
			return -1;
		}

		//端口号
		xmlChar* szAttrPort = xmlGetProp(paramNode,BAD_CAST "port");	
		if(szAttrPort != NULL)
		{	
			unsigned int nPort = 0;
			
			str2hex((char*)szAttrPort,(unsigned char*)&nPort,8);
			reversecpy((unsigned char*)&g_ParaMet[nPn].basic.port.value,(unsigned char*)&nPort,4);
			
			printf("port = %08x\n",g_ParaMet[nPn].basic.port.value);
			xmlFree(szAttrPort);
		}

		//设备类型
		xmlChar* szAttrType = xmlGetProp(paramNode,BAD_CAST "userType"); 	
		if(szAttrType != NULL)
		{						
			str2dec((char*)szAttrType,(int*)&g_ParaMet[nPn].basic.userType);
			
			printf("userType = %d\n",g_ParaMet[nPn].basic.userType);
			xmlFree(szAttrType);
		}
		else 
		{
			printf("get userType error\n");
			return -1;
		}

		//设备级别
		xmlChar* szAttrLevel = xmlGetProp(paramNode,BAD_CAST "level"); 	
		if(szAttrLevel != NULL)
		{						
			str2dec((char*)szAttrLevel,(int*)&g_ParaMet[nPn].basic.level);
			
			printf("level = %d\n",g_ParaMet[nPn].basic.level);
			xmlFree(szAttrLevel);
		}
		else 
		{
			printf("get level error\n");
			return -1;
		}

		idx++;
	}

	return idx;
}

unsigned char meter_level_check( unsigned short metid)
{
	if(metid >= MAX_METER) return METER_LEV_MAX;
	
	if(EMPTY_METER(metid)) return METER_LEV_MAX;

	return g_ParaMet[metid].basic.level;
}

void meter_data_init( void )
{
	memset(s_UserMetData,0x00,sizeof(s_UserMetData));
	memset(g_ParaMet,0x00,sizeof(g_ParaMet));
}


void meter_data_update( void )
{
	unsigned char  level = 0;
	unsigned short metid = 0;
	unsigned int   data;
	
	for(metid = 0; metid < MAX_MET_NUM;metid++)
	{
		if(EMPTY_METER(metid)) continue;

		//根据level分别获取对应得电能量
		level = meter_level_check(metid);
		if(level >= METER_LEV_MAX) continue;
		
		if(el_read_data(metid,DI_ENERGY[0],data,sizeof(data)))
		{
			s_UserMetData[level].enepaT.nValue[0] += data;
		}

		if(el_read_data(metid,DI_ENERGY[1],data,sizeof(data)))
		{
			s_UserMetData[level].enenaT.nValue[0] += data;
		}		
	
	}
}

unsigned int el_enesales_get(     unsigned char level )
{
	return s_UserMetData[level].enenaT.nValue[0];
}

unsigned int el_enesupply_get(     unsigned char level )
{
	return s_UserMetData[level].enepaT.nValue[0];
}

