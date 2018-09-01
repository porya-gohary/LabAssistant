
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#include "ssd1306.h"

#include <string.h>

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

DAC_HandleTypeDef hdac;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
int value_adc1;
int value_adc2;
int in1,in2;
int out1,out2;
int dig_in1,dig_in2;
int dig_out1,dig_out2;

int t1,t2,t3,t4,t5;

//Recieve Data From Wi-fi Module
char Buffer_Rx[200];
uint8_t rx_data[1];
uint8_t index=0;
char sendMsg1[30];

//Clock And Date
RTC_TimeTypeDef sTime;
RTC_DateTypeDef DateToUpdate;


char clock[10];
char Date[10];


//SIM800
void RecieveMessage();
void Clearbuffer2();
int IsItMsg();
void getMessage();
void clearText();

char Buffer_Rx2[200];
uint8_t rx_data2[1];
uint8_t index2=0;
int t=0;

char getText[100];
char getText2[100];

char MsgText[22];
char x[]={((char)26),0};
char *Ctrlz=x;

char *recMsg1="AT+CMGF=1\r\n";
//char *recMsg2="AT+CNMI=2,2,0,0,0\r\n";
char *recMsg2="AT+CNMI=1,2,0,0,0\r\n";

char *trueMessage="+CMT: \"+989356276099\"";

char *SignalQuality="AT+CSQ\r\n";
char *check="ATT\r\n";






/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_DAC_Init(void);
static void MX_UART4_Init(void);
static void MX_TIM6_Init(void);
static void MX_UART5_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM7_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void Clearbuffer();
void SetTime(void);
void SetDate(void);

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

//ADC Call Back
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
  
  value_adc1=HAL_ADC_GetValue(&hadc1);
  in1=(value_adc1*3300)/4096;
  value_adc2=HAL_ADC_GetValue(&hadc2);
  in2=(value_adc2*3300)/4096;
}


//UART Call Back
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  if(huart->Instance==UART4){
    Buffer_Rx[index]=rx_data[0];
    
    index++;
    if(index>200) index=0;
    HAL_UART_Receive_IT(&huart4,rx_data,1);
    
    if(index>1 && rx_data[0]=='\n' && (strncmp(Buffer_Rx,"WiFi Connected",14)==0)){
      HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
      HAL_TIM_Base_Start_IT(&htim6);
      ssd1306_draw_bitmap(96, 0, w4, 32, 26);//Wifi Signal
      ssd1306_UpdateScreen();
      Clearbuffer();
    }else if(index>1 && rx_data[0]=='\n' && (strncmp(Buffer_Rx,"|G|",3)==0)){
      sscanf (Buffer_Rx,"%*s %d %d %d %d %d",&t1,&t2,&t3,&t4,&t5);
      Clearbuffer();
    }else if(index>1 && rx_data[0]=='\n'){
      Clearbuffer();
    }
  }
  
  if(huart->Instance==UART5){
    Buffer_Rx2[index2]=rx_data2[0];
    index2++;
    if(index2>200) index2=0;
    HAL_UART_Receive_IT(&huart5,rx_data2,1);
    
    if(index2>1 && Buffer_Rx2[index2-1]=='\n' && IsItMsg()==0) Clearbuffer2();
    if(index2>0 && Buffer_Rx2[index2-1]=='A' && IsItMsg()==0) Clearbuffer2();
    if(Buffer_Rx2[index2-1]=='\n' && IsItMsg()==1) {
      if (t==0) 
      {
        t=1;
        return;
        
      }
      getMessage();
      Clearbuffer2();
      t=0;
      
    }
  }
    
  
  
  
}

//TIMER Call Back

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(htim->Instance==TIM6){
    HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
    
    sprintf(sendMsg1,"|S| %d %d %d %d %d %d %d %d \n",in1,in2,dig_in1,dig_in2,out1,out2,dig_out1,dig_out2);
    HAL_UART_Transmit_IT(&huart4,sendMsg1,strlen(sendMsg1));
  }
  if(htim->Instance==TIM7){
   // SetDate();
    char di1[100];
    if(out1<3300) sprintf (di1, "OUT1 = %04d mV", out1);
    else sprintf (di1, "OUT1 = %04d mV", 3300);
    ssd1306_SetCursor(16, 36);
    ssd1306_WriteString(di1, Font_7x10, White);
    char di2[100];
    if(out2<3300) sprintf (di2, "OUT2 = %04d mV", out2);
    else sprintf (di2, "OUT2 = %04d mV", 3300);
    ssd1306_SetCursor(16, 49);
    ssd1306_WriteString(di2, Font_7x10, White);
    SetTime();
  }
}





