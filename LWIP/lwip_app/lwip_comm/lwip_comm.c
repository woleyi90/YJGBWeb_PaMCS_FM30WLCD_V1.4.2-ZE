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
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ��Ӣ������
//lwipͨ������ ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/3/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
//��OS�汾��lwipʵ��,�����Ҫ3������: lwip�ں�����(������),dhcp����(��ѡ),DM9000��������(������)
//�������������񶼴���.
//����,����Ҫ�õ�2���ź���,���ں���ķ���ENC28J60.

//lwip DHCP����
//�����������ȼ�
#define LWIP_DHCP_TASK_PRIO       		7 
//���������ջ��С
#define LWIP_DHCP_STK_SIZE  		    128
//�����ջ�������ڴ����ķ�ʽ��������	
OS_STK * LWIP_DHCP_TASK_STK;	
//������
void lwip_dhcp_task(void *pdata); 

////lwip DM9000���ݽ��մ�������
////�����������ȼ�
//#define LWIP_DM9000_INPUT_TASK_PRIO		6 
////���������ջ��С
//#define LWIP_DM9000_INPUT_TASK_SIZE	    512
////�����ջ�������ڴ����ķ�ʽ��������	
//OS_STK * LWIP_DM9000_INPUT_TASK_STK;	
////������
//void lwip_dm9000_input_task(void *pdata);    

OS_STK * TCPIP_THREAD_TASK_STK;			//lwip�ں������ջ  

OS_EVENT* enc28j60input;				//ENC28J60���������ź���
OS_EVENT* enc28j60lock;					//ENC28J60��д���������ź���
//////////////////////////////////////////////////////////////////////////////////////////
__lwip_dev lwipdev;						//lwip���ƽṹ�� 
struct netif lwip_netif;				//����һ��ȫ�ֵ�����ӿ�
extern u32 memp_get_memorysize(void);	//��memp.c���涨��
extern u8_t *memp_memory;				//��memp.c���涨��.
extern u8_t *ram_heap;					//��mem.c���涨��.

//�����յ����ݺ���� 
void lwip_pkt_handle(void)
{
	//�����绺�����ж�ȡ���յ������ݰ������䷢�͸�LWIP���� 
	ethernetif_input(&lwip_netif);
}

//lwip�ں˲���,�ڴ�����
//����ֵ:0,�ɹ�;
//    ����,ʧ��
u8 lwip_comm_mem_malloc(void)
{
	u32 mempsize;
	u32 ramheapsize; 
	mempsize=memp_get_memorysize();			//�õ�memp_memory�����С
	memp_memory=mymalloc(SRAMEX,mempsize);	//Ϊmemp_memory�����ڴ�
	printf("memp_memory size:%d\r\n",mempsize);
	ramheapsize=LWIP_MEM_ALIGN_SIZE(MEM_SIZE)+2*LWIP_MEM_ALIGN_SIZE(4*3)+MEM_ALIGNMENT;//�õ�ram heap��С
	ram_heap=mymalloc(SRAMEX,ramheapsize);	//Ϊram_heap�����ڴ� 
	printf("ram_heap size:%d\r\n",ramheapsize);
	TCPIP_THREAD_TASK_STK=mymalloc(SRAMEX,TCPIP_THREAD_STACKSIZE*4);			//���ں����������ջ 
	LWIP_DHCP_TASK_STK=mymalloc(SRAMEX,LWIP_DHCP_STK_SIZE*4);					//��dhcp���������ջ 
	if(!memp_memory||!ram_heap||!TCPIP_THREAD_TASK_STK||!LWIP_DHCP_TASK_STK)	//������ʧ�ܵ�
	{
		lwip_comm_mem_free();
		return 1;
	}
	return 0;	
}

//lwip�ں˲���,�ڴ��ͷ�
void lwip_comm_mem_free(void)
{ 	
	myfree(SRAMEX,memp_memory);
	myfree(SRAMEX,ram_heap);
	myfree(SRAMEX,TCPIP_THREAD_TASK_STK);
	myfree(SRAMEX,LWIP_DHCP_TASK_STK);
}

