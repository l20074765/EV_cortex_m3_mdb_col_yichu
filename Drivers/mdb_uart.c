/****************************************Copyright (c)*************************************************
**                      Fujian Junpeng Communicaiton Technology Co.,Ltd.
**                               http://www.easivend.com.cn
**--------------File Info------------------------------------------------------------------------------
** File name:           mdb_uart.h
** Last modified Date:  2013-01-08
** Last Version:         
** Descriptions:        uart2配置MDB 从机模式 收发数据                   
**------------------------------------------------------------------------------------------------------
** Created by:          yoc
** Created date:        2015-04-20
** Version:             V0.1
** Descriptions:        The original version       
********************************************************************************************************/

#include "mdb_uart.h"
#include "..\config.h"

#define MDB_DEBUG
#ifdef MDB_DEBUG
#define print_mdb(...)	Trace(__VA_ARGS__)
#else
#define print_mdb(...)
#endif



#define MDB_BUF_SIZE	36

static volatile uint8  recvbuf[MDB_BUF_SIZE];
static volatile uint8  rx;
static volatile uint8  tx;
static volatile uint8  crc;


#define MDB_DEV_IDLE			0
#define MDB_DEV_START			1
#define MDB_DEV_RECV_ACK		2


static volatile uint8 mdb_status = MDB_DEV_IDLE;
static volatile uint8 mdb_addr = 0;
static volatile uint8 mdb_cmd = 0;

static volatile uint8 mdb_cur_no = 0;

/*********************************************************************************************************
** MDB通信
*********************************************************************************************************/
volatile uint8 m_mdbStatus[MDB_BIN_SIZE] = {MDB_COL_IDLE};
volatile uint8 m_mdbSendStatus[MDB_BIN_SIZE] = {MDB_COL_IDLE};


ST_MDB stMdb[MDB_BIN_SIZE];


uint8 MDB_binIsExsit(uint8 no)
{
	if(no == 0 || no > MDB_BIN_SIZE){ //柜号不符
		return 0;
	}
	if(stMdb[no - 1].exsit == 1){
		return 1;
	}
	else{
		return 0;
	}

}






/*********************************************************************************************************
** Function name:     	MDB_getRequest
** Descriptions:	    查询MDB请求状态
** input parameters:    mdb 结构体指针的指针
** output parameters:   有请求则会将请求的结构体指针赋值给 mdb
** Returned value:      0 无请求 1有请求
*********************************************************************************************************/
uint8 MDB_getRequest(ST_MDB **mdb)
{
	uint8 i;
	for(i = 0;i < MDB_BIN_SIZE;i++){
		if(m_mdbStatus[i] == MDB_COL_BUSY){
			*mdb = &stMdb[i];
		//	print_mdb("MDB_getRequest:bin_no=%d\r\n",i + 1);
			return (*mdb == NULL) ? 0 : 1;
		}
	}
	
	return 0;
}

/*********************************************************************************************************
** Function name:     	MDB_getReqStatus
** Descriptions:	    查询MDB请求状态
** input parameters:    无
** output parameters:   无
** Returned value:      0 无请求 1正在处理请求 2处理完成
*********************************************************************************************************/
uint8 MDB_getStatus(uint8 addr)
{
	if(addr > 0 && addr <= MDB_BIN_SIZE){
		return m_mdbStatus[addr - 1];
	}
	else{
		return MDB_COL_N_A;
	}
}





/*********************************************************************************************************
** Function name:     	MDB_SetRequest
** Descriptions:	    设置MDB请求
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void MDB_setStatus(uint8 addr,uint8 s)
{
	if(addr > 0 && addr <= MDB_BIN_SIZE)
		m_mdbStatus[addr - 1] = s;
	
}

/*********************************************************************************************************
** Function name:     	MDB_getReqStatus
** Descriptions:	    查询MDB请求状态
** input parameters:    无
** output parameters:   无
** Returned value:      0 无请求 1正在处理请求 2处理完成
*********************************************************************************************************/
uint8 MDB_getSendStatus(uint8 addr)
{
	if(addr > 0 && addr <= MDB_BIN_SIZE){
		return m_mdbSendStatus[addr - 1];
	}
	else{
		return MDB_COL_N_A;
	}
}