/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_DAC_Init();
  MX_UART4_Init();
  MX_TIM6_Init();
  MX_UART5_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
  
  ssd1306_Init();
  //ssd1306_draw_bitmap(112, 2, c_chBat816, 16, 8);
  ssd1306_draw_bitmap(0, 0, Splash, 128, 64);//Splash
  ssd1306_UpdateScreen();
  HAL_Delay(3000);
  ssd1306_Fill(Black);
  // ssd1306_draw_bitmap(96, 0, w3, 32, 26);//Wifi Signal
  
  //ssd1306_draw_bitmap(96, 0, l1, 28, 29);//Wifi Signal
  
  ssd1306_SetCursor(2, 0);
 
  ssd1306_draw_bitmap(0, 0, hourGlass, 128, 64);//Wait ICON
 //  ssd1306_WriteString("Please Wait", Font_11x18, White);//Wait
//  ssd1306_SetCursor(20, 20);
//  ssd1306_WriteString("18/07/13", Font_7x10, White);//Date
  
  
  ssd1306_UpdateScreen();
  
  HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
  HAL_DAC_Start(&hdac,DAC_CHANNEL_2);
  
  // HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
  
  HAL_Delay(5000);
  ssd1306_Fill(Black);
  ssd1306_draw_bitmap(0, 32, line, 128, 1);//Line
   
  RecieveMessage();
  HAL_Delay(300);
 HAL_TIM_Base_Start_IT(&htim7);
//    HAL_UART_Transmit_IT(&huart5,SignalQuality,strlen(SignalQuality));
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_UART_Receive_IT(&huart5,rx_data2,1);
    
    HAL_UART_Receive_IT(&huart4,rx_data,1);
    
    HAL_ADC_Start_IT(&hadc1);
    HAL_ADC_Start_IT(&hadc2);
    
    
    
    //SMS Check
    if(strncmp(getText,"ON",2)==0){
      strcpy(getText2,getText);
      clearText(); 
      t1=1;
    }else if(strncmp(getText,"OFF",3)==0){
      strcpy(getText2,getText);
      clearText(); 
      t1=0;
    }
    
    
    
    //Read Digital Input1
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)==GPIO_PIN_SET) dig_in1=1;
    else dig_in1=0;
        
    //Read Digital Input2
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11)==GPIO_PIN_SET) dig_in2=1;
    else dig_in2=0;
    
    //Set Main Switch
    if(t1==0){
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET);
    }else{
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_SET);
    }
    
    
    //Set Digital Output1
    if(t4>=0){      
      if(t4==0) dig_out1=0;
      else dig_out1=1;
      
    }else{
      if(t4==-1) dig_out1= dig_in1 && dig_in2;
      if(t4==-2) dig_out1= dig_in1 || dig_in2;
    }
    
    if(dig_out1==0) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
    else if(dig_out1==1) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
    
    //Set Digital Output2
    if(t5>=0){
      if(t5==0) dig_out2=0;
      else dig_out2=1;
    }else{
      if(t5==-1) dig_out2= dig_in1 & dig_in2;
      if(t5==-2) dig_out2= dig_in1 | dig_in2;
    }
    
    if(dig_out2==0) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
    else if(dig_out2==1) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
    
    //Set Analog Output1
    if(t2>=0){
      out1=t2;
      HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(t2*4095/3300));
    }
    else{
      switch(t2){
      case -1:
        out1=in1*in2;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;
      case -2:
        out1=in1*in2;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;
      case -3:
        out1=in1*in1;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;
      case -4:
        out1=in2*in2;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;
      case -5:
        out1=in1/in2;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;
      case -6:
        out1=in2/in1;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;
      case -7:
        out1=in1/in1;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;
      case -8:
        out1=in2/in2;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;
      case -9:
        out1=in1+in2;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;        
      case -10:
        out1=in2+in1;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;     
      case -11:
        out1=in1+in1;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;  
      case -12:
        out1=in2+in2;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;
      case -13:
        out1=in1-in2;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else if(out1 <0) HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,0);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;
      case -14:
        out1=in2-in1;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else if(out1 <0) HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,0);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;
      case -15:
        out1=in1-in1;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else if(out1 <0) HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,0);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;
      case -16:
        out1=in2-in2;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,4095);
        else if(out1 <0) HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,0);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,(out1*4095/3300));
        break;
        
      }
    }
    //Set Analog Output2
    if(t3>=0){
      out2=t3;
      HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(t3*4095/3300));
    }
    else{
      switch(t3){
      case -1:
        out2=in1*in2;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;
      case -2:
        out2=in1*in2;
        if(out1 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;
      case -3:
        out2=in1*in1;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;
      case -4:
        out2=in2*in2;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;
      case -5:
        out2=in1/in2;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;
      case -6:
        out2=in2/in1;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;
      case -7:
        out2=in1/in1;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;
      case -8:
        out2=in2/in2;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;
      case -9:
        out2=in1+in2;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;        
      case -10:
        out2=in2+in1;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;     
      case -11:
        out2=in1+in1;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;  
      case -12:
        out2=in2+in2;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;
      case -13:
        out2=in1-in2;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else if(out2 <0) HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,0);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;
      case -14:
        out2=in2-in1;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else if(out2 <0) HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,0);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;
      case -15:
        out2=in1-in1;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else if(out2 <0) HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,0);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;
      case -16:
        out2=in2-in2;
        if(out2 > 3300)HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,4095);
        else if(out2 <0) HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,0);
        else HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,(out2*4095/3300));
        break;
        
      }
    }
    
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
    
  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /**Configure the Systick interrupt time 
    */
  __HAL_RCC_PLLI2S_ENABLE();

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* ADC2 init function */
static void MX_ADC2_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* DAC init function */
static void MX_DAC_Init(void)
{

  DAC_ChannelConfTypeDef sConfig;

    /**DAC Initialization 
    */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**DAC channel OUT1 config 
    */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**DAC channel OUT2 config 
    */
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* RTC init function */
static void MX_RTC_Init(void)
{
  
  
  /* USER CODE BEGIN RTC_Init 0 */
  
  
  /* USER CODE END RTC_Init 0 */
  
  
  /* USER CODE BEGIN RTC_Init 1 */
  
  HAL_PWR_EnableBkUpAccess();
  
  // __PWR_CLK_ENABLE();
  
  
  /* USER CODE END RTC_Init 1 */
  
  /**Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  
  
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  /* USER CODE BEGIN RTC_Init 2 */
  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2){
    sTime.Hours = 9;
    sTime.Minutes = 45;
    sTime.Seconds = 50;
    
    
    
    /* USER CODE END RTC_Init 2 */
    
    /**Initialize RTC and set the Time and Date 
    */
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }
    /* USER CODE BEGIN RTC_Init 3 */
    
    
    /* USER CODE END RTC_Init 3 */
    
    DateToUpdate.WeekDay = RTC_WEEKDAY_TUESDAY;
    DateToUpdate.Month = RTC_MONTH_JULY;
    DateToUpdate.Date = 17;
    DateToUpdate.Year = 18;
    
    if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }
    /* USER CODE BEGIN RTC_Init 4 */
    HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0x32F2);
    HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR2,0x32F2);
  }else{
    __HAL_RCC_CLEAR_RESET_FLAGS();
  }
  /* USER CODE END RTC_Init 4 */
  
}

