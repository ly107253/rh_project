#include <stdio.h>
#include <string.h>
#include "mem.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"

POINT_MAP_T       s_pointMap;             // 点表映射表

MEM_MAP_T         s_memMap;               // 内存索引映射列表

static void *s_pDataMem = NULL;           // 连续内存地址

static OOP_METER_T     s_ParaMet[MAX_METER];

/**
 *  @brief      : 数字字符串转换为十进制数
 *  @param[in]  : pStr，字符串
 *  @param[in]  : nLen，字符串长度
 *  @param[out] : 无
 *  @return     : 转换后的十进制数
*/
static BOOL str2dec(char *pStr, int *pDst)
{
    unsigned int data = 0;
    BOOL prefix = FALSE;
    int i = 0;
    int nLen = strlen((const char*)pStr);

    for(i=0; i<nLen; i++)
    {
        if(pStr[i]<'0'||pStr[i]>'9')
        {
            if((i == 0)&&(pStr[i] == '-'))
            {
                prefix = TRUE;
                continue;
            }
            break;
        }

        data *= 10;
        data += pStr[i]-'0';
    }

    if(prefix)
    {
        *pDst = data * -1;
    }
    else
    {
        *pDst = data;
    }

    return TRUE;
}

/**
 *  @brief      : 将字符串转换为16进制,如"11"->0x11
 *  @param[in]  : pStr，字符串
 *  @param[in]  : nLen, 字符串长度
 *  @param[out] : pDst，转换后字节数组
 *  @return     : 成功TRUE，失败FLASE
*/
static int str2hex(const char *pStr, unsigned char *pBuff, int nLen)
{
	char c = 0;
	unsigned char uc = 0;
	unsigned char number = 0;
	BOOL bUpper = FALSE;
	int curLen = 0;
	while(0 != *pStr && curLen < nLen)
	{
		c = *pStr++;
		if(c >= '0' && c <= '9')
		{
			uc = (unsigned char)(c-'0');
		}
		else if((c >= 'A') && (c <= 'F'))
		{
			uc = (unsigned char)(c-'A') + 10;
		}
		else if((c >= 'a') && (c <= 'f'))
		{
			uc = (unsigned char)(c-'a') + 10;
		}
		else
		{
			continue;
		}

		if(bUpper)
		{
			number = (number << 4) + uc;
			*pBuff++ = number;
			bUpper = FALSE;
			curLen++;
		}
		else
		{
			number = uc;
			bUpper = TRUE;
		}
	}

	return curLen;	
}
/**
 *  @brief      : 倒序拷贝
 *  @param[out]  : dest,目的数据
 *  @param[in]  : src,源数据
 *  @param[in] : len,长度
*/
static void reversecpy(unsigned char * dest, const unsigned char* src,  int len)
{
    int i = 0, j = 0;

    for(i=0,j=len-1; i<len; i++,j--)
    {
        dest[j] = src[i];
    }
}

/**
 *  @brief      : 小段内存拷贝
 *  @param[in]  : pSrc，源数据
 *  @param[in]  : len，拷贝长度
 *  @param[out] : pDst，目的数据
 *  @return     :
*/
void smallcpy(void *pDst, void *pSrc, int len)
{
    int i = 0;
    unsigned char  *des = (unsigned char*)pDst;
    unsigned char  *src = (unsigned char*)pSrc;

    for(i=0; i<len; i++)
    {
        des[i] = src[i];
    }
}

/**
 ******************************************************************************
 * @brief		    获取节点的num属性
 * @param[in]       node     xml节点
 * @return                   失败返回 -1 成功返回 num
 * @details
 ******************************************************************************
 */
static int get_cfg_num(xmlNodePtr node)
{
	//未找到IED_CFG节点
	if(node == NULL) return -1;

	int Num = 0;
	xmlChar* szAttr = xmlGetProp(node,BAD_CAST "num"); 	
	if(szAttr != NULL)
	{
		str2dec((char*)szAttr,&Num);
		xmlFree(szAttr);
	}


	return Num;
}

/**
 ******************************************************************************
 * @brief		    获取内存映射地址
 * @param[in]      
 * @return           
 * @details
 ******************************************************************************
 */
static void * get_mem_map( void )
{
	return (void*)&s_memMap;
}

/**
 ******************************************************************************
 * @brief		    获取点表
 * @param[in]      
 * @return           
 * @details
 ******************************************************************************
 */
static void *get_point_map( void )
{
	return (void*)&s_pointMap;
}

/**
 ******************************************************************************
 * @brief		  点表path解析
 * @param[in]     pItem      点表
 * @param[in]     path       据路径
 * @return           
 * @details
 ******************************************************************************
 */
static void point_path_parse(POINT_ITEM_T*pItem,char *path)
{
	int i = 0;
	int offset = 0;
	printf("point_path_parse begin\n");
	if( (pItem == NULL)  || (path == NULL))
	{
		return;
	}

	//解析iedtag
	printf("path len = %d\n",strlen(path));
	
	for(i = 0; i < (int)strlen(path);i++)
	{
		if(path[i] != '.')
		{			
			pItem->ied_tag[i] = path[i];
		}
		else 
		{
			pItem->ied_tag[i] = 0;
			break;
		}
	}

	offset = i+1;
	
	printf("offset1   = %d\n",offset);
	//do_tag解析
	for(i = offset; i < (int)strlen(path);i++)
	{
		if(path[i] != '.')
		{			
			pItem->do_tag[i-offset] = path[i];
		}
		else 
		{
			pItem->do_tag[i-offset] = 0;
			break;
		}
	}
	
	offset = i+1;
	printf("offset2   = %d\n",offset);

	//解析dotype_tag
	for(i = offset; i < (int)strlen(path);i++)
	{			
		pItem->dotype_tag[i-offset] = path[i];
	}
	
	pItem->dotype_tag[i-offset] = 0;

	printf("%s %s %s\n",pItem->ied_tag,pItem->do_tag,pItem->dotype_tag);
	
	printf("point_path_parse end\n");
}

