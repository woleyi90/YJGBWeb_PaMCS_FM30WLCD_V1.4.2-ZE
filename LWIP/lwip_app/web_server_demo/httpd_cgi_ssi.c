#include "lwip/debug.h"
#include "httpd.h"
#include "lwip/tcp.h"
#include "fs.h"
#include "lwip_comm.h"
#include "global.h"	
#include "includes.h"
#include <string.h>
#include <stdlib.h>	  
#include "udp_demo.h"

extern unsigned char data_response_ssi[];
#define RESPONSE_PAGE_SET_CGI_RSP_URL     "/response.ssi"
#define data_response_buf (data_response_ssi+14)

#define NUM_CONFIG_CGI_URIS	(sizeof(ppcURLs ) / sizeof(tCGI))  //CGI的URI数量
#define NUM_CONFIG_SSI_TAGS	(sizeof(ppcTAGs) / sizeof (char *))  //SSI的TAG数量
extern u8 ManualChanging;
extern u8  FAN_ManCtrl_Delay;
//CGI handler
static char *ReadPaState_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static char *ReadDeviceData_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static char *DevicePost_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static char *ParamPost_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static char *ReadControlData_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static char *PowerPostData_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static char *PowerctlPost_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static char *JlqPost_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );

static const char *ppcTAGs[]=  //SSI的Tag
{
	"s", //ADC值
};
static const tCGI ppcURLs[]= //cgi程序
{
		{ "/getPaState.cgi",    ReadPaState_CGIHandler },
		{ "/getDeviceData.cgi",    ReadDeviceData_CGIHandler }, 
		{ "/devicePost.cgi",    DevicePost_CGIHandler }, 
		{ "/paramPost.cgi",    ParamPost_CGIHandler },
		{ "/jlqPost.cgi",    JlqPost_CGIHandler },
		{ "/getControlData.cgi",    ReadControlData_CGIHandler },
		{ "/powerPost.cgi",    PowerPostData_CGIHandler },
		{ "/ctlPost.cgi",    PowerctlPost_CGIHandler },

};
void alldate_deal(s16 num,char len);
//清除缓冲区的内容
void  clear_response_bufer(unsigned char *buffer)
{
  memset(buffer,0,strlen((unsigned char*)buffer));
}
void str2shu(u8 *instr,u8 *outstr, u8 len)
{
    char   *p;
	char a;
	u16 ipport = 0;	
	p=strtok(instr,".");//第一次调用,传要处理的串参数地址s    
	while(p && (len--)) 
    { 
			
		a=atoi(p);
		*outstr++ = a;
		p=strtok(NULL,"."); 
	 
	}

}
u16 str2shuIP(u8 *instr,u8 *outstr)
{
    char   *p;
	char a;
	u16 ipport = 0;	
	
	p=strtok(instr,".");//第一次调用,传要处理的串参数地址s    
	a=atoi(p);
	*outstr++ = a;
	
	p=strtok(NULL,"."); 
	a=atoi(p);
	*outstr++ = a; 
	
	p=strtok(NULL,"."); 
	a=atoi(p);
	*outstr++ = a;	
			
	p=strtok(NULL,":"); 
	a=atoi(p);
	*outstr++ = a;		
		
	p=strtok(NULL,":"); 	  
	ipport = atoi(p);
	return ipport;			
}

int GetString(char *SrcBuf, char *FindBuf, char *NeedBuf)
{
	char *pTail;
	int pTaillen = 0;
	int len = strlen(FindBuf);

	if (((pTail = strstr(SrcBuf, FindBuf)) != NULL) )
	{
		pTaillen = strlen(pTail);
		memcpy(NeedBuf, pTail+len, (pTaillen - len)); 
		return (pTaillen - len);
	}
	else
	{
		return 0; 
	}	
}
//m^n函数
//返回值:m^n次方.
u32 Num_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//正数字->字符串转换函数
//将num数字(位数为len)转为字符串,存放在buf里面
//num:数字,整形
//buf:字符串缓存
//len:长度
void num2str(s16 num0,u8 *buf,u8 len)
{
	u8 i;	
	s16 num;
	num = num0;
	if(num <10)
	{
		len = 1;
		buf[0]=num+0x30;
	}
	else if((num>=10)&&(num<100))
	{
		len = 2;
		buf[0]=num/10+0x30;
		buf[1]=num%10+0x30;
	}
	else if((num>=100)&&(num<1000))
	{
		len = 3;
		buf[0]=num/100+0x30;
		buf[1]=num%100/10+0x30;
		buf[2]=num%10+0x30;
	}
	else if((num>=1000)&&(num<10000))
	{
		len = 4;
		buf[0]=num/1000+0x30;
		buf[1]=num%1000/100+0x30;
		buf[2]=num%100/10+0x30;
		buf[3]=num%10+0x30;
	}
	else if((num>=10000)&&(num<32767))
	{
		len = 5;
		buf[0]=num/10000+0x30;
		buf[1]=num%10000/1000+0x30;
		buf[2]=num%1000/100+0x30;
		buf[3]=num%100/10+0x30;
		buf[4]=num%10+0x30;
	}
	else
	{
		for(i=0;i<len;i++)
		{
			buf[i]=(num/Num_Pow(10,len-i-1))%10+'0';
		}
	}
	
}

//包含有负的数字->字符串转换函数
//负数的数字s16的类型在保存的类型为u16类型
//在web页面进行-65536的处理得到负数
void num2nstr(u16 num,u8 *buf,u8 len)
{
	u8 i;	
	if(num <10)
	{
		len = 1;
		buf[0]=num+0x30;
	}
	else if((num>=10)&&(num<100))
	{
		len = 2;
		buf[0]=num/10+0x30;
		buf[1]=num%10+0x30;
	}
	else if((num>=100)&&(num<1000))
	{
		len = 3;
		buf[0]=num/100+0x30;
		buf[1]=num%100/10+0x30;
		buf[2]=num%10+0x30;
	}
	else if((num>=1000)&&(num<10000))
	{
		len = 4;
		buf[0]=num/1000+0x30;
		buf[1]=num%1000/100+0x30;
		buf[2]=num%100/10+0x30;
		buf[3]=num%10+0x30;
	}
	else if((num>=10000)&&(num<65535))
	{
		len = 5;
		buf[0]=num/10000+0x30;
		buf[1]=num%10000/1000+0x30;
		buf[2]=num%1000/100+0x30;
		buf[3]=num%100/10+0x30;
		buf[4]=num%10+0x30;
	}
	else
	{
		for(i=0;i<len;i++)
		{
			buf[i]=(num/Num_Pow(10,len-i-1))%10+'0';
		}
	}
	
}

//当web客户端请求浏览器的时候,使用此函数被CGI handler调用
static int FindCGIParameter(const char *pcToFind,char *pcParam[],int iNumParams)
{
	int iLoop;
	for(iLoop = 0;iLoop < iNumParams;iLoop ++ )
	{
		if(strcmp(pcToFind,pcParam[iLoop]) == 0)
		{
			return (iLoop); //返回iLOOP
		}
	}
	return (-1);
}
//当web客户端请求浏览器的时候,使用此函数被CGI handler调用
static int FindCGI_Parameter_Two(const char *pcToFind,const char *pcToFind0,char *pcParam[])
{


	if(strcmp(pcToFind,pcParam[1]) == 0)
	{
		return (0); //返回0
	}
	if(strcmp(pcToFind0,pcParam[1]) == 0)
	{
		return (1); //返回1
	}
	return (-1);
}
//当web客户端请求浏览器的时候,使用此函数被CGI handler调用
static int FindCGI_Parameter_Three(const char *pcToFind,const char *pcToFind0,const char *pcToFind1,char *pcParam[])
{


	if(strcmp(pcToFind,pcParam[1]) == 0)
	{
		return (0); //返回0
	}
	if(strcmp(pcToFind0,pcParam[1]) == 0)
	{
		return (1); //返回1
	}
	if(strcmp(pcToFind1,pcParam[1]) == 0)
	{
		return (2); //返回2
	}
	return (-1);
}

