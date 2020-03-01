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

// 'A' key is intersection of colomn 0 and row 1 (ie 2nd row), so bottom 3 bits 0 and next 3 bits 1  = 0x08
// 'A' key PS2 event is 0x1c
// So for 'A' at 0x1c into the table, we put 0x08
// 'B' key is insersection of column 0 and row 2 (ie 3rd row), so bottom 3 bits 0 and next 3 bits 010 = 0x10
// 'B' key PS2 envent is 0x32
// So for 'B' at 0x32 into the table, we put 0x10
// 'C' key is intersection of column 0 and row 3 (ie 4th row) , so bottom 3 bits 0 and next 3 bits 011 = 0x18
// 'C' key PS2 event is 0x21
// keyarray[] converts a PS2 keycode to a 6 bit rowcol ref in the Galaksija keyboard
// Galaksija special keys
//   BRK - maps to F1
//   RPT - maps to F2
//   LIST - maps to F3
//   DEL  -maps to DEL (backspace is not mapped)
byte keyarray[256] = {255,255,255,255,0x26,0x0e,0x16,255,  // 0x00 - 0x07
                      255,255,255,255,255,255,255,255,  // 0x08 - 0x0f
                      255,0x2c,0x2e,255,0x14,0x0a,0x0c,255,  // 0x10 - 0x17
                      255,255,0x13,0x1a,0x08,0x3a,0x14,255,  // 0x18 - 0x1f
                      255,0x18,0x03,0x20,0x28,0x24,0x1c,255,  // 0x20 - 0x27
                      255,0x3b,0x32,0x30,0x22,0x12,0x2c,255,  // 0x28 - 0x2f
                      255,0x31,0x10,0x01,0x38,0x0b,0x34,255,  // 0x30 - 0x37
                      255,255,0x29,0x11,0x2a,0x3c,0x05,255,  // 0x38 - 0x3f
                      255,0x25,0x19,0x09,0x39,0x04,0x0d,255,  // 0x40 - 0x47
                      255,0x35,0x3d,0x21,0x15,0x02,0x1b,255,  // 0x48 - 0x4f
                      255,255,0x1f,255,0x2f,0x2d,255,255,  // 0x50 - 0x57
                      255,0x36,0x06,0x2e,255,0x1d,255,255,  // 0x58 - 0x5f
                      255,255,255,255,255,255,255,255,  // 0x60 - 0x67
                      255,255,255,255,255,255,255,255,  // 0x68 - 0x6f
                      255,255,255,255,255,255,0x0e,255,  // 0x70 - 0x77
                      255,255,255,255,255,255,255,255,  // 0x78 - 0x7f
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
                      255,255,255,0x2b,255,255,255,255,  // 0xe8 - 0xef
                      255,0x1e,0x23,255,0x33,0x1b,255,255,  // 0xf0 - 0xf7
                      255,255,255,255,255,255,255,255 }; // 0xf8 - 0xff


  // We need to quickly determine which of 8 bits is low and convert to a number from 0 to 7, so we use a lookup table
  // Galaksija is 7 rows so we really only need 128 entries, but leaving here as 256 to make it easier to use
  // for other computers
  byte columnarray[256] = {
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  //00 - 0f
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  //10 - 1f
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  //20 - 2f
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,6,  //30 - 3f
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  //40 - 4f
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,5,  //50 - 5f
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,4,  //60 - 6f
    255,255,255,255,255,255,255,3,255,255,255,2,255,1,0,255, //70 - 7f
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  //80 - 8f
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  //90 - 9f
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  //A0 - Af
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  //B0 - Bf
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  //c0 - cf
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  //d0 - df
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  //e0 - ef
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255 };  //f0 - ff

  byte columncache[8] = { 255,255,255,255, 255,255,255,255 };

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

  /* Configure NVIC for USER_BUTTON_EXTI_IRQn */
  NVIC_EnableIRQ(EXTI0_IRQn);
  NVIC_SetPriority(EXTI0_IRQn,0x03);

  kbd_begin(PS2_DATA_PIN, PS2_CLOCK_PIN, TRUE);


  //LL_mDelay(3000);
  //CDC_Transmit_FS(buffer,sizeof(buffer));
  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    col = GPIOD->IDR &0x7f;
    if (col != 0x7f) { 
      //sprintf((char * restrict) buffer,"col %02x\n",columnarray[col]);
      //CDC_Transmit_FS(buffer,sizeof(buffer));
    
      row = columncache[columnarray[col]];
      // Normally this will output an 0xFF (ie no key pressed)
      GPIOE->ODR = row;

    }

    if(kbd_available()) {
      // Get a PS2 keycode
      byte       c = kbd_read();
      if ((c != 0xf0) && (c!= 0xe0)) {
        c|=special_key_flag; // if we previously had an E0 we OR the current key with 0x80
        special_key_flag=0;
        if (last_key == 0xf0) {
          // UP
          pressed_col = keyarray[c] & 0x07;
          pressed_row = (keyarray[c] & 0x38 ) >> 3;
          columncache[pressed_col] |= (1 << pressed_row);
          

        } else {

          // DOWN
          pressed_col = keyarray[c] & 0x07;
          pressed_row = (keyarray[c] & 0x38 ) >> 3;
          columncache[pressed_col] &= ~(1 << pressed_row);
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
  LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5 
                          |LL_GPIO_PIN_6|LL_GPIO_PIN_7|LL_GPIO_PIN_0|LL_GPIO_PIN_1);

  /**/
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5 
                          |LL_GPIO_PIN_6|LL_GPIO_PIN_7|LL_GPIO_PIN_0|LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
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
  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE0);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_0;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);

  /**/
  LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_0, LL_GPIO_MODE_INPUT);

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
