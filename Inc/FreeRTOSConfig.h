/*
 * FreeRTOS Kernel V11.1.0
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 */

/*******************************************************************************
 * Этот файл предоставляет пример заголовочного файла FreeRTOSConfig.h 
 * с сокращённым объяснением каждого параметра конфигурации.
 * https://www.freertos.org/a00110.html
 ******************************************************************************/
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

void xPortSysTickHandler(void);
void vPortSVCHandler(void);
void xPortPendSVHandler(void);

#define vPortSVCHandler								SVC_Handler
#define xPortPendSVHandler							PendSV_Handler
/******************************************************************************/
/* Определения, связанные с описанием аппаратного обеспечения *****************/
/******************************************************************************/

/* Частота тактирования ядра процессора, от которой зависит частота прерываний тика.*/
#define configCPU_CLOCK_HZ    ( ( unsigned long ) 168000000 )

/* Для Cortex-M портов: частота тактирования SysTick таймера.
 * Обычно SysTick работает на той же частоте, что и ядро, поэтому оставляем закомментированным.
 * Раскомментируйте только если SysTick тактируется от другого источника. */
/* #define configSYSTICK_CLOCK_HZ    [Зависит от платформы] */

/******************************************************************************/
/* Определения, связанные с поведением планировщика ***************************/
/******************************************************************************/

/* Частота прерываний тика в Герцах (обычно 1000 Гц = 1 мс) */
#define configTICK_RATE_HZ                         1000

/* 1 = вытесняющая многозадачность, 0 = кооперативная (задачи сами отдают управление) */
#define configUSE_PREEMPTION                       1

/* 1 = переключение между задачами одного приоритета по каждому тику, 0 = без тайм-слайсинга */
#define configUSE_TIME_SLICING                     0

/* 1 = оптимизированный алгоритм выбора следующей задачи (с использованием инструкции CLZ),
 * 0 = стандартный алгоритм на C (более переносимый) */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION    0

/* 1 = энергосберегающий режим без тиков (tickless idle), 0 = тик всегда активен */
#define configUSE_TICKLESS_IDLE                    0

/* Количество доступных приоритетов задач (от 0 до configMAX_PRIORITIES-1, где 0 - низший) */
#define configMAX_PRIORITIES                       5

/* Размер стека для задачи Idle (в словах, а не байтах!) */
#define configMINIMAL_STACK_SIZE                   128

/* Максимальная длина имени задачи (включая завершающий ноль) */
#define configMAX_TASK_NAME_LEN                    16

/* Тип для хранения счётчика тиков:
 * TICK_TYPE_WIDTH_16_BITS - 16 бит (переполнение через ~65 секунд при 1 кГц)
 * TICK_TYPE_WIDTH_32_BITS - 32 бита (переполнение через ~49 дней)
 * TICK_TYPE_WIDTH_64_BITS - 64 бита (практически никогда) */
#define configTICK_TYPE_WIDTH_IN_BITS              TICK_TYPE_WIDTH_32_BITS

/* 1 = задача Idle уступает выполнение приложению, если есть задача с приоритетом 0 готова к выполнению */
#define configIDLE_SHOULD_YIELD                    1

/* Количество уведомлений (индексов в массиве) для каждой задачи. По умолчанию 1 */
#define configTASK_NOTIFICATION_ARRAY_ENTRIES      1

/* Размер реестра очередей для отладчика. 0 = отключено */
#define configQUEUE_REGISTRY_SIZE                  0

/* 1 = включить обратную совместимость со старыми версиями FreeRTOS */
#define configENABLE_BACKWARD_COMPATIBILITY        0

/* Количество указателей локального хранилища потока (thread local storage) */
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS    0

/* Использовать мини-списки (экономит RAM) */
#define configUSE_MINI_LIST_ITEM                   1

/* Тип для хранения размера стека (size_t или uint16_t для экономии) */
#define configSTACK_DEPTH_TYPE                     size_t

/* Тип хранения длины сообщения в буферах сообщений */
#define configMESSAGE_BUFFER_LENGTH_TYPE           size_t

/* 1 = очищать память нулями при освобождении (vPortFree) */
#define configHEAP_CLEAR_MEMORY_ON_FREE            1

/* Максимальная длина буфера для статистики (vTaskList) */
#define configSTATS_BUFFER_MAX_LENGTH              0xFFFF

/* 1 = выделять структуру newlib reent для каждой задачи (требует locks для malloc) */
#define configUSE_NEWLIB_REENTRANT                 0

/******************************************************************************/
/* Определения, связанные с программными таймерами ****************************/
/******************************************************************************/