/**
 ******************************************************************************
 * @brief		    点表加载
 * @param[in]       szFile     配置文件名
 * @return                     失败返回 -1 成功返回 0
 * @details
 ******************************************************************************
 */
int rh_meter_param_load( const char *szFile )
{		
	int nMetNum = 0;
	int idx = 0;
	xmlDocPtr	   doc;  
	xmlNodePtr rootNode;
	xmlNodePtr iedPara;
	xmlNodePtr paramNode;

	memset(&s_ParaMet,0x00,sizeof(s_ParaMet));

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
			
			s_ParaMet[nPn].nIndex = nPn;

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
			str2hex((char*)szAttrAddr,(unsigned char*)&s_ParaMet[nPn].basic.tsa.add,strlen((char*)szAttrAddr));
			//reversecpy((unsigned char*)&s_ParaMet[nPn].id,(unsigned char*)&id,4);
			
			printf("addr %02x %02x \n",s_ParaMet[nPn].basic.tsa.add[0],s_ParaMet[nPn].basic.tsa.add[1]);
			xmlFree(szAttrAddr);
		}

		//波特率
		xmlChar* szAttrBaud = xmlGetProp(paramNode,BAD_CAST "baud"); 	
		if(szAttrBaud != NULL)
		{						
			str2dec((char*)szAttrBaud,(int*)&s_ParaMet[nPn].basic.baud);
			printf("baud = %d\n",s_ParaMet[nPn].basic.baud);
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
			str2dec((char*)szAttrproto,(int*)&s_ParaMet[nPn].basic.protocol);
			printf("proto = %d\n",s_ParaMet[nPn].basic.protocol);
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
			reversecpy((unsigned char*)&s_ParaMet[nPn].basic.port.value,(unsigned char*)&nPort,4);
			
			printf("port = %08x\n",s_ParaMet[nPn].basic.port.value);
			xmlFree(szAttrPort);
		}

		//设备类型
		xmlChar* szAttrType = xmlGetProp(paramNode,BAD_CAST "userType"); 	
		if(szAttrType != NULL)
		{						
			str2dec((char*)szAttrType,(int*)&s_ParaMet[nPn].basic.userType);
			
			printf("userType = %d\n",s_ParaMet[nPn].basic.userType);
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
			str2dec((char*)szAttrLevel,(int*)&s_ParaMet[nPn].basic.level);
			
			printf("level = %d\n",s_ParaMet[nPn].basic.level);
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

/**
 ******************************************************************************
 * @brief		    点表加载
 * @param[in]       szFile     配置文件名
 * @return                     失败返回 -1 成功返回 0
 * @details
 ******************************************************************************
 */
int rh_point_list_load(const char *szFile)
{	
	xmlDocPtr	   doc;  
	xmlNodePtr rootNode;
    xmlNodePtr iedCfgNode;  
	xmlNodePtr iedNode;
	xmlNodePtr ycNode;
	xmlNodePtr yxNode;
	xmlNodePtr ymNode;
	xmlNodePtr doNode;		
	xmlNodePtr idCfgNode;	
	xmlNodePtr idNode;

	printf("rh_point_list_load begin\n");
	
	memset(&s_pointMap,0x00,sizeof(POINT_MAP_T));

	//忽略空白字符，忽略text节点
	xmlKeepBlanksDefault(0);

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
	
	//获取IED_CFG节点
	iedCfgNode = rootNode->children;	
	while(iedCfgNode != NULL)
	{
		//查找IED_CFG节点
		 if (!xmlStrcmp(iedCfgNode->name, BAD_CAST "IED_CFG"))
         {					
			break;
         } 		 

		 iedCfgNode = iedCfgNode->next;
	}

	if(iedCfgNode == NULL) return -1;

	//查找点表配置 ied
	iedNode = iedCfgNode->children;	
	for(;iedNode != NULL;iedNode = iedNode->next)
	{		
		xmlChar* szAttr = xmlGetProp(iedNode,BAD_CAST "tag"); 
		if(szAttr != NULL)
		{
			if(!xmlStrcmp(szAttr,BAD_CAST "104tstied"))
			{				
				xmlFree(szAttr);
				break;
			}
			
			xmlFree(szAttr);
		}

	}

	if(iedNode == NULL) return -1;

	printf("find pint cfg ok \n");
	//加载遥测点表
	ycNode = iedNode->children;	
	for(;ycNode != NULL;ycNode = ycNode->next)
	{		
		xmlChar* szAttr = xmlGetProp(ycNode,BAD_CAST "tag"); 	
		if(szAttr != NULL)
		{
			if(!xmlStrcmp(szAttr,BAD_CAST "YC"))
			{
				printf("find YC\n");
				xmlFree(szAttr);
				break;
			}
			
			xmlFree(szAttr);
		}

	}

	if( ycNode != NULL )
	{
	
		printf("find yc node ok\n");
		//获取遥测点表个数
		xmlChar* szAttr = xmlGetProp(ycNode,BAD_CAST "num"); 		
		str2dec((char*)szAttr,&s_pointMap.yc_list.nNum);		
		xmlFree(szAttr);
		
		printf("yc_list.nNum = %d\n",s_pointMap.yc_list.nNum);

		//遥测互斥锁初始化
		s_pointMap.yc_list.lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		if(s_pointMap.yc_list.lock != NULL)
		{			
			pthread_mutex_init(s_pointMap.yc_list.lock,NULL);	
		}
		else 
		{
			return -1;
		}
		//分配遥测点表内存
		if(s_pointMap.yc_list.nNum > 0)
		{
			s_pointMap.yc_list.point = (POINT_ITEM_T*)malloc(s_pointMap.yc_list.nNum*sizeof(POINT_ITEM_T));
			if(s_pointMap.yc_list.point != NULL)
			{
				int ycNum = 0;
				doNode = ycNode->children;
				for(;doNode != NULL; doNode = doNode->next)
				{
					if( ycNum > s_pointMap.yc_list.nNum ) break;
					
					printf("doNode->name = %s\n",(char*)doNode->name);
					xmlChar* szAttrTag = xmlGetProp(doNode,BAD_CAST "tag");
					
					printf("tag = %s\n",(char*)szAttrTag);
					
					xmlChar* szAttrAddr = xmlGetProp(doNode,BAD_CAST "addr"); 	
					if(szAttrAddr != NULL)
					{						
						str2dec((char*)szAttrAddr,&s_pointMap.yc_list.point[ycNum].addr);
						xmlFree(szAttrAddr);
						printf("addr = %d\n",s_pointMap.yc_list.point[ycNum].addr);
					}
					else 
					{
						printf("get addr error\n");
						return -1;
					}

					xmlChar* szAttrPath = xmlGetProp(doNode,BAD_CAST "path"); 
					if(szAttrPath != NULL)
					{					
						printf("path = %s\n",(char*)szAttrPath);
						point_path_parse(&s_pointMap.yc_list.point[ycNum],(char*)szAttrPath);
						
						xmlFree(szAttrPath);
					}	
					else 
					{						
						printf("get path error\n");
						return -1;
					}

					ycNum++;
				}
			}
		}
		
	}

	printf("1111111111111\n");
	//加载遥信点表
	yxNode = iedNode->children;	
	for(;yxNode != NULL;yxNode = yxNode->next)
	{		
		xmlChar* szAttr = xmlGetProp(yxNode,BAD_CAST "tag"); 
		if(szAttr != NULL)
		{
			if(!xmlStrcmp(szAttr,BAD_CAST "YX"))
			{
				xmlFree(szAttr);
				break;
			}
			
			xmlFree(szAttr);
		}
	}
	

	if( yxNode != NULL )
	{
	
		printf("find yx node ok\n");
		//获取遥测点表个数
		xmlChar* szAttr = xmlGetProp(yxNode,BAD_CAST "num"); 		
		str2dec((char*)szAttr,&s_pointMap.yx_list.nNum);		
		xmlFree(szAttr);
		printf("yx_list.nNum = %d\n",s_pointMap.yx_list.nNum);
		
		//遥信互斥锁初始化
		s_pointMap.yx_list.lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		if(s_pointMap.yx_list.lock != NULL)
		{			
			pthread_mutex_init(s_pointMap.yx_list.lock,NULL);	
		}
		else 
		{
			return -1;
		}

		//分配遥测点表内存
		if(s_pointMap.yx_list.nNum > 0)
		{
			s_pointMap.yx_list.point = (POINT_ITEM_T*)malloc(s_pointMap.yx_list.nNum*sizeof(POINT_ITEM_T));
			if(s_pointMap.yx_list.point != NULL)
			{
				int yxNum = 0;
				doNode = yxNode->children;
				for(;doNode != NULL; doNode = doNode->next)
				{
					if( yxNum > s_pointMap.yx_list.nNum ) break;
					
					xmlChar* szAttrAddr = xmlGetProp(doNode,BAD_CAST "addr"); 
					if(szAttrAddr != NULL)
					{
						str2dec((char*)szAttrAddr,&s_pointMap.yx_list.point[yxNum].addr);
						xmlFree(szAttrAddr);
					}
					
					xmlChar* szAttrPn = xmlGetProp(doNode,BAD_CAST "pn"); 
					if(szAttrPn != NULL)
					{
						str2dec((char*)szAttrPn,(int*)&s_pointMap.yx_list.point[yxNum].pn);
						printf("yx pn = %d\n",s_pointMap.yx_list.point[yxNum].pn);
						xmlFree(szAttrPn);
					}

					unsigned int id = 0;
					
					xmlChar* szAttrId = xmlGetProp(doNode,BAD_CAST "id");	
					if(szAttrId != NULL)
					{
						str2hex((char*)szAttrId,(unsigned char*)&id,8);
						reversecpy((unsigned char*)&s_pointMap.yx_list.point[yxNum].id,(unsigned char*)&id,4);
						
						printf("yx id = %08x\n",s_pointMap.yx_list.point[yxNum].id);
						xmlFree(szAttrId);
					}
					
					xmlChar* szAttrPath = xmlGetProp(doNode,BAD_CAST "path"); 
					if(szAttrPath != NULL)
					{
						point_path_parse(&s_pointMap.yx_list.point[yxNum],(char*)szAttrPath);
						xmlFree(szAttrPath);	
					}	

					yxNum++;
				}
			}
		}		
	}

	printf("2222222222222222\n");
	//加载遥脉点表
	ymNode = iedNode->children;	
	for(;ymNode != NULL;ymNode = ymNode->next)
	{		
		xmlChar* szAttr = xmlGetProp(ymNode,BAD_CAST "tag"); 		
		if(szAttr != NULL)
		{
			if(!xmlStrcmp(szAttr,BAD_CAST "YM"))
			{
				xmlFree(szAttr);
				break;
			}
			
			xmlFree(szAttr);
		}

	}

	if( ymNode != NULL )
	{
	
		printf("find ym node ok\n");
		//获取遥测点表个数
		
		xmlChar* szAttr = xmlGetProp(ymNode,BAD_CAST "num"); 		
		str2dec((char*)szAttr,&s_pointMap.ym_list.nNum);		
		xmlFree(szAttr);
		
		printf("ym_list.nNum = %d\n",s_pointMap.ym_list.nNum);
		
		//遥脉互斥锁初始化
		s_pointMap.ym_list.lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		if(s_pointMap.ym_list.lock != NULL)
		{			
			pthread_mutex_init(s_pointMap.ym_list.lock,NULL);	
		}
		else 
		{
			return -1;
		}

		//分配遥测点表内存
		if(s_pointMap.ym_list.nNum > 0)
		{
			s_pointMap.ym_list.point = (POINT_ITEM_T*)malloc(s_pointMap.ym_list.nNum*sizeof(POINT_ITEM_T));
			if(s_pointMap.ym_list.point != NULL)
			{
				int ymNum = 0;
				doNode = ymNode->children;
				for(;doNode != NULL; doNode = doNode->next)
				{
					if( ymNum > s_pointMap.ym_list.nNum ) break;
					
					xmlChar* szAttrAddr = xmlGetProp(doNode,BAD_CAST "addr"); 	
					str2dec((char*)szAttrAddr,&s_pointMap.ym_list.point[ymNum].addr);
					xmlFree(szAttrAddr);

					xmlChar* szAttrPath = xmlGetProp(doNode,BAD_CAST "path"); 	
					point_path_parse(&s_pointMap.ym_list.point[ymNum],(char*)szAttrPath);
					xmlFree(szAttrPath);	

					ymNum++;
				}
			}
		}		
	}

	//加载id列	
	printf("3333333333333\n");
	
	idCfgNode = rootNode->children;	
	while(idCfgNode != NULL)
	{
		//查找IED_CFG节点
		 if (!xmlStrcmp(idCfgNode->name, BAD_CAST "ID_CFG"))
         {					
			break;
         } 		 

		 idCfgNode = idCfgNode->next;
	}

	if(idCfgNode != NULL)
	{		
		xmlChar* szAttr = xmlGetProp(idCfgNode,BAD_CAST "num"); 	
		str2dec((char*)szAttr,&s_pointMap.id_list.nNum);
		xmlFree(szAttr);

		printf("s_pointMap.id_list.nNum = %d\n",s_pointMap.id_list.nNum);
		
		s_pointMap.id_list.pItem = (ID_ITEM_T*)malloc(s_pointMap.id_list.nNum*sizeof(ID_ITEM_T));
		if(s_pointMap.id_list.pItem == NULL) return -1;
		
		int idNum = 0;
		idNode = idCfgNode->children;
		for(;idNode != NULL;idNode = idNode->next)
		{
			if(idNum >= s_pointMap.id_list.nNum) break;
	
			xmlChar* szAttrName = xmlGetProp(idNode,BAD_CAST "name");	
			strcpy(s_pointMap.id_list.pItem[idNum].do_tag,(char*)szAttrName);
			xmlFree(szAttrName);
	
			xmlChar* szAttrTag = xmlGetProp(idNode,BAD_CAST "tag"); 	
			strcpy(s_pointMap.id_list.pItem[idNum].dotype_tag,(char*)szAttrTag);
			xmlFree(szAttrTag);
			
		
			unsigned int id= 0;
			
			xmlChar* szAttrId = xmlGetProp(idNode,BAD_CAST "id");	
			str2hex((char*)szAttrId,(unsigned char*)&id,8);
			reversecpy((unsigned char*)&s_pointMap.id_list.pItem[idNum].id,(unsigned char*)&id,4);
			xmlFree(szAttrId);

			printf("id = %08x\n",s_pointMap.id_list.pItem[idNum].id);
			idNum++;
		}
	}

	
	xmlFreeDoc(doc);
	return 0;
}


/**
 ******************************************************************************
 * @brief		    内存映射列表初始化
 * @param[in]       szFile     配置文件名
 * @return                     失败返回 -1 成功返回 0
 * @details
 ******************************************************************************
 */
int rh_mem_map_init( const char *szFile )
{	
	int len = 0;
	xmlDocPtr      doc;  
	xmlNodePtr rootNode;
    xmlNodePtr iedCfgNode;  
	xmlNodePtr iedNode;
	xmlNodePtr doNode;
	xmlNodePtr TempltesNode;
	xmlNodePtr dotTypeNode;
	xmlNodePtr dotTypeDoNode;
		
	memset(&s_memMap,0x00,sizeof(MEM_MAP_T));
	
	//忽略空白字符，忽略text节点
	xmlKeepBlanksDefault(0);
	
	//打开文件
    doc = xmlReadFile(szFile,"UTF-8",XML_PARSE_RECOVER);
    if (NULL == doc)
    { 
        return -1;
    }
	//获取xmlroot节点
	rootNode = xmlDocGetRootElement(doc);
    if (NULL == rootNode)
    {    	
        return -1;
    }
	
	//获取IED_CFG节点
	iedCfgNode = rootNode->children;	
	while(iedCfgNode != NULL)
	{
		//查找IED_CFG节点
		 if (!xmlStrcmp(iedCfgNode->name, BAD_CAST "IED_CFG"))
         {					
			break;
         } 		 

		 iedCfgNode = iedCfgNode->next;
	}

	if(iedCfgNode == NULL) return -1;

	//获取DataTypeTempltes节点
	TempltesNode = rootNode->children;	
	while(TempltesNode != NULL)
	{
		//查找IED_CFG节点
		 if (!xmlStrcmp(TempltesNode->name, BAD_CAST "DataTypeTempltes"))
         {					
			break;
         } 		 

		 TempltesNode = TempltesNode->next;
	}

	if(TempltesNode == NULL) return -1;


	//ied节点处理
	s_memMap.nNum = get_cfg_num(iedCfgNode);
	if(s_memMap.nNum <= 0) return -1;

	printf("s_memMap.nNum = %d\n",s_memMap.nNum);
	//分配一级表ied列表内存
	s_memMap.pIedList = (IED_LIST_T*)malloc(s_memMap.nNum*sizeof(IED_LIST_T));
	if(s_memMap.pIedList != NULL)
	{
		memset(s_memMap.pIedList,0x00,s_memMap.nNum*sizeof(IED_LIST_T));
	}
	else 
	{
		return -1;
	}

	int nNum  = 0;

	//ied节点处理
	iedNode = iedCfgNode->children;	
	for(;iedNode != NULL;iedNode = iedNode->next)
	{		
		printf("1111111111111 iedNode->name %s\n",(char*)iedNode->name);
		if (!xmlStrcmp(iedNode->name, BAD_CAST "IED"))
		{
			xmlChar* szAttr = xmlGetProp(iedNode,BAD_CAST "tag"); 	
			if(szAttr == NULL) 
			{
				printf("get tag error\n");
			 	return -1;
			}
			
			if(xmlStrcmp(szAttr,BAD_CAST "104tstied"))
			{				
				if(nNum >= s_memMap.nNum) break;

				//获取ied_tag				
				strcpy(s_memMap.pIedList[nNum].ied_tag,(char*)szAttr);
				
				//设备锁初始化
				s_memMap.pIedList[nNum].lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
				if(s_memMap.pIedList[nNum].lock != NULL)
				{				
					pthread_mutex_init(s_memMap.pIedList[nNum].lock,NULL);	
				}

				// 获取ied设备的测量点号
				xmlChar* szAttr = xmlGetProp(iedNode,BAD_CAST "pn");	
				if(szAttr != NULL)
				{
					str2dec((char*)szAttr,(int*)&s_memMap.pIedList[nNum].nPn);
					xmlFree(szAttr);
				}
				else 
				{
					printf("get pn failed\n");
					return -1;
				}

				
				//分配二级表dolist内存			
				s_memMap.pIedList[nNum].nNum = get_cfg_num(iedNode);
				if(s_memMap.pIedList[nNum].nNum <= 0) return -1;

				printf("s_memMap.pIedList[nNum].nNum = %d\n",s_memMap.pIedList[nNum].nNum);
				
				s_memMap.pIedList[nNum].pDoList = (IED_DO_LIST_T*)malloc(s_memMap.pIedList[nNum].nNum*sizeof(IED_DO_LIST_T));
			   	if(s_memMap.pIedList[nNum].pDoList != NULL)
			   	{
					memset(s_memMap.pIedList[nNum].pDoList,0x00,s_memMap.pIedList[nNum].nNum*sizeof(IED_DO_LIST_T));
				}
				else 
				{
					return -1;
				}
				
				int doNum = 0;
				int dotTypeLen = 0;
				doNode = iedNode->children;
			   	for(;doNode != NULL;doNode = doNode->next)
			   	{		   
			   	
					xmlChar* szTag = xmlGetProp(doNode,BAD_CAST "tag");
					printf("szTag = %s\n",(char*)szTag);
					
					if(xmlStrcmp(doNode->name, BAD_CAST "text"))
					{
						if(doNum >= s_memMap.pIedList[nNum].nNum) break;
					
				   		IED_DO_LIST_T *pDo =  &s_memMap.pIedList[nNum].pDoList[doNum];
						//获取数据项名称
						xmlChar* szAttrTag = xmlGetProp(doNode,BAD_CAST "tag"); 
						if(szAttrTag != NULL)
						{
							strcpy(pDo->do_tag,(char*)szAttrTag);
							printf("tag = %s\n",(char*)szAttrTag);
							xmlFree(szAttrTag);
						}
						else 
						{		
							printf("find tag failed\n");
							return -1;
						}

						//do在连续内存中的偏移
						pDo->offset = len;
						
						//获取数据类型长度
						xmlChar* szAttr = xmlGetProp(doNode,BAD_CAST "len"); 
						if(szAttr != NULL)
						{
							str2dec((char*)szAttr,&dotTypeLen);
							xmlFree(szAttr);
						}
						else 
						{
							printf("find len error\n");
							return -1;
						}

						printf("pDo->offset = %d dotTypeLen = %d\n",pDo->offset,dotTypeLen);

						//判断是否为单类型(如：float int)
						xmlChar* szdotType = xmlGetProp(doNode,BAD_CAST "dotype"); 							
						if(szdotType == NULL)
						{
							printf("szdotType is NULL\n");
							dotTypeNode = doNode;


							pDo->nNum = get_cfg_num(doNode);
							
							printf("szdotType is NULL %d\n",pDo->nNum);
						}
						else 
						{
						
							printf("szdotType is Not NULL\n");
							//获取dotype do节点
							dotTypeNode= TempltesNode->children;					
							for(;dotTypeNode != NULL;dotTypeNode = dotTypeNode->next)
							{
								//查找dotype节点
								
								printf("doNode->name %s\n",(char*)dotTypeNode->name);
								if(xmlStrcmp(dotTypeNode->name, BAD_CAST "text"))
								{
									xmlChar* szName = xmlGetProp(dotTypeNode,BAD_CAST "tag"); 
									if(szName != NULL)
									{
										if(!xmlStrcmp(szdotType,szName))
										{			
											printf("find dotTypeNode %s\n",(char*)szName);
											xmlFree(szName);
											break;
										}	
										
										xmlFree(szName);
									}

								}

							}
							
							xmlFree(szdotType);

							
							if(dotTypeNode == NULL) return -1;

							//分配四级表内存
							
							pDo->nNum = get_cfg_num(dotTypeNode);
							printf("pDo->nNum = %d \n",pDo->nNum);
						}
						

						pDo->pDotList_104 = (DOTTYPE_DO_UNIT_T*)malloc(pDo->nNum*sizeof(DOTTYPE_DO_UNIT_T));					
						pDo->pDotList_698 = (DOTTYPE_DO_UNIT_T*)malloc(pDo->nNum*sizeof(DOTTYPE_DO_UNIT_T));

						//处理104索引表
						int doTypeNum = 0;
						dotTypeDoNode = dotTypeNode->children;
						for(;dotTypeDoNode != NULL;dotTypeDoNode=dotTypeDoNode->next)
						{
							if(xmlStrcmp(dotTypeDoNode->name, BAD_CAST "text"))
							{
								if(doTypeNum >= pDo->nNum) break;
							
								xmlChar* szAttrLen = xmlGetProp(dotTypeDoNode,BAD_CAST "len"); 	
								if(szAttrLen != NULL)
								{
									str2dec((char*)szAttrLen,(int*)&pDo->pDotList_104[doTypeNum].dataLen);
									xmlFree(szAttrLen);
									
									printf("dotTypeDoNode len = %d\n",pDo->pDotList_104[doTypeNum].dataLen);
								}
								else 
								{
									printf("get dotTypeDoNode len failed\n");
									return -1;
								}

								xmlChar* szAttrOff = xmlGetProp(dotTypeDoNode,BAD_CAST "offset"); 	
								if(szAttrOff != NULL)
								{
									str2dec((char*)szAttrOff,(int*)&pDo->pDotList_104[doTypeNum].offset);
									xmlFree(szAttrOff);
									
									printf("dotTypeDoNode offset = %d\n",pDo->pDotList_104[doTypeNum].offset);
								}
								else 
								{
									printf("get dotTypeDoNode offset failed\n");
									return -1;
								}								
								
								xmlChar* szAttrTag = xmlGetProp(dotTypeDoNode,BAD_CAST "tag"); 	
								if(szAttrTag != NULL)
								{
									strcpy(pDo->pDotList_104[doTypeNum].dotype_tag,(char*)szAttrTag);
									
									printf("dotTypeDoNode tag = %s\n",(char*)szAttrTag);
									xmlFree(szAttrTag);
										
								}
								else 
								{
									printf("dotTypeDoNode failed\n");
									return -1;
								}

					
								doTypeNum++;
							}
							
						}

						//处理698索引表
						int i = 0;
						for(i = 0; i < doTypeNum;++i)
						{							
							smallcpy(&pDo->pDotList_698[i],&pDo->pDotList_104[i],sizeof(DOTTYPE_DO_UNIT_T));
						}
						
						//连续内存长度增加				
						doNum++;
						len += dotTypeLen;
					}
					
					
				}

			   nNum++;
			}
		}
	}
	
	xmlFreeDoc(doc);

	return len;
}

/**
 ******************************************************************************
 * @brief		    698写数据接口
 * @param[in]       nPn        设备编号
 * @param[in]       nDi        数据项ID
 * @param[in]       data       数据缓存
 * @param[in]       datalen    数据缓存长度
 * @return                     失败返回 -1 成功返回 0
 * @details
 ******************************************************************************
 */
int rh_mem_data_write_oop(unsigned short nPn,unsigned int nDi,void *data,int datalen)
{	
	int i = 0;
	MEM_MAP_T * pMapList = (MEM_MAP_T *)get_mem_map();	
	POINT_MAP_T*  pointList = (POINT_MAP_T*)get_point_map();

	
	if(pointList == NULL) return -1;
	if(pMapList == NULL) return -1;


	//查找ied
	for(i = 0; i < pMapList->nNum;++i)
	{
		if(nPn == pMapList->pIedList[i].nPn) break;
	}

	if(i >= pMapList->nNum) return -1;
	
	IED_LIST_T* pied= &pMapList->pIedList[i];
	printf("find the ied\n");

	//获取ID list
	
	for(i = 0;i < pointList->id_list.nNum;++i)
	{
		if(nDi == pointList->id_list.pItem[i].id) break;
	}

	if(i >= pointList->id_list.nNum) return -1;

	ID_ITEM_T * pIDItem = &pointList->id_list.pItem[i];
	
	printf("find the id\n");
	//查找do
	for(i = 0;i < pied->nNum;++i)
	{
		if(!strcmp(pIDItem->do_tag,pied->pDoList[i].do_tag)) break;
	}

	if(i >= pied->nNum) return -1;

	IED_DO_LIST_T *pDo = &pied->pDoList[i];
	printf("find the ied_do\n");

	//匹配dotType
	for(i = 0;i < pDo->nNum;i++)
	{
		if(!strcmp(pIDItem->dotype_tag,pDo->pDotList_698[i].dotype_tag)) break;
	}
	
	
	DOTTYPE_DO_UNIT_T *pDotType = &pDo->pDotList_698[i];
	
	printf("find the offset = %d DotType = %d \n",pDotType->offset,pDotType->dataLen);

	//数据长度判断
	if(datalen != pDotType->dataLen) return -1;
	
	//开始写数据
	pthread_mutex_lock(pied->lock);

	smallcpy((void*)((unsigned char *)s_pDataMem+pDo->offset+pDotType->offset),data,datalen);
	
	pthread_mutex_unlock(pied->lock);

	return 0;
}

/**
 ******************************************************************************
 * @brief		    698读数据接口
 * @param[in]       nPn        设备编号
 * @param[in]       nDi        数据项ID
 * @param[in]       data       数据缓存
 * @param[in]       datalen    数据缓存长度
 * @return                     失败返回 -1 成功返回 数据项长度
 * @details
 ******************************************************************************
 */
int rh_mem_data_read_oop(unsigned short nPn,unsigned int nDi,void *data,int datalen)
{	
	int i = 0;
	MEM_MAP_T*    pMapList  = (MEM_MAP_T *)get_mem_map();
	POINT_MAP_T*  pointList = (POINT_MAP_T*)get_point_map();
	
	if(pMapList == NULL) return -1;
	if(pointList == NULL) return -1;
	
	//查找ied
	for(i = 0; i < pMapList->nNum;++i)
	{
		if(nPn == pMapList->pIedList[i].nPn) break;
	}

	if(i >= pMapList->nNum) return -1;
	
	IED_LIST_T* pied= &pMapList->pIedList[i];

	printf("find the ied\n");
	//获取ID list
	for(i = 0;i < pointList->id_list.nNum;++i)
	{
		if(nDi == pointList->id_list.pItem[i].id) break;
	}

	if(i >= pointList->id_list.nNum) return -1;

	ID_ITEM_T * pIDItem = &pointList->id_list.pItem[i];
	
	printf("find the id\n");
	//查找do
	for(i = 0;i < pied->nNum;++i)
	{
		if(!strcmp(pIDItem->do_tag,pied->pDoList[i].do_tag)) break;
	}

	if(i >= pied->nNum) return -1;

	IED_DO_LIST_T *pDo = &pied->pDoList[i];
	
	printf("find the ied_do\n");

	//匹配dotType
	for(i = 0;i < pDo->nNum;i++)
	{
		if(!strcmp(pIDItem->dotype_tag,pDo->pDotList_698[i].dotype_tag)) break;
	}
	

	DOTTYPE_DO_UNIT_T *pDotType = &pDo->pDotList_698[i];
	
	printf("find the offset = %d DotType = %d \n",pDotType->offset,pDotType->dataLen);
	//开始写数据
	pthread_mutex_lock(pied->lock);

	smallcpy(data,(void*)((unsigned char *)s_pDataMem+pDo->offset+pDotType->offset),pDotType->dataLen);
	
	pthread_mutex_unlock(pied->lock);

	return pDotType->dataLen;
}

/**
 ******************************************************************************
 * @brief			配电写数据接口
 * @param[in]       nPn        设备编号
 * @param[in]       addr_start 写数据起始点号
 * @param[in]       nNum       点号数量
 * @param[in]       data       数据缓存
 * @param[in]       datalen    数据缓存长度
 * @return                     失败返回 -1 成功返回 0
 * @details
 ******************************************************************************
 */

int rh_mem_data_write_iec(unsigned char dataType,int addr_start,int nNum,void *data,int datalen)
{
	int i = 0,nMid = 0, nStart = 0, nEnd = 0;
	MEM_MAP_T * pMapList = (MEM_MAP_T *)get_mem_map();	
	POINT_MAP_T*  pointList = (POINT_MAP_T*)get_point_map();
	POINT_LIST_T *pList = NULL;
	
	if(pointList == NULL) return -1;
	if(pMapList == NULL) return -1;

	//查找点表
	switch(dataType)
	{
		case DATA_YC:
			pList = &pointList->yc_list;
			break;
		case DATA_YX:			
			pList = &pointList->yx_list;
			break;
		case DATA_YM:			
			pList = &pointList->ym_list;
			break;
		default:
			return -1;
	}

	
	pthread_mutex_lock(pList->lock);
	
	nEnd = pList->nNum;
	
	while ( nStart <= nEnd )
	{
		nMid = (nStart + nEnd) / 2;
		if( pList->point[nMid].addr == addr_start )
		{
			break;
		}
		
		if( pList->point[nMid].addr < addr_start )
		{
			nStart = nMid + 1;
		}
		else
		{
			 nEnd = nMid - 1;
		}
	}

	if( nStart > nEnd )
	{		
		pthread_mutex_unlock(pList->lock);
		return -1;
	}

	//查找ied
	POINT_ITEM_T  *point = &pList->point[nMid];
	
	printf("find the iec addr\n");
	for(i = 0; i < pMapList->nNum;++i)
	{
		if(strcmp(point->ied_tag,pMapList->pIedList[i].ied_tag) == 0) break;
	}

	if(i >= pMapList->nNum)
	{		
		pthread_mutex_unlock(pList->lock);
		
		return -1;
	}
	
	IED_LIST_T* pied= &pMapList->pIedList[i];
	printf("find the ied\n");

	//查找do
	for(i = 0;i < pied->nNum;++i)
	{
		if(!strcmp(point->do_tag,pied->pDoList[i].do_tag)) break;
	}

	if(i >= pied->nNum) 
	{		
		pthread_mutex_unlock(pList->lock);
		
		return -1;
	}

	IED_DO_LIST_T *pDo = &pied->pDoList[i];
	printf("find the do\n");

	//匹配dotType
	for(i = 0;i < pDo->nNum;i++)
	{
		if(!strcmp(point->dotype_tag,pDo->pDotList_104[i].dotype_tag)) break;
	}
	
	DOTTYPE_DO_UNIT_T *pDotType = &pDo->pDotList_104[i];
	
	printf("pDotType %d %d %d\n",pDo->offset,pDotType->offset,datalen);
	//开始写数据
	pthread_mutex_lock(pied->lock);

	smallcpy((void*)((unsigned char*)s_pDataMem+pDo->offset+pDotType->offset),data,datalen);
	
	pthread_mutex_unlock(pied->lock);

	
	pthread_mutex_unlock(pList->lock);
	return 0;
}
/**
 ******************************************************************************
 * @brief			配电读数据接口
 * @param[in]       nPn        设备编号
 * @param[in]       addr_start 写数据起始点号
 * @param[in]       nNum       点号数量
 * @param[in]       data       数据缓存
 * @param[in]       datalen    数据缓存长度
 * @return                     失败返回 -1 成功返回 数据长度
 * @details
 ******************************************************************************
 */
int rh_mem_data_read_iec(unsigned char dataType,int addr_start,int nNum,void *data,int size)
{
	int i = 0,nMid = 0, nStart = 0, nEnd = 0;
	MEM_MAP_T * pMapList = (MEM_MAP_T *)get_mem_map();	
	POINT_MAP_T*  pointList = (POINT_MAP_T*)get_point_map();
	POINT_LIST_T *pList = NULL;
	
	if(pointList == NULL) return -1;
	if(pMapList == NULL) return -1;

	//查找点表
	switch(dataType)
	{
		case DATA_YC:
			pList = &pointList->yc_list;
			break;
		case DATA_YX:			
			pList = &pointList->yx_list;
			break;
		case DATA_YM:			
			pList = &pointList->ym_list;
			break;
		default:
			return -1;
	}
	
	pthread_mutex_lock(pList->lock);
	
	nEnd = pList->nNum;
	
	while ( nStart <= nEnd )
	{
		nMid = (nStart + nEnd) / 2;
		if( pList->point[nMid].addr == addr_start )
		{
			break;
		}
		
		if( pList->point[nMid].addr < addr_start )
		{
			nStart = nMid + 1;
		}
		else
		{
			 nEnd = nMid - 1;
		}
	}

	if( nStart > nEnd )
	{		
		pthread_mutex_unlock(pList->lock);

		return -1;
	}

	printf("find the iec addr\n");
	//查找ied
	POINT_ITEM_T  *point = &pList->point[nMid];
	
	for(i = 0; i < pMapList->nNum;++i)
	{
		if(strcmp(point->ied_tag,pMapList->pIedList[i].ied_tag) == 0) break;
	}

	if(i >= pMapList->nNum) 
	{
		
		
		return -1;
	}
	
	IED_LIST_T* pied= &pMapList->pIedList[i];
	
	printf("find the ied\n");

	//查找do
	for(i = 0;i < pied->nNum;++i)
	{
		if(!strcmp(point->do_tag,pied->pDoList[i].do_tag)) break;
	}

	if(i >= pied->nNum) 
	{
		pthread_mutex_unlock(pList->lock);

		return -1;
	}

	IED_DO_LIST_T *pDo = &pied->pDoList[i];
	printf("find the do\n");

	//匹配dotType
	for(i = 0;i < pDo->nNum;i++)
	{
		if(!strcmp(point->dotype_tag,pDo->pDotList_104[i].dotype_tag)) break;
	}
	
	DOTTYPE_DO_UNIT_T *pDotType = &pDo->pDotList_104[i];
	
	printf("find the DotType\n");

	//获取要读取的数据总长度	
	int datalen = 0;
	for(i = 0;i < nNum;i++)
	{
		datalen += pDotType[i].dataLen;
		
		printf("pDotType = %d\n",pDotType[i].dataLen);
	}

	if(datalen > size) 
	{
		pthread_mutex_unlock(pList->lock);

		return -1;
	}
	

	
	printf("pDotType %d %d %d\n",pDo->offset,pDotType->offset,datalen);
	//开始写数据
	pthread_mutex_lock(pied->lock);

	smallcpy(data,(void*)((unsigned char*)s_pDataMem+pDo->offset+pDotType->offset),datalen);

	pthread_mutex_unlock(pied->lock);


	pthread_mutex_unlock(pList->lock);

	return datalen;

}

int rh_mem_get_yx_addr(unsigned short nPn,unsigned int id)
{
	int i = 0;

	int yx_num = s_pointMap.yx_list.nNum;
	
	for(i = 0; i < yx_num;++i)
	{
		POINT_ITEM_T item = s_pointMap.yx_list.point[i];
		if((nPn == item.pn) && (id == item.id))
		{
			return item.addr;
		}
	}

	return -1;
}

int rh_mem_init( void )
{
	int malloc_len = -1;

	//加载表参
	
	if(rh_meter_param_load("/opt/app/meter.xml") < 0)
	{
		return -1;
	}
	//加载遥信 遥测 遥脉点表
	if(rh_point_list_load("/opt/app/meter.xml") < 0)
	{
		return -1;
	}

	//获取内存长度
	malloc_len = rh_mem_map_init("/data/app/test.xml");
	//if( malloc_len < 0 )
	{
		return -1;
	}
	printf("malloc_len = %d\n",malloc_len);
	
	//分配数据内存
	s_pDataMem = malloc(malloc_len);
	if(s_pDataMem == NULL)
	{
		return -1;
	}
	
	return 0;
}