static char *ReadPaState_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
   u8 i;
	unsigned char *buf;
	u8 puf[10];
	int  index;
for(i =0 ;i<20;i++ );//延时一下，否则会重启	
//g_DI.Pa[0].Data.WorkState =0x53;	// 工作模块状态
//g_DI.Pa[0].Data.DetState = 0xa1;	// 检测模块状态
//g_DI.Pa[0].Data.ProState = 0x36;	// 保护模块状态
//g_DI.Pa[0].Data.Power_FR = 29.89;	//入射功率						
//g_DI.Pa[0].Data.Power_RV = 0.89;	//反射功率
//g_DI.Pa[0].Data.CPU_TPT = 25.79;	//CPU温度	
//g_DI.Pa[0].Data.PA_TPT = 35.7;	//功放温度	
//g_DI.Pa[0].Data.ENV_TPT = 27.8;	//环境温度
//g_DI.Pa[0].Data.Power_IN = -19.5;	//输入功率
//g_DI.Pa[0].Data.PA_VOL = 50.5;	//功放电压
//g_DI.Pa[0].Data.PA_BECUR = 0.35;	//前级电流
//g_DI.Pa[0].Data.PA_CUR1 = 0.79;	//功放电流1
//g_DI.Pa[0].Data.PA_CUR2 = 0.19;	//功放电流2
//g_DI.Pa[0].Data.Power_SET = 30.0;	//功率设置
//g_DI.Pa[0].Data.Serial_Num = 2130;	//序列号				
//g_DI.Pa[0].Data.ATT_POT = 10;	//第一个衰减电位器
//g_DI.Pa[0].Data.ATT_POT1 = 20;	//第二个衰减电位器
	 
	clear_response_bufer(data_response_buf);		//清除缓冲区的内容			

//	sprintf((char *)(data_response_buf), "1;0;1;0;0;1;0;0;1;0;1;0;1;1;0;0;0;1;1;0;0.2322000;0.223;13;-32.500;11.963;35.386;0.211;33.008;0.113;-273.000;0.000;30.000;60.0;70.0;0002030;");	

//输出停播位  bit7
if((g_DI.Pa[0].Data.WorkState & 0x80) == 0x80)
{

	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{

	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}	
//功率小环 bit1
if((g_DI.Pa[0].Data.WorkState & 0x02) == 0x02)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}
//开机操作响应位（播出位） bit4
if((g_DI.Pa[0].Data.WorkState & 0x10) == 0x10)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}
//正在发声音报警位
sprintf((char *)(puf), "0;");
strcat((char *)(data_response_buf),puf);

//定时开关机控制位 bit3
if((g_DI.Pa[0].Data.WorkState & 0x08) == 0x08)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}
//自动 bit2
if((g_DI.Pa[0].Data.WorkState & 0x04) == 0x04)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}
//正在调节功率 bit0
if((g_DI.Pa[0].Data.WorkState & 0x01) == 0x01)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}


//电流过流 bit0	
if((g_DI.Pa[0].Data.ProState & 0x01) == 0x01)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}
//温度超限 bit1
if((g_DI.Pa[0].Data.ProState & 0x02) == 0x02)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}
//反射过高 bit2
if((g_DI.Pa[0].Data.ProState & 0x04) == 0x04)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}
//过流降功率 bit3
if((g_DI.Pa[0].Data.ProState & 0x08) == 0x08)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);

}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);

}
//温度降功率 bit4
if((g_DI.Pa[0].Data.ProState & 0x10) == 0x10)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}
//反射降功率 bit5
if((g_DI.Pa[0].Data.ProState & 0x20) == 0x20)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}


//输出功率检测停播位 bit6   
if((g_DI.Pa[0].Data.DetState & 0x40) == 0x40)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}
//调频调制版通信状态
if(g_TMTLink[0] == 1)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}
//主电压异常位 bit4
if((g_DI.Pa[0].Data.DetState & 0x10) == 0x10)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}
//栅压状态 bit2
if((g_DI.Pa[0].Data.DetState & 0x04) == 0x04)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}
//功放温度过高 bit5
if((g_DI.Pa[0].Data.DetState & 0x20) == 0x20)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}
//风扇1	bit1
if((g_DI.Pa[0].Data.DetState & 0x02) == 0x02)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}
//风扇2	bit0
if((g_DI.Pa[0].Data.DetState & 0x01) == 0x01)
{
	sprintf((char *)(puf), "1;");
	strcat((char *)(data_response_buf),puf);
}
else
{
	sprintf((char *)(puf), "0;");
	strcat((char *)(data_response_buf),puf);
}	
//入射功率
sprintf((char *)(puf), "%.3f W;",g_DI.Pa[0].Data.Power_FR);
strcat((char *)(data_response_buf),puf);
//反射功率
sprintf((char *)(puf), "%.3f W;",g_DI.Pa[0].Data.Power_RV);
strcat((char *)(data_response_buf),puf);
//效率
sprintf((char *)(puf), "%3.3f ;",(float)(100*g_DI.Pa[0].Data.Power_FR / (g_DI.Pa[0].Data.PA_VOL*g_DI.Pa[0].Data.PA_CUR1)));
strcat((char *)(data_response_buf),puf);
//输入功率
sprintf((char *)(puf), "%.3f dBm;",g_DI.Pa[0].Data.Power_IN);
strcat((char *)(data_response_buf),puf);
//功放电压
sprintf((char *)(puf), "%.3f V;",g_DI.Pa[0].Data.PA_VOL);
strcat((char *)(data_response_buf),puf);
//CPU温度
sprintf((char *)(puf), "%.3f;",g_DI.Pa[0].Data.CPU_TPT);
strcat((char *)(data_response_buf),puf);
//前级电流
sprintf((char *)(puf), "%.3f A;",g_DI.Pa[0].Data.PA_BECUR);
strcat((char *)(data_response_buf),puf);
//功放温度
sprintf((char *)(puf), "%.3f;",g_DI.Pa[0].Data.PA_TPT);
strcat((char *)(data_response_buf),puf);
//功放电流1
sprintf((char *)(puf), "%.3f A;",g_DI.Pa[0].Data.PA_CUR1);
strcat((char *)(data_response_buf),puf);
//环境温度
sprintf((char *)(puf), "%.3f;",g_DI.Pa[0].Data.ENV_TPT);
strcat((char *)(data_response_buf),puf);
//功放电流2
sprintf((char *)(puf), "%.3f A;",g_DI.Pa[0].Data.PA_CUR2);
strcat((char *)(data_response_buf),puf);
//功率设置
//sprintf((char *)(puf), "%.3fW;",g_DI.Pa[0].Data.Power_SET);
//strcat((char *)(data_response_buf),puf);
sprintf((char *)(puf), "%dW;", (int)g_DI.Cfg.PWR_RateOut);
strcat((char *)(data_response_buf),puf);
//衰减电位器 
sprintf((char *)(puf), "%d;",g_DI.Pa[0].Data.RES_Rear);
strcat((char *)(data_response_buf),puf);
sprintf((char *)(puf), "%d;",0);
strcat((char *)(data_response_buf),puf); 