/* TIM6 init function */
static void MX_TIM6_Init(void)
{
  
  TIM_MasterConfigTypeDef sMasterConfig;
  
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 15999;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 999;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
}

/* TIM7 init function */
static void MX_TIM7_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;

  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 15999;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 500;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* UART4 init function */
static void MX_UART4_Init(void)
{

  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* UART5 init function */
static void MX_UART5_Init(void)
{

  huart5.Instance = UART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void Clearbuffer(){
  memset(Buffer_Rx, 0, sizeof(Buffer_Rx));
  index=0;
  
}

//Sim800 Functions
void RecieveMessage(){
  HAL_UART_Transmit_IT(&huart5,recMsg1,strlen(recMsg1));
  HAL_Delay(1000);
  HAL_UART_Transmit_IT(&huart5,recMsg2,strlen(recMsg2));
  
}

void Clearbuffer2(){
  memset(Buffer_Rx2, 0, sizeof(Buffer_Rx2));
  index2=0;
 
}

int IsItMsg(){
  
  if(strncmp(Buffer_Rx2,trueMessage,21)==0){
    return 1;
  }

  
  return 0;
}

void getMessage(){
  
 clearText();
 
  int j=0;
  
  
  for(int i=0;i<100;i++){
    if(Buffer_Rx2[i]=='\n'){
      i++;
      for(int k=i;k<100;k++){
        if(Buffer_Rx2[k]=='\r') {
          return;
          
          }
        getText[j]=Buffer_Rx2[k];
        
        j++;
      
       
      }
      
    }
    
  }
  
  
}

void clearText(){
  for(int i=0;i<99;i++){
    getText[i]='\0';
  }
}



//Set TIME Function
void SetTime(){
  
  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  
  sprintf (clock, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
  ssd1306_SetCursor(2, 5);
  ssd1306_WriteString(clock, Font_11x18, White);
  ssd1306_UpdateScreen();
  
}


//Set DATE Function
void SetDate(){
  
  
  HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
  
  sprintf (Date, "%02d/%02d/%02d", DateToUpdate.Year, DateToUpdate.Month, DateToUpdate.Date);
  ssd1306_SetCursor(20, 20);
  ssd1306_WriteString(Date, Font_7x10, White);//Date
  ssd1306_UpdateScreen();
}




/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
  tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
