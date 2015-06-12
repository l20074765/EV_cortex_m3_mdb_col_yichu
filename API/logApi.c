
/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           common.h
** Last modified Date:  2013-01-06
** Last Version:        No
** Descriptions:        ϵͳ�������弰ͨ�ú�������                     
**------------------------------------------------------------------------------------------------------
** Created by:          yanbo
** Created date:        20134-02-18
** Version:             V0.1
** Descriptions:        The original version        
********************************************************************************************************/


#include "..\config.h"


#if 0

/*********************************************************************************************************
** Function name:       save_data
** Descriptions:        ������Ϣ
** input parameters:    ptr��д��flash������ָ�룬
						len д�볤�� ��󳤶Ȳ�����240�ֽ�
						addrд�����ʼ��ַ
** output parameters:   ��
** Returned value:     0 ʧ��   1�ɹ�
*********************************************************************************************************/
static unsigned char  save_data(void *ptr,unsigned short len,unsigned short addr)
{
	unsigned short crc;
	unsigned char buf[256] = {0};//256�ֽ� ��8�δ洢
	if(!ptr || !len || (len > 240))
		return 0;
	
	memcpy(buf,ptr,len);
	crc = CrcCheck(buf,len);
	buf[len + 0] = HUINT16(crc);
	buf[len + 1] = LUINT16(crc);
	len += 2;
	saveFlash(addr,buf,256);	
	return 1;

}

/*********************************************************************************************************
** Function name:       read_data
** Descriptions:        ��ȡ��Ϣ
** input parameters:    
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
static unsigned char read_data(void *ptr,unsigned short len,unsigned short addr)
{
	unsigned short crc;
	unsigned char buf[256] = {0};
	if(!ptr || !len || (len > 240))
			return 0;
	
	readFlash(addr,buf,256);	
	crc = CrcCheck(buf,len);
	if(crc == INTEG16(buf[len],buf[len + 1]))
	{
		memcpy(ptr,buf,len);
		return 1;
	}	
	return 0;
}




#endif