//输出频点
sprintf((char *)(puf), "%d.%1d MHz;", g_DI.Excitdata.FMFRQ/10, g_DI.Excitdata.FMFRQ%10);
strcat((char *)(data_response_buf),puf); 

//激励器预加重
if(g_DI.Excitdata.FMPre_Emphasis == 0)
{
	sprintf((char *)(puf), "0 us;");
}
else if(g_DI.Excitdata.FMPre_Emphasis == 1)
{
	sprintf((char *)(puf), "50 us;");
}
else if(g_DI.Excitdata.FMPre_Emphasis == 2)
{
	sprintf((char *)(puf), "75 us;");
}
else
{
	sprintf((char *)(puf), "err;");
}
strcat((char *)(data_response_buf),puf); 

//激励器输出增益
sprintf((char *)(puf), "%d;", g_DI.Excitdata.FMGain_RF);
strcat((char *)(data_response_buf),puf); 

//调制模式 0:关闭 1:立体声 2:L单声道 4:R单声道 6:L+R声道
if(g_DI.Excitdata.FMMODULATIONMODE == 0)
{
	sprintf((char *)(puf), "Close;");
}
else if(g_DI.Excitdata.FMMODULATIONMODE == 1)
{
	sprintf((char *)(puf), "Stereo;");
}
else if(g_DI.Excitdata.FMMODULATIONMODE == 2)
{
	sprintf((char *)(puf), "L Sound;");
}
else if(g_DI.Excitdata.FMMODULATIONMODE == 4)
{
	sprintf((char *)(puf), "R Sound;");
}
else if(g_DI.Excitdata.FMMODULATIONMODE == 6)
{
	sprintf((char *)(puf), "L+R Sound;");
}
else
{
	sprintf((char *)(puf), "err;");
}
strcat((char *)(data_response_buf),puf); 


//调制度
sprintf((char *)(puf), "%d.%d%%;", g_DI.Excitdata.ModulationDCT/10,g_DI.Excitdata.ModulationDCT%10);
strcat((char *)(data_response_buf),puf); 

//RDS幅度 0~15 0:无载波
sprintf((char *)(puf), "%d;", g_DI.Excitdata.FMRDSVAL);
strcat((char *)(data_response_buf),puf); 

//序列号     
sprintf((char *)(puf), "V%d.%d.%d;", SOFT_VERSION/100,SOFT_VERSION%100/10,SOFT_VERSION%10);
strcat((char *)(data_response_buf),puf);

    return RESPONSE_PAGE_SET_CGI_RSP_URL;
}
static char *ReadDeviceData_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
  	u8 i,err;
	unsigned char *buf;
	u8 puf[50];
	u8 SendBuffer[10];
	int  index;

	for(i =0 ;i<20;i++ );//延时一下，否则会重启	
	 
	clear_response_bufer(data_response_buf);		//清除缓冲区的内容			
		

#if IWDG_EN
	  IWDG_ReloadCounter(); //手动喂狗  
#endif

//	sprintf((char *)(data_response_buf), "55;shdzld;192.168.1.207:2000;192.168.1.1;192.168.1.2;192.168.1.20;192.168.1.50:5000;20;10;192.168.1.50:5000;20;10;192.168.1.50:5000;20;10;1012102122012;2.5.0;3.0;50;55;57;65;70;1.500;1.800;30;5;30W;5;-20.00;");	

/*
//设备地址
sprintf((char *)(puf), "%d;", 55);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 0----------------------------\r\n");
//设备名
sprintf((char *)(puf), "GX_FM_30W;" );
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 1----------------------------\r\n");

//IP地址
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", 192,168,1,207,7000);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 2----------------------------\r\n");
//默认网关
sprintf((char *)(puf), "%d.%d.%d.%d;", 192,168,1,1);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 3----------------------------\r\n");
//本地管理1
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", 192,168,1,2, 5000);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 4----------------------------\r\n");
//本地管理2
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", 192,168,1,20, 5000);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 5----------------------------\r\n");
//远程主机IP1
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", 192,168,1,200, 5000);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 6----------------------------\r\n");
//主动间隔1
sprintf((char *)(puf), "%d;", 30);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 7----------------------------\r\n");
//主动间隔2
sprintf((char *)(puf), "%d;", 20);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 8----------------------------\r\n");
//远程主机IP2
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", 192,168,1,100, 5000);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 9----------------------------\r\n");
//主动间隔1
sprintf((char *)(puf), "%d;", 10);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 10----------------------------\r\n");
//主动间隔2
sprintf((char *)(puf), "%d;", 20);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 11----------------------------\r\n");
//远程主机IP3
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", 192,168,1,150, 5000);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 12----------------------------\r\n");
//主动间隔1
sprintf((char *)(puf), "%d;", 20);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 13----------------------------\r\n");
//主动间隔2
sprintf((char *)(puf), "%d;", 10);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 14----------------------------\r\n");
//序列号
sprintf((char *)(puf), "101010101010;" );
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 15----------------------------\r\n");
//软件版本号
sprintf((char *)(puf), "%d.%d;", 2,5,0);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 16----------------------------\r\n");
//硬件版本号
sprintf((char *)(puf), "%d.%d;", 3,0);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 17----------------------------\r\n");

//关风扇温度
sprintf((char *)(puf), "%d;", 50);
strcat((char *)(data_response_buf),puf);
//开风扇1温度
sprintf((char *)(puf), "%d;", 55);
strcat((char *)(data_response_buf),puf);
//开风扇2温度
sprintf((char *)(puf), "%d;", 60);
strcat((char *)(data_response_buf),puf);
//降功率温度
sprintf((char *)(puf), "%d;", 65);
strcat((char *)(data_response_buf),puf);
//过温关机保护温度
sprintf((char *)(puf), "%d;", 70);
strcat((char *)(data_response_buf),puf);
//降功率电流
sprintf((char *)(puf), "%.3f;", 1.500);
strcat((char *)(data_response_buf),puf);
//过流保护电流
sprintf((char *)(puf), "%.3f;", 1.800);
strcat((char *)(data_response_buf),puf);
//过流保护重启延时
sprintf((char *)(puf), "%d;", 300);
strcat((char *)(data_response_buf),puf);
//过流保护重启次数
sprintf((char *)(puf), "%d;", 5);
strcat((char *)(data_response_buf),puf);
//额定功率
sprintf((char *)(puf), "%dW;", (int)30);
strcat((char *)(data_response_buf),puf);
//反射保护门限
sprintf((char *)(puf), "%d;", 5);
strcat((char *)(data_response_buf),puf);
//输入最小功率			
sprintf((char *)(puf), "%.2f;", -20.00);
strcat((char *)(data_response_buf),puf);  	
 */
	
//设备地址
sprintf((char *)(puf), "%d;", g_DI.Cfg.DeviceID);
strcat((char *)(data_response_buf),puf);

