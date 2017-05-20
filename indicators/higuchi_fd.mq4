//+------------------------------------------------------------------+
//|                                                   Higuchi_FD.mq4 |
//| Higuchi's Fractgal Dimention              Copyright 2017, fxborg |
//|                                   http://fxborg-labo.hateblo.jp/ |
//+------------------------------------------------------------------+
#property copyright "Copyright 2017, fxborg"
#property link      "http://fxborg-labo.hateblo.jp/"
#property version   "1.0"
#property strict

//---
#property indicator_separate_window
#property indicator_buffers 1
#property indicator_level1     1.5

#property indicator_color1 clrCornflowerBlue   // 

#property indicator_type1 DRAW_LINE

#property indicator_width1 2

//--- input parameter
input int length = 128;
input int k_max = 32;

//--- buffers
//--- i, q, Hq, tq, dq, hq))
#import "fractal.dll"
   int Create(int,int); 
   int Push(int,int,double,datetime,datetime);//
   double Calculate (int); // 
   void Destroy(int); // 
#import


//--- 
int instance;
double HFD[];
//---
//+------------------------------------------------------------------+
//| Custom indicator initialization function                         |
//+------------------------------------------------------------------+
int OnInit()
  {

   SetIndexBuffer(0,HFD);
   SetIndexEmptyValue(0,0);
	instance = Create(length,k_max); //インスタンスを生成
   return(INIT_SUCCEEDED);
  }
//+------------------------------------------------------------------+
//| De-initialization                                                |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {
   ObjectsDeleteAll(0);
	Destroy(instance); //インスタンスを破棄
  }
//+------------------------------------------------------------------+
//| Custom indicator iteration function                              |
//+------------------------------------------------------------------+
int OnCalculate(const int rates_total,
                const int prev_calculated,
                const datetime &time[],
                const double &open[],
                const double &high[],
                const double &low[],
                const double &close[],
                const long &tick_volume[],
                const long &volume[],
                const int &spread[])
  {
  
    if(ArrayGetAsSeries(close))ArraySetAsSeries(close,false);
    if(ArrayGetAsSeries(time))ArraySetAsSeries(time,false);
    if(ArrayGetAsSeries(HFD))ArraySetAsSeries(HFD,false);
    
    for(int i=(int)MathMax(prev_calculated-1,0);i<rates_total && !IsStopped();i++)
     {
      datetime prev = (i>0) ? time[i-1]: 0;
      int n = Push(instance, i,close[i],time[i],prev);      
      if(n == -1 )continue;   
      if(n == -9999)
      {
         Print(i," ",time[i]);
         Print(n ," ------------- Reset --------------- ",time[i]);
      	Destroy(instance); //インスタンスを破棄
      	instance = Create(length,k_max); //インスタンスを生成
      	return 0;      
      }
      if(rates_total-3000<i )
      {
       double fd=Calculate(instance);
       if(fd != -1.0)HFD[i-1]=fd;       
      }
     }
//--- return value of prev_calculated for next call
   return(rates_total);
  }
