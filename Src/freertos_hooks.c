#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx_hal.h"

/*-----------------------------------------------------------------------------
 * Хуки (callback функции) FreeRTOS - их должен предоставить пользователь
 *----------------------------------------------------------------------------*/

/* Вызывается, если в конфиге включено configCHECK_FOR_STACK_OVERFLOW */
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char *pcTaskName )
{
    (void) xTask;
    (void) pcTaskName;
    
    /* Здесь можно мигать LED в знак ошибки или сохранить отладочную информацию */
    while(1)
    {
    }
}

/* Вызывается, если не хватает памяти при создании задачи/очереди/и т.д. */
void vApplicationMallocFailedHook( void )
{
    /* Не хватает кучи (configTOTAL_HEAP_SIZE) */
    while(1)
    {
    }
}

/* Вызывается при запуске Idle задачи (опционально) */
void vApplicationIdleHook( void )
{
    /* Можно добавить энергосберегающий режим здесь */
    /* Например: __WFI();  (Wait For Interrupt) */
}

/* Вызывается при запуске Tick задачи (опционально) */
void vApplicationTickHook( void )
{
    /* Вызывается из прерывания! Будьте осторожны */
    /* Здесь нельзя вызывать FreeRTOS API с блокировками */
        /* Вызывается из SysTick прерывания */
}

/* Вызывается при старте планировщика (опционально) */
void vApplicationDaemonTaskStartupHook( void )
{
    /* Вызывается перед запуском задач таймера */
}
