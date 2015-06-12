#include "deviceTask.h"
#include "..\config.h"

//#define DEV_DEBUG
#ifdef DEV_DEBUG

#define print_dev(...)	Trace(__VA_ARGS__)
#else
#define print_dev(...)
#endif

//����������
OS_EVENT *g_KeyMsg;
void *KeyMsgArray[2]; // ������������

extern OS_EVENT *g_mdb_event; //����MDb�¼�



/*********************************************************************************************************
** Function name:       CreateMBox
** Descriptions:        Ϊ����֮��ͨ�Ŵ���������ź���
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void CreateMBox(void)
{
	//������������
	g_KeyMsg = OSQCreate(&KeyMsgArray[0],2);
}

/*********************************************************************************************************
** Function name:       SystemInit
** Descriptions:        ϵͳ��ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void SystemInit()
{
	InitUart0();
	InitUart1();
	uart2Init();
	InitUart3();
    InitBuzzer();
	InitI2C0();
	InitTimer(0,240000);
	
}



//���� 
static void DEV_mdbSwitch(ST_MDB *mdb)
{
	uint8 res;
	res = BT_open(mdb->binNo,mdb->sw.col);
	if(res == 1){
		MDB_setStatus(mdb->mdbAddr,MDB_COL_SUCCESS);
	}
	else{
		MDB_setStatus(mdb->mdbAddr,MDB_COL_FAILED);
	}
	
}

static void DEV_mdbCtrl(ST_MDB *mdb)
{
	uint8 res;
	if(mdb->bin.islight == 1){
		res = EV_bento_light(mdb->binNo,mdb->ctrl.lightCtrl);
	}
	
	if(mdb->bin.iscool == 1){
		res = EV_bento_light(mdb->binNo,mdb->ctrl.coolCtrl);
	}
	if(mdb->bin.ishot == 1){
		res = EV_bento_light(mdb->binNo,mdb->ctrl.hotCtrl);
	}
	
	
	if(res == 1){
		MDB_setStatus(mdb->mdbAddr,MDB_COL_IDLE);
	}
	else{
		MDB_setStatus(mdb->mdbAddr,MDB_COL_ERROR);
	}
	
}


static void DEV_mdbReset(ST_MDB *mdb)
{
	uint8 res;
	print_dev("DEV_mdbReset:%d\r\n",mdb->binNo);
	res = 1;
	//res = EV_bento_check(mdb->binNo,&mdb->bin);
	if(res == 1){
		MDB_setStatus(mdb->mdbAddr,MDB_COL_JUSTRESET);
	}
	else{
		MDB_setStatus(mdb->mdbAddr,MDB_COL_ERROR);
	}
	//print_dev("MDB_getRequest() = %d\r\n",MDB_getStatus(mdb->mdbAddr));
	
}





void DEV_taskPoll(void)
{
	ST_MDB *mdb = NULL;
	if(MDB_getRequest(&mdb) == 1){
		switch(mdb->cmd){
			case G_MDB_RESET:
				DEV_mdbReset(mdb);
				break;
			case G_MDB_SWITCH:
				DEV_mdbSwitch(mdb);
				break;
			case G_MDB_CTRL:
				DEV_mdbCtrl(mdb);
				break;
			default:break;
		}
	}
}




void DEV_task(void *pdata)
{	
	//uint32 i;
	//ϵͳ�����ӿڳ�ʼ��
	SystemInit();
	//print_dev("Hello Booooo...\r\n");
	FIO2DIR &= ~(0x01UL << 2);
	//�������䡢�ź���	
	CreateMBox();
	MDB_binInit();//��ʼ������
	while(1){
		DEV_taskPoll();
		msleep(20);
	}
}