//IP地址
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", g_DI.Cfg.IP_Addr[0],g_DI.Cfg.IP_Addr[1],g_DI.Cfg.IP_Addr[2],g_DI.Cfg.IP_Addr[3],clocal_port);
strcat((char *)(data_response_buf),puf);

//子网掩码
sprintf((char *)(puf), "%d.%d.%d.%d;", g_DI.Cfg.IP_Mask[0],g_DI.Cfg.IP_Mask[1],g_DI.Cfg.IP_Mask[2],g_DI.Cfg.IP_Mask[3]);
strcat((char *)(data_response_buf),puf);

//默认网关
sprintf((char *)(puf), "%d.%d.%d.%d;", g_DI.Cfg.IP_GateWay[0],g_DI.Cfg.IP_GateWay[1],g_DI.Cfg.IP_GateWay[2],g_DI.Cfg.IP_GateWay[3]);
strcat((char *)(data_response_buf),puf);

//本地管理1
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", g_DI.Cfg.IP_LOCAL1[0],g_DI.Cfg.IP_LOCAL1[1],g_DI.Cfg.IP_LOCAL1[2],g_DI.Cfg.IP_LOCAL1[3], g_DI.Cfg.LOCAL1NetPort1);
strcat((char *)(data_response_buf),puf);

//本地管理2
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", g_DI.Cfg.IP_LOCAL2[0],g_DI.Cfg.IP_LOCAL2[1],g_DI.Cfg.IP_LOCAL2[2],g_DI.Cfg.IP_LOCAL2[3], g_DI.Cfg.LOCAL1NetPort2);
strcat((char *)(data_response_buf),puf);

//远程主机IP1
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", g_DI.Cfg.IP_RemoteHost1[0],g_DI.Cfg.IP_RemoteHost1[1],g_DI.Cfg.IP_RemoteHost1[2],g_DI.Cfg.IP_RemoteHost1[3],g_DI.Cfg.RemoteHostNetPort1);
strcat((char *)(data_response_buf),puf);

//主动间隔1
sprintf((char *)(puf), "%d;", g_DI.Cfg.RemoteHostDelay1_1);
strcat((char *)(data_response_buf),puf);

//主动间隔2
sprintf((char *)(puf), "%d;", g_DI.Cfg.RemoteHostDelay1_2);
strcat((char *)(data_response_buf),puf);

//远程主机IP2
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", g_DI.Cfg.IP_RemoteHost2[0],g_DI.Cfg.IP_RemoteHost2[1],g_DI.Cfg.IP_RemoteHost2[2],g_DI.Cfg.IP_RemoteHost2[3],g_DI.Cfg.RemoteHostNetPort2);
strcat((char *)(data_response_buf),puf);

//主动间隔1
sprintf((char *)(puf), "%d;", g_DI.Cfg.RemoteHostDelay2_1);
strcat((char *)(data_response_buf),puf);

//主动间隔2
sprintf((char *)(puf), "%d;", g_DI.Cfg.RemoteHostDelay2_2);
strcat((char *)(data_response_buf),puf);

//远程主机IP3
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", g_DI.Cfg.IP_RemoteHost3[0],g_DI.Cfg.IP_RemoteHost3[1],g_DI.Cfg.IP_RemoteHost3[2],g_DI.Cfg.IP_RemoteHost3[3],g_DI.Cfg.RemoteHostNetPort3);
strcat((char *)(data_response_buf),puf);

//主动间隔1
sprintf((char *)(puf), "%d;", g_DI.Cfg.RemoteHostDelay3_1);
strcat((char *)(data_response_buf),puf);

//主动间隔2
sprintf((char *)(puf), "%d;", g_DI.Cfg.RemoteHostDelay3_2);
strcat((char *)(data_response_buf),puf);

//序列号
sprintf((char *)(puf), "%.16s;", g_DI.Cfg.SerialNo);
//sprintf((char *)(puf), "101010101010;" );
strcat((char *)(data_response_buf),puf);

//软件版本号
sprintf((char *)(puf), "%d.%d.%d;", SOFT_VERSION/100,SOFT_VERSION%100/10,SOFT_VERSION%10);
strcat((char *)(data_response_buf),puf);

//硬件版本号
sprintf((char *)(puf), "%d.%d;", Hard_VERSION/10,Hard_VERSION%10);
strcat((char *)(data_response_buf),puf);

//关风扇温度
sprintf((char *)(puf), "%d;", g_DI.Cfg.CloseFan_TPTlimit);
strcat((char *)(data_response_buf),puf);
//开风扇1温度
sprintf((char *)(puf), "%d;", g_DI.Cfg.OpenFan1_TPTlimit);
strcat((char *)(data_response_buf),puf);
//开风扇2温度
sprintf((char *)(puf), "%d;", g_DI.Cfg.OpenFan2_TPTlimit);
strcat((char *)(data_response_buf),puf);
//降功率温度
sprintf((char *)(puf), "%d;", g_DI.Cfg.PWRRED_TPTlimit);
strcat((char *)(data_response_buf),puf);
//过温关机保护温度
sprintf((char *)(puf), "%d;", g_DI.Cfg.PWROFF_TPTlimit);
strcat((char *)(data_response_buf),puf);
//降功率电流
sprintf((char *)(puf), "%.3f;", g_DI.Cfg.PWRRED_CURlimit);
strcat((char *)(data_response_buf),puf);
//过流保护电流
sprintf((char *)(puf), "%.3f;", g_DI.Cfg.PWROFF_CURlimit);
strcat((char *)(data_response_buf),puf);
//过流保护重启延时
sprintf((char *)(puf), "%d;", g_DI.Cfg.CurRstDelay);
strcat((char *)(data_response_buf),puf);
//过流保护重启次数
sprintf((char *)(puf), "%d;", g_DI.Cfg.CurRstNum);
strcat((char *)(data_response_buf),puf);
//额定功率
sprintf((char *)(puf), "%dW;", (int)g_DI.Cfg.PWR_SetRateOut);
strcat((char *)(data_response_buf),puf);
//反射保护门限
sprintf((char *)(puf), "%d;", g_DI.Cfg.PWR_RVlimit);
strcat((char *)(data_response_buf),puf);
//输入最小功率			
sprintf((char *)(puf), "%.2f;", g_DI.Cfg.PWR_INlimit);
strcat((char *)(data_response_buf),puf);    


//输出频点
sprintf((char *)(puf), "%d.%1d;", g_DI.Excitdata.FMFRQ/10, g_DI.Excitdata.FMFRQ%10);
strcat((char *)(data_response_buf),puf); 

//激励器预加重
if(g_DI.Excitdata.FMPre_Emphasis == 0)
{
	sprintf((char *)(puf), "0us;");
}
else if(g_DI.Excitdata.FMPre_Emphasis == 1)
{
	sprintf((char *)(puf), "50us;");
}
else if(g_DI.Excitdata.FMPre_Emphasis == 2)
{
	sprintf((char *)(puf), "75us;");
}
else
{
	sprintf((char *)(puf), "err;");
}
strcat((char *)(data_response_buf),puf); 

//激励器输出增益
sprintf((char *)(puf), "%d;", g_DI.Excitdata.FMGain_RF);
strcat((char *)(data_response_buf),puf); 