/* 1 = включить функциональность программных таймеров (требует timers.c в сборке) */
#define configUSE_TIMERS                           1

/* Приоритет задачи-сервера таймеров (обычно высокий) */
#define configTIMER_TASK_PRIORITY                  ( configMAX_PRIORITIES - 1 )

/* Размер стека для задачи-сервера таймеров (в словах) */
#define configTIMER_TASK_STACK_DEPTH               configMINIMAL_STACK_SIZE

/* Длина очереди команд для задачи-сервера таймеров */
#define configTIMER_QUEUE_LENGTH                   10

/******************************************************************************/
/* Определения, связанные с группами событий **********************************/
/******************************************************************************/

/* 1 = включить группы событий (требует event_groups.c в сборке) */
#define configUSE_EVENT_GROUPS                     1

/******************************************************************************/
/* Определения, связанные с потоковыми буферами *******************************/
/******************************************************************************/

/* 1 = включить потоковые буферы (требует stream_buffer.c в сборке) */
#define configUSE_STREAM_BUFFERS                   1

/******************************************************************************/
/* Определения, связанные с выделением памяти *********************************/
/******************************************************************************/

/* 1 = включить API для статического выделения памяти (создание объектов через ...Static()) */
#define configSUPPORT_STATIC_ALLOCATION            1

/* 1 = включить API для динамического выделения памяти (создание объектов через ...Create()) */
#define configSUPPORT_DYNAMIC_ALLOCATION           1

/* Общий размер кучи FreeRTOS в байтах (для heap_1/2/3/4) */
#define configTOTAL_HEAP_SIZE                      8192

/* 1 = массив под кучу выделяет приложение, 0 = линковщик */
#define configAPPLICATION_ALLOCATED_HEAP           0

/* 1 = стеки задач выделяются из отдельной кучи (для быстрой памяти) */
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP  0

/* 1 = защита указателей кучи от повреждений (heap_4/5) */
#define configENABLE_HEAP_PROTECTOR                0

/******************************************************************************/
/* Настройка вложенности прерываний *******************************************/
/******************************************************************************/

/* Приоритет прерываний ядра (тик и переключение контекста). 0 = наивысший */
#define configKERNEL_INTERRUPT_PRIORITY            0xF0

/* Максимальный приоритет прерываний, из которых можно вызывать API FreeRTOS.
 * Прерывания с приоритетом выше (численно меньше) этого значения никогда не отключаются. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY       0x50

/* Альтернативное имя для configMAX_SYSCALL_INTERRUPT_PRIORITY */
#define configMAX_API_CALL_INTERRUPT_PRIORITY      0x50

/******************************************************************************/
/* Хуки (функции обратного вызова) ********************************************/
/******************************************************************************/

/* 1 = включить соответствующий хук (функцию должен предоставить пользователь) */
#define configUSE_IDLE_HOOK                        0
#define configUSE_TICK_HOOK                        1
#define configUSE_MALLOC_FAILED_HOOK               0
#define configUSE_DAEMON_TASK_STARTUP_HOOK         0

/* 1 = включить callbacks для завершения отправки/приёма в буферах сообщений */
#define configUSE_SB_COMPLETED_CALLBACK            0

/* Проверка переполнения стека:
 * 0 = отключено
 * 1 = быстрая проверка (только выход за границы)
 * 2 = надёжная проверка (запись паттерна в конец стека) */
#define configCHECK_FOR_STACK_OVERFLOW             2

/******************************************************************************/
/* Сбор статистики времени выполнения *****************************************/
/******************************************************************************/

/* 1 = сбор статистики времени выполнения задач (требует таймер) */
#define configGENERATE_RUN_TIME_STATS              0

/* 1 = включить структуры для трассировки и визуализации */
#define configUSE_TRACE_FACILITY                   0

/* 1 = включить функции vTaskList() и vTaskGetRunTimeStats() (требуют sprintf) */
#define configUSE_STATS_FORMATTING_FUNCTIONS       0

/******************************************************************************/
/* Совместные задачи (co-routines) - устаревший механизм **********************/
/******************************************************************************/

/* 1 = включить совместные задачи (требует croutine.c) */
#define configUSE_CO_ROUTINES                      0

/* Количество приоритетов для совместных задач */
#define configMAX_CO_ROUTINE_PRIORITIES            1

/******************************************************************************/
/* Отладочные проверки ********************************************************/
/******************************************************************************/

/* Проверка условия - при ошибке отключает прерывания и зависает */
#define configASSERT( x )         \
    if( ( x ) == 0 )              \
    {                             \
        taskDISABLE_INTERRUPTS(); \
        for( ; ; )                \
        ;                         \
    }