/*********************************************************************************************************
** Function name:     	MDB_SetRequest
** Descriptions:	    设置MDB请求
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void MDB_setSendStatus(uint8 addr,uint8 s)
{
	if(addr > 0 && addr <= MDB_BIN_SIZE){
		m_mdbSendStatus[addr - 1] = s;
	}
}










static void MDB_recv_ack(uint8 cmd)
{
	if(cmd == POLL){
		if(MDB_getSendStatus(mdb_cur_no) != MDB_COL_BUSY){
			MDB_setStatus(mdb_cur_no,MDB_COL_IDLE);
			MDB_setSendStatus(mdb_cur_no,MDB_COL_IDLE);
		}
	}	
}


/*********************************************************************************************************
** Function name:     	uart2Init
** Descriptions:	    串口初始化，设置为8位数据位，1位停止位，波特率为9600
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart2Init(void)
{	
	PCONP = PCONP | (1<<24);
    U2LCR  = 0x80;                         
    U2DLM  = 0;
    U2DLL  = 125;
	U2FDR  = 0x41; 
    U2LCR  = 0x03;                      
    U2FCR  = 0x07;                      
    U2IER  = 0x05;
	uart2SetParityMode(PARITY_F_0);//初始化需要将串口设置成0校验模式 用于接收地址
	uart2Clear();
	memset((void *)recvbuf,0x00,MDB_BUF_SIZE);                       
    zyIsrSet(NVIC_UART2,(unsigned long)Uart2IsrHandler,PRIO_ONE); 
	
	
}



/*********************************************************************************************************
** Function name:     	uart2Clear
** Descriptions:	    清除串口2接收缓冲区
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart2Clear(void) 
{
	rx = 0;
	tx = 0;
}

/*********************************************************************************************************
** Function name:     	uart2SetParityMode
** Descriptions:	    设置串口奇偶校验位
** input parameters:    mode:奇偶校验设置,即设置MDB协议的标志位
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart2SetParityMode(unsigned char mode) 
{
	switch(mode) 
	{		
		case PARITY_ODD	: 	U2LCR = UART_LCR_PARITY_ODD|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		case PARITY_EVEN:	U2LCR = UART_LCR_PARITY_EVEN|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		case PARITY_F_1	:	U2LCR = UART_LCR_PARITY_F_1|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		case PARITY_F_0	:	U2LCR = UART_LCR_PARITY_F_0|UART_LCR_PARITY_EN|UART_LCR_WLEN8;
						 	break;
		default:		 	U2LCR = UART_LCR_WLEN8;
						 	break;
	}
}



/*********************************************************************************************************
** Function name:     	Uart2PutCh
** Descriptions:	    向串口2发送一个字节
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Uart2PutCh(unsigned char dat) 
{
   while(!(U2LSR & UART_LSR_THRE));
   U2THR = dat; 
}




/*********************************************************************************************************
** Function name:     	MDB_putChr
** Descriptions:	    向MDB总线上送出一个字节的数据
** input parameters:    dat--要送的数据；mode--发送方式：0为地址字节/1为数据字节;按MDB协议处理
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void MDB_putChr(unsigned char dat,unsigned char mode)
{
	switch(mode) 
	{
		case MDB_DAT :	uart2SetParityMode(PARITY_F_0); //强制0 校验 发送MDB数据
						Uart2PutCh(dat);
						break;
		case MDB_ADD:	uart2SetParityMode(PARITY_F_1); // 强制1校验 发送MDB地址
						Uart2PutCh(dat);
						break;
		default	:		uart2SetParityMode(PARITY_DIS);
						Uart2PutCh(dat);
						break;
	}
	while(!(U2LSR & UART_LSR_TEMT));
	
}



/*********************************************************************************************************
** Function name:     	Uart2IsrHandler
** Descriptions:	    串口2中断处理函数
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void Uart2IsrHandler(void) 
{
	volatile uint8 dummy,udata;
	volatile uint32 intsrc, tmp, tmp1;
	OSIntEnter();	
	intsrc = U2IIR;									//Determine the interrupt source 
	tmp = intsrc & UART_IIR_INTID_MASK;				//UART_IIR_INTID_MASK = 0x0000000E,U2IIR[3:1]为中断标识
	//U2IIR[3:1] = 011;RLS接收线状态，产生该中断为四个条件之一(OE,PE,FE,BI);需要通过查看LSR寄存区得到错误原因
	
	
	if(tmp == UART_IIR_INTID_RLS) 
	{
		tmp1 = U2LSR;
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_BI | UART_LSR_RXFE);
		if(tmp1 & UART_LSR_PE)//每次VMC发送完MDB数据后
		{
			udata = U2RBR & UART_RBR_MASKBIT;	//数据可用	
			if(mdb_status == MDB_DEV_IDLE){ //收到第一个字节 即为地址号
				mdb_addr = udata & 0xF8;
				mdb_cmd = udata & 0x07;
				if(MDB_colAddrIsOk(mdb_addr)){ //地址正确 开始接收MDB数据
					mdb_status = MDB_DEV_START;
					rx = 0;
					recvbuf[rx++] = udata;
					crc = udata;//校验码
				}			
			}				
		}
		else if(tmp1)//其他错误引起的中断则忽略掉 
		{		
		    dummy = U2RBR & UART_RBR_MASKBIT;
		}
	}
	//U2IIR[3:1]=010接收数据可用 || U2IIR[3:1]=110字符超时指示	  
	if((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI))  
	{	    
		udata = U2RBR & UART_RBR_MASKBIT;
		if(mdb_status == MDB_DEV_START){
			recvbuf[rx++] = udata;
			if(rx < MDB_BUF_SIZE){
				if(MDB_recvOk(rx)){ //接收数据完成准备发送回应 必须尽快
					if(crc == recvbuf[rx - 1]){
						if(MDB_analysis() == 0){
							mdb_status = MDB_DEV_IDLE;
						}
						else{
							//易触协议 不是标准 MDB
							MDB_recv_ack(mdb_cmd);
							mdb_status = MDB_DEV_IDLE;
							//mdb_status = MDB_DEV_RECV_ACK;
						}
						
					}
					else{
						mdb_status = MDB_DEV_IDLE;
					}
				}
				else{
					crc += udata;
				}
			}
			else{
				mdb_status = MDB_DEV_IDLE;
			}
		}
		else if(mdb_status == MDB_DEV_RECV_ACK){
			mdb_status = MDB_DEV_IDLE;
			if(udata == MDB_ACK){ //收到主机的ACK 至此确定主机收到从机的数据包
				mdb_status = MDB_DEV_IDLE;
				MDB_recv_ack(mdb_cmd);
			}
		}
	}
    OSIntExit();
}







unsigned char MDB_colAddrIsOk(unsigned char addr)
{
	//print_mdb("MDB_colAddrIsOk:addr=%x\r\n",addr);
	return (addr == MDB_ADDR);
}


unsigned char MDB_recvOk(unsigned char len)
{
	unsigned char ok = 0;
	switch(mdb_cmd){
		case RESET : case COLUMN :case POLL :
			if(len >= 3) ok = 1;
			break;
		case SWITCH: 
			if(len >= 5) ok = 1;
			break;
		case CTRL:	
			if(len >= 7) ok = 1;
			break;
		default:break;
	}
	#if 0
	if(ok == 1){
		print_mdb("MDB-RECV[%d]:",len);
		for(i = 0;i < len;i++){
			print_mdb("%02x ",recvbuf[i]);
		}
		print_mdb("\r\n");
	}
	#endif
	//print_mdb("MDB_recvOk:cmd=%x,len=%d,ok=%x\r\n",mdb_cmd,len,ok);
	
	return ok;
}


void MDB_sendACK(uint8 ack)
{
	if(ack == 1){
		MDB_putChr(MDB_ACK,MDB_ADD);
	}
	else{
		MDB_putChr(MDB_NAK,MDB_ADD);
	}
	uart2SetParityMode(PARITY_F_0);
	
}


uint8 MDB_send(uint8 *data,uint8 len)
{
	uint8 i,crc = 0; 
	//OSIntEnter(); //此函数已在中断中运行 中断已经关闭了
	if(len == 0){
		MDB_putChr(MDB_ACK,MDB_ADD);
	}
	else{
		for(i = 0;i < len;i++){
			MDB_putChr(data[i],MDB_DAT);
			crc += data[i];
		}
		MDB_putChr(crc,MDB_ADD);
		
		#if 0
		print_mdb("MDB-SEND[%d]:",len + 1);
		for(i = 0;i < len;i++){
			print_mdb("%02x ",data[i]);
		}
		print_mdb("%02x ",crc);
		print_mdb("\r\n");
		#endif
	}
	uart2SetParityMode(PARITY_F_0);
	//OSIntExit();
	return 1;	
}




static uint8 MDB_poll_rpt(void)
{
	uint8 buf[4] = {0};
	buf[0] = MDB_ADDR + POLL;
	buf[1] = recvbuf[1];
	buf[2] = MDB_getStatus(buf[1]);
	MDB_setSendStatus(buf[1],buf[2]);	
	MDB_send(buf,3);
	print_mdb("POLL:State[%d] = %d\r\n",buf[1],buf[2]);
	return 1;
}




static uint8 MDB_reset_rpt(ST_MDB *mdb)
{
	uint8 buf[2] = {0};
	buf[0] = MDB_ADDR + RESET;
	buf[1] = recvbuf[1];
	
	MDB_send(buf,2);
	return 1;
}


static uint8 MDB_switch_rpt(ST_MDB *mdb)
{

	uint8 buf[2] = {0};
	buf[0] = MDB_ADDR + SWITCH;
	buf[1] = recvbuf[1];
	mdb->bin_no = recvbuf[1];
	if(MDB_getStatus(mdb->bin_no) == MDB_COL_BUSY){ 
		//MDB_sendACK(0); //忙状态不回应
		print_mdb("Switch:Busy--bin_no = %d\r\n",mdb->bin_no);
		return 0;
	}
	else{
		mdb->cmd = G_MDB_SWITCH;
		mdb->bin_no = recvbuf[1];
		mdb->col_no = recvbuf[2];
		MDB_setStatus(mdb->bin_no,MDB_COL_BUSY);
		MDB_send(buf,2);
		print_mdb("Switch:bin_no = %d,col_no=%d,bin_addr=%d,col_addr=%d\r\n",
			mdb->bin_no,mdb->col_no,mdb->bin_addr,mdb->col_addr[mdb->col_no - 1]);
		return 1;
	}
	
}





static uint8 MDB_ctrl_rpt(ST_MDB *mdb)
{
//	MDB_CTRL *ctrl;
	uint8 buf[2] = {0};
	buf[0] = MDB_ADDR + CTRL;
	buf[1] = recvbuf[1];
	MDB_send(buf,2);
	return 1;
	#if 0
	if(MDB_getStatus(mdb->bin_no) == MDB_COL_BUSY){
		//MDB_sendACK(0);
		return 0;
	}
	else{
		mdb->cmd = G_MDB_CTRL;
		ctrl = &mdb->ctrl;
		ctrl->coolCtrl = recvbuf[2] & 0x01;
		ctrl->lightCtrl = (recvbuf[2] >> 1) & 0x01;
		ctrl->hotCtrl = (recvbuf[2] >> 3) & 0x01;
		ctrl->coolTemp = (int8)recvbuf[3];
		ctrl->hotTemp  = (int8)recvbuf[4];	
		MDB_setStatus(mdb->bin_no,MDB_COL_BUSY);
		MDB_send(buf,2);
		return 1;
	}
	#endif
}

static uint8 MDB_column_rpt(ST_MDB *mdb)
{
	uint8 index = 0,i,temp;
	uint8 buf[36] = {0};
	
	buf[index++] = MDB_ADDR + COLUMN;
	buf[index++] = recvbuf[1];
	//货道最大40个货道
	for(i = 0;i < 6;i++){
		temp = 0;
		buf[index++] = temp;
	}
	
	buf[index++] = mdb->sum;
	buf[index++] = 0x00; //reserved
	buf[index++] = 0x00; //左室温度
	buf[index++] = 0x00; //预留
	buf[index++] = 0x00; //右室温度

	MDB_send(buf,index);	
	return 1;
	
	
}




uint8 MDB_analysis(void)
{
	ST_MDB *mdb = NULL;
	uint8 res = 0;
	mdb_cur_no = recvbuf[1];
	//print_mdb("MDB_analysis:no = %d,type=%x\r\n",mdb_cur_no,mdb_cmd);
	if(mdb_cur_no == 0 || mdb_cur_no > MDB_BIN_SIZE){ //柜号不符
		return 0;
	}
	
	mdb = &stMdb[mdb_cur_no - 1];
	if(MDB_binIsExsit(mdb_cur_no) == 0){ //柜子不存在则不能应答
		//print_mdb("Exsit[NONE]\r\n");
		return 0;
	}
	
	//print_mdb("Exsit[OK]\r\n");
	switch(mdb_cmd){
		case RESET : 
			res = MDB_reset_rpt(mdb);
			break;
		case SWITCH:
			res = MDB_switch_rpt(mdb);
			break;
		case CTRL:
			res = MDB_ctrl_rpt(mdb);
			break;
		case POLL:
			res = MDB_poll_rpt();
			break;
		case COLUMN:
			res = MDB_column_rpt(mdb);
			break;
		default:res = 0;break;
	}
	//延时
	return res;
}	


