#include "lwip_comm.h" 
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/init.h"
#include "ethernetif.h" 
#include "lwip/timers.h"
#include "lwip/tcp_impl.h"
#include "lwip/ip_frag.h"
#include "lwip/tcpip.h" 
#include "malloc.h"
#include "delay.h"
#include "includes.h"
#include "global.h"
#include <stdio.h>
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK 精英开发板
//lwip通用驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/3/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
//带OS版本的lwip实验,最多需要3个任务: lwip内核任务(必须有),dhcp任务(可选),DM9000接收任务(必须有)
//本例程三个任务都创建.
//另外,还需要用到2个信号量,用于合理的访问ENC28J60.

//lwip DHCP任务
//设置任务优先级
#define LWIP_DHCP_TASK_PRIO       		7 
//设置任务堆栈大小
#define LWIP_DHCP_STK_SIZE  		    128
//任务堆栈，采用内存管理的方式控制申请	
OS_STK * LWIP_DHCP_TASK_STK;	
//任务函数
void lwip_dhcp_task(void *pdata); 

////lwip DM9000数据接收处理任务
////设置任务优先级
//#define LWIP_DM9000_INPUT_TASK_PRIO		6 
////设置任务堆栈大小
//#define LWIP_DM9000_INPUT_TASK_SIZE	    512
////任务堆栈，采用内存管理的方式控制申请	
//OS_STK * LWIP_DM9000_INPUT_TASK_STK;	
////任务函数
//void lwip_dm9000_input_task(void *pdata);    

OS_STK * TCPIP_THREAD_TASK_STK;			//lwip内核任务堆栈  

OS_EVENT* enc28j60input;				//ENC28J60接收数据信号量
OS_EVENT* enc28j60lock;					//ENC28J60读写互锁控制信号量
//////////////////////////////////////////////////////////////////////////////////////////
__lwip_dev lwipdev;						//lwip控制结构体 
struct netif lwip_netif;				//定义一个全局的网络接口
extern u32 memp_get_memorysize(void);	//在memp.c里面定义
extern u8_t *memp_memory;				//在memp.c里面定义.
extern u8_t *ram_heap;					//在mem.c里面定义.

//当接收到数据后调用 
void lwip_pkt_handle(void)
{
	//从网络缓冲区中读取接收到的数据包并将其发送给LWIP处理 
	ethernetif_input(&lwip_netif);
}

//lwip内核部分,内存申请
//返回值:0,成功;
//    其他,失败
u8 lwip_comm_mem_malloc(void)
{
	u32 mempsize;
	u32 ramheapsize; 
	mempsize=memp_get_memorysize();			//得到memp_memory数组大小
	memp_memory=mymalloc(SRAMEX,mempsize);	//为memp_memory申请内存
	printf("memp_memory size:%d\r\n",mempsize);
	ramheapsize=LWIP_MEM_ALIGN_SIZE(MEM_SIZE)+2*LWIP_MEM_ALIGN_SIZE(4*3)+MEM_ALIGNMENT;//得到ram heap大小
	ram_heap=mymalloc(SRAMEX,ramheapsize);	//为ram_heap申请内存 
	printf("ram_heap size:%d\r\n",ramheapsize);
	TCPIP_THREAD_TASK_STK=mymalloc(SRAMEX,TCPIP_THREAD_STACKSIZE*4);			//给内核任务申请堆栈 
	LWIP_DHCP_TASK_STK=mymalloc(SRAMEX,LWIP_DHCP_STK_SIZE*4);					//给dhcp任务申请堆栈 
	if(!memp_memory||!ram_heap||!TCPIP_THREAD_TASK_STK||!LWIP_DHCP_TASK_STK)	//有申请失败的
	{
		lwip_comm_mem_free();
		return 1;
	}
	return 0;	
}

//lwip内核部分,内存释放
void lwip_comm_mem_free(void)
{ 	
	myfree(SRAMEX,memp_memory);
	myfree(SRAMEX,ram_heap);
	myfree(SRAMEX,TCPIP_THREAD_TASK_STK);
	myfree(SRAMEX,LWIP_DHCP_TASK_STK);
}

