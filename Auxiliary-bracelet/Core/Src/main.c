/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "max30102.h"
#include "string.h"
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
uint8_t spo ;
uint8_t heartReat;
uint8_t *heartnum;
char UartBuffer[32];

uint8_t u4_data=0; 
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//void USART1_SendString(char* s) {
//	HAL_UART_Transmit(&huart1, (uint8_t*) s, strlen(s), 1000);
//}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_USART1_UART_Init();
  MX_I2C2_Init();
  MX_USART2_UART_Init();
  MX_TIM7_Init();
  MX_TIM6_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */
    HAL_Delay(200);
    OLED_Init();
    max30102_init();

	HAL_TIM_Base_Start_IT(&htim6);
	HAL_TIM_Base_Start_IT(&htim7);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&mesg, 1);
	HAL_UART_Receive_IT(&huart4,&u4_data,1);
		Net_Things_Init();
		int k =0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		heartnum[1]=heartReat;
//		heartnum=&heartReat;
//			printf("heart:%d",heartReat);
//				Property_Post(heartReat,spo);
//HAL_Delay(50);		
            // 显示心率
HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);
            if (heartReat % 1000 / 100 > 0)
                OLED_showNum(0, heartReat % 1000 / 100);
            else
                OLED_showNum(0, 10);
            OLED_showNum(1, heartReat % 100 / 10);
            OLED_showNum(2, heartReat % 10);
            // 显示血氧
            if (spo % 1000 / 100 > 0)
                OLED_showNum(3, spo % 1000 / 100);
            else
                OLED_showNum(3, 10);
            OLED_showNum(4, spo % 100 / 10);
            OLED_showNum(5, spo % 10);
            // 画图表
            int16_t diff = max30102_getDiff();
            OLED_drawChart(diff);
            if (diff > 50)
                OLED_showHeart(0);
            else
                OLED_showHeart(1);
						HAL_Delay(10);
						Property_Post(heartReat,spo);
		}
		



	
			
		

  
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void  HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)    //定时器中断回调函数
  {
     
     if(htim == &htim7)   //确定是定时器6产生的中断
     {
					if(RX_Flag == 1){
				HAL_UART_Transmit(&huart1, message, offset, 0xffff);
				memset(message, '\0', sizeof(message));

				RX_Flag = 0;

				}
			else{
				HAL_UART_Transmit(&huart1, "Hello World!\r\n", sizeof("Hello World!\r\n"), 0xffff);
						}
       
     }
			if(htim == &htim6)   //确定是定时器6产生的中断
			{
				        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == GPIO_PIN_RESET)
        {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);
            max30102_cal();

            spo = max30102_getSpO2();
             heartReat = max30102_getHeartRate();
					if(heartReat>100)heartReat=100;
	
//					          Property_Post(heartReat,spo);
				}
			}
        
  }
	void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart==&huart4)
	{
		 
		if(u4_data=='0') {printf("%d",u4_data);HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);}
		else if(u4_data=='1')HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
		HAL_UART_Transmit(&huart4, "Hello World!\r\n", sizeof("Hello World!\r\n"), 0xffff);
    //else if(u1_data=='r') {_set_FAULTMASK(1);HAL_NVIC_System_Reset();}
		
    HAL_UART_Receive_IT(&huart4,&u4_data,1);
	}

	
}
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//	if (GPIO_Pin == INT_Pin) {
//	 HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);
//		max30102_cal();

//spo = max30102_getSpO2();
//heartReat = max30102_getHeartRate();
//	}
//}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

