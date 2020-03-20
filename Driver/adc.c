/*******************************************************************************
 *
 * Filename  : adc.c
 *
 * Function  : Implememt ADC function form for this workbench.
 *
 * Created   : 2016-10-09 15:23:00
 *           : Mr. WQH
 *
 * Check In  : $Date: 2016-10-09 15:23:00 +0800  $
 *           : $Author: wqh $
 * Revision  : $Revision: * $
 *
 * Copyright(c) by ShangHai DongZhou Lawton Telecom Technologies Co.,Ltd.  
 *              		All Rights Reserved         
 *
 ******************************************************************************/

#include "includes.h"
#include "global.h"	




float PowerCal_Out[2][CALIBRATIONCOUNT] = 
{

	{0.65,2.4 ,0},
	{0  ,0.31,0}
};


float PowerCal_Ref[2][CALIBRATIONCOUNT] = 
{
    {0.65,2.4 ,0},
	{0  ,0.31,0}
};
float PowerCal_In[2][CALIBRATIONCOUNT] = 
{
    {0.65,2.4 ,0},
	{0  ,0.31,0}
};

/*
 *******************************************************************************
 *
 * API routines
 *
 *******************************************************************************
 */
/*******************************************************************************
 * 函数名称：Get_ADC()
 * 函数功能：AD0处理函数
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
void Get_ADC1(void)
{
	u8   ch, count;	
	u16  After_filter[ADC_CH_NUM];      //用来存放求平均值之后的结果
	u32  sum =0;  
		   
	for(ch=0;ch<ADC_CH_NUM;ch++)
   	{ 
		for( count=0; count<ADCTIMES; count++) 
      	{
        	sum += AD_Value[count][ch];

        }
       	After_filter[ch]=sum/ADCTIMES;  
		
		g_ADC[ch] =(After_filter[ch]* 330) / 4096;   //求的结果扩大了100倍，方便下面求出小数
//        printf("g_ADC[%d]= %d \n",ch,g_ADC[ch]);
		sum=0;
	}
} 
void Caculate_ADC(void)
{
    u16 nADindex =0; 
	u16 nADCalIndex=0;	 
	double v3;
	float ftmp;
	
	for(nADindex=0;nADindex<ADC_CH_NUM;nADindex++ )
	{
	    	
		switch( nADindex )
		{
		    case 0://环境温度
				g_DI.Pa[0].Data.ENV_TPT = (float)(g_ADC[ADC_ENV_TPT]*10/6.8-273);	
			
			    break;
				
			case 1://输入功率
				g_DI.Pa[0].Data.P_InVal = (float)(g_ADC[ADC_PA_PWRIN]/100.0);
			   
				v3 = GetPwr( g_DI.Pa[0].Data.P_InVal,PowerCal_In[0],PowerCal_In[1],0);
				
				v3 = v3 * v3 * g_DI.Cfg.P_InCal;
		        g_DI.Pa[0].Data.Power_IN = 10*log10(v3*1000);
			    break;
				
			case 2:	//末级电流
				
				ftmp = (float)(g_ADC[ADC_PA_CUR]/100.0);			
				if( ftmp >= 2.47 )
				{         
					g_DI.Pa[0].Data.PA_CUR1 = (ftmp - 2.47)/0.185;
				}
				else
				    g_DI.Pa[0].Data.PA_CUR1 = 0;

 			  break;
			  
			case 3://入射功率

				g_DI.Pa[0].Data.P_OutVal = (float)(g_ADC[ADC_PA_PWROUT]/100.0);
			   
				v3 = GetPwr( g_DI.Pa[0].Data.P_OutVal,PowerCal_Out[0],PowerCal_Out[1],0);

				v3 = v3 * v3 * g_DI.Cfg.P_OutCal;
				g_DI.Pa[0].Data.Power_FR = v3;
				if((g_DI.Cfg.P_FmFrq>=70.0)&&(g_DI.Cfg.P_FmFrq<=94.0))
				{ 
					g_DI.Pa[0].Data.Power_FR = g_DI.Pa[0].Data.Power_FR*(1+(87.0 - g_DI.Cfg.P_FmFrq)*0.0245);
				}
				else if((g_DI.Cfg.P_FmFrq>94.0)&&(g_DI.Cfg.P_FmFrq<=109.0))
				{
					g_DI.Pa[0].Data.Power_FR = g_DI.Pa[0].Data.Power_FR*(1+(87.0 - g_DI.Cfg.P_FmFrq)*0.02);
				}
													
				if(g_DI.Pa[0].Data.Power_FR < 0)
				{
					g_DI.Pa[0].Data.Power_FR = 0.0;
				}	
			    break;
			case 4://反射功率

				g_DI.Pa[0].Data.P_ReflexVal = (float)(g_ADC[ADC_PA_PWRRV]/100.0);
				ftmp = GetPwr( g_DI.Pa[0].Data.P_ReflexVal,PowerCal_Ref[0],PowerCal_Ref[1],0); 
				g_DI.Pa[0].Data.Power_RV = ftmp*ftmp*g_DI.Cfg.P_RefCal; 
				if((g_DI.Cfg.P_FmFrq>=70.0)&&(g_DI.Cfg.P_FmFrq<=79.0))
				{ 
					g_DI.Pa[0].Data.Power_RV = g_DI.Pa[0].Data.Power_RV+(g_DI.Cfg.P_FmFrq - 87.0)*0.07 +0.5;
				}
				else if((g_DI.Cfg.P_FmFrq>79.0)&&(g_DI.Cfg.P_FmFrq<=87.0))
				{
					g_DI.Pa[0].Data.Power_RV = g_DI.Pa[0].Data.Power_RV+(87.0 - g_DI.Cfg.P_FmFrq)*0.07;
				}
				else if((g_DI.Cfg.P_FmFrq>87.0)&&(g_DI.Cfg.P_FmFrq<=93.0))
				{
					g_DI.Pa[0].Data.Power_RV = g_DI.Pa[0].Data.Power_RV+(87.0 - g_DI.Cfg.P_FmFrq)*0.37;
				}
				else if(g_DI.Cfg.P_FmFrq>93.0)
				{
					g_DI.Pa[0].Data.Power_RV = g_DI.Pa[0].Data.Power_RV/20.0;
				}
				if(g_DI.Pa[0].Data.Power_RV < 0)
				{
					g_DI.Pa[0].Data.Power_RV = 0.0;
				}
				
			    break;
			
			case 5://输入电压	
				g_DI.Pa[0].Data.PA_VOL	= (float)(g_ADC[ADC_PA_VOL]/100.0)*21;
			    break;
			case 6://CPU温度					
				g_DI.Pa[0].Data.CPU_TPT	= (float)((1.43-(float)(g_ADC[ADC_CPU_TPT]/100.0))*1000/4.35+25);
			    break;			
		}
	}
}
//功率换算
float GetPwr(float dy,float *V,float *W,float valmin)      //功率标定转换
{
    u8 n1,n2,index=0;
	u8 n;

    float rt;

    for(n=0;n<CALIBRATIONCOUNT;n++)
    {
   
        if(  V[n] >0.0001 )
        {
            index++;
        }
    }

    if( dy < V[0] )
    {
  
         n1 = 1;
         n2 = 0;
    }
    else if( dy> V[index-1])
    {
       
         n1 = index-1;
         n2 = index-2;
    }
    else
    {
        for( n=index-1;n>=0;n--)
        {
	
            if( fabs(dy- V[n])<0.0001 ) //相等
            {
                return W[n];
            }
            else
            {
                if( V[n]<dy )
                {
                    n1 = n;
                    break;
                }
            }
	
        }
        for( n=0;n<=index;n++)
        {
	
            if( fabs(dy- V[n])<0.0001 )
            {
                return W[n];
            }
            else
            {
                if( V[n]>dy )
                {
                    n2 = n;
                    break;
                }
            }
        }
    }
    rt = (dy*W[n2]-V[n1]*W[n2]-dy*W[n1]+V[n2]*W[n1])/(V[n2]-V[n1]);
    if( rt <valmin )
        rt = valmin;
    return  rt ;
}

