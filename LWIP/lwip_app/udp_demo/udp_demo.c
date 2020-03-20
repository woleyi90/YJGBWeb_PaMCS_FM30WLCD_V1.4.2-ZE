#include "udp_demo.h"
#include "lwip_comm.h"
#include "lwip/api.h"
#include "lwip/lwip_sys.h"
#include "lwip/opt.h"
#include "lwip/sockets.h"
#include "string.h"
#include "..\Private\iap.h"
#include "..\Private\ConstValue.h"
#include "includes.h"
#include "global.h"	   

#if 0						  
#define My_Printf(...) \
    do { \
        printf(" \t" __VA_ARGS__); \
        printf(" "); \
    } while (0)
#else
#define My_Printf(...)
#endif
#define SERPORT		7000	//服务器的端口号


void UDP_RemoteRec(u8 *recvline, u16 recvlen, int sfd);
void UDP_BrocastData(char *RevBuf,  int Revlen, int sfd);

int udpsockfd = 0;
struct sockaddr_in servaddr;

struct sockaddr_in clientAddr;
 
static int  clientAddrlen = sizeof(clientAddr);
 
int udpsockfdBC = 0;
struct sockaddr_in BC_from;  

int udp_connected = 0;
int udp_broadcast_con = 0;	

u8 init_udp_com_server(void)
{
	if(udp_connected) return 0;

    udpsockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if( udpsockfd <= 0 ) 
	{
		printf("server : udpsockfd creat error......\n"); 
		return 0;		
	}	
  		printf("udpsockfd ok : %d\n", udpsockfd); 
	// 绑定地址，为服务器端
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERPORT);
  
    if(bind(udpsockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{
		printf("udpsockfd bind error...\n");  
        return 0;		
	}
	printf(" init_udp_com_server ok\n");
	
	udp_connected = 1;
	
	return 1;
}
 
u8 init_udp_broadcast(void)
{
	const int opt = 1;  
    //设置该套接字为广播类型，  
    int nb = 0;  
	
	if(udp_broadcast_con) return 0;

    udpsockfdBC = socket(PF_INET, SOCK_DGRAM, 0);
	if( udpsockfdBC <= 0 ) 
	{
		printf("server : udpsockfdBC creat error......\n"); 
		return 0;		
	}	
  	printf("udpsockfdBC ok : %d\n", udpsockfdBC); 

	//广播地址  
    memset(&BC_from, 0, sizeof(struct sockaddr_in)); 
    BC_from.sin_family = AF_INET;  
    BC_from.sin_addr.s_addr = htonl(INADDR_ANY);  //INADDR_ANY INADDR_BROADCAST
    BC_from.sin_port = htons(8888);  //广播端口为8888
	 

    nb = setsockopt(udpsockfdBC, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));  
    if(nb == -1)  
    {  
        printf("set udpsockfdBC error...\n");  
        return 0;  
    }  
  
 
    if(bind(udpsockfdBC, (struct sockaddr *)&BC_from, sizeof(BC_from)) == -1)
	{
		printf("udpsockfdBC bind error...\n");  
        return 0;		
	}
	printf("\n init_udp_broadcast ok\n");
	udp_broadcast_con = 1;
	return 1;
}
u32_t My_ipaddr(char *buf)
{
	u32_t addrnew = 0;
	addrnew = (buf[3] <<24 | buf[2] <<16 | buf[1] <<8 | buf[0]); 
	return addrnew;
}
void udp_com_server_loop( void )
{
   	
	int n;
	int i;	
    char recvline[1024];
	u32_t ips1,ips2,ips3,ipman,ipman2; 

	if( !udp_connected)
		return;
	
	ips1 = My_ipaddr(g_DI.Cfg.IP_RemoteHost1);
	ips2 = My_ipaddr(g_DI.Cfg.IP_RemoteHost2);
	ips3 = My_ipaddr(g_DI.Cfg.IP_RemoteHost3);
	ipman = My_ipaddr( g_DI.Cfg.IP_LOCAL1);
	ipman2 = My_ipaddr(g_DI.Cfg.IP_LOCAL2);
	

	n = recvfrom(udpsockfd, recvline, 1024,MSG_DONTWAIT, (struct sockaddr *)&clientAddr, &clientAddrlen);//MSG_DONTWAIT:为非阻塞   0:为阻塞
	
	if( n > 0)//有数据接收
	{ 
//		My_Printf("\n udp recv: %d :",n);
//		My_Printf("%s %u \n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
//		My_Printf("IP: 0x%s   port: 0x%x \n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
//		My_Printf("0x%x   0x%x \n", clientAddr.sin_addr, clientAddr.sin_port);
//		My_Printf("says: ");  
//		for(i=0;i<n;i++)
//		{
//			My_Printf(" 0x%x ", (u8)(recvline[i]));  
//		}
//		My_Printf(" \n");           
		if(
			((ips1 == clientAddr.sin_addr.s_addr)&&(clientAddr.sin_port == htons(g_DI.Cfg.RemoteHostNetPort1)))
		   	||((ips2 == clientAddr.sin_addr.s_addr)&&(clientAddr.sin_port == htons(g_DI.Cfg.RemoteHostNetPort2)))
		   	||((ips3 == clientAddr.sin_addr.s_addr)&&(clientAddr.sin_port == htons(g_DI.Cfg.RemoteHostNetPort3)))
			||(ipman == clientAddr.sin_addr.s_addr)
			||(ipman2 == clientAddr.sin_addr.s_addr)
		
		)	
		{
			UDP_RemoteRec(&recvline[0],  n, udpsockfd); 
		}
			
	}

}
void udp_com_Broadcast_loop( void )
{
   	
	int n;
	int i;	
    char recvline[1024];

	if( !udp_broadcast_con)
		return;
    n = recvfrom(udpsockfdBC, recvline, 1024,MSG_DONTWAIT, (struct sockaddr *)&clientAddr, &clientAddrlen);//MSG_DONTWAIT:为非阻塞   0:为阻塞
 
	if( n > 0)//有数据接收
    { 
//       My_Printf("\n BROADCAST udp recv: %d :",n);
//	   My_Printf("%s %u \n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
//	   My_Printf("0x%x   0x%x \n", clientAddr.sin_addr, clientAddr.sin_port);
//	   My_Printf("says: ");   
//	   for(i=0;i<n;i++)
//	   {
//			My_Printf(" 0x%x ", (char)(recvline[i]));  
//	   }
//	   My_Printf(" \n"); 
       //sendto(udpsockfd, " BROADCAST Recv OK !", 50, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
	   UDP_BrocastData(recvline, n, udpsockfdBC); 
    }
			   
}
void deinit_udp_com_server(void)
{
    close(udpsockfd);
	close(udpsockfdBC);
	udpsockfd = 0;
	udpsockfdBC = 0;
	udp_connected = 0;
	udp_broadcast_con = 0;	
}

