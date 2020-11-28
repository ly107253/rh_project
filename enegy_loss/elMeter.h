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

#define OOP_MAX_RATES		(4)				/**< ���ʸ���		*/
#define OOP_FULL_RATES		(OOP_MAX_RATES+1)	/**< �ܼ��ַ��ʸ���	*/

typedef struct tag_OOP_ENERGY
{
	unsigned int        nNum;
	unsigned int		rsv;
	float               nValue[OOP_FULL_RATES];
}OOP_ENERGY_T;

typedef struct tag_MET_ENERGY_BLOCK
{
	OOP_ENERGY_T enepaT;	/**< �����й�������*/
	OOP_ENERGY_T enepaA;	/**< A�������й�*/
	OOP_ENERGY_T enepaB;	/**< B�������й�*/
	OOP_ENERGY_T enepaC;	/**< C�������й�*/
	
	OOP_ENERGY_T enenaT;	/**< �����й�������*/
	OOP_ENERGY_T enenaA;	/**< A�෴���й�*/
	OOP_ENERGY_T enenaB;	/**< B�෴���й�*/
	OOP_ENERGY_T enenaC;	/**< C�෴���й�*/

}MET_ENERGY_BLOCK_T;

typedef struct Target_Server_Address
{
	union
	{
		unsigned char		af;
		struct
		{
			unsigned char	len	 	 : 4; /**< ��ַ����ȡֵ��Χ��0��15����Ӧ��ʾ1��16���ֽڳ���										*/
			unsigned char	oldvxd	 : 1; /**< �߼���ַ																*/
			unsigned char	flag 	 : 1; /**< ��չ�߼���ַ��־															*/
			unsigned char	type 	 : 2; /**< ��ַ����0��ʾ����ַ��1��ʾͨ���ַ��2��ʾ���ַ��3��ʾ�㲥��ַ								*/
		};
	};
	unsigned char vxd;						/**< ��չ�߼���ַ			*/
	unsigned char add[16-1];		/**< ��ַ��			*/
}OOP_TSA_T;

typedef union Object_Attribute_Descriptor
{
	unsigned int			value;				/**< OOP_OAD_Uֵ					*/
	struct
	{
		unsigned char		nIndex;				/**< ������������			*/
		struct
		{
			unsigned char	attID  : 5;			/**< ����ID					*/
			unsigned char	attPro : 3;			/**< ���Ա�ʶ(�����¼�)		*/
		};
		unsigned short			nObjID;				/**< �����ʶ				*/
	};
}OOP_OAD_U;

typedef struct tag_Meter_Basic
{
	OOP_TSA_T		tsa;						/**< ��Ƶ�ַ	*/
	unsigned char			baud;						/**< ������		*/
	unsigned char			protocol;					/**< ��Լ����	*/
	OOP_OAD_U		port;						/**< �˿�		*/
	char			password[12];		/**< ͨ������	*/
	unsigned char			pwdLen;						/**< ���볤��	*/
	unsigned char			rateNum;					/**< ���ʸ���	*/
	unsigned char			userType;					/**< �û�����	*/
	unsigned char           level;                      /**< �豸���� */
	unsigned char			pwrType;					/**< ���߷�ʽ	*/
	unsigned short 			stVolt;						/**< ���ѹ	*/
	unsigned short			stAmp;						/**< �����	*/
}OOP_METER_BASIC_T;

typedef struct tag_OOP_METER
{
	unsigned short			nIndex;		/**< �������	*/
	OOP_METER_BASIC_T		basic;		/**< ������Ϣ	*/

}OOP_METER_T;


extern 	OOP_METER_T 			  g_ParaMet[MAX_MET_NUM];

void meter_data_init( void );

void meter_data_update( void );

int meter_para_load( const char *szFile );

float el_ene_get(     unsigned char level,unsigned char nPhase);

unsigned char meter_level_check( unsigned short metid);





#endif

