#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery_lcd.h"

#define LCD_FRAME_BUFFER          SDRAM_DEVICE_ADDR
#define RGB565_BYTE_PER_PIXEL     2
#define ARBG8888_BYTE_PER_PIXEL   4

/* FatFs includes component */
#include "ff_gen_drv.h"
#include "sd_diskio.h"
			
static void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);

void DMA2_Stream3_IRQHandler(void);
void DMA2_Stream6_IRQHandler(void);
void SDMMC1_IRQHandler(void);

FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL   MyFile;     /* File object */
char  SDPath[4]; /* SD card logical drive path */

int main(void)
{
	CPU_CACHE_Enable();
	HAL_Init();
	SystemClock_Config();

	// Set up the LCD
	BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FRAME_BUFFER);
	BSP_LCD_SetLayerVisible(LTDC_ACTIVE_LAYER, ENABLE);
	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayOn();



    uint32_t byteswritten, bytesread;                     /* File write/read counts */
	uint8_t wtext[] = "This is KEITH AWESOME SD CARD PROGRAMM!!!111"; /* File write buffer */
	uint8_t rtext[100];                                   /* File read buffer */

	/*MOUNT FILE SYSTEM*/
	/*##-1- Link the micro SD disk I/O driver ##################################*/
	FATFS_LinkDriver(&SD_Driver, SDPath);

	/*##-2- Register the file system object to the FatFs module ##############*/
	f_mount(&SDFatFs, (TCHAR const*)SDPath, 0);


	/*FORMAT AND CREATE FILE SYSTEM*/
	/*##-3- Create a FAT file system (format) on the logical drive #########*/
	/* WARNING: Formatting the uSD card will delete all content on the device */
	f_mkfs((TCHAR const*)SDPath, 0, 0);

	  /*##-4- Create and Open a new text file object with write access #####*/
	f_open(&MyFile, "STM32KEITH.TXT", FA_CREATE_ALWAYS | FA_WRITE);

	/*##-5- Write data to the text file ################################*/
	f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);

	/*##-6- Close the open text file #################################*/
	f_close(&MyFile);


	f_mkdir ("/subDIR");								/* Create a sub directory */

	DIR dir;
	f_opendir(&dir, "/subDIR");

	uint8_t WRITEIT[] = "THIS IS THE SONG THAT NEVER ENDS IT GOES ON AND ON!!"; /* File write buffer */
	  /*##-4- Create and Open a new text file object with write access #####*/
	f_open(&MyFile, "/subDIR/KEITHFILETYPE.TXT", FA_CREATE_ALWAYS | FA_WRITE);

	/*##-5- Write data to the text file ################################*/
	f_write(&MyFile, WRITEIT, sizeof(WRITEIT), (void *)&byteswritten);
	/*##-6- Close the open text file #################################*/
	f_close(&MyFile);



	/*##-7- Open the text file object with read access ###############*/
	f_open(&MyFile, "STM32KEITH.TXT", FA_READ);
	/*##-8- Read data from the text file ###########################*/
	f_read(&MyFile, rtext, sizeof(rtext), (UINT*)&bytesread);
	/*##-9- Close the open text file #############################*/
	f_close(&MyFile);

	BSP_LCD_DisplayStringAtLine(0, (uint8_t *) rtext) ;

	/*##-7- Open the text file object with read access ###############*/
	f_open(&MyFile, "/subDIR/KEITHFILETYPE.TXT", FA_READ);
	/*##-8- Read data from the text file ###########################*/
	f_read(&MyFile, rtext, sizeof(rtext), (UINT*)&bytesread);
	/*##-9- Close the open text file #############################*/
	f_close(&MyFile);

	BSP_LCD_DisplayStringAtLine(4, (uint8_t *) rtext) ;

	/*##-11- Unlink the micro SD disk I/O driver ###############################*/
	FATFS_UnLinkDriver(SDPath);

	/* Infinite loop */
	while (1)
	{
	}
}

void DMA2_Stream3_IRQHandler(void)
{
  BSP_SD_DMA_Rx_IRQHandler();
}

/**
  * @brief  This function handles DMA2 Stream 6 interrupt request.
  */
void DMA2_Stream6_IRQHandler(void)
{
  BSP_SD_DMA_Tx_IRQHandler();
}

/**
  * @brief  This function handles SDIO interrupt request.
  */
void SDMMC1_IRQHandler(void)
{
  BSP_SD_IRQHandler();
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Activate the OverDrive to reach the 216 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