//调制模式 0:关闭 1:立体声 2:L单声道 4:R单声道 6:L+R声道
if(g_DI.Excitdata.FMMODULATIONMODE == 0)
{
	sprintf((char *)(puf), "Clsoe;");
}
else if(g_DI.Excitdata.FMMODULATIONMODE == 1)
{
	sprintf((char *)(puf), "Stereo;");
}
else if(g_DI.Excitdata.FMMODULATIONMODE == 2)
{
	sprintf((char *)(puf), "L Sound;");
}
else if(g_DI.Excitdata.FMMODULATIONMODE == 4)
{
	sprintf((char *)(puf), "R Sound;");
}
else if(g_DI.Excitdata.FMMODULATIONMODE == 6)
{
	sprintf((char *)(puf), "L+R Sound;");
}
else
{
	sprintf((char *)(puf), "err;");
}
strcat((char *)(data_response_buf),puf); 

//粗调调制度0~7
sprintf((char *)(puf), "%d;", g_DI.Excitdata.FMSIGNAL_VAL);
strcat((char *)(data_response_buf),puf); 

//调制度0~255
sprintf((char *)(puf), "%d;", g_DI.Excitdata.FMMODULATIONVAL);
strcat((char *)(data_response_buf),puf); 

//RDS幅度 0~15 0:无载波
sprintf((char *)(puf), "%d;", g_DI.Excitdata.FMRDSVAL);
strcat((char *)(data_response_buf),puf); 


    return RESPONSE_PAGE_SET_CGI_RSP_URL;
		
}
static char *DevicePost_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
	u8 i = 0 ,err;
	u8 SendBuffer[200];
	u8 Buf[10];
	u8 TxLen = 0;

//	for(i = 0;i < iNumParams;i++)
//	{
//		printf("pcParam[%d]: %s  ",i,pcParam[i]);
//		printf("pcValue[%d]: %s  \n",i,pcValue[i]);
//	}
	//设备地址
	GetString(pcValue[0],"deviceAddr=",&Buf[0]);
	g_DI.Cfg.DeviceID = atoi(Buf);
	if(g_DI.Cfg.DeviceID >= 255)
	{
		g_DI.Cfg.DeviceID = 255;
	}
//	printf("g_DI.Cfg.DeviceID: %d \n",g_DI.Cfg.DeviceID);
	
	
//IP地址
str2shuIP(pcValue[1], g_DI.Cfg.IP_Addr);
//printf( "g_DI.Cfg.IP_Addr: %d.%d.%d.%d:%d \n", g_DI.Cfg.IP_Addr[0],g_DI.Cfg.IP_Addr[1],g_DI.Cfg.IP_Addr[2],g_DI.Cfg.IP_Addr[3],clocal_port);

//子网掩码
str2shuIP(pcValue[2], g_DI.Cfg.IP_Mask);
//printf( "g_DI.Cfg.IP_Mask: %d.%d.%d.%d \n", g_DI.Cfg.IP_Mask[0],g_DI.Cfg.IP_Mask[1],g_DI.Cfg.IP_Mask[2],g_DI.Cfg.IP_Mask[3]);


//默认网关
str2shu(pcValue[3], g_DI.Cfg.IP_GateWay,4);
//printf( "g_DI.Cfg.IP_GateWay: %d.%d.%d.%d\n", g_DI.Cfg.IP_GateWay[0],g_DI.Cfg.IP_GateWay[1],g_DI.Cfg.IP_GateWay[2],g_DI.Cfg.IP_GateWay[3]);

//本地管理1
g_DI.Cfg.LOCAL1NetPort1 = str2shuIP(pcValue[4], g_DI.Cfg.IP_LOCAL1);
//printf( "g_DI.Cfg.IP_LOCAL1: %d.%d.%d.%d:%d\n", g_DI.Cfg.IP_LOCAL1[0],g_DI.Cfg.IP_LOCAL1[1],g_DI.Cfg.IP_LOCAL1[2],g_DI.Cfg.IP_LOCAL1[3],g_DI.Cfg.LOCAL1NetPort1);

//本地管理2
g_DI.Cfg.LOCAL1NetPort2 = str2shuIP(pcValue[5], g_DI.Cfg.IP_LOCAL2);
//printf( "g_DI.Cfg.IP_LOCAL2: %d.%d.%d.%d:%d\n", g_DI.Cfg.IP_LOCAL2[0],g_DI.Cfg.IP_LOCAL2[1],g_DI.Cfg.IP_LOCAL2[2],g_DI.Cfg.IP_LOCAL2[3],g_DI.Cfg.LOCAL1NetPort2);
//远程主机IP1
g_DI.Cfg.RemoteHostNetPort1 = str2shuIP(pcValue[6], g_DI.Cfg.IP_RemoteHost1);
//printf( "g_DI.Cfg.IP_RemoteHost1: %d.%d.%d.%d:%d\n", g_DI.Cfg.IP_RemoteHost1[0],g_DI.Cfg.IP_RemoteHost1[1],g_DI.Cfg.IP_RemoteHost1[2],g_DI.Cfg.IP_RemoteHost1[3],g_DI.Cfg.RemoteHostNetPort1);

//主动间隔1
g_DI.Cfg.RemoteHostDelay1_1 = atoi(pcValue[7]); 
//printf( "RemoteHostDelay1:%d\n", g_DI.Cfg.RemoteHostDelay1_1);

//主动间隔2
g_DI.Cfg.RemoteHostDelay1_2 = atoi(pcValue[8]); 
//printf( "RemoteHostDelay2: %d\n", g_DI.Cfg.RemoteHostDelay1_2);

//远程主机IP2
g_DI.Cfg.RemoteHostNetPort2 = str2shuIP(pcValue[9], g_DI.Cfg.IP_RemoteHost2);
//printf( "g_DI.Cfg.IP_RemoteHost2: %d.%d.%d.%d:%d\n", g_DI.Cfg.IP_RemoteHost2[0],g_DI.Cfg.IP_RemoteHost2[1],g_DI.Cfg.IP_RemoteHost2[2],g_DI.Cfg.IP_RemoteHost2[3],g_DI.Cfg.RemoteHostNetPort2);

//主动间隔1
g_DI.Cfg.RemoteHostDelay2_1 = atoi(pcValue[10]); 
//printf( "RemoteHostDelay1:%d\n", g_DI.Cfg.RemoteHostDelay2_1);

//主动间隔2
g_DI.Cfg.RemoteHostDelay2_2 = atoi(pcValue[11]); 
//printf( "RemoteHostDelay2: %d\n", g_DI.Cfg.RemoteHostDelay2_1);

//远程主机IP3
g_DI.Cfg.RemoteHostNetPort3 = str2shuIP(pcValue[12], g_DI.Cfg.IP_RemoteHost3);
//printf( "g_DI.Cfg.IP_RemoteHost3: %d.%d.%d.%d:%d\n", g_DI.Cfg.IP_RemoteHost3[0],g_DI.Cfg.IP_RemoteHost3[1],g_DI.Cfg.IP_RemoteHost3[2],g_DI.Cfg.IP_RemoteHost3[3],g_DI.Cfg.RemoteHostNetPort3);

//主动间隔1
g_DI.Cfg.RemoteHostDelay3_1 = atoi(pcValue[13]); 
//printf( "RemoteHostDelay1:%d\n", g_DI.Cfg.RemoteHostDelay3_1);

//主动间隔2
g_DI.Cfg.RemoteHostDelay3_2 = atoi(pcValue[14]); 
//printf( "RemoteHostDelay2: %d\n", g_DI.Cfg.RemoteHostDelay3_1);

