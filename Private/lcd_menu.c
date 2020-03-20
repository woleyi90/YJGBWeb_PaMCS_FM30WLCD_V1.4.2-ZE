#include "includes.h"
#include "global.h"


#pragma  diag_suppress 870,167

extern u8 ManualChanging;
extern u8  FAN_ManCtrl_Delay;

u8 func_index = 0;
u8 key_flag = 0;
u8 Clr_flag = 0;
u8 FunPatONF = 0;	// ȷ����ȡ�����ܲ��� 0:ȡ��  1��ȷ��
u8 OLED_TxLen = 0;
u8 OLED_SendBuf[50];

static int SelectedNum = 0;

u8 LCD_Power_SET = 0;
float  LCD_Step_dbm = 0; //db
float  LCD_Step_initdbm = 0; //db

u16 LCD_FMFRQ = 0;
u8 LCD_Pre_Emphasis = 0;
u8 LCD_FMModulation = 0;
u8 LCD_JLQPWR_Gain = 0;

void (*current_operation_index)(); //ִ�е�ǰ��ʾ����
void LcdMachinePAState(void);
void LcdMachineJLQState(void);
void SetSystemParamOneA(void);
void SetSystemParamOneB(void);
void SetSystemParamTwoA(void);
void SetSystemParamTwoB(void);
void SetSystemParamThreeA(void);
void SetSystemParamThreeB(void);


//��������ش�����
void SetRFSWI(int s);	
void SetRFSWIAdjust(int s);
void SetRFSWIhandle(int s);

//�����������
void SetRFPWR(int s);
void SetRFPWRAdjust(int s);
void SetRFPWRhandle(int s);
 
//������ʲ���
void SetRFStep(int s);
void SetRFStepAdjust(int s);
void SetRFStephandle(int s);
void SetRFStephandleOK(int s);

//���Ƶ������
void SetJLQFRQ(int s);	   
void SetJLQFRQAdjust(int s);
void SetJLQFRQhandle(int s);

//Ԥ����
void SetJLQPEP(int s);	   
void SetJLQPEPAdjust(int s);
void SetJLQPEPhandle(int s);

//����
void SetJLQGain(int s);	   
void SetJLQGainAdjust(int s);
void SetJLQGainhandle(int s);


void SystemPaALM(void);
void SystemCom(void);
void SystemIpDisplay(void);

void LcdMachineJLQStateB(void);
void SetSystemParamForA(void);
void SetSystemParamForB(void);
void SetSystemParamFive(void);
//����ģʽ
void SetJLQMODULATIONMODE(int s);
void SetJLQMODULATIONMODEAdjust(int s);
void SetJLQMODULATIONMODEhandle(int s);

//���ƶ�
void SetJLQMODULATIONVAL(int s);	   
void SetJLQMODULATIONVALAdjust(int s);
void SetJLQMODULATIONVALhandle(int s);

//RDS����
void SetJLQRdsVal(int s);	   
void SetJLQRdsValAdjust(int s);
void SetJLQRdsValhandle(int s);

void SetSystemParamFiveB(void);
void SetJLQMduVal(int s);	   
void SetJLQMduValAdjust(int s);
void SetJLQMduValhandle(int s);

Key_MenuStruct KeyMenuTab[50]=
{
/**	״̬ �ϼ�  �¼�  ȷ�ϼ�  ȡ��/�˳���  ��ǰ״ִ̬�еĺ��� **/
	//��һ��:��ʾϵͳ״̬
	{0,	  0,    1,     2,       0,		   (*LcdMachinePAState)},//���������״̬
 	{1,	  0,    28,    2,       0,         (*LcdMachineJLQState)}, //��Ƶ����״̬
	//�ڶ��������ò˵���
	{2,   43,   3,     9,       0,         (*SetSystemParamOneA)},//���������
	{3,   2,    4,     12,      0,         (*SetSystemParamOneB)},//�����������
	{4,   3,    5,     15,      0,		   (*SetSystemParamTwoA)},//������ʲ���
	{5,   4,    6,     19,      0,         (*SetSystemParamTwoB)},//���Ƶ������
	{6,   5,    7,     22,      0,         (*SetSystemParamThreeA)},//Ԥ����
	{7,   6,    8,     25,      0,         (*SetSystemParamThreeB)},//����
	{8,   7,    35,    32,      0,         (*SetSystemParamForA)},//����ģʽ
	
	//�����������������				   
	{9,   9,    9,     10,      2,         (*SetRFSWI)},
	{10,  10,   10,    11,      9,         (*SetRFSWIAdjust)},
	{11,  11,   11,    10,      2,         (*SetRFSWIhandle)},	 //��������ز���

	//�������������������				   
	{12,  12,   12,    13,      3,         (*SetRFPWR)},
	{13,  13,   13,    14,      12,        (*SetRFPWRAdjust)},
	{14,  14,   14,    13,      3,         (*SetRFPWRhandle)},
	
	//��������������ʲ���				   
	{15,  15,   15,    16,      4,         (*SetRFStep)},
	{16,  16,   16,    17,      15,        (*SetRFStepAdjust)},
	{17,  17,   17,    16,      4,         (*SetRFStephandle)},
	{18,  18,   18,    16,      4,         (*SetRFStephandleOK)},

	//�����������Ƶ������				   
	{19,  19,   19,    20,      5,         (*SetJLQFRQ)},
	{20,  20,   20,    21,      19,        (*SetJLQFRQAdjust)},
	{21,  21,   21,    20,      5,         (*SetJLQFRQhandle)},

	//��������Ԥ����				   
	{22,  22,   22,    23,      6,         (*SetJLQPEP)},
	{23,  23,   23,    24,      22,        (*SetJLQPEPAdjust)},
	{24,  24,   24,    23,      6,         (*SetJLQPEPhandle)},

	//������������				   
	{25,  25,   25,    26,      7,         (*SetJLQGain)},
	{26,  26,   26,    27,      25,        (*SetJLQGainAdjust)},
	{27,  27,   27,    26,      7,         (*SetJLQGainhandle)},
  	
	//ϵͳ�澯����
	{28,  1,    29,    2,       0,         (*LcdMachineJLQStateB)},
	{29,  28,   30,    2,       0,         (*SystemPaALM)},
	
	//ϵͳͨѶ����
	{30,  29,   31,    2,       0,         (*SystemCom)},
	//ϵͳIP��ַ
	{31,  30,   0,     2,       0,         (*SystemIpDisplay)},
	
	//������������ģʽ				   
	{32,  32,   32,    33,      8,         (*SetJLQMODULATIONMODE)},
	{33,  33,   33,    34,      32,        (*SetJLQMODULATIONMODEAdjust)},
	{34,  34,   34,    33,      8,         (*SetJLQMODULATIONMODEhandle)},
	
	//���ƶ�
	{35,   8,    39,   36,      0,         (*SetSystemParamForB)},//���ƶ�
	//�����������ƶ�				   
	{36,  36,   36,    37,      35,         (*SetJLQMODULATIONVAL)},
	{37,  37,   37,    38,      36,        (*SetJLQMODULATIONVALAdjust)},
	{38,  38,   38,    37,      35,         (*SetJLQMODULATIONVALhandle)},
	
	//RDS����
	{39,  35,   43,     40,      0,         (*SetSystemParamFive)},//RDS����
	//��������RDS����				   
	{40,  40,   40,    41,      39,         (*SetJLQRdsVal)},
	{41,  41,   41,    42,      40,        (*SetJLQRdsValAdjust)},
	{42,  42,   42,    41,      39,         (*SetJLQRdsValhandle)},
	
	//���ƶȴֵ�
	{43,  39,   2,     44,      0,         (*SetSystemParamFiveB)},//���ƶȴֵ�
	//�����������ƶȴֵ�				   
	{44,  44,   44,    45,      43,        (*SetJLQMduVal)},
	{45,  45,   45,    46,      44,        (*SetJLQMduValAdjust)},
	{46,  46,   46,    45,      43,        (*SetJLQMduValhandle)},
};
								 