//lwip 默认IP设置
//lwipx:lwip控制结构体指针
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	
	//默认远端IP为:192.168.1.7
	lwipx->remoteip[0]=g_DI.Cfg.IP_Server[0];	
	lwipx->remoteip[1]=g_DI.Cfg.IP_Server[1];
	lwipx->remoteip[2]=g_DI.Cfg.IP_Server[2];
	lwipx->remoteip[3]=g_DI.Cfg.IP_Server[3];
	//MAC地址设置(高三字节固定为:2.0.0,低三字节用STM32唯一ID)
	lwipx->mac[0]=g_DI.MAC[0];
	lwipx->mac[1]=g_DI.MAC[1];
	lwipx->mac[2]=g_DI.MAC[2];
	lwipx->mac[3]=g_DI.MAC[3];
	lwipx->mac[4]=g_DI.MAC[4];
	lwipx->mac[5]=g_DI.MAC[5]; 

	//默认本地IP为:192.168.1.30
	lwipx->ip[0]=g_DI.Cfg.IP_Addr[0];	
	lwipx->ip[1]=g_DI.Cfg.IP_Addr[1];
	lwipx->ip[2]=g_DI.Cfg.IP_Addr[2];
	lwipx->ip[3]=g_DI.Cfg.IP_Addr[3];
	//默认子网掩码:255.255.255.0
	lwipx->netmask[0]=g_DI.Cfg.IP_Mask[0];	
	lwipx->netmask[1]=g_DI.Cfg.IP_Mask[1];
	lwipx->netmask[2]=g_DI.Cfg.IP_Mask[2];
	lwipx->netmask[3]=g_DI.Cfg.IP_Mask[3];
	//默认网关:192.168.1.1
	lwipx->gateway[0]=g_DI.Cfg.IP_GateWay[0];	
	lwipx->gateway[1]=g_DI.Cfg.IP_GateWay[1];
	lwipx->gateway[2]=g_DI.Cfg.IP_GateWay[2];
	lwipx->gateway[3]=g_DI.Cfg.IP_GateWay[3];	
	lwipx->dhcpstatus=0;//没有DHCP
} 

