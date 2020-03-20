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

#define NUM_CONFIG_CGI_URIS	(sizeof(ppcURLs ) / sizeof(tCGI))  //CGI��URI����
#define NUM_CONFIG_SSI_TAGS	(sizeof(ppcTAGs) / sizeof (char *))  //SSI��TAG����
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

static const char *ppcTAGs[]=  //SSI��Tag
{
	"s", //ADCֵ
};
static const tCGI ppcURLs[]= //cgi����
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
//���������������
void  clear_response_bufer(unsigned char *buffer)
{
  memset(buffer,0,strlen((unsigned char*)buffer));
}
void str2shu(u8 *instr,u8 *outstr, u8 len)
{
    char   *p;
	char a;
	u16 ipport = 0;	
	p=strtok(instr,".");//��һ�ε���,��Ҫ����Ĵ�������ַs    
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
	
	p=strtok(instr,".");//��һ�ε���,��Ҫ����Ĵ�������ַs    
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
//m^n����
//����ֵ:m^n�η�.
u32 Num_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//������->�ַ���ת������
//��num����(λ��Ϊlen)תΪ�ַ���,�����buf����
//num:����,����
//buf:�ַ�������
//len:����
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

//�����и�������->�ַ���ת������
//����������s16�������ڱ��������Ϊu16����
//��webҳ�����-65536�Ĵ���õ�����
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

//��web�ͻ��������������ʱ��,ʹ�ô˺�����CGI handler����
static int FindCGIParameter(const char *pcToFind,char *pcParam[],int iNumParams)
{
	int iLoop;
	for(iLoop = 0;iLoop < iNumParams;iLoop ++ )
	{
		if(strcmp(pcToFind,pcParam[iLoop]) == 0)
		{
			return (iLoop); //����iLOOP
		}
	}
	return (-1);
}
//��web�ͻ��������������ʱ��,ʹ�ô˺�����CGI handler����
static int FindCGI_Parameter_Two(const char *pcToFind,const char *pcToFind0,char *pcParam[])
{


	if(strcmp(pcToFind,pcParam[1]) == 0)
	{
		return (0); //����0
	}
	if(strcmp(pcToFind0,pcParam[1]) == 0)
	{
		return (1); //����1
	}
	return (-1);
}
//��web�ͻ��������������ʱ��,ʹ�ô˺�����CGI handler����
static int FindCGI_Parameter_Three(const char *pcToFind,const char *pcToFind0,const char *pcToFind1,char *pcParam[])
{


	if(strcmp(pcToFind,pcParam[1]) == 0)
	{
		return (0); //����0
	}
	if(strcmp(pcToFind0,pcParam[1]) == 0)
	{
		return (1); //����1
	}
	if(strcmp(pcToFind1,pcParam[1]) == 0)
	{
		return (2); //����2
	}
	return (-1);
}

static char *ReadPaState_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
   u8 i;
	unsigned char *buf;
	u8 puf[10];
	int  index;
for(i =0 ;i<20;i++ );//��ʱһ�£����������	
//g_DI.Pa[0].Data.WorkState =0x53;	// ����ģ��״̬
//g_DI.Pa[0].Data.DetState = 0xa1;	// ���ģ��״̬
//g_DI.Pa[0].Data.ProState = 0x36;	// ����ģ��״̬
//g_DI.Pa[0].Data.Power_FR = 29.89;	//���书��						
//g_DI.Pa[0].Data.Power_RV = 0.89;	//���书��
//g_DI.Pa[0].Data.CPU_TPT = 25.79;	//CPU�¶�	
//g_DI.Pa[0].Data.PA_TPT = 35.7;	//�����¶�	
//g_DI.Pa[0].Data.ENV_TPT = 27.8;	//�����¶�
//g_DI.Pa[0].Data.Power_IN = -19.5;	//���빦��
//g_DI.Pa[0].Data.PA_VOL = 50.5;	//���ŵ�ѹ
//g_DI.Pa[0].Data.PA_BECUR = 0.35;	//ǰ������
//g_DI.Pa[0].Data.PA_CUR1 = 0.79;	//���ŵ���1
//g_DI.Pa[0].Data.PA_CUR2 = 0.19;	//���ŵ���2
//g_DI.Pa[0].Data.Power_SET = 30.0;	//��������
//g_DI.Pa[0].Data.Serial_Num = 2130;	//���к�				
//g_DI.Pa[0].Data.ATT_POT = 10;	//��һ��˥����λ��
//g_DI.Pa[0].Data.ATT_POT1 = 20;	//�ڶ���˥����λ��
	 
	clear_response_bufer(data_response_buf);		//���������������			

//	sprintf((char *)(data_response_buf), "1;0;1;0;0;1;0;0;1;0;1;0;1;1;0;0;0;1;1;0;0.2322000;0.223;13;-32.500;11.963;35.386;0.211;33.008;0.113;-273.000;0.000;30.000;60.0;70.0;0002030;");	

//���ͣ��λ  bit7
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
//����С�� bit1
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
//����������Ӧλ������λ�� bit4
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
//���ڷ���������λ
sprintf((char *)(puf), "0;");
strcat((char *)(data_response_buf),puf);

//��ʱ���ػ�����λ bit3
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
//�Զ� bit2
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
//���ڵ��ڹ��� bit0
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


//�������� bit0	
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
//�¶ȳ��� bit1
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
//������� bit2
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
//���������� bit3
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
//�¶Ƚ����� bit4
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
//���併���� bit5
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


//������ʼ��ͣ��λ bit6   
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
//��Ƶ���ư�ͨ��״̬
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
//����ѹ�쳣λ bit4
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
//դѹ״̬ bit2
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
//�����¶ȹ��� bit5
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
//����1	bit1
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
//����2	bit0
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
//���书��
sprintf((char *)(puf), "%.3f W;",g_DI.Pa[0].Data.Power_FR);
strcat((char *)(data_response_buf),puf);
//���书��
sprintf((char *)(puf), "%.3f W;",g_DI.Pa[0].Data.Power_RV);
strcat((char *)(data_response_buf),puf);
//Ч��
sprintf((char *)(puf), "%3.3f ;",(float)(100*g_DI.Pa[0].Data.Power_FR / (g_DI.Pa[0].Data.PA_VOL*g_DI.Pa[0].Data.PA_CUR1)));
strcat((char *)(data_response_buf),puf);
//���빦��
sprintf((char *)(puf), "%.3f dBm;",g_DI.Pa[0].Data.Power_IN);
strcat((char *)(data_response_buf),puf);
//���ŵ�ѹ
sprintf((char *)(puf), "%.3f V;",g_DI.Pa[0].Data.PA_VOL);
strcat((char *)(data_response_buf),puf);
//CPU�¶�
sprintf((char *)(puf), "%.3f;",g_DI.Pa[0].Data.CPU_TPT);
strcat((char *)(data_response_buf),puf);
//ǰ������
sprintf((char *)(puf), "%.3f A;",g_DI.Pa[0].Data.PA_BECUR);
strcat((char *)(data_response_buf),puf);
//�����¶�
sprintf((char *)(puf), "%.3f;",g_DI.Pa[0].Data.PA_TPT);
strcat((char *)(data_response_buf),puf);
//���ŵ���1
sprintf((char *)(puf), "%.3f A;",g_DI.Pa[0].Data.PA_CUR1);
strcat((char *)(data_response_buf),puf);
//�����¶�
sprintf((char *)(puf), "%.3f;",g_DI.Pa[0].Data.ENV_TPT);
strcat((char *)(data_response_buf),puf);
//���ŵ���2
sprintf((char *)(puf), "%.3f A;",g_DI.Pa[0].Data.PA_CUR2);
strcat((char *)(data_response_buf),puf);
//��������
//sprintf((char *)(puf), "%.3fW;",g_DI.Pa[0].Data.Power_SET);
//strcat((char *)(data_response_buf),puf);
sprintf((char *)(puf), "%dW;", (int)g_DI.Cfg.PWR_RateOut);
strcat((char *)(data_response_buf),puf);
//˥����λ�� 
sprintf((char *)(puf), "%d;",g_DI.Pa[0].Data.RES_Rear);
strcat((char *)(data_response_buf),puf);
sprintf((char *)(puf), "%d;",0);
strcat((char *)(data_response_buf),puf); 


//���Ƶ��
sprintf((char *)(puf), "%d.%1d MHz;", g_DI.Excitdata.FMFRQ/10, g_DI.Excitdata.FMFRQ%10);
strcat((char *)(data_response_buf),puf); 

//������Ԥ����
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

//�������������
sprintf((char *)(puf), "%d;", g_DI.Excitdata.FMGain_RF);
strcat((char *)(data_response_buf),puf); 

//����ģʽ 0:�ر� 1:������ 2:L������ 4:R������ 6:L+R����
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


//���ƶ�
sprintf((char *)(puf), "%d.%d%%;", g_DI.Excitdata.ModulationDCT/10,g_DI.Excitdata.ModulationDCT%10);
strcat((char *)(data_response_buf),puf); 

//RDS���� 0~15 0:���ز�
sprintf((char *)(puf), "%d;", g_DI.Excitdata.FMRDSVAL);
strcat((char *)(data_response_buf),puf); 

//���к�     
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

	for(i =0 ;i<20;i++ );//��ʱһ�£����������	
	 
	clear_response_bufer(data_response_buf);		//���������������			
		

#if IWDG_EN
	  IWDG_ReloadCounter(); //�ֶ�ι��  
#endif

//	sprintf((char *)(data_response_buf), "55;shdzld;192.168.1.207:2000;192.168.1.1;192.168.1.2;192.168.1.20;192.168.1.50:5000;20;10;192.168.1.50:5000;20;10;192.168.1.50:5000;20;10;1012102122012;2.5.0;3.0;50;55;57;65;70;1.500;1.800;30;5;30W;5;-20.00;");	

/*
//�豸��ַ
sprintf((char *)(puf), "%d;", 55);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 0----------------------------\r\n");
//�豸��
sprintf((char *)(puf), "GX_FM_30W;" );
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 1----------------------------\r\n");

//IP��ַ
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", 192,168,1,207,7000);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 2----------------------------\r\n");
//Ĭ������
sprintf((char *)(puf), "%d.%d.%d.%d;", 192,168,1,1);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 3----------------------------\r\n");
//���ع���1
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", 192,168,1,2, 5000);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 4----------------------------\r\n");
//���ع���2
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", 192,168,1,20, 5000);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 5----------------------------\r\n");
//Զ������IP1
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", 192,168,1,200, 5000);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 6----------------------------\r\n");
//�������1
sprintf((char *)(puf), "%d;", 30);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 7----------------------------\r\n");
//�������2
sprintf((char *)(puf), "%d;", 20);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 8----------------------------\r\n");
//Զ������IP2
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", 192,168,1,100, 5000);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 9----------------------------\r\n");
//�������1
sprintf((char *)(puf), "%d;", 10);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 10----------------------------\r\n");
//�������2
sprintf((char *)(puf), "%d;", 20);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 11----------------------------\r\n");
//Զ������IP3
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", 192,168,1,150, 5000);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 12----------------------------\r\n");
//�������1
sprintf((char *)(puf), "%d;", 20);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 13----------------------------\r\n");
//�������2
sprintf((char *)(puf), "%d;", 10);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 14----------------------------\r\n");
//���к�
sprintf((char *)(puf), "101010101010;" );
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 15----------------------------\r\n");
//����汾��
sprintf((char *)(puf), "%d.%d;", 2,5,0);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 16----------------------------\r\n");
//Ӳ���汾��
sprintf((char *)(puf), "%d.%d;", 3,0);
strcat((char *)(data_response_buf),puf);
printf("ReadDeviceData_CGIHandler 17----------------------------\r\n");

//�ط����¶�
sprintf((char *)(puf), "%d;", 50);
strcat((char *)(data_response_buf),puf);
//������1�¶�
sprintf((char *)(puf), "%d;", 55);
strcat((char *)(data_response_buf),puf);
//������2�¶�
sprintf((char *)(puf), "%d;", 60);
strcat((char *)(data_response_buf),puf);
//�������¶�
sprintf((char *)(puf), "%d;", 65);
strcat((char *)(data_response_buf),puf);
//���¹ػ������¶�
sprintf((char *)(puf), "%d;", 70);
strcat((char *)(data_response_buf),puf);
//�����ʵ���
sprintf((char *)(puf), "%.3f;", 1.500);
strcat((char *)(data_response_buf),puf);
//������������
sprintf((char *)(puf), "%.3f;", 1.800);
strcat((char *)(data_response_buf),puf);
//��������������ʱ
sprintf((char *)(puf), "%d;", 300);
strcat((char *)(data_response_buf),puf);
//����������������
sprintf((char *)(puf), "%d;", 5);
strcat((char *)(data_response_buf),puf);
//�����
sprintf((char *)(puf), "%dW;", (int)30);
strcat((char *)(data_response_buf),puf);
//���䱣������
sprintf((char *)(puf), "%d;", 5);
strcat((char *)(data_response_buf),puf);
//������С����			
sprintf((char *)(puf), "%.2f;", -20.00);
strcat((char *)(data_response_buf),puf);  	
 */
	
//�豸��ַ
sprintf((char *)(puf), "%d;", g_DI.Cfg.DeviceID);
strcat((char *)(data_response_buf),puf);

//IP��ַ
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", g_DI.Cfg.IP_Addr[0],g_DI.Cfg.IP_Addr[1],g_DI.Cfg.IP_Addr[2],g_DI.Cfg.IP_Addr[3],clocal_port);
strcat((char *)(data_response_buf),puf);

//��������
sprintf((char *)(puf), "%d.%d.%d.%d;", g_DI.Cfg.IP_Mask[0],g_DI.Cfg.IP_Mask[1],g_DI.Cfg.IP_Mask[2],g_DI.Cfg.IP_Mask[3]);
strcat((char *)(data_response_buf),puf);

//Ĭ������
sprintf((char *)(puf), "%d.%d.%d.%d;", g_DI.Cfg.IP_GateWay[0],g_DI.Cfg.IP_GateWay[1],g_DI.Cfg.IP_GateWay[2],g_DI.Cfg.IP_GateWay[3]);
strcat((char *)(data_response_buf),puf);

//���ع���1
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", g_DI.Cfg.IP_LOCAL1[0],g_DI.Cfg.IP_LOCAL1[1],g_DI.Cfg.IP_LOCAL1[2],g_DI.Cfg.IP_LOCAL1[3], g_DI.Cfg.LOCAL1NetPort1);
strcat((char *)(data_response_buf),puf);

//���ع���2
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", g_DI.Cfg.IP_LOCAL2[0],g_DI.Cfg.IP_LOCAL2[1],g_DI.Cfg.IP_LOCAL2[2],g_DI.Cfg.IP_LOCAL2[3], g_DI.Cfg.LOCAL1NetPort2);
strcat((char *)(data_response_buf),puf);

//Զ������IP1
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", g_DI.Cfg.IP_RemoteHost1[0],g_DI.Cfg.IP_RemoteHost1[1],g_DI.Cfg.IP_RemoteHost1[2],g_DI.Cfg.IP_RemoteHost1[3],g_DI.Cfg.RemoteHostNetPort1);
strcat((char *)(data_response_buf),puf);

//�������1
sprintf((char *)(puf), "%d;", g_DI.Cfg.RemoteHostDelay1_1);
strcat((char *)(data_response_buf),puf);

//�������2
sprintf((char *)(puf), "%d;", g_DI.Cfg.RemoteHostDelay1_2);
strcat((char *)(data_response_buf),puf);

//Զ������IP2
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", g_DI.Cfg.IP_RemoteHost2[0],g_DI.Cfg.IP_RemoteHost2[1],g_DI.Cfg.IP_RemoteHost2[2],g_DI.Cfg.IP_RemoteHost2[3],g_DI.Cfg.RemoteHostNetPort2);
strcat((char *)(data_response_buf),puf);

//�������1
sprintf((char *)(puf), "%d;", g_DI.Cfg.RemoteHostDelay2_1);
strcat((char *)(data_response_buf),puf);

//�������2
sprintf((char *)(puf), "%d;", g_DI.Cfg.RemoteHostDelay2_2);
strcat((char *)(data_response_buf),puf);

//Զ������IP3
sprintf((char *)(puf), "%d.%d.%d.%d:%d;", g_DI.Cfg.IP_RemoteHost3[0],g_DI.Cfg.IP_RemoteHost3[1],g_DI.Cfg.IP_RemoteHost3[2],g_DI.Cfg.IP_RemoteHost3[3],g_DI.Cfg.RemoteHostNetPort3);
strcat((char *)(data_response_buf),puf);

//�������1
sprintf((char *)(puf), "%d;", g_DI.Cfg.RemoteHostDelay3_1);
strcat((char *)(data_response_buf),puf);

//�������2
sprintf((char *)(puf), "%d;", g_DI.Cfg.RemoteHostDelay3_2);
strcat((char *)(data_response_buf),puf);

//���к�
sprintf((char *)(puf), "%.16s;", g_DI.Cfg.SerialNo);
//sprintf((char *)(puf), "101010101010;" );
strcat((char *)(data_response_buf),puf);

//����汾��
sprintf((char *)(puf), "%d.%d.%d;", SOFT_VERSION/100,SOFT_VERSION%100/10,SOFT_VERSION%10);
strcat((char *)(data_response_buf),puf);

//Ӳ���汾��
sprintf((char *)(puf), "%d.%d;", Hard_VERSION/10,Hard_VERSION%10);
strcat((char *)(data_response_buf),puf);

//�ط����¶�
sprintf((char *)(puf), "%d;", g_DI.Cfg.CloseFan_TPTlimit);
strcat((char *)(data_response_buf),puf);
//������1�¶�
sprintf((char *)(puf), "%d;", g_DI.Cfg.OpenFan1_TPTlimit);
strcat((char *)(data_response_buf),puf);
//������2�¶�
sprintf((char *)(puf), "%d;", g_DI.Cfg.OpenFan2_TPTlimit);
strcat((char *)(data_response_buf),puf);
//�������¶�
sprintf((char *)(puf), "%d;", g_DI.Cfg.PWRRED_TPTlimit);
strcat((char *)(data_response_buf),puf);
//���¹ػ������¶�
sprintf((char *)(puf), "%d;", g_DI.Cfg.PWROFF_TPTlimit);
strcat((char *)(data_response_buf),puf);
//�����ʵ���
sprintf((char *)(puf), "%.3f;", g_DI.Cfg.PWRRED_CURlimit);
strcat((char *)(data_response_buf),puf);
//������������
sprintf((char *)(puf), "%.3f;", g_DI.Cfg.PWROFF_CURlimit);
strcat((char *)(data_response_buf),puf);
//��������������ʱ
sprintf((char *)(puf), "%d;", g_DI.Cfg.CurRstDelay);
strcat((char *)(data_response_buf),puf);
//����������������
sprintf((char *)(puf), "%d;", g_DI.Cfg.CurRstNum);
strcat((char *)(data_response_buf),puf);
//�����
sprintf((char *)(puf), "%dW;", (int)g_DI.Cfg.PWR_SetRateOut);
strcat((char *)(data_response_buf),puf);
//���䱣������
sprintf((char *)(puf), "%d;", g_DI.Cfg.PWR_RVlimit);
strcat((char *)(data_response_buf),puf);
//������С����			
sprintf((char *)(puf), "%.2f;", g_DI.Cfg.PWR_INlimit);
strcat((char *)(data_response_buf),puf);    


//���Ƶ��
sprintf((char *)(puf), "%d.%1d;", g_DI.Excitdata.FMFRQ/10, g_DI.Excitdata.FMFRQ%10);
strcat((char *)(data_response_buf),puf); 

//������Ԥ����
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

//�������������
sprintf((char *)(puf), "%d;", g_DI.Excitdata.FMGain_RF);
strcat((char *)(data_response_buf),puf); 

//����ģʽ 0:�ر� 1:������ 2:L������ 4:R������ 6:L+R����
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

//�ֵ����ƶ�0~7
sprintf((char *)(puf), "%d;", g_DI.Excitdata.FMSIGNAL_VAL);
strcat((char *)(data_response_buf),puf); 

//���ƶ�0~255
sprintf((char *)(puf), "%d;", g_DI.Excitdata.FMMODULATIONVAL);
strcat((char *)(data_response_buf),puf); 

//RDS���� 0~15 0:���ز�
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
	//�豸��ַ
	GetString(pcValue[0],"deviceAddr=",&Buf[0]);
	g_DI.Cfg.DeviceID = atoi(Buf);
	if(g_DI.Cfg.DeviceID >= 255)
	{
		g_DI.Cfg.DeviceID = 255;
	}
//	printf("g_DI.Cfg.DeviceID: %d \n",g_DI.Cfg.DeviceID);
	
	
//IP��ַ
str2shuIP(pcValue[1], g_DI.Cfg.IP_Addr);
//printf( "g_DI.Cfg.IP_Addr: %d.%d.%d.%d:%d \n", g_DI.Cfg.IP_Addr[0],g_DI.Cfg.IP_Addr[1],g_DI.Cfg.IP_Addr[2],g_DI.Cfg.IP_Addr[3],clocal_port);

//��������
str2shuIP(pcValue[2], g_DI.Cfg.IP_Mask);
//printf( "g_DI.Cfg.IP_Mask: %d.%d.%d.%d \n", g_DI.Cfg.IP_Mask[0],g_DI.Cfg.IP_Mask[1],g_DI.Cfg.IP_Mask[2],g_DI.Cfg.IP_Mask[3]);


//Ĭ������
str2shu(pcValue[3], g_DI.Cfg.IP_GateWay,4);
//printf( "g_DI.Cfg.IP_GateWay: %d.%d.%d.%d\n", g_DI.Cfg.IP_GateWay[0],g_DI.Cfg.IP_GateWay[1],g_DI.Cfg.IP_GateWay[2],g_DI.Cfg.IP_GateWay[3]);

//���ع���1
g_DI.Cfg.LOCAL1NetPort1 = str2shuIP(pcValue[4], g_DI.Cfg.IP_LOCAL1);
//printf( "g_DI.Cfg.IP_LOCAL1: %d.%d.%d.%d:%d\n", g_DI.Cfg.IP_LOCAL1[0],g_DI.Cfg.IP_LOCAL1[1],g_DI.Cfg.IP_LOCAL1[2],g_DI.Cfg.IP_LOCAL1[3],g_DI.Cfg.LOCAL1NetPort1);

//���ع���2
g_DI.Cfg.LOCAL1NetPort2 = str2shuIP(pcValue[5], g_DI.Cfg.IP_LOCAL2);
//printf( "g_DI.Cfg.IP_LOCAL2: %d.%d.%d.%d:%d\n", g_DI.Cfg.IP_LOCAL2[0],g_DI.Cfg.IP_LOCAL2[1],g_DI.Cfg.IP_LOCAL2[2],g_DI.Cfg.IP_LOCAL2[3],g_DI.Cfg.LOCAL1NetPort2);
//Զ������IP1
g_DI.Cfg.RemoteHostNetPort1 = str2shuIP(pcValue[6], g_DI.Cfg.IP_RemoteHost1);
//printf( "g_DI.Cfg.IP_RemoteHost1: %d.%d.%d.%d:%d\n", g_DI.Cfg.IP_RemoteHost1[0],g_DI.Cfg.IP_RemoteHost1[1],g_DI.Cfg.IP_RemoteHost1[2],g_DI.Cfg.IP_RemoteHost1[3],g_DI.Cfg.RemoteHostNetPort1);

//�������1
g_DI.Cfg.RemoteHostDelay1_1 = atoi(pcValue[7]); 
//printf( "RemoteHostDelay1:%d\n", g_DI.Cfg.RemoteHostDelay1_1);

//�������2
g_DI.Cfg.RemoteHostDelay1_2 = atoi(pcValue[8]); 
//printf( "RemoteHostDelay2: %d\n", g_DI.Cfg.RemoteHostDelay1_2);

//Զ������IP2
g_DI.Cfg.RemoteHostNetPort2 = str2shuIP(pcValue[9], g_DI.Cfg.IP_RemoteHost2);
//printf( "g_DI.Cfg.IP_RemoteHost2: %d.%d.%d.%d:%d\n", g_DI.Cfg.IP_RemoteHost2[0],g_DI.Cfg.IP_RemoteHost2[1],g_DI.Cfg.IP_RemoteHost2[2],g_DI.Cfg.IP_RemoteHost2[3],g_DI.Cfg.RemoteHostNetPort2);

//�������1
g_DI.Cfg.RemoteHostDelay2_1 = atoi(pcValue[10]); 
//printf( "RemoteHostDelay1:%d\n", g_DI.Cfg.RemoteHostDelay2_1);

//�������2
g_DI.Cfg.RemoteHostDelay2_2 = atoi(pcValue[11]); 
//printf( "RemoteHostDelay2: %d\n", g_DI.Cfg.RemoteHostDelay2_1);

//Զ������IP3
g_DI.Cfg.RemoteHostNetPort3 = str2shuIP(pcValue[12], g_DI.Cfg.IP_RemoteHost3);
//printf( "g_DI.Cfg.IP_RemoteHost3: %d.%d.%d.%d:%d\n", g_DI.Cfg.IP_RemoteHost3[0],g_DI.Cfg.IP_RemoteHost3[1],g_DI.Cfg.IP_RemoteHost3[2],g_DI.Cfg.IP_RemoteHost3[3],g_DI.Cfg.RemoteHostNetPort3);

//�������1
g_DI.Cfg.RemoteHostDelay3_1 = atoi(pcValue[13]); 
//printf( "RemoteHostDelay1:%d\n", g_DI.Cfg.RemoteHostDelay3_1);

//�������2
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
	u8	FmTzMode = 5;	//����ģʽ 0:�ر� 1:������ 2:L������ 4:R������ 6:L+R����
	u8	FmTzVAL = 0;//���ƶ�0~255
	u8	FmRdsVal;	//RDS���� 0~15 0:���ز�
	u8	FmtempVAL = 0;//���ƶ�0~7
	
	u8  PreEmpOut = 5;
	
//	for(i = 0;i < iNumParams;i++)
//	{
//		printf("pcParam[%d]: %s  ",i,pcParam[i]);
//		printf("pcValue[%d]: %s  \n",i,pcValue[i]);
//	}
//�ط����¶�
GetString(pcValue[0],"offFanTemperature=",&buf[0]);
g_DI.Cfg.CloseFan_TPTlimit = atoi(buf);
//printf( "g_DI.Cfg.CloseFan_TPTlimit : %d \n", g_DI.Cfg.CloseFan_TPTlimit);

//������1�¶�
g_DI.Cfg.OpenFan1_TPTlimit = atoi(pcValue[1]);
//printf( "g_DI.Cfg.OpenFan1_TPTlimit: %d\n", g_DI.Cfg.OpenFan1_TPTlimit);

//������2�¶�
g_DI.Cfg.OpenFan2_TPTlimit = atoi(pcValue[2]);
//printf( "g_DI.Cfg.OpenFan2_TPTlimit: %d\n", g_DI.Cfg.OpenFan2_TPTlimit);

//�������¶�
g_DI.Cfg.PWRRED_TPTlimit = atoi(pcValue[3]);
//printf( "g_DI.Cfg.PWRRED_TPTlimit: %d\n", g_DI.Cfg.PWRRED_TPTlimit);

//���¹ػ������¶�
g_DI.Cfg.PWROFF_TPTlimit = atoi(pcValue[4]);
//printf( "g_DI.Cfg.PWROFF_TPTlimit: %d\n", g_DI.Cfg.PWROFF_TPTlimit);

//�����ʵ���
g_DI.Cfg.PWRRED_CURlimit = atof(pcValue[5]);
//printf( "g_DI.Cfg.PWRRED_CURlimit: %.3f\n", g_DI.Cfg.PWRRED_CURlimit);

//������������
g_DI.Cfg.PWROFF_CURlimit = atof(pcValue[6]);
//printf( "g_DI.Cfg.PWROFF_CURlimit: %.3f\n", g_DI.Cfg.PWROFF_CURlimit);

//��������������ʱ
g_DI.Cfg.CurRstDelay = atoi(pcValue[7]);
//printf( "g_DI.Cfg.CurRstDelay: %d\n", g_DI.Cfg.CurRstDelay);

//����������������
g_DI.Cfg.CurRstNum = atoi(pcValue[8]);
//printf( "g_DI.Cfg.CurRstNum: %d\n", g_DI.Cfg.CurRstNum);

//�����
memset(buf, 0, 5);
//printf( "strlen((u8 *)pcValue[9]): %d\n", strlen((u8 *)pcValue[9]));
for(i = 0;i < (strlen((u8 *)pcValue[9]) - 1);i++)
{
	buf[i] = pcValue[9][i];
}	
g_DI.Cfg.PWR_SetRateOut = atoi(buf);
//printf( "g_DI.Cfg.PWR_RateOut: %dW\n", (int)g_DI.Cfg.PWR_RateOut);

//���䱣������
g_DI.Cfg.PWR_RVlimit = atoi(pcValue[10]);
//printf( "g_DI.Cfg.PWR_RVlimit: %d\n", g_DI.Cfg.PWR_RVlimit);

//������С����
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
	u8	FmTzMode = 5;	//����ģʽ 0:�ر� 1:������ 2:L������ 4:R������ 6:L+R����
	u8	FmTzVAL = 0;//���ƶ�0~255
	u8	FmRdsVal;	//RDS���� 0~15 0:���ز�
	u8	FmtempVAL = 0;//���ƶ�0~7
	
	u8  PreEmpOut = 5;
	
//	for(i = 0;i < iNumParams;i++)
//	{
//		printf("pcParam[%d]: %s  ",i,pcParam[i]);
//		printf("pcValue[%d]: %s  \n",i,pcValue[i]);
//	}
#if IWDG_EN
	IWDG_ReloadCounter(); //�ֶ�ι��  
#endif	
	//���Ƶ��
	GetString(pcValue[0],"fmfrequency=",&buf[0]);
	g_DI.Cfg.P_FmFrq = atof(buf);
	frqout = (u16)(atof(buf) *10);
//	printf( "frqout: %d\n", frqout);

	//������Ԥ����
	memset(buf, 0, 5);
	//printf( "strlen((u8 *)pcValue[13]): %d\n", strlen((u8 *)pcValue[13]));
	for(i = 0;i < (strlen((u8 *)pcValue[1]) - 2);i++)
	{
		buf[i] = pcValue[1][i];
	}	
	jlqEmphasis = (u8)atoi(buf);
	//printf( "jlqEmphasis: %d\n", jlqEmphasis);

	//�������������
	jlqGain = (u8)atoi(pcValue[2]);			
//	printf( "jlqGain: %d\n", jlqGain);
	if(jlqGain >=63)
	{
		jlqGain =63;
	}

	if(jlqEmphasis == 0)//00��0us  01:50us  02:75us
	{
		PreEmpOut = 0;	
	}
	else if(jlqEmphasis == 50)//00��0us  01:50us  02:75us
	{
		PreEmpOut = 1;	
	}
	else if(jlqEmphasis == 75)//00��0us  01:50us  02:75us
	{
		PreEmpOut = 2;	
	}
//	printf( "00-0us  01-50us  02-75us PreEmpOut: %d\n", PreEmpOut);

	//����ģʽ 0:�ر� 1:������ 2:L������ 4:R������ 6:L+R����
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
	/****************�������������*********************/
	if(g_TMTLink[0] == 1) 
	{
		fEitRTU = TRUE;
		OSSemPend(JLQRS232SEM, 0, &err);
		//���Ƶ��
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
	
	for(i =0 ;i<20;i++ );//��ʱһ�£����������
	clear_response_bufer(data_response_buf);		//���������������			
		
	
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

	if(Opetype == 1)	//У������
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
	else if(Opetype == 2)	//У������
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
	else if(Opetype == 3)	//У������
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
	else if(Opetype == 4)	//�ֶ����ڵ�λ�� -
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
	else if(Opetype == 5)	//�ֶ����ڵ�λ�� +
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
	if(iNumParams == 1)		//�������ػ����Զ���������1���ط���1��������2���ط���2
	{
		GetString(pcValue[0],"actionid=",&buf[0]);
		OpeCtrType = atoi(buf);	
//		printf("OpeCtrType: %d  \n",OpeCtrType);
		
		switch(OpeCtrType)
		{
			case 1:	 //����
				
				AP_Startup();
			    g_DI.Pa[0].Data.P_DownOffset = 0;
			    g_DI.Pa[0].Data.I_OverLoadRestartCount = 0;
				if(g_DI.Cfg.P_AutoContrl == 0)
				{
					ManualChanging = 0;
				}
			
				break;
			case 2:	//�ػ�
				
				AP_Shutdown();
				ATT_Senddata(0);
				g_DI.Pa[0].Data.P_DownOffset = 0;
				break;

			case 8:	 //������1 43 35 c3 fc c1 ee ba cf 01 b6 f4  �����  
				
				 FAN_ManCtrl_Delay = 60;
				 SW_FAN1_ON;	
				break;
			case 9:	//�ط���1  43 35 c3 fc c1 ee b7 d6 01 2c a7  �����  
				FAN_ManCtrl_Delay = 60;
				SW_FAN1_OFF;	
				break;
			case 10: //������2
				FAN_ManCtrl_Delay = 60;
				SW_FAN2_ON;
				break;
			case 11: //�ط���2
				FAN_ManCtrl_Delay = 60;
				SW_FAN2_OFF;
				break;
			case 15: //�ָ���������
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
	else if(iNumParams == 2)   //��ʼ�������ʲ���+ -
	{
		OpeCtrType = atoi(pcValue[1]);
//		printf("OpeCtrType: %d  \n",OpeCtrType);

		GetString(pcValue[0],"value=",&buf[0]);
		Pvalue = atoi(buf);	
//		printf("Pvalue: %d  \n",Pvalue);		
		switch(OpeCtrType)
		{
			case 3:	 //��ʼ��
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
		
			case 4:	 //�Զ�	 43 35 c3 fc c1 ee ba cf 04 76 f7
				g_DI.Pa[0].Data.P_DownOffset = 0;
				AP_Startup();	
				g_DI.Cfg.P_AutoContrl = 1;
				g_DI.Pa[0].Data.RES_Front = 0;
				Sys_Flag.Bit.fSaveStart = TRUE;
				Sys_Flag.Bit.fSave = TRUE;
				g_DI.Pa[0].Data.I_OverLoadRestartCount = 0;	
				break;
			
			case 5:	//���ʲ���-
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

			case 6:	//���ʲ���+
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
//SSI��Handler���
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






//SSI�����ʼ��
void httpd_ssi_init(void)
{  
	//����SSI���
	http_set_ssi_handler(SSIHandler,ppcTAGs,NUM_CONFIG_SSI_TAGS);
}

//CGI�����ʼ��
void httpd_cgi_init(void)
{ 
	//����CGI���
	http_set_cgi_handlers(ppcURLs, NUM_CONFIG_CGI_URIS);
}







