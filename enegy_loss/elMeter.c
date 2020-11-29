#include <string.h>
#include <assert.h>
#include <sys/un.h>
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "elMeter.h"
#include "elCommon.h"
#include "elDbase.h"
#include "dbase_msg_interface.h"

OOP_METER_T               g_ParaMet[MAX_MET_NUM];

MET_ENERGY_BLOCK_T	      s_EneUseUp[METER_LEV_MAX];       // 消耗电量

MET_ENERGY_BLOCK_T        s_MetData[MAX_MET_NUM];           //用户电能示值
MET_ENERGY_BLOCK_T        s_MetDataBak[MAX_MET_NUM];        //备份用户电能示值



static const unsigned int DI_ENERGY[] = 
{
	0x00010000, // 正有
	0x00150000, // A相正有
	0x00290000, // B相正有
	0x003D0000, // C相正有
	0x00020000, // 反有
	0x00160000, // A相反有
	0x002A0000, // B相反有
	0x003E0000, // C相反有
};


int meter_para_load( const char *szFile )
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
	memset(s_EneUseUp,0x00,sizeof(s_EneUseUp));
	memset(s_MetData,0x00,sizeof(s_MetData));
	memset(s_MetDataBak,0x00,sizeof(s_MetDataBak));
	memset(g_ParaMet,0x00,sizeof(g_ParaMet));
}

//返回本次用电量
static float meter_ene_update(float *pCurEne,float *pBakEne)
{
	float eneUseup = 0.0;
	int eneCur = 0;
	int eneBak = 0;
	
	printf("pCurEne = %f pBakEne = %f\n",*pCurEne,*pBakEne);

	if((pCurEne == NULL) || (pBakEne == NULL)) return eneUseup;

	//取小数位后两位
	eneCur = (int)((*pCurEne)*100);
	eneBak = (int)((*pBakEne)*100);
	
	if(eneCur < eneBak) return eneUseup;

	//第一次抄读 
	if(eneBak == 0) 
	{		
		//备份电量
		*pBakEne = *pCurEne;

		return eneUseup;
	}
	

	//计算本次用电量
	printf("eneCur = %d eneBak = %d\n",eneCur,eneBak);
	eneUseup = (float)(eneCur-eneBak)/100;

	//备份电量
	*pBakEne = *pCurEne;

	printf("eneUseup = %f\n",eneUseup);
	
	return eneUseup;
}

void meter_data_update( void )
{
	int nRet = 0;
	unsigned char  level = 0;
	unsigned short metid = 0;
	
	for(metid = 0; metid < MAX_MET_NUM;metid++)
	{
		if(EMPTY_METER(metid)) continue;

		//根据level分别获取对应得电能量
		level = meter_level_check(metid);
		if(level >= METER_LEV_MAX) continue;

		printf("metid = %d level = %d\n",metid,level);
		
		nRet = el_read_data(metid,DI_ENERGY[0],(unsigned char*)&s_MetData[metid].enepaT.nValue[0],sizeof(float));
		if(nRet > 0)
		{
			printf("data1  = %f\n",s_MetData[metid].enepaT.nValue[0]);
			s_EneUseUp[level].enepaT.nValue[0] += meter_ene_update(&s_MetData[metid].enepaT.nValue[0],&s_MetDataBak[metid].enepaT.nValue[0]);
		}
		
		nRet = el_read_data(metid,DI_ENERGY[1],(unsigned char*)&s_MetData[metid].enepaA.nValue[0],sizeof(float));
		if(nRet > 0)
		{
			printf("data2  = %f\n",s_MetData[metid].enepaA.nValue[0]);
			s_EneUseUp[level].enepaA.nValue[0] += meter_ene_update(&s_MetData[metid].enepaA.nValue[0],&s_MetDataBak[metid].enepaA.nValue[0]);
		}

		nRet = el_read_data(metid,DI_ENERGY[2],(unsigned char*)&s_MetData[metid].enepaB.nValue[0],sizeof(float));
		if(nRet > 0)
		{
			printf("data3  = %f\n",s_MetData[metid].enepaB.nValue[0]);
			s_EneUseUp[level].enepaB.nValue[0] += meter_ene_update(&s_MetData[metid].enepaB.nValue[0],&s_MetDataBak[metid].enepaB.nValue[0]);
		}

		nRet = el_read_data(metid,DI_ENERGY[3],(unsigned char*)&s_MetData[metid].enepaC.nValue[0],sizeof(float));
		if(nRet > 0)
		{
			printf("data4  = %f\n",s_MetData[metid].enepaC.nValue[0]);
			s_EneUseUp[level].enepaC.nValue[0] += meter_ene_update(&s_MetData[metid].enepaC.nValue[0],&s_MetDataBak[metid].enepaC.nValue[0]);
		}

		nRet = el_read_data(metid,DI_ENERGY[4],(unsigned char*)&s_MetData[metid].enenaT.nValue[0],sizeof(float));
		if(nRet > 0)
		{
			printf("data5  = %f\n",s_MetData[metid].enenaT.nValue[0]);
			s_EneUseUp[level].enenaT.nValue[0] += meter_ene_update(&s_MetData[metid].enenaT.nValue[0],&s_MetDataBak[metid].enenaT.nValue[0]);
		}

		nRet = el_read_data(metid,DI_ENERGY[5],(unsigned char*)&s_MetData[metid].enenaA.nValue[0],sizeof(float));
		if(nRet > 0)
		{
			printf("data6  = %f\n",s_MetData[metid].enenaA.nValue[0]);
			s_EneUseUp[level].enenaA.nValue[0] += meter_ene_update(&s_MetData[metid].enenaA.nValue[0],&s_MetDataBak[metid].enenaA.nValue[0]);
		}

		nRet = el_read_data(metid,DI_ENERGY[6],(unsigned char*)&s_MetData[metid].enenaB.nValue[0],sizeof(float));
		if(nRet > 0)
		{
			printf("data7  = %f\n",s_MetData[metid].enenaB.nValue[0]);
			s_EneUseUp[level].enenaB.nValue[0] += meter_ene_update(&s_MetData[metid].enenaB.nValue[0],&s_MetDataBak[metid].enenaB.nValue[0]);
		}

		nRet = el_read_data(metid,DI_ENERGY[7],(unsigned char*)&s_MetData[metid].enenaC.nValue[0],sizeof(float));
		if(nRet > 0)
		{
			printf("data8  = %f\n",s_MetData[metid].enenaC.nValue[0]);
			s_EneUseUp[level].enenaC.nValue[0] += meter_ene_update(&s_MetData[metid].enenaC.nValue[0],&s_MetDataBak[metid].enenaC.nValue[0]);
		}
	}
}

float el_ene_get(     unsigned char level,unsigned char nPhase)
{
	if(nPhase == 0)
	{
		return s_EneUseUp[level].enepaT.nValue[0];
	}
	else if(nPhase == 1)
	{
		return s_EneUseUp[level].enepaA.nValue[0];
	}
	else if(nPhase ==2)
	{
		return s_EneUseUp[level].enepaB.nValue[0];
	}
	else if(nPhase ==3)
	{
		return s_EneUseUp[level].enepaC.nValue[0];
	}

	return -1;
}