//LWIP初始化(LWIP启动的时候使用)
//返回值:0,成功
//      1,内存错误
//      2,DM9000初始化失败
//      3,网卡添加失败.
u8 lwip_comm_init(void)
{
	struct netif *Netif_Init_Flag;		//调用netif_add()函数时的返回值,用于判断网络初始化是否成功
	struct ip_addr ipaddr;  			//ip地址
	struct ip_addr netmask; 			//子网掩码
	struct ip_addr gw;      			//默认网关 
	if(lwip_comm_mem_malloc())//内存申请失败
	{
		printf("malloc err 1 \r\n");
		return 1;	
	}	
	if(ENC28J60_Init())
	{
		lwip_comm_mem_free();
		printf("ENC28J60_Init err 2 \r\n");
		return 2;		//初始化ENC28J60
	}
	tcpip_init(NULL,NULL);				//初始化tcp ip内核,该函数里面会创建tcpip_thread内核任务
	lwip_comm_default_ip_set(&lwipdev);	//设置默认IP等信息
#if LWIP_DHCP		//使用动态IP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;
#else				//使用静态IP
	IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	IP4_ADDR(&netmask,lwipdev.netmask[0],lwipdev.netmask[1] ,lwipdev.netmask[2],lwipdev.netmask[3]);
	IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
	printf("MAC:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
	printf("static IP:........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	printf("netmask:..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
	printf("gateway:..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
#endif
	Netif_Init_Flag=netif_add(&lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&tcpip_input);//向网卡列表中添加一个网口
	if(Netif_Init_Flag != NULL) 	//网口添加成功后,设置netif为默认值,并且打开netif网口
	{
		netif_set_default(&lwip_netif); //设置netif为默认网口
		netif_set_up(&lwip_netif);		//打开netif网口
	}
#if	LWIP_DHCP
	lwip_comm_dhcp_creat();		//创建DHCP任务
#endif	
	return 0;//操作OK.
}   

//如果使能了DHCP
#if LWIP_DHCP
//创建DHCP任务
void lwip_comm_dhcp_creat(void)
{
	OS_CPU_SR cpu_sr;
	OS_ENTER_CRITICAL();  //进入临界区
	printf("lwip_comm_dhcp_creat......\n");
	OSTaskCreate(lwip_dhcp_task,(void*)0,(OS_STK*)&LWIP_DHCP_TASK_STK[LWIP_DHCP_STK_SIZE-1],LWIP_DHCP_TASK_PRIO);//创建DHCP任务 
	OS_EXIT_CRITICAL();  //退出临界区
}
//删除DHCP任务
void lwip_comm_dhcp_delete(void)
{
	dhcp_stop(&lwip_netif); 		//关闭DHCP
	OSTaskDel(LWIP_DHCP_TASK_PRIO);	//删除DHCP任务
}
//DHCP处理任务
void lwip_dhcp_task(void *pdata)
{
	u32 ip=0,netmask=0,gw=0;
	dhcp_start(&lwip_netif);//开启DHCP 
	lwipdev.dhcpstatus=0;	//正在DHCP
	printf("正在查找DHCP服务器,请稍等...........\r\n");   
	while(1)
	{ 
		printf("正在获取地址...\r\n");
		ip=lwip_netif.ip_addr.addr;		//读取新IP地址
		netmask=lwip_netif.netmask.addr;//读取子网掩码
		gw=lwip_netif.gw.addr;			//读取默认网关 
		if(ip!=0)   					//当正确读取到IP地址的时候
		{
			lwipdev.dhcpstatus=2;	//DHCP成功
 			printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
			//解析出通过DHCP获取到的IP地址
			lwipdev.ip[3]=(uint8_t)(ip>>24); 
			lwipdev.ip[2]=(uint8_t)(ip>>16);
			lwipdev.ip[1]=(uint8_t)(ip>>8);
			lwipdev.ip[0]=(uint8_t)(ip);
			printf("通过DHCP获取到IP地址..............%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
			//解析通过DHCP获取到的子网掩码地址
			lwipdev.netmask[3]=(uint8_t)(netmask>>24);
			lwipdev.netmask[2]=(uint8_t)(netmask>>16);
			lwipdev.netmask[1]=(uint8_t)(netmask>>8);
			lwipdev.netmask[0]=(uint8_t)(netmask);
			printf("通过DHCP获取到子网掩码............%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
			//解析出通过DHCP获取到的默认网关
			lwipdev.gateway[3]=(uint8_t)(gw>>24);
			lwipdev.gateway[2]=(uint8_t)(gw>>16);
			lwipdev.gateway[1]=(uint8_t)(gw>>8);
			lwipdev.gateway[0]=(uint8_t)(gw);
			printf("通过DHCP获取到的默认网关..........%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			break;
		}else if(lwip_netif.dhcp->tries>LWIP_MAX_DHCP_TRIES) //通过DHCP服务获取IP地址失败,且超过最大尝试次数
		{  
			lwipdev.dhcpstatus=0XFF;//DHCP失败.
			//使用静态IP地址
			IP4_ADDR(&(lwip_netif.ip_addr),lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
			IP4_ADDR(&(lwip_netif.netmask),lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
			IP4_ADDR(&(lwip_netif.gw),lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			printf("DHCP服务超时,使用静态IP地址!\r\n");
			printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
			printf("静态IP地址........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
			printf("子网掩码..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
			printf("默认网关..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			break;
		}  
		delay_ms(250); //延时250ms
	}
	lwip_comm_dhcp_delete();//删除DHCP任务 
}
#endif 

/*******************************************************************************
 *	函数名: Reset_lwipnetif
 *	输  入: 无
 *	输  出: 无
 *	功能说明：重新设置g_DI.Cfg.IP_Addr与g_DI.Cfg.IP_Server后修改lwip_netif
 *		
 *******************************************************************************/
void Reset_lwipnetif(void)
{
	OS_CPU_SR cpu_sr;
	struct ip_addr ipaddr;  			//ip地址
	struct ip_addr netmask; 			//子网掩码
	struct ip_addr gw;      			//默认网关 	
	
	OS_ENTER_CRITICAL(); //关中断
	lwip_comm_default_ip_set(&lwipdev);	//设置默认IP等信息
	//使用静态IP
	IP4_ADDR(&ipaddr,g_DI.Cfg.IP_Addr[0],g_DI.Cfg.IP_Addr[1],g_DI.Cfg.IP_Addr[2],g_DI.Cfg.IP_Addr[3]);
	IP4_ADDR(&netmask,g_DI.Cfg.IP_Mask[0],g_DI.Cfg.IP_Mask[1] ,g_DI.Cfg.IP_Mask[2],g_DI.Cfg.IP_Mask[3]);
	IP4_ADDR(&gw,g_DI.Cfg.IP_GateWay[0],g_DI.Cfg.IP_GateWay[1],g_DI.Cfg.IP_GateWay[2],g_DI.Cfg.IP_GateWay[3]);
	printf("MAC address:................%d.%d.%d.%d.%d.%d\r\n",g_DI.MAC[0],g_DI.MAC[1],lwipdev.mac[2],g_DI.MAC[3],g_DI.MAC[4],g_DI.MAC[5]);
	printf("static IP address........................%d.%d.%d.%d\r\n",g_DI.Cfg.IP_Addr[0],g_DI.Cfg.IP_Addr[1],g_DI.Cfg.IP_Addr[2],g_DI.Cfg.IP_Addr[3]);
	printf("Mask..........................%d.%d.%d.%d\r\n",g_DI.Cfg.IP_Mask[0],g_DI.Cfg.IP_Mask[1],g_DI.Cfg.IP_Mask[2],g_DI.Cfg.IP_Mask[3]);
	printf("Gateway..........................%d.%d.%d.%d\r\n",g_DI.Cfg.IP_GateWay[0],g_DI.Cfg.IP_GateWay[1],g_DI.Cfg.IP_GateWay[2],g_DI.Cfg.IP_GateWay[3]);

//	netif_set_addr(&lwip_netif,&ipaddr,&netmask,&gw);
	netif_set_ipaddr(&lwip_netif,&ipaddr);
	netif_set_default(&lwip_netif); //设置netif为默认网口
	netif_set_up(&lwip_netif);		//打开netif网口

	OS_EXIT_CRITICAL();  //开中断

	printf("netif_set_up \r\n");

}




