Sys_Flag.Bit.fSaveStart = TRUE;
Sys_Flag.Bit.fSave = TRUE;
Sys_Flag.Bit.fNetRecom = TRUE;

return "/rw.html";	
}
static char *ParamPost_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
  	u8 i = 0 , err;
	u8 buf[5];
	u8 SendBuffer[200];
	u8 PaTxLen = 0;
	u16 frqout = 0, jlqGain = 0, jlqEmphasis = 0;
	u8	FmTzMode = 5;	//调制模式 0:关闭 1:立体声 2:L单声道 4:R单声道 6:L+R声道
	u8	FmTzVAL = 0;//调制度0~255
	u8	FmRdsVal;	//RDS幅度 0~15 0:无载波
	u8	FmtempVAL = 0;//调制度0~7
	
	u8  PreEmpOut = 5;
	
//	for(i = 0;i < iNumParams;i++)
//	{
//		printf("pcParam[%d]: %s  ",i,pcParam[i]);
//		printf("pcValue[%d]: %s  \n",i,pcValue[i]);
//	}
//关风扇温度
GetString(pcValue[0],"offFanTemperature=",&buf[0]);
g_DI.Cfg.CloseFan_TPTlimit = atoi(buf);
//printf( "g_DI.Cfg.CloseFan_TPTlimit : %d \n", g_DI.Cfg.CloseFan_TPTlimit);

//开风扇1温度
g_DI.Cfg.OpenFan1_TPTlimit = atoi(pcValue[1]);
//printf( "g_DI.Cfg.OpenFan1_TPTlimit: %d\n", g_DI.Cfg.OpenFan1_TPTlimit);

//开风扇2温度
g_DI.Cfg.OpenFan2_TPTlimit = atoi(pcValue[2]);
//printf( "g_DI.Cfg.OpenFan2_TPTlimit: %d\n", g_DI.Cfg.OpenFan2_TPTlimit);

//降功率温度
g_DI.Cfg.PWRRED_TPTlimit = atoi(pcValue[3]);
//printf( "g_DI.Cfg.PWRRED_TPTlimit: %d\n", g_DI.Cfg.PWRRED_TPTlimit);

//过温关机保护温度
g_DI.Cfg.PWROFF_TPTlimit = atoi(pcValue[4]);
//printf( "g_DI.Cfg.PWROFF_TPTlimit: %d\n", g_DI.Cfg.PWROFF_TPTlimit);

//降功率电流
g_DI.Cfg.PWRRED_CURlimit = atof(pcValue[5]);
//printf( "g_DI.Cfg.PWRRED_CURlimit: %.3f\n", g_DI.Cfg.PWRRED_CURlimit);

//过流保护电流
g_DI.Cfg.PWROFF_CURlimit = atof(pcValue[6]);
//printf( "g_DI.Cfg.PWROFF_CURlimit: %.3f\n", g_DI.Cfg.PWROFF_CURlimit);

//过流保护重启延时
g_DI.Cfg.CurRstDelay = atoi(pcValue[7]);
//printf( "g_DI.Cfg.CurRstDelay: %d\n", g_DI.Cfg.CurRstDelay);

//过流保护重启次数
g_DI.Cfg.CurRstNum = atoi(pcValue[8]);
//printf( "g_DI.Cfg.CurRstNum: %d\n", g_DI.Cfg.CurRstNum);

//额定功率
memset(buf, 0, 5);
//printf( "strlen((u8 *)pcValue[9]): %d\n", strlen((u8 *)pcValue[9]));
for(i = 0;i < (strlen((u8 *)pcValue[9]) - 1);i++)
{
	buf[i] = pcValue[9][i];
}	
g_DI.Cfg.PWR_SetRateOut = atoi(buf);
//printf( "g_DI.Cfg.PWR_RateOut: %dW\n", (int)g_DI.Cfg.PWR_RateOut);

//反射保护门限
g_DI.Cfg.PWR_RVlimit = atoi(pcValue[10]);
//printf( "g_DI.Cfg.PWR_RVlimit: %d\n", g_DI.Cfg.PWR_RVlimit);

//输入最小功率
g_DI.Cfg.PWR_INlimit = atof(pcValue[11]);			
//printf( "g_DI.Cfg.PWR_INlimit: %.2f\n", g_DI.Cfg.PWR_INlimit);


Sys_Flag.Bit.fSaveStart = TRUE;
Sys_Flag.Bit.fSave = TRUE;


return "/rw.html";
}

static char *JlqPost_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
	u8 i = 0 , err;
	u8 buf[5];
	u8 SendBuffer[200];
	u8 PaTxLen = 0;
	u16 frqout = 0, jlqGain = 0, jlqEmphasis = 0;
	u8	FmTzMode = 5;	//调制模式 0:关闭 1:立体声 2:L单声道 4:R单声道 6:L+R声道
	u8	FmTzVAL = 0;//调制度0~255
	u8	FmRdsVal;	//RDS幅度 0~15 0:无载波
	u8	FmtempVAL = 0;//调制度0~7
	
	u8  PreEmpOut = 5;
	
//	for(i = 0;i < iNumParams;i++)
//	{
//		printf("pcParam[%d]: %s  ",i,pcParam[i]);
//		printf("pcValue[%d]: %s  \n",i,pcValue[i]);
//	}
#if IWDG_EN
	IWDG_ReloadCounter(); //手动喂狗  
#endif	
	//输出频点
	GetString(pcValue[0],"fmfrequency=",&buf[0]);
	g_DI.Cfg.P_FmFrq = atof(buf);
	frqout = (u16)(atof(buf) *10);
//	printf( "frqout: %d\n", frqout);

	//激励器预加重
	memset(buf, 0, 5);
	//printf( "strlen((u8 *)pcValue[13]): %d\n", strlen((u8 *)pcValue[13]));
	for(i = 0;i < (strlen((u8 *)pcValue[1]) - 2);i++)
	{
		buf[i] = pcValue[1][i];
	}	
	jlqEmphasis = (u8)atoi(buf);
	//printf( "jlqEmphasis: %d\n", jlqEmphasis);

	//激励器输出增益
	jlqGain = (u8)atoi(pcValue[2]);			
//	printf( "jlqGain: %d\n", jlqGain);
	if(jlqGain >=63)
	{
		jlqGain =63;
	}

	if(jlqEmphasis == 0)//00：0us  01:50us  02:75us
	{
		PreEmpOut = 0;	
	}
	else if(jlqEmphasis == 50)//00：0us  01:50us  02:75us
	{
		PreEmpOut = 1;	
	}
	else if(jlqEmphasis == 75)//00：0us  01:50us  02:75us
	{
		PreEmpOut = 2;	
	}
//	printf( "00-0us  01-50us  02-75us PreEmpOut: %d\n", PreEmpOut);

	//调制模式 0:关闭 1:立体声 2:L单声道 4:R单声道 6:L+R声道
	if(strstr(pcValue[3],"L+R") != NULL)
	{
		FmTzMode = 6;
	}
	else if(strstr(pcValue[3],"R") != NULL)
	{
		FmTzMode = 4;
	}
	else if(strstr(pcValue[3],"L") != NULL)
	{
		FmTzMode = 2;
	}
	else if(strstr(pcValue[3],"Stereo") != NULL)
	{
		FmTzMode = 1;
	}
	else if(strstr(pcValue[3],"Clsoe") != NULL)
	{
		FmTzMode = 0;
	}
	FmtempVAL = (u8)atoi(pcValue[4]);
	if(FmtempVAL >=7)
	{
		FmtempVAL = 7;
	}
	FmTzVAL = (u8)atoi(pcValue[5]);
	FmRdsVal = (u8)atoi(pcValue[6]);
	if(FmRdsVal >15)
	{
		FmRdsVal = 15;
	}

