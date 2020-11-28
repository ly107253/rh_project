#ifndef _DBASE_MSG_INTERFACE_H__
#define _DBASE_MSG_INTERFACE_H__

//��ϢID
#define EL_KEY_ID      1024

//����app����
#define EL_APP_NAME    "elAPP"
#define RH_APP_NAME    "rhApp"


#define IID_EL            1

//��Ϣ��ʱĬ��ʱ��
#define WAITE_TIMEOUT     30


/**
 ******************************************************************************
 @brief ��Ϣ������
 @verbatim
		REQ ��= SEQUENCE
		{
			��Ϣ���		     long-unsigned��
			�������ͱ�ʶ           double-long-unsigned��
		}
		ACK ��= SEQUENCE
		{
			��������             oct-string;
		}
 @endverbatim
 ******************************************************************************
 */
#define IOP_DBASE_READ     0x10  

/**
 ******************************************************************************
 @brief ��Ϣд����
 @verbatim
		REQ ��= SEQUENCE
		{
			��Ϣ���		     long-unsigned��
			�������ͱ�ʶ           double-long-unsigned��
			��������             oct-string;
		}
		ACK ��= BOOL

 @endverbatim
 ******************************************************************************
 */
#define IOP_DBASE_WRITE    0x11


//��
#define ID_IBJ_60510201           0x60510201           //������                 double-long-unsigned��  
#define ID_IBJ_60510202           0x60510202           //�۵���                 double-long-unsigned��
#define ID_IBJ_60510203           0x60510203           //���͵���                double-long-unsigned��
#define ID_IBJ_60510204           0x60510204           //�ֲ�ʽ��Դ��������           double-long-unsigned
#define ID_IBJ_60510205           0x60510205           //������                 long�����㣺-2����λ��%��
#define ID_IBJ_60510206           0x60510206           //�쳣��־                ����			��0���ܱ�����쳣	��1����������쳣	��2��

//A��
#define ID_IBJ_61510201           0x61510201           //������                 double-long-unsigned��  
#define ID_IBJ_61510202           0x61510202           //�۵���                 double-long-unsigned��
#define ID_IBJ_61510203           0x61510203           //���͵���                double-long-unsigned��
#define ID_IBJ_61510204           0x61510204           //�ֲ�ʽ��Դ��������           double-long-unsigned
#define ID_IBJ_61510205           0x61510205           //������                 long�����㣺-2����λ��%��
#define ID_IBJ_61510206           0x61510206           //�쳣��־                ����			��0���ܱ�����쳣	��1����������쳣	��2��

//B��
#define ID_IBJ_62510201           0x62510201           //������                 double-long-unsigned��  
#define ID_IBJ_62510202           0x62510202           //�۵���                 double-long-unsigned��
#define ID_IBJ_62510203           0x62510203           //���͵���                double-long-unsigned��
#define ID_IBJ_62510204           0x62510204           //�ֲ�ʽ��Դ��������           double-long-unsigned
#define ID_IBJ_62510205           0x62510205           //������                 long�����㣺-2����λ��%��
#define ID_IBJ_62510206           0x62510206           //�쳣��־                ����			��0���ܱ�����쳣	��1����������쳣	��2��

//C��
#define ID_IBJ_63510201           0x63510201           //������                 double-long-unsigned��  
#define ID_IBJ_63510202           0x63510202           //�۵���                 double-long-unsigned��
#define ID_IBJ_63510203           0x63510203           //���͵���                double-long-unsigned��
#define ID_IBJ_63510204           0x63510204           //�ֲ�ʽ��Դ��������           double-long-unsigned
#define ID_IBJ_63510205           0x63510205           //������                 long�����㣺-2����λ��%��
#define ID_IBJ_63510206           0x63510206           //�쳣��־                ����			��0���ܱ�����쳣	��1����������쳣	��2��


#endif