void KEY_MENU_DEAL(void)				   
{
	u8 reflash = 0;							
	static u32 OLEDReflashCnt = 0;
	static u32 OLEDReflashAlmCnt = 0;
	static u32 LedDisplayCnt = 0;
	static u32 LcdFlag = 0;
	int data = 0;	
	
	if(Encoder_CW != 0)
	{
		//printf(".......zx....... \n");
		Encoder_CW = 0;
		key_flag = 1;
	}
	else if(Encoder_CCW != 0)	
	{
		//printf("-------- fx --------- \n");
		Encoder_CCW = 0;
		key_flag = 2;
	}
	if(Encoder_KEY_OK == 0)
	{
		OSTimeDlyHMSM(0, 0, 0, 20);
		if(Encoder_KEY_OK == 0)
		{
			while(!Encoder_KEY_OK);
			//printf("********** OK ************* \n");
			key_flag = 3;	
		}
	}
			 
	switch(key_flag)
	{ 
		case 1:
			reflash = 1;
			data = -1;
			func_index = KeyMenuTab[func_index].up;  break;  //���Ϸ�
		case 2:
			reflash = 1;
			data = 1;
			func_index = KeyMenuTab[func_index].down; break;   //���·�
		case 3:
			reflash = 1;
			FunPatONF = 1;
			func_index = KeyMenuTab[func_index].enter; break;   //ȷ��
		case 4:
			reflash = 1;
			FunPatONF = 0;
			func_index = KeyMenuTab[func_index].quit; break; 	 //�˳�
		default:
			reflash = 0;
			break;
	}
	if(reflash == 1)
	{
		Led_Display |= 0x30;
		SM1668_DisplayLED(0xC0,Led_Display);
		if(LcdFlag == 0)
		{
			LCD_Display_SW(1);
			LcdFlag = 1;			
		}
		LedDisplayCnt = 0;
	 	//printf("DoMSEvent key_flag: %d \n",key_flag);	
		//printf("KeyMenuTab[%d].current: %d \n",func_index,KeyMenuTab[func_index].current);	
		current_operation_index=KeyMenuTab[func_index].current_operation;
		(*current_operation_index)(data);//ִ�е�ǰ��������
		//KeyMenuTab[func_index].current_operation();
		key_flag = 0;
		//printf("DoMSEvent key_flag: %d \n",key_flag);
	}
	
	LedDisplayCnt++;
	if(LedDisplayCnt >= 100)  //5s
	{
		LedDisplayCnt = 0;
		Led_Display &= 0xCF;
		SM1668_DisplayLED(0xC0,Led_Display);
		LCD_Display_SW(0);
		LcdFlag = 0;
	}	
	
	if(func_index ==0 || func_index == 1 )
	{
		OLEDReflashCnt ++;
		if(OLEDReflashCnt >= 30)  //1.5sˢ��һ����Ļ
		{
			OLEDReflashCnt = 0; 
			Clr_flag = 1;
			current_operation_index = KeyMenuTab[func_index].current_operation;
			(*current_operation_index)(data);//ִ�е�ǰ��������
		}			
	} 
	else
	{
		OLEDReflashCnt = 0;	
	
	}
	if(func_index == 28 || func_index == 29 || func_index == 30 )
	{
		OLEDReflashAlmCnt ++;
		if(OLEDReflashAlmCnt >= 50)  //2.5sˢ��һ����Ļ
		{
			OLEDReflashAlmCnt = 0; 
			current_operation_index = KeyMenuTab[func_index].current_operation;
			(*current_operation_index)(data);//ִ�е�ǰ��������
		}			
	}   
	else
	{
		OLEDReflashAlmCnt = 0;	
	
	}
}				    
	  
