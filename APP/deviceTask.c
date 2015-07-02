#include "deviceTask.h"
#include "..\config.h"

#define DEV_DEBUG
#ifdef DEV_DEBUG

#define print_dev(...)	Trace(__VA_ARGS__)
#else
#define print_dev(...)
#endif



extern OS_EVENT *g_mdb_event; //声明MDb事件


/*********************************************************************************************************
** Function name:       SystemInit
** Descriptions:        系统初始化
** input parameters:    无
** output parameters:   无
** Returned value:      无
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



//出货 
static void DEV_mdbSwitch(ST_MDB *mdb)
{
	uint8 res,col;
	print_dev("DEV_mdbSwitch:bin_no = %d,col_no=%d,bin_addr=%d,col_addr=%d\r\n",
			mdb->bin_no,mdb->col_no,mdb->bin_addr,mdb->col_addr[mdb->col_no - 1]);
	if(mdb->col_no == 0){
		MDB_setStatus(mdb->bin_no,MDB_COL_FAILED);
		return;
	}
	col = mdb->col_addr[mdb->col_no - 1];
	res = BT_open(mdb->bin_addr,col);
	
	print_dev("DEV_mdbSwitch:res=%d\r\n",res);
	if(res == 1){
		MDB_setStatus(mdb->bin_no,MDB_COL_SUCCESS);
	}
	else{
		MDB_setStatus(mdb->bin_no,MDB_COL_FAILED);
	}
}

static void DEV_mdbCtrl(ST_MDB *mdb)
{
	uint8 res;
	if(mdb->islight == 1){
		res = EV_bento_light(mdb->bin_addr,mdb->ctrl.lightCtrl);
	}
	
	if(mdb->iscool == 1){
		res = EV_bento_col(mdb->bin_addr,mdb->ctrl.coolCtrl);
	}
	if(mdb->ishot == 1){
		res = EV_bento_hot(mdb->bin_addr,mdb->ctrl.hotCtrl);
	}

	if(res == 1){
		MDB_setStatus(mdb->bin_no,MDB_COL_IDLE);
	}
	else{
		MDB_setStatus(mdb->bin_no,MDB_COL_IDLE);
	}
	
}


static void DEV_mdbInit(void)
{
	uint8 res,sum,i,col,j,z;
	ST_BIN bin;
	uint8 binAddr = 1;
	uint8 exsit[MDB_BIN_SIZE] = {0};

	for(i = 0;i < MDB_BIN_SIZE;i++){
		memset((void *)&stMdb[i],0,sizeof(ST_MDB));
	}
	
	for(i = 0;i < MDB_BIN_SIZE;){
		print_dev("DEV_mdbInit:[%d]\r\n",binAddr);
		for(j = 0;j < 2;j++){
			res = EV_bento_check(binAddr,&bin);
			if(res == 1){
				break;
			}
		}
		if(res != 1){
			break;
		}
		
		sum = bin.sum;
		col = 1;
		while(sum >= 40){
			sum -= 40;
			stMdb[i].sum = 40;
			for(z = 0;z < 40;z++){
				stMdb[i].col_addr[z] = col++;
			}
			stMdb[i].iscool = bin.iscool;
			stMdb[i].ishot = bin.ishot;
			stMdb[i].islight = bin.islight;
			stMdb[i].bin_addr = binAddr;
			stMdb[i].bin_no = i + 1;
			exsit[i] = 1;
			i++;
		}
		if(sum > 0){
			stMdb[i].sum = sum;
			for(z = 0;z < sum;z++){
				stMdb[i].col_addr[z] = col++;
			}
			stMdb[i].iscool = bin.iscool;
			stMdb[i].ishot = bin.ishot;
			stMdb[i].islight = bin.islight;
			stMdb[i].bin_addr = binAddr;
			stMdb[i].bin_no = i + 1;
			exsit[i] = 1;
			i++;
		}
		binAddr++;	
	}
	
	
	for(i = MDB_BIN_SIZE;i > 0;i--){
		stMdb[i - 1].exsit = exsit[i - 1];
		print_dev("stMdb[%d].exsit = %d\r\n",i - 1,stMdb[i - 1].exsit);
	}
	
}





void DEV_taskPoll(void)
{
	ST_MDB *mdb = NULL;
	
	if(MDB_getRequest(&mdb) == 1){
		switch(mdb->cmd){
			case G_MDB_RESET:
				//DEV_mdbReset(mdb);
				break;
			case G_MDB_SWITCH:
				print_dev("G_MDB_SWITCH:\r\n");
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
	//系统基本接口初始化
	SystemInit();
	FIO2DIR &= ~(0x01UL << 2);
	print_dev("DEV_task:start....\r\n");
	msleep(500);
	DEV_mdbInit();
	while(1){
		DEV_taskPoll();
		msleep(20);
	}
}