//	printf( "FmTzMode: %d  FmtempVAL: %d FmTzVAL:%d FmRdsVal:%d\n", FmTzMode,FmtempVAL,FmTzVAL,FmRdsVal);
	/****************激励器输出配置*********************/
	if(g_TMTLink[0] == 1) 
	{
		fEitRTU = TRUE;
		OSSemPend(JLQRS232SEM, 0, &err);
		//输出频点
		SendBuffer[0]= g_DI.Cfg.Excit_IPHead; 
		SendBuffer[1]= 0x35;
		SendBuffer[2]= 0xC3;
		SendBuffer[3]= 0xFC;
		SendBuffer[4]= 0xC1;
		SendBuffer[5]= 0xEE;
		SendBuffer[6]= 0xC9;
		SendBuffer[7]= 0xE8;
		SendBuffer[8]= 0x20;
		SendBuffer[9]= (u8)frqout;
		SendBuffer[10]= (u8)(frqout>>8);
		SendBuffer[11]= (u8)(PreEmpOut);	
		SendBuffer[12]= (u8)jlqGain;
		SendBuffer[13]= (u8)FmTzMode;
		SendBuffer[14]= (u8)FmTzVAL;
		SendBuffer[15]= (u8)FmRdsVal;
		SendBuffer[16]= (u8)FmtempVAL;
		Excit_RS232_Send(SendBuffer,17, 10);

		OSSemPost(JLQRS232SEM);
		fEitRTU = FALSE;
		ATT_Senddata(0);
		Sys_Flag.Bit.fSaveStart = TRUE;
		Sys_Flag.Bit.fSave = TRUE;	
		
	}


	return "/rw.html";
}

static char *ReadControlData_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
    u8 i;
	unsigned char *buf;
	u8 puf[10];
	int  index;
	
	for(i =0 ;i<20;i++ );//延时一下，否则会重启
	clear_response_bufer(data_response_buf);		//清除缓冲区的内容			
		
	
	sprintf((char *)(puf), "%d;",(u8)(g_DI.Pa[0].Data.Power_SET));
	strcat((char *)(data_response_buf),puf);

	strcat((char *)(data_response_buf), "1;");			
     
    return RESPONSE_PAGE_SET_CGI_RSP_URL;
}

static char *PowerPostData_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
	u8 i = 0, err, Opetype = 0;
	float CalValue;
	u8 Pvalue = 0;
	u8 buf[10];
	u8 SendBuffer[150];
	u8 PaTxLen = 0;
	float fin,fout,ftmp;
//	printf("iNumParams: %d  \n",iNumParams);
//	for(i = 0;i < iNumParams;i++)
//	{
//		printf("pcParam[%d]: %s  ",i,pcParam[i]);
//		printf("pcValue[%d]: %s  \n",i,pcValue[i]);
//	}

    Opetype = atoi(pcValue[1]);
//	printf("Opetype: %d  \n",Opetype);

	if(Opetype == 1)	//校正入射
	{
		GetString(pcValue[0],"value=",&buf[0]);
		CalValue = atof(buf);
//		printf("Out CalValue: %f  \n",CalValue);
		fout =  CalValue;
		if( fout == 0 )
		{
			g_DI.Cfg.P_OutCal = 0;
		}
		else
		{
			
			ftmp = GetPwr( g_DI.Pa[0].Data.P_OutVal,PowerCal_Out[0],PowerCal_Out[1],0);
		
			fout = 10*log10(fout*1000);
			fout = sqrt(50*pow(10,fout/10));
			
			if( ftmp != 0 )
				g_DI.Cfg.P_OutCal = fout*fout/(ftmp*ftmp*50000);
			else
				  g_DI.Cfg.P_OutCal = 0;
		}

		Sys_Flag.Bit.fSaveStart = TRUE;
		Sys_Flag.Bit.fSave = TRUE;
	}
	else if(Opetype == 2)	//校正输入
	{
		GetString(pcValue[0],"value=",&buf[0]);
		CalValue = atof(buf);
//		printf("CalValue: %f  \n",CalValue);
		
		fout = CalValue;
		if( fout == 0 )
		{
			g_DI.Cfg.P_InCal = 0;
		}
		else
		{
			ftmp = GetPwr( g_DI.Pa[0].Data.P_InVal,PowerCal_In[0],PowerCal_In[1],0);
			
			//fout = 10*log10(fout*1000);
			
			fout = sqrt(50*pow(10,fout/10));
			if( ftmp != 0 )
				g_DI.Cfg.P_InCal = fout*fout/(ftmp*ftmp*50000);
			else
				  g_DI.Cfg.P_InCal = 0;
		}
		
		Sys_Flag.Bit.fSaveStart = TRUE;
		Sys_Flag.Bit.fSave = TRUE;
	}
	else if(Opetype == 3)	//校正反射
	{
		GetString(pcValue[0],"value=",&buf[0]);
		CalValue = atof(buf);
//		printf("CalValue: %f  \n",CalValue);
		fout =  CalValue;
		if( fout == 0 )
		{
			g_DI.Cfg.P_RefCal = 0;
		}
		else
		{
			ftmp = GetPwr( g_DI.Pa[0].Data.P_ReflexVal,PowerCal_Ref[0],PowerCal_Ref[1],0);
			
			fout = 10*log10(fout*1000);
			fout = sqrt(50*pow(10,fout/10));
			if( ftmp != 0 )
				g_DI.Cfg.P_RefCal = fout*fout/(ftmp*ftmp*50000);
			else
				  g_DI.Cfg.P_RefCal = 0;
		}
		
		Sys_Flag.Bit.fSaveStart = TRUE;
		Sys_Flag.Bit.fSave = TRUE;
	}
	else if(Opetype == 4)	//手动调节电位器 -
	{
		GetString(pcValue[0],"value=",&buf[0]);
		Pvalue = atoi(buf);
//		printf(" - Pvalue: %d  \n",Pvalue);
		g_DI.Cfg.P_Res_Front_Target = g_DI.Pa[0].Data.RES_Rear - Pvalue;
		if(g_DI.Cfg.P_Res_Front_Target <=0)
		{
			g_DI.Cfg.P_Res_Front_Target = 0;
		}
		ATT_Senddata( g_DI.Cfg.P_Res_Front_Target  );

		g_DI.Cfg.P_AutoContrl =0;
		Sys_Flag.Bit.fSaveStart = TRUE;
		Sys_Flag.Bit.fSave = TRUE;

	}
	else if(Opetype == 5)	//手动调节电位器 +
	{
		GetString(pcValue[0],"value=",&buf[0]);
		Pvalue = atoi(buf);
//		printf(" + Pvalue: %d  \n",Pvalue);
		g_DI.Cfg.P_Res_Front_Target = g_DI.Pa[0].Data.RES_Rear + Pvalue;
		if(g_DI.Cfg.P_Res_Front_Target >=62)
		{
			g_DI.Cfg.P_Res_Front_Target = 62;
		}
		ATT_Senddata( g_DI.Cfg.P_Res_Front_Target  );
		g_DI.Cfg.P_AutoContrl =0;
		Sys_Flag.Bit.fSaveStart = TRUE;
		Sys_Flag.Bit.fSave = TRUE;
	
	}

	return "/control.html";
}