void LcdMachinePAState(void)
{
	u8 display[50];

	if(Clr_flag == 0)
	{
		LCD_Write_Command(0x01);//����	
	}
	Clr_flag = 0;

	sprintf(display, " FWD   REFPWR  TEMP");
	PrintGB(0,0,display);


	sprintf(display, "%3.1fW  ", g_DI.Pa[0].Data.Power_FR);
	PrintGB(0,1,display);

	sprintf(display, " %2.2fW  ", g_DI.Pa[0].Data.Power_RV);	
	PrintGB(3,1,display);
	
	sprintf(display, "%2.1f�� ", g_DI.Pa[0].Data.PA_TPT);				
	PrintGB(7,1,display);
	

} 
void LcdMachineJLQState(void)
{
	u8 display[50];
	u8 emphasis;

	if(Clr_flag == 0)
	{
		LCD_Write_Command(0x01);//����	
	}
	Clr_flag = 0;
	
	if(g_DI.Excitdata.FMPre_Emphasis == 0)
	{
		emphasis = 0;
	}
	else if(g_DI.Excitdata.FMPre_Emphasis == 1)
	{
		emphasis = 50;
	}
	else if(g_DI.Excitdata.FMPre_Emphasis == 2)
	{
		emphasis = 75;
	}
	else
	{
		emphasis = 99;
	}

	sprintf(display, " FREQ  PRE-EMP  GAIN");
	PrintGB(0,0,display);


	
	sprintf(display, "%3d.%1dMHz", g_DI.Excitdata.FMFRQ/10, g_DI.Excitdata.FMFRQ%10);
	PrintGB(0,1,display);


	sprintf(display, "%2dus", emphasis);		
	PrintGB(5,1,display);
	
	sprintf(display, "%3d", g_DI.Excitdata.FMGain_RF);				
	PrintGB(8,1,display);

} 
void LcdMachineJLQStateB(void)
{
	u8 display[50];
	u8 MduMode;
	
	if(Clr_flag == 0)
	{
		LCD_Write_Command(0x01);//����	
	}
	Clr_flag = 0;
	

	sprintf(display, "MODE  MODUVAL RDSVAL");
	PrintGB(0,0,display);

	if(g_DI.Excitdata.FMMODULATIONMODE == 0)
	{
		sprintf(display, " CLOSE  ");
	}
	else if(g_DI.Excitdata.FMMODULATIONMODE == 1)
	{
		sprintf(display, " STEREO ");
	}
	else if(g_DI.Excitdata.FMMODULATIONMODE == 2)
	{
		sprintf(display, " L Sound");
	}
	else if(g_DI.Excitdata.FMMODULATIONMODE == 4)
	{
		sprintf(display, " R Sound");
	}
	else if(g_DI.Excitdata.FMMODULATIONMODE == 6)
	{
		sprintf(display, " L+R    ");	
	}
	else
	{
		sprintf(display, " ERR    ");	
	}
	PrintGB(0,1,display);


	sprintf(display, "%4d.%d%%  ", g_DI.Excitdata.ModulationDCT/10,g_DI.Excitdata.ModulationDCT%10);		
	PrintGB(4,1,display);
	
	sprintf(display, "%3d  ", g_DI.Excitdata.FMRDSVAL);				
	PrintGB(8,1,display);

}
void SystemPaALM(void)
{
	u8 display[50];
	
	LCD_Write_Command(0x01);//����
	sprintf(display, "CURRENT TEMP  REFPWR");
	PrintGB(0,0,display);
 
	
	if(g_DI.Pa[0].State.OC_Alm == 1)		// �������澯
	{
		sprintf(display, "  OVER ");
		PrintGB(0,1,display);	
	}
	else if(g_DI.Pa[0].State.OCRED_Alm == 1)
	{
		sprintf(display, "  ALM  ");
		PrintGB(0,1,display);	
	}
	else
	{
		sprintf(display, "  OK   ");
		PrintGB(0,1,display);	
	}
	
	if(g_DI.Pa[0].State.TPT_Alm == 1)		// ���¸澯
	{
		sprintf(display, "  OVER ");
		PrintGB(3,1,display);	
	}
	else if(g_DI.Pa[0].State.TPRED_Alm == 1)
	{
		sprintf(display, "  ALM  ");
		PrintGB(3,1,display);
	}
	else
	{
		sprintf(display, "  OK   ");
		PrintGB(3,1,display);	
	}
	
	if(g_DI.Pa[0].State.RV_Alm == 1) 	// ������澯
	{
		sprintf(display, " OVER  ");
		PrintGB(6,1,display);	
	}
	else
	{
		sprintf(display, "  OK   ");
		PrintGB(6,1,display);	
	} 
}
void SystemCom(void)
{
	u8 display[50];

	LCD_Write_Command(0x01);//����
	sprintf(display, "       EXCITER-COMM");
	PrintGB(0,0,display);
	
	if(g_TMTLink[1]==0)//������δ����
	{			
		sprintf(display, "NOT CONNECTION");
		PrintGB(3,1,display);		
	}
	else
	{
		sprintf(display, "  CONNECTION  ");
		PrintGB(3,1,display);
	}
	  
} 
void SystemIpDisplay(void)
{
	u8 display[50];
	LCD_Write_Command(0x01);//����
	sprintf(display, "IP:%3d.%3d.%3d.%3d",g_DI.Cfg.IP_Addr[0],g_DI.Cfg.IP_Addr[1],g_DI.Cfg.IP_Addr[2],g_DI.Cfg.IP_Addr[3]);
	PrintGB(0,0,display);
	
	sprintf(display, "ID:%3d",g_DI.Cfg.DeviceID);
	PrintGB(0,1,display);
	
	sprintf(display, "Ver:V%d.%d.%d", SOFT_VERSION/100,SOFT_VERSION%100/10,SOFT_VERSION%10);
	PrintGB(4,1,display);

	  
}
void SetSystemParamOneA(void)
{
	
	u8 display[50];
	LCD_Write_Command(0x01);//����
	sprintf(display, "1.RF Switch");
	PrintGB(1,0,display);
	
	sprintf(display, "*");
	PrintGB(0,0,display);


	sprintf(display, "2.FWD Power Set");
	PrintGB(1,1,display);

//	sprintf(display, "*");
//	PrintGB(0,1,display);
	
//	sprintf(display, "[ ]");
//	PrintGB(8,1,display);

//	LCD_Write_Command(0x0e);
//	SetCursor(7,1);
}
void SetSystemParamOneB(void)
{
	
	u8 display[50];
	LCD_Write_Command(0x01);//����
	sprintf(display, "1.RF Switch");
	PrintGB(1,0,display);
	
	sprintf(display, "2.FWD Power Set");
	PrintGB(1,1,display);
	
	sprintf(display, "*");
	PrintGB(0,1,display);

}
void SetSystemParamTwoA(void)
{
	
	u8 display[50];
	LCD_Write_Command(0x01);//����
	sprintf(display, "3.DevGain Adjust");
	PrintGB(1,0,display);

	sprintf(display, "*");
	PrintGB(0,0,display);
	
	sprintf(display, "4.Freq Set");
	PrintGB(1,1,display);

}
void SetSystemParamTwoB(void)
{
	
	u8 display[50];
	LCD_Write_Command(0x01);//����
	sprintf(display, "3.DevGain Adjust");
	PrintGB(1,0,display);

	sprintf(display, "4.Freq Set");
	PrintGB(1,1,display);

	sprintf(display, "*");
	PrintGB(0,1,display);
}
void SetSystemParamThreeA(void)
{
	
	u8 display[50];
	LCD_Write_Command(0x01);//����
	sprintf(display, "5.Pre-emphasis Set");
	PrintGB(1,0,display);

	sprintf(display, "*");
	PrintGB(0,0,display);
	
	sprintf(display, "6.Exciter Gain Set");
	PrintGB(1,1,display);
}
void SetSystemParamThreeB(void)
{
	
	u8 display[50];
	LCD_Write_Command(0x01);//����
	sprintf(display, "5.Pre-emphasis Set");
	PrintGB(1,0,display);

	sprintf(display, "6.Exciter Gain Set");
	PrintGB(1,1,display);
	
	sprintf(display, "*");
	PrintGB(0,1,display);
}
void SetSystemParamForA(void)
{	
	u8 display[50];
	LCD_Write_Command(0x01);//����
	sprintf(display, "7.����ģʽ");
	PrintGB(1,0,display);
	
	sprintf(display, "*");
	PrintGB(0,0,display);
	
	sprintf(display, "8.���ƶ�ϸ��");
	PrintGB(1,1,display);
	
}
void SetSystemParamForB(void)
{	
	u8 display[50];
	LCD_Write_Command(0x01);//����
	sprintf(display, "7.����ģʽ");
	PrintGB(1,0,display);

	sprintf(display, "8.���ƶ�ϸ��");
	PrintGB(1,1,display);
	
	sprintf(display, "*");
	PrintGB(0,1,display);
}
void SetSystemParamFive(void)
{	
	u8 display[50];
	LCD_Write_Command(0x01);//����
	sprintf(display, "9.RDS ����");
	PrintGB(1,0,display);
	
	sprintf(display, "*");
	PrintGB(0,0,display);
	
	sprintf(display, "10. ���ƶȴֵ�");
	PrintGB(1,1,display);
}
void SetSystemParamFiveB(void)
{	
	u8 display[50];
	LCD_Write_Command(0x01);//����
	sprintf(display, "9.RDS ����");
	PrintGB(1,0,display);
	
	
	sprintf(display, "10. ���ƶȴֵ�");
	PrintGB(1,1,display);
	
	sprintf(display, "*");
	PrintGB(0,1,display);
}
void SetRFSWI(int s)
{
 	u8 display[50];
	LCD_Write_Command(0x01);//����
	sprintf(display, "RF Switch:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);
	if( g_DI.Pa[0].State.Enable == 0 ) //Ϊ��״̬
	{
		sprintf(display, "OFF");
		PrintGB(7,0,display);
		SelectedNum = 1;
	}
	else	//��״̬
	{
		sprintf(display, "ON ");
		PrintGB(7,0,display);	
		SelectedNum = 2;		
	}     	
}
void SetRFSWIAdjust(int s)
{
	u8 display[50];	
	LCD_Write_Command(0x01);//����
	if(s == 0)
	{
	 	sprintf(display, "*");
		PrintGB(0,0,display);
		sprintf(display, "RF Switch:");
		PrintGB(1,0,display);
		if( SelectedNum == 1 ) //Ϊ��״̬
		{
			
			sprintf(display, "[OFF]");
			PrintGB(7,0,display);
		}
		else if(SelectedNum == 2)	//��״̬
		{
			sprintf(display, "[ON] ");
			PrintGB(7,0,display);
		}      
		return;
	} 
	sprintf(display, "*");
	PrintGB(0,0,display);
	sprintf(display, "RF Switch:");
	PrintGB(1,0,display);  
	if( SelectedNum == 1 ) 
	{
		sprintf(display, "[ON] ");
		PrintGB(7,0,display);
		SelectedNum = 2;
	}
	else if(SelectedNum == 2)	
	{
		sprintf(display, "[OFF]");
		PrintGB(7,0,display);
	    SelectedNum = 1;
	} 

}
void SetRFSWIhandle(int s)
{
	
	u8 err;
	u8 display[50];

	if((s == 1) ||(s == -1))
		return;
	if(FunPatONF == 1)
	{
		LCD_Write_Command(0x01);//����
		sprintf(display, "RF Switch:");
		PrintGB(1,0,display);
		sprintf(display, "*");
		PrintGB(0,0,display);

		if( SelectedNum == 1 ) //Ϊ��״̬
		{
			sprintf(display, "OFF");
			PrintGB(7,0,display);
			SelectedNum = 1;
		}
		else if(SelectedNum == 2)		//��״̬
		{
			sprintf(display, "ON ");
			PrintGB(7,0,display);			
		} 
	}
	if( SelectedNum == 1 ) //Ϊ��״̬
	{
		
		if(FunPatONF == 1)
		{
//			printf("pa off ok......\n"); 
			AP_Shutdown();
			ATT_Senddata(0);
			g_DI.Pa[0].Data.P_DownOffset = 0;
		}
	}
	else if(SelectedNum == 2)	//��״̬
	{
		
		if(FunPatONF == 1)
		{
//			printf("pa on ok......\n");
			AP_Startup();
			g_DI.Pa[0].Data.P_DownOffset = 0;
			g_DI.Pa[0].Data.I_OverLoadRestartCount = 0;
			if(g_DI.Cfg.P_AutoContrl == 0)
			{
				ManualChanging = 0;
			}	  
		}
		
	}	
}
void SetRFPWR(int s)
{
	u8 display[50];

	LCD_Write_Command(0x01);//����
	sprintf(display, "FWD Power Set:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);
	LCD_Power_SET =  (u8)(g_DI.Cfg.PWR_RateOut);
	sprintf(display, "%3dW",(u8)(LCD_Power_SET));
	PrintGB(7,1,display);

}
void SetRFPWRAdjust(int s)
{
	u8 display[50];
	
	if(s == 0)
	{
	 	LCD_Write_Command(0x01);//����
	 	sprintf(display, "*");
		PrintGB(0,0,display);
	 	sprintf(display, "FWD Power Set:");
		PrintGB(1,0,display);
		sprintf(display, "[%3dW]",(int)(LCD_Power_SET));
		PrintGB(7,1,display);
		return;
	} 
	LCD_Power_SET +=s;
	if(LCD_Power_SET >= (g_DI.Cfg.PWR_SetRateOut*1.1))
	{
		LCD_Power_SET = g_DI.Cfg.PWR_SetRateOut*1.1;		
	} 
	if(LCD_Power_SET <= 0)
	{
		LCD_Power_SET = 0;		
	}
	sprintf(display, "[%3dW]",(int)(LCD_Power_SET));
	PrintGB(7,1,display); 
}
void SetRFPWRhandle(int s)
{
 	u8 display[50];
	u8 err;
	u8 Powerdata = 0;
	if((s == 1) ||(s == -1))
		return;
	LCD_Write_Command(0x01);//����
	sprintf(display, "FWD Power Set:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);

	if(FunPatONF == 1)//ȷ����ʼ���������
	{
	 	sprintf(display, "%3dW",(u8)(LCD_Power_SET));
		PrintGB(7,1,display);

//		printf("pa pwr init : %d......\n",Powerdata);
	
		ATT_Senddata(0);
		AP_Startup();
		g_DI.Cfg.PWR_RateOut = (u8)(LCD_Power_SET);
		g_DI.Cfg.P_AutoContrl = 1;
		g_DI.Pa[0].Data.RES_Front = 0;
		Sys_Flag.Bit.fSaveStart = TRUE;
		Sys_Flag.Bit.fSave = TRUE;	
	}
	else
	{
		LCD_Power_SET = (u8)(g_DI.Cfg.PWR_RateOut);
		sprintf(display, "%3dW",(u8)(LCD_Power_SET));
		PrintGB(7,1,display);
		
//		printf("pa pwr no......\n");
	}
}
void SetRFStep(int s)
{
	u8 display[50];

	LCD_Write_Command(0x01);//����
	LCD_Step_dbm = g_DI.Pa[0].Data.RES_Rear / 2.0; 
	LCD_Step_initdbm = LCD_Step_dbm;
	sprintf(display, "DevGain Adjust:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);
	sprintf(display, " %2.2f dB ",LCD_Step_dbm);
	PrintGB(5,1,display);


}
void SetRFStepAdjust(int s)
{
	u8 display[50];

	if(s == 0)
	{
		LCD_Write_Command(0x01);//����
		
		sprintf(display, "DevGain Adjust:");
		PrintGB(1,0,display);
		sprintf(display, "*");
		PrintGB(0,0,display);
		sprintf(display, "[%2.2f dB]",LCD_Step_dbm);
		PrintGB(5,1,display);
		
		return;
	}
	if(s>0)
	{
		LCD_Step_dbm = LCD_Step_dbm + 0.5;
	}
	else
	{
		LCD_Step_dbm = LCD_Step_dbm - 0.5;	
	}
	if(LCD_Step_dbm >= 31)
	{
		LCD_Step_dbm = 31;
	}
	if(LCD_Step_dbm <= 0)
	{
		LCD_Step_dbm = 0;
	}
	
	sprintf(display, "[%2.2f dB]",LCD_Step_dbm);
	PrintGB(5,1,display);


}
void SetRFStephandle(int s)
{
	u8 err;
	u8 display[50];
	u8 stepval = 0;
	if((s == 1) ||(s == -1))
		return;

	LCD_Write_Command(0x01);//����
	
	sprintf(display, "DevGain Adjust:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);
	sprintf(display, " %2.2f dB ",LCD_Step_dbm);
	PrintGB(5,1,display);
	
	if(FunPatONF == 1)//ȷ����ʼ���������
	{
		
		g_DI.Cfg.P_Res_Front_Target = (u8)(LCD_Step_dbm *2);
		ATT_Senddata( g_DI.Cfg.P_Res_Front_Target  );
		g_DI.Cfg.P_Res_Front_Target = g_DI.Pa[0].Data.RES_Rear;
		g_DI.Cfg.P_AutoContrl =0;
		Sys_Flag.Bit.fSaveStart = TRUE;
		Sys_Flag.Bit.fSave = TRUE;
	}

}
void SetRFStephandleOK(int s)
{
	

}
void SetJLQFRQ(int s)
{
	u8 display[50];
	u16 intdata=0, pointdata = 0; 
	
	LCD_Write_Command(0x01);//����
	sprintf(display, "Freq Set:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);

	LCD_FMFRQ = g_DI.Excitdata.FMFRQ;
	intdata = (u16)LCD_FMFRQ/10;
	pointdata = (u16)(LCD_FMFRQ%10);
	sprintf(display, " %3d.%1dMHz ",intdata, pointdata);
	PrintGB(5,1,display);	
}	   
void SetJLQFRQAdjust(int s)
{
	u8 display[50];
	u16 intdata=0, pointdata = 0; 
	if(s == 0)
	{
	 	LCD_Write_Command(0x01);//����
		sprintf(display, "Freq Set:");
		PrintGB(1,0,display);
		sprintf(display, "*");
		PrintGB(0,0,display);
	
		intdata = (u16)LCD_FMFRQ/10;
		pointdata = (u16)(LCD_FMFRQ%10);
		sprintf(display, "[%3d.%1dMHz]",intdata, pointdata);
		PrintGB(5,1,display);
		return;
	} 
	LCD_FMFRQ +=s;
	if(LCD_FMFRQ >= 1080)
	{
		LCD_FMFRQ = 1080;		
	} 
	if(LCD_FMFRQ <= 760) 
	{
		LCD_FMFRQ = 760;		
	}
	intdata = (u16)LCD_FMFRQ/10;
	pointdata = (u16)(LCD_FMFRQ%10);
	sprintf(display, "[%3d.%1dMHz]",intdata, pointdata);
	PrintGB(5,1,display); 
}
void SetJLQFRQhandle(int s)
{
	u8 display[50];
	u8 err;
	u16 intdata=0, pointdata = 0;

	if((s == 1) ||(s == -1))
		return;

	sprintf(display, "Freq Set:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);	

	if(FunPatONF == 1)//ȷ���޸����Ƶ��
	{
	 	intdata = (u16)LCD_FMFRQ/10;
		pointdata = (u16)(LCD_FMFRQ%10);
		sprintf(display, " %3d.%1dMHz ",intdata, pointdata);
		PrintGB(5,1,display); 

//		printf("jlq frq init......\n");
		if(g_TMTLink[0] == 1) 
		{
			fEitRTU = TRUE;
			OSSemPend(JLQRS232SEM, 0, &err);
			OLED_SendBuf[0]= g_DI.Cfg.Excit_IPHead; 
			OLED_SendBuf[1]= 0x35;
			OLED_SendBuf[2]= 0xC3;
			OLED_SendBuf[3]= 0xFC;
			OLED_SendBuf[4]= 0xC1;
			OLED_SendBuf[5]= 0xEE;
			OLED_SendBuf[6]= 0xC9;
			OLED_SendBuf[7]= 0xE8;
			OLED_SendBuf[8]= 0x00;
			OLED_SendBuf[9]= (u8)LCD_FMFRQ;
			OLED_SendBuf[10]= (u8)(LCD_FMFRQ>>8);	
			Excit_RS232_Send(OLED_SendBuf,11, 10); 
			OSSemPost(JLQRS232SEM);
			fEitRTU = FALSE;
			g_DI.Cfg.P_FmFrq = (float)(LCD_FMFRQ /10.0);
			Sys_Flag.Bit.fSaveStart = TRUE;
			Sys_Flag.Bit.fSave = TRUE;
			ATT_Senddata(0);
		}
	}
	else
	{
		intdata = (u16)g_DI.Excitdata.FMFRQ/10;
		pointdata = (u16)(g_DI.Excitdata.FMFRQ%10);
		sprintf(display, " %3d.%1dMHz ",intdata, pointdata);
		PrintGB(5,1,display);
		
		LCD_FMFRQ = g_DI.Excitdata.FMFRQ;
//		printf("jlq frq no......\n"); 
	}
}
//Ԥ����
void SetJLQPEP(int s)
{
	u8 display[50];
	
	LCD_Write_Command(0x01);//����
	sprintf(display, "Pre-emphasis:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);

	LCD_Pre_Emphasis = g_DI.Excitdata.FMPre_Emphasis; 
	if(LCD_Pre_Emphasis == 0)
	{
		SelectedNum = 1;		//0us 
		sprintf(display, " %2dus ",0);
	}
	else if(LCD_Pre_Emphasis == 1)
	{
		SelectedNum = 2;		//50us
		sprintf(display, " %2dus ",50);
	}
	else if(LCD_Pre_Emphasis == 2)
	{
		SelectedNum = 3;	 	//75us
		sprintf(display, " %2dus ",75);
	}

	PrintGB(7,1,display);
}	   
void SetJLQPEPAdjust(int s)
{
	u8 display[50];

	if(s == 0)
	{
	 	LCD_Write_Command(0x01);//����
		sprintf(display, "Pre-emphasis:");
		PrintGB(1,0,display);
		sprintf(display, "*");
		PrintGB(0,0,display);
	
		if(LCD_Pre_Emphasis == 0)
		{
			SelectedNum = 1;		//0us 
			sprintf(display, "[%2dus]",0);
		}
		else if(LCD_Pre_Emphasis == 1)
		{
			SelectedNum = 2;		//50us
			sprintf(display, "[%2dus]",50);
		}
		else if(LCD_Pre_Emphasis == 2)
		{
			SelectedNum = 3;	 	//75us
			sprintf(display, "[%2dus]",75);
		}
		PrintGB(7,1,display);
		return;
	} 
	SelectedNum +=s;
	if(SelectedNum >2)
	{
		SelectedNum = 1;
	}
	if(SelectedNum <= 0)
	{
		SelectedNum = 2;
	}
	if(SelectedNum == 1)	   //0us 
	{
		LCD_Pre_Emphasis = 0;
		sprintf(display, "[%2dus]",00);
	}
	else if(SelectedNum == 2) 	//50us
	{
		LCD_Pre_Emphasis = 1;
		sprintf(display, "[%2dus]",50);
	}
	else if(SelectedNum == 3)	//75us
	{
 		LCD_Pre_Emphasis = 2;
		sprintf(display, "[%2dus]",75);
	} 

	PrintGB(7,1,display);
}
void SetJLQPEPhandle(int s)
{
	u8 display[50];
	u8 err;

	if((s == 1) ||(s == -1))
		return;
	
	LCD_Write_Command(0x01);//����
	sprintf(display, "Pre-emphasis:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);	

	if(FunPatONF == 1)//ȷ���޸����Ƶ��
	{
	 	if(LCD_Pre_Emphasis == 0)	   //0us 
		{
		
			sprintf(display, " %2dus ",0);
		}
		else if(LCD_Pre_Emphasis == 1) 	//50us
		{
	
			sprintf(display, " %2dus ",50);
		}
		else if(LCD_Pre_Emphasis == 2)	//75us
		{
	 	
			sprintf(display, " %2dus ",75);
		}
		PrintGB(7,1,display); 

		//printf("jlq pep init : %d......\n",OLED_Pre_Emphasis);
		if(g_TMTLink[0] == 1) 
		{
			fEitRTU = TRUE;
			OSSemPend(JLQRS232SEM, 0, &err);
			OLED_SendBuf[0]= g_DI.Cfg.Excit_IPHead; 
			OLED_SendBuf[1]= 0x35;
			OLED_SendBuf[2]= 0xC3;
			OLED_SendBuf[3]= 0xFC;
			OLED_SendBuf[4]= 0xC1;
			OLED_SendBuf[5]= 0xEE;
			OLED_SendBuf[6]= 0xC9;
			OLED_SendBuf[7]= 0xE8;
			OLED_SendBuf[8]= 0x02;
			OLED_SendBuf[9]= (u8)(LCD_Pre_Emphasis);	
			Excit_RS232_Send(OLED_SendBuf,10, 20); 
			OSSemPost(JLQRS232SEM);
			fEitRTU = FALSE;
		}
	}
	else
	{
		LCD_Pre_Emphasis = g_DI.Excitdata.FMPre_Emphasis; 
		if(LCD_Pre_Emphasis == 0)
		{
			SelectedNum = 1;		//0us 
			sprintf(display, " %2dus ",0);
		}
		else if(LCD_Pre_Emphasis == 1)
		{
			SelectedNum = 2;		//50us
			sprintf(display, " %2dus ",50);
		}
		else if(LCD_Pre_Emphasis == 2)
		{
			SelectedNum = 3;	 	//75us
			sprintf(display, " %2dus ",75);
		}
	
		PrintGB(7,1,display);
//		printf("jlq pep no......\n"); 
	}
}
//����
void SetJLQGain(int s)
{
	u8 display[50];
	
	LCD_Write_Command(0x01);//����
	sprintf(display, "Exciter Gain:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);

	LCD_JLQPWR_Gain = g_DI.Excitdata.FMGain_RF;

	sprintf(display, " %3d ",LCD_JLQPWR_Gain);
	PrintGB(6,1,display);
}	   
void SetJLQGainAdjust(int s)
{
	u8 display[50];

	if(s == 0)
	{
	 	LCD_Write_Command(0x01);//����
		sprintf(display, "Exciter Gain:");
		PrintGB(1,0,display);
		sprintf(display, "*");
		PrintGB(0,0,display);
	
		sprintf(display, "[%3d]",LCD_JLQPWR_Gain);
		PrintGB(6,1,display);
		return;
	} 
	LCD_JLQPWR_Gain +=s;
	if(LCD_JLQPWR_Gain >= 127)
	{
		LCD_JLQPWR_Gain = 127;		
	} 
	if(LCD_JLQPWR_Gain <= 0)
	{
		LCD_JLQPWR_Gain = 0;		
	}
	sprintf(display, "[%3d]",LCD_JLQPWR_Gain);
	PrintGB(6,1,display);
}
void SetJLQGainhandle(int s)
{
	u8 display[50];
	u8 err;

	if((s == 1) ||(s == -1))
		return;
	
	LCD_Write_Command(0x01);//����
	sprintf(display, "Exciter Gain:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);	

	if(FunPatONF == 1)//ȷ���޸����Ƶ��
	{
	 	sprintf(display, " %3d ",LCD_JLQPWR_Gain);
		PrintGB(6,1,display); 

//		printf("jlq Gain init......\n");
		if(g_TMTLink[0] == 1) 
		{
			fEitRTU = TRUE;
			OSSemPend(JLQRS232SEM, 0, &err);
			OLED_SendBuf[0]= g_DI.Cfg.Excit_IPHead; 
			OLED_SendBuf[1]= 0x35;
			OLED_SendBuf[2]= 0xC3;
			OLED_SendBuf[3]= 0xFC;
			OLED_SendBuf[4]= 0xC1;
			OLED_SendBuf[5]= 0xEE;
			OLED_SendBuf[6]= 0xC9;
			OLED_SendBuf[7]= 0xE8;
			OLED_SendBuf[8]= 0x09;
			OLED_SendBuf[9]= (u8)LCD_JLQPWR_Gain;	
			Excit_RS232_Send(OLED_SendBuf,10, 20); 
			OSSemPost(JLQRS232SEM);
			fEitRTU = FALSE; 
		}
	}
	else
	{
		LCD_JLQPWR_Gain = g_DI.Excitdata.FMGain_RF;

		sprintf(display, " %3d ",LCD_JLQPWR_Gain);
		PrintGB(6,1,display);
//		printf("jlq gain no......\n"); 
	}
}
//Ԥ����
void SetJLQMODULATIONMODE(int s)
{
	u8 display[50];
	
	LCD_Write_Command(0x01);//����
	sprintf(display, "����ģʽ:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);

	LCD_Pre_Emphasis = g_DI.Excitdata.FMMODULATIONMODE; 
	if(LCD_Pre_Emphasis == 0)//0:�ر� 1:������ 2:L������ 4:R������ 6:L+R����
	{
		SelectedNum = 1;		
		sprintf(display, "  �ر�  ");
	}
	else if(LCD_Pre_Emphasis == 1)
	{
		SelectedNum = 2;		
		sprintf(display, "  ������  ");
	}
	else if(LCD_Pre_Emphasis == 2)
	{
		SelectedNum = 3;	 	
		sprintf(display, " L������ ");
	}
	else if(LCD_Pre_Emphasis == 4)
	{
		SelectedNum = 4;	 	
		sprintf(display, " R������ ");
	}
	else if(LCD_Pre_Emphasis == 6)
	{
		SelectedNum = 5;	 	
		sprintf(display, " L+R���� ");
	}

	PrintGB(5,1,display);
}	   
void SetJLQMODULATIONMODEAdjust(int s)
{
	u8 display[50];

	if(s == 0)
	{
	 	LCD_Write_Command(0x01);//����
		sprintf(display, "����ģʽ:");
		PrintGB(1,0,display);
		sprintf(display, "*");
		PrintGB(0,0,display);
	
		if(LCD_Pre_Emphasis == 0)//0:�ر� 1:������ 2:L������ 4:R������ 6:L+R����
		{
			SelectedNum = 1;		
			sprintf(display, " [�ر�] ");
		}
		else if(LCD_Pre_Emphasis == 1)
		{
			SelectedNum = 2;		
			sprintf(display, " [������] ");
		}
		else if(LCD_Pre_Emphasis == 2)
		{
			SelectedNum = 3;	 	
			sprintf(display, "[L������]");
		}
		else if(LCD_Pre_Emphasis == 4)
		{
			SelectedNum = 4;	 	
			sprintf(display, "[R������]");
		}
		else if(LCD_Pre_Emphasis == 6)
		{
			SelectedNum = 5;	 	
			sprintf(display, "[L+R����]");
		}
		PrintGB(5,1,display);
		return;
	} 
	SelectedNum +=s;
	if(SelectedNum >5)
	{
		SelectedNum = 1;
	}
	if(SelectedNum <= 0)
	{
		SelectedNum = 5;
	}
	if(SelectedNum == 1)	    
	{
		LCD_Pre_Emphasis = 0;
		sprintf(display, " [�ر�]   ");
	}
	else if(SelectedNum == 2) 	
	{
		LCD_Pre_Emphasis = 1;
		sprintf(display, " [������] ");
	}
	else if(SelectedNum == 3)	
	{
 		LCD_Pre_Emphasis = 2;
		sprintf(display, "[L������]");
	} 
	else if(SelectedNum == 4)	
	{
 		LCD_Pre_Emphasis = 4;
		sprintf(display, "[R������]");
	}
	else if(SelectedNum == 5)	
	{
 		LCD_Pre_Emphasis = 6;
		sprintf(display, "[L+R����]");
	}

	PrintGB(5,1,display);
}
void SetJLQMODULATIONMODEhandle(int s)
{
	u8 display[50];
	u8 err;

	if((s == 1) ||(s == -1))
		return;
	
	LCD_Write_Command(0x01);//����
	sprintf(display, "����ģʽ:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);	

	if(FunPatONF == 1)//ȷ���޸����Ƶ��
	{
	 	if(LCD_Pre_Emphasis == 0)//0:�ر� 1:������ 2:L������ 4:R������ 6:L+R����
		{
			SelectedNum = 1;		 
			sprintf(display, "  �ر�    ");
		}
		else if(LCD_Pre_Emphasis == 1)
		{
			SelectedNum = 2;		
			sprintf(display, "  ������  ");
		}
		else if(LCD_Pre_Emphasis == 2)
		{
			SelectedNum = 3;	 	
			sprintf(display, " L������ ");
		}
		else if(LCD_Pre_Emphasis == 4)
		{
			SelectedNum = 4;	 	
			sprintf(display, " R������ ");
		}
		else if(LCD_Pre_Emphasis == 6)
		{
			SelectedNum = 5;	 	
			sprintf(display, " L+R���� ");
		}
		PrintGB(5,1,display); 

		//printf("jlq pep init : %d......\n",OLED_Pre_Emphasis);

		fEitRTU = TRUE;
		OSSemPend(JLQRS232SEM, 0, &err);
		OLED_SendBuf[0]= g_DI.Cfg.Excit_IPHead; 
		OLED_SendBuf[1]= 0x35;
		OLED_SendBuf[2]= 0xC3;
		OLED_SendBuf[3]= 0xFC;
		OLED_SendBuf[4]= 0xC1;
		OLED_SendBuf[5]= 0xEE;
		OLED_SendBuf[6]= 0xC9;
		OLED_SendBuf[7]= 0xE8;
		OLED_SendBuf[8]= 0x01;
		OLED_SendBuf[9]= (u8)(LCD_Pre_Emphasis);	
		Excit_RS232_Send(OLED_SendBuf,10, 20); 
		OSSemPost(JLQRS232SEM);
		fEitRTU = FALSE;
	}
	else
	{
		LCD_Pre_Emphasis = g_DI.Excitdata.FMMODULATIONMODE; 
		if(LCD_Pre_Emphasis == 0)//0:�ر� 1:������ 2:L������ 4:R������ 6:L+R����
		{
			SelectedNum = 1;		
			sprintf(display, "  �ر�  ");
		}
		else if(LCD_Pre_Emphasis == 1)
		{
			SelectedNum = 2;		
			sprintf(display, "  ������  ");
		}
		else if(LCD_Pre_Emphasis == 2)
		{
			SelectedNum = 3;	 	
			sprintf(display, " L������ ");
		}
		else if(LCD_Pre_Emphasis == 4)
		{
			SelectedNum = 4;	 	
			sprintf(display, " R������ ");
		}
		else if(LCD_Pre_Emphasis == 6)
		{
			SelectedNum = 5;	 	
			sprintf(display, " L+R���� ");
		}
	
		PrintGB(5,1,display);
//		printf("jlq pep no......\n"); 
	}
}
//���ƶ�
void SetJLQMODULATIONVAL(int s)
{
	u8 display[50];
	
	LCD_Write_Command(0x01);//����
	sprintf(display, "���ƶ�ϸ��:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);

	LCD_JLQPWR_Gain = g_DI.Excitdata.FMMODULATIONVAL;

	sprintf(display, " %3d   ",LCD_JLQPWR_Gain);
	PrintGB(7,0,display);
}	   
void SetJLQMODULATIONVALAdjust(int s)
{
	u8 display[50];

	if(s == 0)
	{
	 	LCD_Write_Command(0x01);//����
		sprintf(display, "���ƶ�ϸ��:");
		PrintGB(1,0,display);
		sprintf(display, "*");
		PrintGB(0,0,display);
	
		sprintf(display, "[%3d]  ",LCD_JLQPWR_Gain);
		PrintGB(7,0,display);
		return;
	} 
	LCD_JLQPWR_Gain +=s;
	if(LCD_JLQPWR_Gain >= 255)
	{
		LCD_JLQPWR_Gain = 255;		
	} 
	if(LCD_JLQPWR_Gain <= 0)
	{
		LCD_JLQPWR_Gain = 0;		
	}
	sprintf(display, "[%3d]  ",LCD_JLQPWR_Gain);
	PrintGB(7,0,display);
}
void SetJLQMODULATIONVALhandle(int s)
{
	u8 display[50];
	u8 err;

	if((s == 1) ||(s == -1))
		return;
	
	LCD_Write_Command(0x01);//����
	sprintf(display, "���ƶ�ϸ��:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);	

	if(FunPatONF == 1)//ȷ���޸����Ƶ��
	{
	 	sprintf(display, " %3d   ",LCD_JLQPWR_Gain);
		PrintGB(7,0,display); 

		fEitRTU = TRUE;
		OSSemPend(JLQRS232SEM, 0, &err);
		OLED_SendBuf[0]= g_DI.Cfg.Excit_IPHead; 
		OLED_SendBuf[1]= 0x35;
		OLED_SendBuf[2]= 0xC3;
		OLED_SendBuf[3]= 0xFC;
		OLED_SendBuf[4]= 0xC1;
		OLED_SendBuf[5]= 0xEE;
		OLED_SendBuf[6]= 0xC9;
		OLED_SendBuf[7]= 0xE8;
		OLED_SendBuf[8]= 0x0C;
		OLED_SendBuf[9]= (u8)LCD_JLQPWR_Gain;	
		Excit_RS232_Send(OLED_SendBuf,10, 20); 
		OSSemPost(JLQRS232SEM);
		fEitRTU = FALSE; 
	}
	else
	{
		LCD_JLQPWR_Gain = g_DI.Excitdata.FMMODULATIONVAL;

		sprintf(display, " %3d   ",LCD_JLQPWR_Gain);
		PrintGB(7,0,display);

	}
}
//RDS����
void SetJLQRdsVal(int s)
{
	u8 display[50];
	
	LCD_Write_Command(0x01);//����
	sprintf(display, "RDS ����:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);

	LCD_JLQPWR_Gain = g_DI.Excitdata.FMRDSVAL;

	sprintf(display, " %2d   ",LCD_JLQPWR_Gain);
	PrintGB(6,0,display);
}	   
void SetJLQRdsValAdjust(int s)
{
	u8 display[50];

	if(s == 0)
	{
	 	LCD_Write_Command(0x01);//����
		sprintf(display, "RDS ����:");
		PrintGB(1,0,display);
		sprintf(display, "*");
		PrintGB(0,0,display);
	
		sprintf(display, "[%2d]  ",LCD_JLQPWR_Gain);
		PrintGB(6,0,display);
		return;
	} 
	LCD_JLQPWR_Gain +=s;
	if(LCD_JLQPWR_Gain >= 15)
	{
		LCD_JLQPWR_Gain = 15;		
	} 
	if(LCD_JLQPWR_Gain <= 0)
	{
		LCD_JLQPWR_Gain = 0;		
	}
	sprintf(display, "[%2d]  ",LCD_JLQPWR_Gain);
	PrintGB(6,0,display);
}
void SetJLQRdsValhandle(int s)
{
	u8 display[50];
	u8 err;

	if((s == 1) ||(s == -1))
		return;
	
	LCD_Write_Command(0x01);//����
	sprintf(display, "RDS ����:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);	

	if(FunPatONF == 1)//ȷ���޸����Ƶ��
	{
	 	sprintf(display, " %2d   ",LCD_JLQPWR_Gain);
		PrintGB(6,0,display); 

		fEitRTU = TRUE;
		OSSemPend(JLQRS232SEM, 0, &err);
		OLED_SendBuf[0]= g_DI.Cfg.Excit_IPHead; 
		OLED_SendBuf[1]= 0x35;
		OLED_SendBuf[2]= 0xC3;
		OLED_SendBuf[3]= 0xFC;
		OLED_SendBuf[4]= 0xC1;
		OLED_SendBuf[5]= 0xEE;
		OLED_SendBuf[6]= 0xC9;
		OLED_SendBuf[7]= 0xE8;
		OLED_SendBuf[8]= 0x0A;
		OLED_SendBuf[9]= (u8)LCD_JLQPWR_Gain;	
		Excit_RS232_Send(OLED_SendBuf,10, 20); 
		OSSemPost(JLQRS232SEM);
		fEitRTU = FALSE; 
	}
	else
	{
		LCD_JLQPWR_Gain = g_DI.Excitdata.FMRDSVAL;

		sprintf(display, " %2d   ",LCD_JLQPWR_Gain);
		PrintGB(6,0,display);

	}
}
//���ƶȴֵ�
void SetJLQMduVal(int s)
{
	u8 display[50];
	
	LCD_Write_Command(0x01);//����
	sprintf(display, "���ƶȴֵ�:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);

	LCD_JLQPWR_Gain = g_DI.Excitdata.FMSIGNAL_VAL;

	sprintf(display, " %3d   ",LCD_JLQPWR_Gain);
	PrintGB(7,0,display);
}	   
void SetJLQMduValAdjust(int s)
{
	u8 display[50];

	if(s == 0)
	{
	 	LCD_Write_Command(0x01);//����
		sprintf(display, "���ƶȴֵ�:");
		PrintGB(1,0,display);
		sprintf(display, "*");
		PrintGB(0,0,display);
	
		sprintf(display, "[%3d]  ",LCD_JLQPWR_Gain);
		PrintGB(7,0,display);
		return;
	} 
	LCD_JLQPWR_Gain +=s;
	if(LCD_JLQPWR_Gain >= 7)
	{
		LCD_JLQPWR_Gain = 7;		
	} 
	if(LCD_JLQPWR_Gain <= 0)
	{
		LCD_JLQPWR_Gain = 0;		
	}
	sprintf(display, "[%3d]  ",LCD_JLQPWR_Gain);
	PrintGB(7,0,display);
}
void SetJLQMduValhandle(int s)
{
	u8 display[50];
	u8 err;

	if((s == 1) ||(s == -1))
		return;
	
	LCD_Write_Command(0x01);//����
	sprintf(display, "���ƶȴֵ�:");
	PrintGB(1,0,display);
	sprintf(display, "*");
	PrintGB(0,0,display);	

	if(FunPatONF == 1)//ȷ���޸����Ƶ��
	{
	 	sprintf(display, " %3d   ",LCD_JLQPWR_Gain);
		PrintGB(7,0,display); 

		fEitRTU = TRUE;
		OSSemPend(JLQRS232SEM, 0, &err);
		OLED_SendBuf[0]= g_DI.Cfg.Excit_IPHead; 
		OLED_SendBuf[1]= 0x35;
		OLED_SendBuf[2]= 0xC3;
		OLED_SendBuf[3]= 0xFC;
		OLED_SendBuf[4]= 0xC1;
		OLED_SendBuf[5]= 0xEE;
		OLED_SendBuf[6]= 0xC9;
		OLED_SendBuf[7]= 0xE8;
		OLED_SendBuf[8]= 0x11;
		OLED_SendBuf[9]= (u8)LCD_JLQPWR_Gain;	
		Excit_RS232_Send(OLED_SendBuf,10, 20); 
		OSSemPost(JLQRS232SEM);
		fEitRTU = FALSE; 
	}
	else
	{
		LCD_JLQPWR_Gain = g_DI.Excitdata.FMSIGNAL_VAL;

		sprintf(display, " %3d   ",LCD_JLQPWR_Gain);
		PrintGB(7,0,display);

	}
}






