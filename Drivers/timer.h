/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           timer.c
** Last modified Date:  2013-01-07
** Last Version:         
** Descriptions:        ��ʱ����������ͷ�ļ�                   
**------------------------------------------------------------------------------------------------------
** Created by:          sunway 
** Created date:        2013-01-07
** Version:             V0.1 
** Descriptions:        The original version       
********************************************************************************************************/
#ifndef __TIMER_H 
#define __TIMER_H

//==============================��ʱ�����������ṹ��========================================================
//ע���������Ϊunsigned short �ͣ�ֻ�趨�弴��ʹ��


typedef struct _timer_st_{
	unsigned short bentoTimeout;
}TIMER_ST;
extern TIMER_ST Timer;




void InitTimer(unsigned char TimerNumb,unsigned int TimerInterval);
void TIMER0_IRQHandler (void);
void TIMER1_IRQHandler (void);
void TIMER2_IRQHandler (void);
void TIMER3_IRQHandler (void);

#endif
/**************************************End Of File*******************************************************/
