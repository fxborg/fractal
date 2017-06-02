//+------------------------------------------------------------------+
//|                                                          afa.mq4 |
//| Adaptive Fractal Analysis                 Copyright 2017, fxborg |
//|                                   http://fxborg-labo.hateblo.jp/ |
//+------------------------------------------------------------------+
#property copyright "Copyright 2017, fxborg"
#property link      "http://fxborg-labo.hateblo.jp/"
#property version   "1.0"
#property strict
#include <MovingAverages.mqh>

//---
#property indicator_separate_window
#property indicator_buffers 2
#property indicator_level1     1.5

#property indicator_color1 clrRed   // 
#property indicator_color2 clrCornflowerBlue   // 

#property indicator_type1 DRAW_LINE
#property indicator_type2 DRAW_LINE

#property indicator_width1 2
#property indicator_width2 2

//--- input parameter
input int length = 1024;
input int order = 2;
input int MaPeriod=4;

//--- buffers
//--- i, q, Hq, tq, dq, hq))
#import "afa.dll"
   int Create(int,int); 
   int Push(int,int,double,datetime,datetime);//
   double Calculate (int); // 
   void Destroy(int); // 
#import


//--- 
int instance;
double AFA[];
double AFA_MA[];
//---
//+------------------------------------------------------------------+
//| Custom indicator initialization function                         |
//+------------------------------------------------------------------+
int OnInit()
  {

   SetIndexBuffer(0,AFA);
   SetIndexBuffer(1,AFA_MA);
   SetIndexEmptyValue(0,0);
   SetIndexEmptyValue(1,0);
	instance = Create(length,order); //インスタンスを生成
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
    if(ArrayGetAsSeries(AFA))ArraySetAsSeries(AFA,false);
    if(ArrayGetAsSeries(AFA_MA))ArraySetAsSeries(AFA_MA,false);
    
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
      	instance = Create(length,order); //インスタンスを生成
      	return 0;      
      }
      if(rates_total-3000<i )
      {
       double hurst=Calculate(instance);
       if(hurst != -1.0)AFA[i-1]=2.0-hurst;       
       if(i-1<=MaPeriod)continue;
       AFA_MA[i-1]=SimpleMA(i-1,MaPeriod,AFA);
      }
     }
//--- return value of prev_calculated for next call
   return(rates_total);
  }
