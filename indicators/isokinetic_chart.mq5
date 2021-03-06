//+------------------------------------------------------------------+
//|                                             isokinetic_chart.mq5 |
//| iso-kinetic chart v1.00                   Copyright 2017, fxborg |
//|                                   http://fxborg-labo.hateblo.jp/ |
//+------------------------------------------------------------------+
#property copyright "Copyright 2017, fxborg"
#property link      "http://fxborg-labo.hateblo.jp/"
#property version   "1.00"

#property indicator_buffers 5
#property indicator_plots   1
#property indicator_separate_window

#property indicator_type1 DRAW_COLOR_CANDLES
#property indicator_color1 clrLimeGreen,clrDarkOrange
#property indicator_width1 1


//--- input parameters
input int InpCalcBarLimit=5000; // calc BarLimit
input int InpDispBarLimit=1000; // disp BarLimit
input int InpThreshold=100;  //Threshold(in Points) 
double threshold=InpThreshold*_Point;
double  OPEN[];
double  HIGH[];
double  LOW[];
double  CLOSE[];
double  COLOR[];

//---- declaration of global variables
int min_rates_total;
//+------------------------------------------------------------------+
//| Custom indicator initialization function                         |
//+------------------------------------------------------------------+
int OnInit()
  {
//---- Initialization of variables of data calculation starting point
   min_rates_total=2;
//--- indicator buffers mapping

//--- indicator buffers
   int i=0;
   SetIndexBuffer(0,OPEN,INDICATOR_DATA);
   SetIndexBuffer(1,HIGH,INDICATOR_DATA);
   SetIndexBuffer(2,LOW,INDICATOR_DATA);
   SetIndexBuffer(3,CLOSE,INDICATOR_DATA);
   SetIndexBuffer(4,COLOR,INDICATOR_COLOR_INDEX);

//---
//---
   return(INIT_SUCCEEDED);
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
//---

//---
   int limit=fmin(Bars(_Symbol,PERIOD_M1),InpCalcBarLimit);
   double Close[];
   datetime Time[];
   if(CopyClose(_Symbol,PERIOD_M1,0,limit,Close)!=limit || 
      CopyTime(_Symbol,PERIOD_M1,0,limit,Time)!=limit)
     {
      Print("Failed to copy history data for the M1 time frame. Error #",GetLastError());
      return(0);
     }
   MqlRates wk[];
   MqlRates bk[];
   MqlRates rate;
   ArrayResize(wk,limit);
   ArrayResize(bk,limit);
//---
   int cnt=0;
   double accum_volat=0;
   double last_close = Close[0];
//---
   create_bar(rate,last_close);
//---
   MqlDateTime last_time;
   TimeToStruct(Time[0],last_time);
   int year_week_num = getYearWeekNum(last_time.year);
   int last_week_num = getWeekNum(last_time,year_week_num);
//---
   for(int i=1;i<limit;i++)
     {
      if(IsStopped()) break;
      if(cnt>ArraySize(wk)-100)
        {
         ArrayCopy(bk,wk);
         ArrayResize(wk,ArraySize(wk)+100);
         ArrayCopy(wk,bk,0,0,cnt-1);
         ArrayResize(bk,ArraySize(bk)+100);
        }

      double diff=Close[i]-last_close;
      double adiff=fabs(diff);
      int sign=(diff>0) -(diff<0);
      if(accum_volat+adiff<threshold)
        {
         //---
         update_bar(rate,Close[i]);
         //---
         last_close=rate.close;
         accum_volat+=adiff;
        }
      else
        {
         while(accum_volat+adiff>=threshold)
           {
            double decrease=(threshold-accum_volat);
            last_close=last_close+sign*decrease;
            //---
            update_bar(rate,last_close);
            //---
            accum_volat=0;
            adiff-=decrease;
            //output
            wk[cnt].open = rate.open;
            wk[cnt].high = rate.high;
            wk[cnt].low=rate.low;
            wk[cnt].close=rate.close;
            cnt++;
            //---
            create_bar(rate,last_close);
            //---
           }
        }

      MqlDateTime new_time;
      TimeToStruct(Time[i],new_time);
      if(last_time.year!=new_time.year)
        {
         year_week_num=getYearWeekNum(new_time.year);
        }
      int week_num=getWeekNum(new_time,year_week_num);
      if(last_week_num!=week_num && last_close!=Close[i])
        {
         last_close=Close[i];
         //---
         update_bar(rate,last_close);
         //---
         //set buffer
         wk[cnt].open = rate.open;
         wk[cnt].high = rate.high;
         wk[cnt].low=rate.low;
         wk[cnt].close=rate.close;
         cnt++;
         //---
         create_bar(rate,last_close);
         //---
        }
      last_week_num=week_num;
      last_time=new_time;
     }

//---
   ArrayInitialize(OPEN,EMPTY_VALUE);
   ArrayInitialize(HIGH,EMPTY_VALUE);
   ArrayInitialize(LOW,EMPTY_VALUE);
   ArrayInitialize(CLOSE,EMPTY_VALUE);
   ArrayInitialize(COLOR,EMPTY_VALUE);
//---
   int sz=fmin(fmin(cnt,InpDispBarLimit),rates_total-1)-1;
   int i=rates_total;
   for(int j=sz;j>=0;j--)
     {
      OPEN[i-j-1]=wk[cnt-j-1].open;
      HIGH[i-j-1]=wk[cnt-j-1].high;
      LOW[i-j-1]=wk[cnt-j-1].low;
      CLOSE[i-j-1]=wk[cnt-j-1].close;
      COLOR[i-j-1]=(CLOSE[i-j-1]>=OPEN[i-j-1])?0:1;
     }
//----    

   return(rates_total);
  }
//+------------------------------------------------------------------+
int getWeekNum(MqlDateTime &mdt,int year_week_num)
  {
   int week_num=int((mdt.day_of_year+6)/7);
   if(mdt.day_of_week<year_week_num)++week_num;
   return week_num;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int getYearWeekNum(int y)
  {
   datetime d=StringToTime(IntegerToString(y)+".01.01");
   MqlDateTime md;
   TimeToStruct(d,md);
   return md.day_of_week;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void create_bar(MqlRates &r,const double v)
  {
   r.open=v;
   r.low=v;
   r.high=v;
   r.close=v;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void update_bar(MqlRates &r,const double v)
  {
   if(r.low>v) r.low=v;
   if(r.high<v) r.high=v;
   r.close=v;
  }
//+------------------------------------------------------------------+
