#include "flashAPi.h"
#include "../config.h"





/*********************************************************************************************************
** Function name:       saveFlash
** Descriptions:        �������ݵ�flash��
** input parameters:    page��ҳ�ţ�buf����Ҫ�������������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
unsigned char saveFlash(unsigned int page,unsigned char *buf,unsigned short len)
{
	if(!buf)
		return 0;
	return I2C0WrNByte(0xa0,2,page,buf,len);
}


/*********************************************************************************************************
** Function name:       readFlash
** Descriptions:        ��ȡָ��Flash��ַ�ϵ�����
** input parameters:    addr����ַһ�ι̶���ȡ256�ֽ�
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
unsigned char readFlash(unsigned int page,unsigned char *buf,unsigned short len)
{
	if(!buf)
		return 0;
	
	return I2C0RdNByte(0xa0,2,page,buf,len);
}