void thread_udp_broadcast_server( void * arg)
{
	u8 trynum = 0, Result = 0;
	
	while(1)
	{
		init_udp_com_server();
		init_udp_broadcast();
		udp_com_Broadcast_loop();
		udp_com_server_loop();
		if(SysNet_Flag.Bit.fUdpRecom == TRUE)
		{
		 	SysNet_Flag.Bit.fUdpRecom = FALSE;
			deinit_udp_com_server();		
		}
		OSTimeDlyHMSM(0, 0, 0, 50);//延时50ms		
	}
	deinit_udp_com_server();
	return;
}
void myudp_send(int sfd, char *str, int n) 
{ 
  sendto(sfd, str, n, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));   
} 

void ReceiveNetData(u8 *buf, int buflen)
{
	u8 CheckSum = 0;
	u8 CodeData = 0;
	int i=0;   

	if((buf[0] == 0x55) && (buf[1] == 0xaa) && (buf[2] == 0x01))
	{
		CodeData = buf[4];
	
	    for(i = 0; i < buflen; ++i)
		{
		  CheckSum = CheckSum + buf[i];
		}
	
	    if(CheckSum == 0)
	    {
			switch(CodeData)
			{
			    case SERIAL_CODE_SET_STM32_TO_BOOT:
			    {
					STMFLASH_WtiteU16DataToFlash(FLASH_ADDR_UPDATE_FLAG, FLAG_TO_RESET);  
					//STMFLASH_WtiteU16DataToFlash(FLASH_ADDR_UPDATE_FLAG, FLAG_TO_BOOT);  
					if(((*(vu32*)(FLASH_ADDR_BOOT + 4)) & 0xFF000000) == 0x08000000)//判断是否为0X08XXXXXX.
					{	 
						Iap_Load_App(FLASH_ADDR_BOOT);//执行FLASH APP代码
					}
			
			    }break;
			  	default:
				 break;
			}							
	    }
	}
	
}
void UDP_RemoteRec(u8 *recvline, u16 recvlen, int sfd)
{
	u8   *nptr; 
	u8   *recdata;
	u8   outbuff[512];
	u16 len,outlen,c; 
	
	len = recvlen;									//读取数据长度 
	nptr = (u8 *)recvline; 							//取得数据起始指针 	 
	recdata	= &recvline[0];
	if(len<26)
	{
	 	ReceiveNetData(recdata,len);
	 	return;
	}
		
	if(nptr[0]== 0x26 )    
	{    
		
		
		outlen = PC_Protocol(&recdata[20], len-20, 0);
		if( outlen >0 )
		{ 	
			len = 0;
			outbuff[0] = '%';
			for(len=0;len<17;len++)
				 outbuff[len+1] = g_DI.Cfg.StationInfo[len];
			outbuff[18] = (u8)(cDevTypeCode);
			outbuff[19] = (u8)(cDevTypeCode>>8);//设备类型码
			outbuff[20] = (u8)(cMfrCode);
			outbuff[19] = (u8)(cMfrCode>>8);//厂家编号
			outbuff[22] = (u8)(Hard_VERSION);
			outbuff[23] = (u8)(Hard_VERSION>>8);//硬件版本号
			outbuff[24] = (u8)(SOFT_VERSION);
			outbuff[25] = (u8)(SOFT_VERSION>>8);//软件版本号
			outbuff[26] = (u8)(AGREEMENT_VERSION);
			outbuff[27] = 0x00;//预留
			DataCopy( &outbuff[28], g_UART_TBuf, outlen );
			outlen += 28;

			myudp_send(sfd, outbuff, outlen);
		
		}
	
	}
	
}
void UDP_BrocastData(char *RevBuf,  int Revlen, int sfd) 
{
	char   *nptr; 
	char   *recdata;
	char   *outdata; 
	char   outbuff[512];
  	u16 len,outlen,c; 
	int i = 0;
	len = Revlen;	//读取数据长度 
	
	if(len < 26 )
	{		
//		My_Printf(" UDP_BrocastData len < 26 \n");
		return;	
	}
	    						
  	nptr = (char *)RevBuf; 				//取得数据起始指针 	 
	outdata = &outbuff[28];
	recdata	= &nptr[0];
	
	
	if(nptr[0]== 0x26  )    
	{    
		CMD_BC_Analys( &recdata[20],len-20,outdata,&outlen);
//		My_Printf(" UDP_BrocastData outlen :%d \n",outlen);

		if( outlen >0 )
		{ 	
			len = 0;
			outbuff[0] = '%';
			for(len=0;len<17;len++)
				 outbuff[len+1] = g_DI.Cfg.StationInfo[len];
			outbuff[18] = (u8)(cDevTypeCode);
			outbuff[19] = (u8)(cDevTypeCode>>8);//设备类型码
			outbuff[20] = (u8)(cMfrCode);
			outbuff[19] = (u8)(cMfrCode>>8);//厂家编号
			outbuff[22] = (u8)(Hard_VERSION);
			outbuff[23] = (u8)(Hard_VERSION>>8);//硬件版本号
			outbuff[24] = (u8)(SOFT_VERSION);
			outbuff[25] = (u8)(SOFT_VERSION>>8);//软件版本号
			outbuff[26] = (u8)(AGREEMENT_VERSION);
			outbuff[27] = 0x00;//预留
			outlen += 28;
//			My_Printf("outbuff says: ");   
//			for(i=0;i<outlen;i++)
//			{
//				My_Printf(" 0x%x ", (char)(outbuff[i]));  
//			}
//			My_Printf(" \n"); 
			myudp_send(sfd, outbuff, outlen); 
			//sendto(sfd, outbuff, outlen, 0, (struct sockaddr *)&BC_from, sizeof(BC_from));	
		}
	}
}