/******************************************************************************/
/* FreeRTOS с поддержкой MPU (Memory Protection Unit) *************************/
/******************************************************************************/

/* 1 = приложение может предоставлять привилегированные функции */
#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS    0

/* Количество MPU регионов на вашем оборудовании (обычно 8 или 16) */
#define configTOTAL_MPU_REGIONS                                   8

/* Настройки TEX/S/C/B для области Flash */
#define configTEX_S_C_B_FLASH                                     0x07UL

/* Настройки TEX/S/C/B для области RAM */
#define configTEX_S_C_B_SRAM                                      0x07UL

/* 1 = запретить повышение привилегий из-за пределов ядра */
#define configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY               1

/* 1 = разрешить непривилегированным задачам входить в критическую секцию */
#define configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS                0

/* 0 = использовать новую v2 обёртку MPU, 1 = старую v1 */
#define configUSE_MPU_WRAPPERS_V1                                 0

/* Размер пула объектов ядра для защищённого режима */
#define configPROTECTED_KERNEL_OBJECT_POOL_SIZE                   10

/* Размер стека системных вызовов в словах */
#define configSYSTEM_CALL_STACK_SIZE                              128

/* 1 = включить списки контроля доступа (ACL) */
#define configENABLE_ACCESS_CONTROL_LIST                          1

/******************************************************************************/
/* Специфичные для ARMv8-M (TrustZone) ****************************************/
/******************************************************************************/

/* Максимальное количество безопасных контекстов */
#define secureconfigMAX_SECURE_CONTEXTS                           5

/* Ядро предоставляет память для задач Idle и Timer */
#define configKERNEL_PROVIDED_STATIC_MEMORY                       1

/* 1 = включить TrustZone (для небезопасной стороны) */
#define configENABLE_TRUSTZONE                                    1

/* 1 = запуск FreeRTOS на безопасной стороне без переходов в небезопасную */
#define configRUN_FREERTOS_SECURE_ONLY                            1

/* 1 = включить MPU, 0 = отключить */
#define configENABLE_MPU                                          1

/* 1 = включить FPU (Floating Point Unit) */
#define configENABLE_FPU                                          1

/* 1 = включить MVE (M-Profile Vector Extension) - только для Cortex-M55/M85 */
#define configENABLE_MVE                                          1

/******************************************************************************/
/* Специфичные для ARMv7-M и ARMv8-M (проверка установки обработчиков) ********/
/******************************************************************************/

/* 1 = проверять, что обработчики прерываний FreeRTOS правильно установлены */
#define configCHECK_HANDLER_INSTALLATION                          1

/******************************************************************************/
/* Включение/исключение конкретных функций ************************************/
/******************************************************************************/

/* Включение базовых возможностей */
#define configUSE_TASK_NOTIFICATIONS                              1
#define configUSE_MUTEXES                                         1
#define configUSE_RECURSIVE_MUTEXES                               1
#define configUSE_COUNTING_SEMAPHORES                             1
#define configUSE_QUEUE_SETS                                      0
#define configUSE_APPLICATION_TASK_TAG                            0

/* Включение конкретных API функций */
#define INCLUDE_vTaskPrioritySet                                  1
#define INCLUDE_uxTaskPriorityGet                                 1
#define INCLUDE_vTaskDelete                                       1
#define INCLUDE_vTaskSuspend                                      1
#define INCLUDE_xResumeFromISR                                    1
#define INCLUDE_vTaskDelayUntil                                   1
#define INCLUDE_vTaskDelay                                        1
#define INCLUDE_xTaskGetSchedulerState                            1
#define INCLUDE_xTaskGetCurrentTaskHandle                         1
#define INCLUDE_uxTaskGetStackHighWaterMark                       0
#define INCLUDE_xTaskGetIdleTaskHandle                            0
#define INCLUDE_eTaskGetState                                     0
#define INCLUDE_xEventGroupSetBitFromISR                          1
#define INCLUDE_xTimerPendFunctionCall                            0
#define INCLUDE_xTaskAbortDelay                                   0
#define INCLUDE_xTaskGetHandle                                    0
#define INCLUDE_xTaskResumeFromISR                                1

/******************************************************************************/
/* Перенаправление обработчиков прерываний (КРИТИЧЕСКИ ВАЖНО!) ****************/
/******************************************************************************/

/* Эти макросы связывают обработчики FreeRTOS с реальными прерываниями */
// #define vPortSVCHandler                 SVC_Handler
// #define xPortPendSVHandler              PendSV_Handler
// #define xPortSysTickHandler             SysTick_Handler

#endif /* FREERTOS_CONFIG_H */