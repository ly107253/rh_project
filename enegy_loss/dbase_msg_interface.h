#ifndef _EL_DBASE_H__
#define _EL_DBASE_H__

//��ϢID
#define EL_KEY_ID      1024

//����app����
#define EL_APP_NAME    "elAPP"


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
			��Ϣ���		     long-unsigned��
			�������ͱ�ʶ           double-long-unsigned��
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








#endif
