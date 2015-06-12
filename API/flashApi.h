#ifndef _FLASH_API_H
#define _FLASH_API_H

//=======================flashҳ��==============================

//flashҳ����󲻳��� 0x1FFF = 8191 ���� flash�ᷭת��ַΪ0x0000
//ϵͳ����д��flash  add by yoc 2014.2.19
#define ADD_SYSTEM_PARA     (256)  //Ԥ��256bytes�ռ�洢
//#define ADD_SYSTEM_PARA_LEN (2) // 32 * 8

//�û������� д��flash��ַ
#define ADD_USER_DATA		(512)
//#define ADD_USER_LEN        (2)//�û����洢���� 256�ֽ�

//������Ϣ�洢
#define ADD_HUODAO_INFO     (768)  //Ԥ��512bytes�ռ�洢
//#define ADD_HUODAO_LEN		(2) //�����洢���� 512�ֽ�

//���׼�¼�洢����
#define ADD_TRADE_INFO      (1280)
#define ADD_TRADE_PAGE_LEN  (64)//ÿ�ʽ��׼�¼���� 64 �ֽ�


//=======================�������ʹ洢==============================
#define TYPE_HUODAO_ADDR			0x0001
#define TYPE_HUODAO_PRICE			0x0002
#define TYPE_HUODAO_STATE		    0x0004
#define TYPE_HUODAO_OPEN_FLAG		0x0008
#define TYPE_HUODAO_REMAIN			0x0010
#define TYPE_HUODAO_TRADE_COUNT		0x0020
#define TYPE_HUODAO_MAX_COUNT		0x0040
#define TYPE_HUODAO_PHYSIC_NO		0x0080
#define TYPE_HUODAO_LEVEL_OPEN      0x0100
#define TYPE_HUODAO_LOGIC      		0x0200

#define TYPE_HUODAO_ALL				0xFFFF

//=====================================================================

unsigned char saveFlash(unsigned int addr,unsigned char *buf,unsigned short len);
unsigned char readFlash(unsigned int addr,unsigned char *buf,unsigned short len);


#endif
