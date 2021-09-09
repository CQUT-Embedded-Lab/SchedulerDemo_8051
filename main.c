#include <reg52.h>

/* temp for anls*/
unsigned char led_t = 0;
unsigned char led0_t = 0;
unsigned char led1_t = 0;
unsigned char led2_t = 0;
unsigned char led3_t = 0;
unsigned char led4_t = 0;
unsigned char led5_t = 0;

/* shce Macro&Func&Var Init*/
#define TICK_RATE_HZ 1000  // Time slice rotation speed
#define TASKS_NUM 6  // ++ for add NewFunc
void TaskA();
void TaskB();
void TaskC();
void TaskD();
void TaskE();
void TaskF();
// declare NewFunc
// add NewFunc behind func[]..
void* const func[TASKS_NUM] = {TaskA,TaskB,TaskC,TaskD,TaskE,TaskF};

unsigned char *funcStkIdx[TASKS_NUM];
unsigned char curFuncIdx = TASKS_NUM-1;
unsigned char preFuncIdx = 0;
unsigned char initFuncIdx = 0;
unsigned int funcDelayList[TASKS_NUM];
unsigned int actualTicks = 0;

/* System */
void InitT0(){
  TMOD = 0x01;
  TH0 = 0xff;
  TL0 = 0xff;
  EA = 1;
  ET0 = 1;
}
void StartT0(){
  TR0 = 1;
}
int main(){
  InitT0();
  StartT0();
  while(1);
}

/* Scheduler */
void osDelay(unsigned int ticks){
	if(initFuncIdx<TASKS_NUM){
		funcDelayList[initFuncIdx-1] = ticks;
	}else{
		funcDelayList[curFuncIdx] = ticks;
	}
	actualTicks = ticks;
	while(actualTicks<=ticks);
}

void it_T0() interrupt 1{ // Use it_T0 time-sharing scheduler
  unsigned char idata *p;
  unsigned char i;
  led_t = ~led_t;  // just for debug
  if(initFuncIdx<TASKS_NUM){  // FirstTimeFunc Need FIRST getFuncAddr
    if(initFuncIdx>0){  // Dont forget save SP
      funcStkIdx[initFuncIdx-1] = (unsigned char *)SP;
    }
    p = (unsigned char *)SP+1;
    *p++ = ((unsigned int)(func[initFuncIdx]))%256;
    *p = ((unsigned int)(func[initFuncIdx]))/256;
    SP = (unsigned char)p;
		// This value depends on how much SP increases when entering the interrupt, which can be viewed through Debug
    SP += 10;
    initFuncIdx++;
  }else{  // After FirstTimeFunc will be runin this way
    preFuncIdx = curFuncIdx;
    curFuncIdx++;
    if(curFuncIdx>TASKS_NUM-1){
      curFuncIdx = 0;
    }
    funcStkIdx[preFuncIdx] = (unsigned char *)SP;
    SP = (unsigned char)funcStkIdx[curFuncIdx];
    for(i=0; i<TASKS_NUM; i++){  // Refresh the timestamp of each task
      funcDelayList[i]--;
    }
    actualTicks = funcDelayList[curFuncIdx];  // Restore context, eg.ticks
  }
	// Reset someThing
  TH0 = (65536-TICK_RATE_HZ)/256;
  TL0 = (65536-TICK_RATE_HZ)%256;
  led_t = ~led_t;
}

/* Tasks */
void TaskA(){
  while(1){
		static unsigned char led0 = 0;
		led0_t = led0;
		led0 = ~led0;
    osDelay(1000);
  }
}
void TaskB(){
  while(1){
		static unsigned char led1 = 0;
		led1_t = led1;
    led1 = ~led1;
    osDelay(2000);
  }
}
void TaskC(){
  while(1){
		static unsigned char led2 = 0;
		led2_t = led2;
    led2 = ~led2;
    osDelay(4000);
  }
}
void TaskD(){
  while(1){
		static unsigned char led3 = 0;
		led3_t = led3;
    led3 = ~led3;
    osDelay(8000);
  }
}
void TaskE(){
  while(1){
		static unsigned char led4 = 0;
		led4_t = led4;
    led4 = ~led4;
    osDelay(16000);
  }
}
void TaskF(){
  while(1){
    static unsigned int delay=1000;
		static char dir=1;
		static unsigned char led5 = 0;
		led5_t = led5;
		led5 = ~led5;
    osDelay(delay);
		delay+=(dir*500);
		if(delay>5000){
			dir=-1;
		}else if(delay<1000){
			dir=1;
		}
  }
}
