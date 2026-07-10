#include "main.h"
#include "led_task.h"
#include "usb_task.h"
#include "usb_report_task.h"
#include "cmd_task.h"
#include "app_config.h"
#include "stm32f4xx_hal_tim.h"
#include "usb_hid.h"

#include "FreeRTOS.h"
#include "task.h"

static void MX_GPIO_Init(void);
TIM_HandleTypeDef htim4;

int main(void)
{
    HAL_Init();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    SystemClock_Config();
    MX_GPIO_Init();

    app_config_init();

    usb_device_init();
    usb_report_init();

    xTaskCreate(USBTask, "USB Task", 256, NULL, 2, NULL);
    xTaskCreate(USBReportTask, "USB Report", 256, NULL, 2, NULL);
    xTaskCreate(vLEDTask, "LED", 128, NULL, 1, NULL);

    TaskHandle_t xCmdTaskHandle = NULL;
    xTaskCreate(CmdTask, "Cmd", 256, NULL, 1, &xCmdTaskHandle);
    cmd_task_init(xCmdTaskHandle);

    vTaskStartScheduler();
}

static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);
}

void Error_Handler(void)
{
    while (1)
    {
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        HAL_Delay(100);
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* HSE = 8 MHz -> VCO_in = 8/4 = 2 MHz, VCO_out = 2*180 = 360 MHz
       SYSCLK = 360/2 = 180 MHz (максимальная частота STM32F446).

       360 МГц не делится нацело на 48, поэтому USB-тактирование (48 МГц)
       нельзя получить из PLLQ основной PLL. На F446 для этого есть отдельный
       PLLSAI и мультиплексор CK48MSEL — он настраивается ниже. PLLQ здесь
       остаётся валидным, но для USB не используется. */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 180;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* 180 MHz requires the regulator Over-Drive mode */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4; /* APB1 max 45 MHz: 180/4 = 45 MHz */
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; /* APB2 max 90 MHz: 180/2 = 90 MHz */
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }

    /* PLLSAI — вторая, независимая PLL в STM32F446 (помимо основной PLL и PLLI2S).
       Изначально задумана для тактирования аудио-интерфейса SAI и LCD-TFT, но на
       F446 её выход P (PLLSAIP) можно подать на мультиплексор CK48MSEL и получить
       48 МГц для USB OTG FS / SDIO / RNG. Плюс в том, что она отвязана от основной
       PLL: SYSCLK можно держать на максимальных 180 МГц (VCO 360 МГц не делится
       нацело на 48), а USB тактировать отдельной цепочкой ровно на 48 МГц.

       Формула выхода: VCO_in = HSE / PLLSAIM, VCO_out = VCO_in * PLLSAIN,
                       выход P = VCO_out / PLLSAIP.
       Здесь: 8/4 = 2 МГц -> 2*96 = 192 МГц -> 192/4 = 48 МГц. */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_CLK48;         /* настраиваем именно 48-МГц домен */
    PeriphClkInit.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP;   /* источник CK48 = выход PLLSAI-P (бит CK48MSEL) */
    PeriphClkInit.PLLSAI.PLLSAIM = 4;                                 /* делитель входа: 8 МГц / 4 = 2 МГц VCO_in (реком. 1..2 МГц) */
    PeriphClkInit.PLLSAI.PLLSAIN = 96;                                /* множитель VCO: 2 МГц * 96 = 192 МГц (VCO 100..432 МГц) */
    PeriphClkInit.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4;                  /* делитель выхода P: 192 МГц / 4 = 48 МГц для USB */
    PeriphClkInit.PLLSAI.PLLSAIQ = 2;                                 /* выход Q (SAI-клок): здесь не используется, но должен быть валиден (2..15) */
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

void _init(void)
{
    /* Пустая функция для C++ инициализации */
}