//lwip Ĭ��IP����
//lwipx:lwip���ƽṹ��ָ��
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	
	//Ĭ��Զ��IPΪ:192.168.1.7
	lwipx->remoteip[0]=g_DI.Cfg.IP_Server[0];	
	lwipx->remoteip[1]=g_DI.Cfg.IP_Server[1];
	lwipx->remoteip[2]=g_DI.Cfg.IP_Server[2];
	lwipx->remoteip[3]=g_DI.Cfg.IP_Server[3];
	//MAC��ַ����(�����ֽڹ̶�Ϊ:2.0.0,�����ֽ���STM32ΨһID)
	lwipx->mac[0]=g_DI.MAC[0];
	lwipx->mac[1]=g_DI.MAC[1];
	lwipx->mac[2]=g_DI.MAC[2];
	lwipx->mac[3]=g_DI.MAC[3];
	lwipx->mac[4]=g_DI.MAC[4];
	lwipx->mac[5]=g_DI.MAC[5]; 

	//Ĭ�ϱ���IPΪ:192.168.1.30
	lwipx->ip[0]=g_DI.Cfg.IP_Addr[0];	
	lwipx->ip[1]=g_DI.Cfg.IP_Addr[1];
	lwipx->ip[2]=g_DI.Cfg.IP_Addr[2];
	lwipx->ip[3]=g_DI.Cfg.IP_Addr[3];
	//Ĭ����������:255.255.255.0
	lwipx->netmask[0]=g_DI.Cfg.IP_Mask[0];	
	lwipx->netmask[1]=g_DI.Cfg.IP_Mask[1];
	lwipx->netmask[2]=g_DI.Cfg.IP_Mask[2];
	lwipx->netmask[3]=g_DI.Cfg.IP_Mask[3];
	//Ĭ������:192.168.1.1
	lwipx->gateway[0]=g_DI.Cfg.IP_GateWay[0];	
	lwipx->gateway[1]=g_DI.Cfg.IP_GateWay[1];
	lwipx->gateway[2]=g_DI.Cfg.IP_GateWay[2];
	lwipx->gateway[3]=g_DI.Cfg.IP_GateWay[3];	
	lwipx->dhcpstatus=0;//û��DHCP
} 

