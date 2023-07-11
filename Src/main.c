/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
volatile byte rowcache[16];

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	// init to all keys up
	for (int i=0; i<16; i++) {
		rowcache[i] = 255;
	}
	//rowcache[16] = { 255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255 };

// '1' key is intersection of row 0 and column 1 , so bottom 4 bits 0 and next 3 bits 1  = 0x10
// '1' key PS2 event is 0x16
// So for '1' at 0x16 into the table, we put 0x10
// '2' key is insersection of row 0 and column 2 , so bottom 4 bits 0 and next 3 bits 010 = 0x20
// '2' key PS2 envent is 0x1e
// So for '2' at 0x1e into the table, we put 0x20
// 'A' key is intersection of row 2 and column 6 , so bottom 4 bits 0010 and next 3 bits 110 = 0x62
// 'A' key PS2 event is 0x1c
// So for 'A' at 0x1c into the table, we put 0x62
//
// Select is F10 on PS2 keyboard
volatile byte keyarray[256] = {255,255,255,0x17,0x76,0x56,0x66,0x46,  // 0x00 - 0x07
                      255,0x67,255,255,0x07,0x37,0x12,255,  // 0x08 - 0x0f
                      255,0x2c,0x06,255,0x16,0x64,0x10,255,  // 0x10 - 0x17
                      255,255,0x75,0x05,0x62,0x45,0x20,255,  // 0x18 - 0x1f
                      255,0x03,0x55,0x13,0x23,0x40,0x30,255,  // 0x20 - 0x27
                      255,0x08,0x35,0x33,0x15,0x74,0x50,255,  // 0x28 - 0x2f
                      255,0x34,0x72,0x53,0x43,0x65,0x60,255,  // 0x30 - 0x37
                      255,255,0x24,0x73,0x25,0x70,0x01,255,  // 0x38 - 0x3f
                      255,0x22,0x04,0x63,0x44,0x00,0x11,255,  // 0x40 - 0x47
                      255,0x32,0x42,0x14,0x71,0x54,0x21,255,  // 0x48 - 0x4f
                      255,255,0x02,255,0x51,0x31,255,255,  // 0x50 - 0x57
                      0x36,0x06,0x77,0x61,255,0x41,255,255,  // 0x58 - 0x5f
                      255,255,255,255,255,255,0x57,255,  // 0x60 - 0x67
                      255,255,255,255,255,255,255,255,  // 0x68 - 0x6f
                      255,255,255,255,255,255,0x27,255,  // 0x70 - 0x77
                      0x47,255,255,255,255,255,255,255,  // 0x78 - 0x7f
// for E0 codes (basically we OR the EO code with 0x80. eg. Left arrow is E0 6B, so EB is the entry below for left arrow
                      255,255,255,255,255,255,255,255,  // 0x80 - 0x87
                      255,255,255,255,255,255,255,255,  // 0x88 - 0x8f
                      255,255,255,255,255,255,255,255,  // 0x90 - 0x97
                      255,255,255,255,255,255,255,255,  // 0x98 - 0x9f
                      255,255,255,255,255,255,255,255,  // 0xa0 - 0xa7
                      255,255,255,255,255,255,255,255,  // 0xa8 - 0xaf
                      255,255,255,255,255,255,255,255,  // 0xb0 - 0xb7
                      255,255,255,255,255,255,255,255,  // 0xb8 - 0xbf
                      255,255,255,255,255,255,255,255,  // 0xc0 - 0xc7
                      255,255,255,255,255,255,255,255,  // 0xc8 - 0xcf
                      255,255,255,255,255,255,255,255,  // 0xd0 - 0xd7
                      255,255,255,255,255,255,255,255,  // 0xd8 - 0xdf
                      255,255,255,255,255,255,255,255,  // 0xe0 - 0xe7
                      255,255,255,0x48,0x18,255,255,255,  // 0xe8 - 0xef
                      0x28,0x38,0x68,255,0x78,0x58,255,255,  // 0xf0 - 0xf7
                      255,255,255,255,255,255,255,255 }; // 0xf8 - 0xff





  byte special_key_flag;
  byte last_key;
  uint32_t col;
  uint32_t row;
  byte pressed_row;
  byte pressed_col;



  last_key = 0;

  special_key_flag = 0; // for E0 codes

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  LL_GPIO_SetOutputPin(GPIOA,LL_GPIO_PIN_1);

  NVIC_EnableIRQ(EXTI4_IRQn);
  NVIC_SetPriority(EXTI4_IRQn,0x03);

  NVIC_EnableIRQ(EXTI0_IRQn);
  NVIC_SetPriority(EXTI0_IRQn,0x03);
  NVIC_EnableIRQ(EXTI1_IRQn);
  NVIC_SetPriority(EXTI1_IRQn,0x03);
  NVIC_EnableIRQ(EXTI2_IRQn);
  NVIC_SetPriority(EXTI2_IRQn,0x03);
  NVIC_EnableIRQ(EXTI3_IRQn);
  NVIC_SetPriority(EXTI3_IRQn,0x03);

  kbd_begin(PS2_DATA_PIN, PS2_CLOCK_PIN, TRUE);


  //CDC_Transmit_FS(buffer,sizeof(buffer));
  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    //row = (GPIOD->IDR & 0x000f);
    //col = rowcache[row];
    //GPIOE->ODR = (uint32_t) col;

    if(kbd_available()) {
      // Get a PS2 keycode
      byte       c = kbd_read();
      if ((c != 0xf0) && (c!= 0xe0)) {
        c|=special_key_flag; // if we previously had an E0 we OR the current key with 0x80
        special_key_flag=0;
        if (last_key == 0xf0) {
          // UP
          pressed_row = keyarray[c] & 0x0f;
          pressed_col = (keyarray[c] & 0x70) >> 4;
	  rowcache[pressed_row] |= (1 << pressed_col);
        } else {
          // DOWN
	  if (keyarray[c] != 0xff) {
            pressed_row = keyarray[c] & 0x0f;
            pressed_col = (keyarray[c] & 0x70) >> 4;
	    rowcache[pressed_row] &= ~(1 << pressed_col);
          }
        }
      } else if (c==0xe0) {
        special_key_flag=0x80;
      }
      last_key = c;

      //sprintf((char * restrict) buffer,"Key %d\n",c);
      //CDC_Transmit_FS(buffer,sizeof(buffer));

      //LL_mDelay(2000);
    }

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_5)
  {
  Error_Handler();  
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
    
  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 336, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
  LL_Init1msTick(168000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(168000000);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);

  /**/
  LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3 
                          |LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7);

  /**/
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1); // LED on my board

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3 
                          |LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3 
                          |LL_GPIO_PIN_4|LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /**/
  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE4);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_4;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_4, LL_GPIO_PULL_NO);

  /**/
  LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_4, LL_GPIO_MODE_INPUT);

  /* Init ints on the row pins*/
  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTD, LL_SYSCFG_EXTI_LINE0);
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_0;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING|LL_EXTI_TRIGGER_RISING;
  LL_EXTI_Init(&EXTI_InitStruct);

  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTD, LL_SYSCFG_EXTI_LINE1);
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_1;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING|LL_EXTI_TRIGGER_RISING;
  LL_EXTI_Init(&EXTI_InitStruct);

  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTD, LL_SYSCFG_EXTI_LINE2);
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_2;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING|LL_EXTI_TRIGGER_RISING;
  LL_EXTI_Init(&EXTI_InitStruct);

  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTD, LL_SYSCFG_EXTI_LINE3);
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_3;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING|LL_EXTI_TRIGGER_RISING;
  LL_EXTI_Init(&EXTI_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
