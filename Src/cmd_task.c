#include "cmd_task.h"
#include "app_config.h"
#include "tusb.h"

#include <stdio.h>
#include <string.h>

/* Кольцевой буфер приёма: пишет tud_cdc_rx_cb (контекст USBTask),
   читает CmdTask. Один писатель + один читатель, поэтому достаточно
   volatile-индексов без критических секций. Размер - степень двойки. */
static uint8_t rx_ring[CMD_RX_RING_SIZE];
static volatile uint16_t rx_head = 0; /* меняет только производитель */
static volatile uint16_t rx_tail = 0; /* меняет только потребитель */

#define RING_MASK (CMD_RX_RING_SIZE - 1u)

static TaskHandle_t xCmdTaskHandle = NULL;

void cmd_task_init(TaskHandle_t xTaskHandle)
{
    xCmdTaskHandle = xTaskHandle;
}

/* Кладёт байт в кольцевой буфер. При переполнении байт отбрасывается. */
static void ring_put(uint8_t byte)
{
    uint16_t next = (rx_head + 1u) & RING_MASK;
    if (next == rx_tail)
    {
        return; /* буфер полон - байт теряется */
    }
    rx_ring[rx_head] = byte;
    rx_head = next;
}

/* Возвращает pdTRUE и байт через *byte, если буфер не пуст */
static BaseType_t ring_get(uint8_t *byte)
{
    if (rx_tail == rx_head)
    {
        return pdFALSE;
    }
    *byte = rx_ring[rx_tail];
    rx_tail = (rx_tail + 1u) & RING_MASK;
    return pdTRUE;
}

/* Callback TinyUSB: пришли данные по CDC.
   Вызывается из tud_task() в контексте USBTask (не из прерывания). */
void tud_cdc_rx_cb(uint8_t itf)
{
    (void)itf;

    /* Предыдущий принятый байт - чтобы пара "\r\n" эхнулась одним переводом строки */
    static uint8_t prev = 0;

    uint8_t buf[64];
    uint32_t count;

    while ((count = tud_cdc_read(buf, sizeof(buf))) > 0)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            uint8_t c = buf[i];

            /* Эхо обратно в COM-порт: конец строки всегда эхлим как "\r\n",
               иначе голый '\r' вернёт курсор в начало строки и ответ
               наложится на введённый текст */
            if (c == '\r' || c == '\n')
            {
                if (!(c == '\n' && prev == '\r'))
                {
                    tud_cdc_write_str("\r\n");
                }
            }
            else
            {
                tud_cdc_write_char((char)c);
            }
            prev = c;

            ring_put(c);

            /* Конец команды ('\n' или '\r') - будим задачу-обработчик */
            if ((c == '\n' || c == '\r') && xCmdTaskHandle != NULL)
            {
                xTaskNotifyGive(xCmdTaskHandle);
            }
        }
    }

    tud_cdc_write_flush();
}

static void cmd_send_response(const char *str)
{
    tud_cdc_write_str(str);
    tud_cdc_write_flush();
}

static void cmd_show_config(void)
{
    const app_config_t *cfg = app_config_get();
    char buf[160];

    snprintf(buf, sizeof(buf),
             "Ok:magic=0x%08lX;version=%lu;extra_info=%s\r\n",
             (unsigned long)cfg->magic,
             (unsigned long)cfg->version,
             cfg->extra_info);

    cmd_send_response(buf);
}

static void cmd_save_config(void)
{
    if (app_config_save(app_config_get()))
    {
        cmd_send_response("Ok\r\n");
    }
    else
    {
        cmd_send_response("Err:flash write failed\r\n");
    }
}

/* Разбор одной завершённой команды вида "имя:параметры" (без конца строки) */
static void cmd_process_line(char *line)
{
    /* Пустая строка (например, '\n' сразу после '\r') - молча пропускаем */
    if (line[0] == '\0')
    {
        return;
    }

    /* Отделяем параметры от имени команды */
    char *params = strchr(line, ':');
    if (params != NULL)
    {
        *params = '\0';
        params++;
    }

    if (strcmp(line, "Test") == 0)
    {
        cmd_send_response("Ok\r\n");
    }
    else if (strcmp(line, "ShowConfig") == 0)
    {
        cmd_show_config();
    }
    else if (strcmp(line, "SaveConfig") == 0)
    {
        cmd_save_config();
    }
    else
    {
        cmd_send_response("Err:unknown command\r\n");
    }
}

/* Достаёт из кольцевого буфера одну строку. Концом строки считается
   '\n' или '\r'; при превышении CMD_MAX_LINE_LEN хвост обрезается. */
static void cmd_take_line(void)
{
    static char line[CMD_MAX_LINE_LEN];
    uint16_t len = 0;
    uint8_t c;

    while (ring_get(&c) == pdTRUE)
    {
        if (c == '\n' || c == '\r')
        {
            line[len] = '\0';
            cmd_process_line(line);
            return;
        }

        if (len < (CMD_MAX_LINE_LEN - 1u))
        {
            line[len++] = (char)c;
        }
    }
}

void CmdTask(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        /* Одно уведомление = одна завершённая '\n' команда в буфере */
        if (ulTaskNotifyTake(pdFALSE, portMAX_DELAY) > 0)
        {
            cmd_take_line();
        }
    }
}