//LWIP��ʼ��(LWIP������ʱ��ʹ��)
//����ֵ:0,�ɹ�
//      1,�ڴ����
//      2,DM9000��ʼ��ʧ��
//      3,�������ʧ��.
u8 lwip_comm_init(void)
{
	struct netif *Netif_Init_Flag;		//����netif_add()����ʱ�ķ���ֵ,�����ж������ʼ���Ƿ�ɹ�
	struct ip_addr ipaddr;  			//ip��ַ
	struct ip_addr netmask; 			//��������
	struct ip_addr gw;      			//Ĭ������ 
	if(lwip_comm_mem_malloc())//�ڴ�����ʧ��
	{
		printf("malloc err 1 \r\n");
		return 1;	
	}	
	if(ENC28J60_Init())
	{
		lwip_comm_mem_free();
		printf("ENC28J60_Init err 2 \r\n");
		return 2;		//��ʼ��ENC28J60
	}
	tcpip_init(NULL,NULL);				//��ʼ��tcp ip�ں�,�ú�������ᴴ��tcpip_thread�ں�����
	lwip_comm_default_ip_set(&lwipdev);	//����Ĭ��IP����Ϣ
#if LWIP_DHCP		//ʹ�ö�̬IP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;
#else				//ʹ�þ�̬IP
	IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	IP4_ADDR(&netmask,lwipdev.netmask[0],lwipdev.netmask[1] ,lwipdev.netmask[2],lwipdev.netmask[3]);
	IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
	printf("MAC:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
	printf("static IP:........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	printf("netmask:..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
	printf("gateway:..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
#endif
	Netif_Init_Flag=netif_add(&lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&tcpip_input);//�������б������һ������
	if(Netif_Init_Flag != NULL) 	//������ӳɹ���,����netifΪĬ��ֵ,���Ҵ�netif����
	{
		netif_set_default(&lwip_netif); //����netifΪĬ������
		netif_set_up(&lwip_netif);		//��netif����
	}
#if	LWIP_DHCP
	lwip_comm_dhcp_creat();		//����DHCP����
#endif	
	return 0;//����OK.
}   

//���ʹ����DHCP
#if LWIP_DHCP
//����DHCP����
void lwip_comm_dhcp_creat(void)
{
	OS_CPU_SR cpu_sr;
	OS_ENTER_CRITICAL();  //�����ٽ���
	printf("lwip_comm_dhcp_creat......\n");
	OSTaskCreate(lwip_dhcp_task,(void*)0,(OS_STK*)&LWIP_DHCP_TASK_STK[LWIP_DHCP_STK_SIZE-1],LWIP_DHCP_TASK_PRIO);//����DHCP���� 
	OS_EXIT_CRITICAL();  //�˳��ٽ���
}
//ɾ��DHCP����
void lwip_comm_dhcp_delete(void)
{
	dhcp_stop(&lwip_netif); 		//�ر�DHCP
	OSTaskDel(LWIP_DHCP_TASK_PRIO);	//ɾ��DHCP����
}
//DHCP��������
void lwip_dhcp_task(void *pdata)
{
	u32 ip=0,netmask=0,gw=0;
	dhcp_start(&lwip_netif);//����DHCP 
	lwipdev.dhcpstatus=0;	//����DHCP
	printf("���ڲ���DHCP������,���Ե�...........\r\n");   
	while(1)
	{ 
		printf("���ڻ�ȡ��ַ...\r\n");
		ip=lwip_netif.ip_addr.addr;		//��ȡ��IP��ַ
		netmask=lwip_netif.netmask.addr;//��ȡ��������
		gw=lwip_netif.gw.addr;			//��ȡĬ������ 
		if(ip!=0)   					//����ȷ��ȡ��IP��ַ��ʱ��
		{
			lwipdev.dhcpstatus=2;	//DHCP�ɹ�
 			printf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
			//������ͨ��DHCP��ȡ����IP��ַ
			lwipdev.ip[3]=(uint8_t)(ip>>24); 
			lwipdev.ip[2]=(uint8_t)(ip>>16);
			lwipdev.ip[1]=(uint8_t)(ip>>8);
			lwipdev.ip[0]=(uint8_t)(ip);
			printf("ͨ��DHCP��ȡ��IP��ַ..............%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
			//����ͨ��DHCP��ȡ�������������ַ
			lwipdev.netmask[3]=(uint8_t)(netmask>>24);
			lwipdev.netmask[2]=(uint8_t)(netmask>>16);
			lwipdev.netmask[1]=(uint8_t)(netmask>>8);
			lwipdev.netmask[0]=(uint8_t)(netmask);
			printf("ͨ��DHCP��ȡ����������............%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
			//������ͨ��DHCP��ȡ����Ĭ������
			lwipdev.gateway[3]=(uint8_t)(gw>>24);
			lwipdev.gateway[2]=(uint8_t)(gw>>16);
			lwipdev.gateway[1]=(uint8_t)(gw>>8);
			lwipdev.gateway[0]=(uint8_t)(gw);
			printf("ͨ��DHCP��ȡ����Ĭ������..........%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			break;
		}else if(lwip_netif.dhcp->tries>LWIP_MAX_DHCP_TRIES) //ͨ��DHCP�����ȡIP��ַʧ��,�ҳ�������Դ���
		{  
			lwipdev.dhcpstatus=0XFF;//DHCPʧ��.
			//ʹ�þ�̬IP��ַ
			IP4_ADDR(&(lwip_netif.ip_addr),lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
			IP4_ADDR(&(lwip_netif.netmask),lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
			IP4_ADDR(&(lwip_netif.gw),lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			printf("DHCP����ʱ,ʹ�þ�̬IP��ַ!\r\n");
			printf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
			printf("��̬IP��ַ........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
			printf("��������..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
			printf("Ĭ������..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			break;
		}  
		delay_ms(250); //��ʱ250ms
	}
	lwip_comm_dhcp_delete();//ɾ��DHCP���� 
}
#endif 

/*******************************************************************************
 *	������: Reset_lwipnetif
 *	��  ��: ��
 *	��  ��: ��
 *	����˵������������g_DI.Cfg.IP_Addr��g_DI.Cfg.IP_Server���޸�lwip_netif
 *		
 *******************************************************************************/
void Reset_lwipnetif(void)
{
	OS_CPU_SR cpu_sr;
	struct ip_addr ipaddr;  			//ip��ַ
	struct ip_addr netmask; 			//��������
	struct ip_addr gw;      			//Ĭ������ 	
	
	OS_ENTER_CRITICAL(); //���ж�
	lwip_comm_default_ip_set(&lwipdev);	//����Ĭ��IP����Ϣ
	//ʹ�þ�̬IP
	IP4_ADDR(&ipaddr,g_DI.Cfg.IP_Addr[0],g_DI.Cfg.IP_Addr[1],g_DI.Cfg.IP_Addr[2],g_DI.Cfg.IP_Addr[3]);
	IP4_ADDR(&netmask,g_DI.Cfg.IP_Mask[0],g_DI.Cfg.IP_Mask[1] ,g_DI.Cfg.IP_Mask[2],g_DI.Cfg.IP_Mask[3]);
	IP4_ADDR(&gw,g_DI.Cfg.IP_GateWay[0],g_DI.Cfg.IP_GateWay[1],g_DI.Cfg.IP_GateWay[2],g_DI.Cfg.IP_GateWay[3]);
	printf("MAC address:................%d.%d.%d.%d.%d.%d\r\n",g_DI.MAC[0],g_DI.MAC[1],lwipdev.mac[2],g_DI.MAC[3],g_DI.MAC[4],g_DI.MAC[5]);
	printf("static IP address........................%d.%d.%d.%d\r\n",g_DI.Cfg.IP_Addr[0],g_DI.Cfg.IP_Addr[1],g_DI.Cfg.IP_Addr[2],g_DI.Cfg.IP_Addr[3]);
	printf("Mask..........................%d.%d.%d.%d\r\n",g_DI.Cfg.IP_Mask[0],g_DI.Cfg.IP_Mask[1],g_DI.Cfg.IP_Mask[2],g_DI.Cfg.IP_Mask[3]);
	printf("Gateway..........................%d.%d.%d.%d\r\n",g_DI.Cfg.IP_GateWay[0],g_DI.Cfg.IP_GateWay[1],g_DI.Cfg.IP_GateWay[2],g_DI.Cfg.IP_GateWay[3]);

//	netif_set_addr(&lwip_netif,&ipaddr,&netmask,&gw);
	netif_set_ipaddr(&lwip_netif,&ipaddr);
	netif_set_default(&lwip_netif); //����netifΪĬ������
	netif_set_up(&lwip_netif);		//��netif����

	OS_EXIT_CRITICAL();  //���ж�

	printf("netif_set_up \r\n");

}




























