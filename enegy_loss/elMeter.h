#ifndef _H_EL_ACMET_H__
#define _H_EL_ACMET_H__

#define MAX_METER                1024


#define MAX_MET_NUM              128

typedef enum
{
	METER_LEV0 = 0,
	METER_LEV1,
	METER_LEV2,
	METER_LEV3,
	METER_LEV_MAX
}METER_LEVEL_E;
	


#define EMPTY_METER(metid)         (g_ParaMet[metid].basic.port.value == 0)

#define OOP_MAX_RATES		(4)				/**< 费率个数		*/
#define OOP_FULL_RATES		(OOP_MAX_RATES+1)	/**< 总及分费率个数	*/

typedef struct tag_OOP_ENERGY
{
	unsigned int        nNum;
	unsigned int		rsv;
	float               nValue[OOP_FULL_RATES];
}OOP_ENERGY_T;

typedef struct tag_MET_ENERGY_BLOCK
{
	OOP_ENERGY_T enepaT;	/**< 正向有功电能量*/
	OOP_ENERGY_T enepaA;	/**< A相正向有功*/
	OOP_ENERGY_T enepaB;	/**< B相正向有功*/
	OOP_ENERGY_T enepaC;	/**< C相正向有功*/
	
	OOP_ENERGY_T enenaT;	/**< 反向有功电能量*/
	OOP_ENERGY_T enenaA;	/**< A相反向有功*/
	OOP_ENERGY_T enenaB;	/**< B相反向有功*/
	OOP_ENERGY_T enenaC;	/**< C相反向有功*/

}MET_ENERGY_BLOCK_T;

typedef struct Target_Server_Address
{
	union
	{
		unsigned char		af;
		struct
		{
			unsigned char	len	 	 : 4; /**< 地址长度取值范围：0…15，对应表示1…16个字节长度										*/
			unsigned char	oldvxd	 : 1; /**< 逻辑地址																*/
			unsigned char	flag 	 : 1; /**< 扩展逻辑地址标志															*/
			unsigned char	type 	 : 2; /**< 地址类型0表示单地址，1表示通配地址，2表示组地址，3表示广播地址								*/
		};
	};
	unsigned char vxd;						/**< 扩展逻辑地址			*/
	unsigned char add[16-1];		/**< 地址域			*/
}OOP_TSA_T;

typedef union Object_Attribute_Descriptor
{
	unsigned int			value;				/**< OOP_OAD_U值					*/
	struct
	{
		unsigned char		nIndex;				/**< 属性数据索引			*/
		struct
		{
			unsigned char	attID  : 5;			/**< 属性ID					*/
			unsigned char	attPro : 3;			/**< 属性标识(用于事件)		*/
		};
		unsigned short			nObjID;				/**< 对象标识				*/
	};
}OOP_OAD_U;

typedef struct tag_Meter_Basic
{
	OOP_TSA_T		tsa;						/**< 表计地址	*/
	unsigned char			baud;						/**< 波特率		*/
	unsigned char			protocol;					/**< 规约类型	*/
	OOP_OAD_U		port;						/**< 端口		*/
	char			password[12];		/**< 通信密码	*/
	unsigned char			pwdLen;						/**< 密码长度	*/
	unsigned char			rateNum;					/**< 费率个数	*/
	unsigned char			userType;					/**< 用户类型	*/
	unsigned char           level;                      /**< 设备类型 */
	unsigned char			pwrType;					/**< 接线方式	*/
	unsigned short 			stVolt;						/**< 额定电压	*/
	unsigned short			stAmp;						/**< 额定电流	*/
}OOP_METER_BASIC_T;

typedef struct tag_OOP_METER
{
	unsigned short			nIndex;		/**< 配置序号	*/
	OOP_METER_BASIC_T		basic;		/**< 基本信息	*/

}OOP_METER_T;


extern 	OOP_METER_T 			  g_ParaMet[MAX_MET_NUM];

void meter_data_init( void );

void meter_data_update( void );

int meter_para_load( const char *szFile );

float el_ene_get(     unsigned char level,unsigned char nPhase);

unsigned char meter_level_check( unsigned short metid);





#endif