static char *PowerctlPost_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
	u8 i = 0, err, OpeCtrType = 0;
	u8 Pvalue = 0 , CurPwr = 0 , CalPwr = 0;
	u8 buf[10];
	u8 SendBuffer[150];
	u8 PaTxLen = 0;
	
//	printf("iNumParams: %d  \n",iNumParams);

//	for(i = 0;i < iNumParams;i++)
//	{
//		printf("pcParam[%d]: %s  ",i,pcParam[i]);
//		printf("pcValue[%d]: %s  \n",i,pcValue[i]);
//	}
	if(iNumParams == 1)		//开机，关机，自动，开风扇1，关风扇1，开风扇2，关风扇2
	{
		GetString(pcValue[0],"actionid=",&buf[0]);
		OpeCtrType = atoi(buf);	
//		printf("OpeCtrType: %d  \n",OpeCtrType);
		
		switch(OpeCtrType)
		{
			case 1:	 //开机
				
				AP_Startup();
			    g_DI.Pa[0].Data.P_DownOffset = 0;
			    g_DI.Pa[0].Data.I_OverLoadRestartCount = 0;
				if(g_DI.Cfg.P_AutoContrl == 0)
				{
					ManualChanging = 0;
				}
			
				break;
			case 2:	//关机
				
				AP_Shutdown();
				ATT_Senddata(0);
				g_DI.Pa[0].Data.P_DownOffset = 0;
				break;

			case 8:	 //开风扇1 43 35 c3 fc c1 ee ba cf 01 b6 f4  命令合  
				
				 FAN_ManCtrl_Delay = 60;
				 SW_FAN1_ON;	
				break;
			case 9:	//关风扇1  43 35 c3 fc c1 ee b7 d6 01 2c a7  命令分  
				FAN_ManCtrl_Delay = 60;
				SW_FAN1_OFF;	
				break;
			case 10: //开风扇2
				FAN_ManCtrl_Delay = 60;
				SW_FAN2_ON;
				break;
			case 11: //关风扇2
				FAN_ManCtrl_Delay = 60;
				SW_FAN2_OFF;
				break;
			case 15: //恢复出厂设置
				ReadbufFromX5323(2000,Save_Size,(u8*)&g_DI.Cfg);
				OSTimeDlyHMSM(0, 0, 0, 70);
				WritebufToX5323(10,Save_Size+50,(u8*)&g_DI.Cfg);
				OSTimeDlyHMSM(0, 0, 0, 70);
				ReadbufFromX5323(10,Save_Size,(u8*)&g_DI.Cfg);
				break;
			default:
				break;
		}			
	}
	else if(iNumParams == 2)   //初始化，功率步进+ -
	{
		OpeCtrType = atoi(pcValue[1]);
//		printf("OpeCtrType: %d  \n",OpeCtrType);

		GetString(pcValue[0],"value=",&buf[0]);
		Pvalue = atoi(buf);	
//		printf("Pvalue: %d  \n",Pvalue);		
		switch(OpeCtrType)
		{
			case 3:	 //初始化
				ATT_Senddata(0);
				AP_Startup();
				
				if(Pvalue >= (g_DI.Cfg.PWR_SetRateOut*1.1))
				{
					Pvalue = g_DI.Cfg.PWR_SetRateOut*1.1;
				}				
				g_DI.Cfg.PWR_RateOut = Pvalue;
				g_DI.Cfg.P_AutoContrl = 1;
				g_DI.Pa[0].Data.RES_Front = 0;
				Sys_Flag.Bit.fSaveStart = TRUE;
				Sys_Flag.Bit.fSave = TRUE;
				g_DI.Pa[0].Data.I_OverLoadRestartCount = 0;
				break;
		
			case 4:	 //自动	 43 35 c3 fc c1 ee ba cf 04 76 f7
				g_DI.Pa[0].Data.P_DownOffset = 0;
				AP_Startup();	
				g_DI.Cfg.P_AutoContrl = 1;
				g_DI.Pa[0].Data.RES_Front = 0;
				Sys_Flag.Bit.fSaveStart = TRUE;
				Sys_Flag.Bit.fSave = TRUE;
				g_DI.Pa[0].Data.I_OverLoadRestartCount = 0;	
				break;
			
			case 5:	//功率步进-
				CurPwr = (u8)g_DI.Cfg.PWR_RateOut;
				CalPwr = CurPwr - Pvalue;
			    
				if(CalPwr >= (g_DI.Cfg.PWR_SetRateOut*1.1))
				{
					CalPwr = g_DI.Cfg.PWR_SetRateOut*1.1;	
				}
				if(CalPwr <= 0)
				{
					CalPwr = 0;	
				}
				g_DI.Cfg.PWR_RateOut = CalPwr;
//				printf("- CalPwr: %d \n",CalPwr);
				AP_Startup();
				g_DI.Cfg.P_AutoContrl = 1;
				g_DI.Pa[0].Data.RES_Front = 0;
				Sys_Flag.Bit.fSaveStart = TRUE;
				Sys_Flag.Bit.fSave = TRUE;
				break;

			case 6:	//功率步进+
				CurPwr = (u8)g_DI.Cfg.PWR_RateOut;
				CalPwr = CurPwr + Pvalue;
				if(CalPwr >= (g_DI.Cfg.PWR_SetRateOut*1.1))
				{
					CalPwr = g_DI.Cfg.PWR_SetRateOut*1.1;
				}
				if(CalPwr <= 0)
				{
					CalPwr = 0;	
				}
				
				g_DI.Cfg.PWR_RateOut = CalPwr;
//				printf("+ CalPwr: %d \n",CalPwr);
				AP_Startup();
				g_DI.Cfg.P_AutoContrl = 1;
				g_DI.Pa[0].Data.RES_Front = 0;				
				Sys_Flag.Bit.fSaveStart = TRUE;
				Sys_Flag.Bit.fSave = TRUE;
				break;

			default:
				break;
		}		
	}

	return "/control.html";
}


void datetonstr_deal(u16 num,char len)
{
	unsigned char *buf;
	u16 number;
	number = num;
	num2nstr(number,buf,len);
	strcat((char *)(data_response_buf),buf);

}
//SSI的Handler句柄
static u16_t SSIHandler(int iIndex,char *pcInsert,int iInsertLen)
{
//	printf("iIndex : %d \r\n",iIndex);
	switch(iIndex)
	{
		case 0: //printf("ipserver_Handler\r\n");
//				ipserver_Handler(pcInsert);
				break;
		case 1://printf("ipaddr_Handler\r\n");
//				ipaddr_Handler(pcInsert);
				break;
	}
		return strlen(pcInsert);
}






//SSI句柄初始化
void httpd_ssi_init(void)
{  
	//配置SSI句柄
	http_set_ssi_handler(SSIHandler,ppcTAGs,NUM_CONFIG_SSI_TAGS);
}

//CGI句柄初始化
void httpd_cgi_init(void)
{ 
	//配置CGI句柄
	http_set_cgi_handlers(ppcURLs, NUM_CONFIG_CGI_URIS);
}







