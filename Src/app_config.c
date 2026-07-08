#include "app_config.h"
#include "stm32f4xx_hal.h"

#include <string.h>
#include <stddef.h>

#define APP_CONFIG_MAGIC     0xC0FFEE01u
#define APP_CONFIG_VERSION   1u

/* STM32F446RE: 512 КиБ flash, последний сектор - сектор 7 (128 КиБ,
   0x08060000-0x0807FFFF). Минимальная единица стирания - целый сектор,
   поэтому под конфиг отведён весь сектор целиком, а не отдельная страница. */
#define APP_CONFIG_FLASH_SECTOR   FLASH_SECTOR_7
#define APP_CONFIG_FLASH_ADDR     0x08060000u

static uint32_t crc32_calc(const uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFFu;

    for (uint32_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint32_t bit = 0; bit < 8; bit++)
        {
            crc = (crc & 1u) ? (crc >> 1) ^ 0xEDB88320u : (crc >> 1);
        }
    }

    return ~crc;
}

void app_config_set_defaults(app_config_t *cfg)
{
    memset(cfg, 0, sizeof(*cfg));
    cfg->magic = APP_CONFIG_MAGIC;
    cfg->version = APP_CONFIG_VERSION;
}

bool app_config_load(app_config_t *cfg)
{
    const app_config_t *flash_cfg = (const app_config_t *)APP_CONFIG_FLASH_ADDR;

    if (flash_cfg->magic == APP_CONFIG_MAGIC)
    {
        uint32_t crc = crc32_calc((const uint8_t *)flash_cfg, offsetof(app_config_t, crc32));
        if (crc == flash_cfg->crc32)
        {
            memcpy(cfg, flash_cfg, sizeof(*cfg));
            return true;
        }
    }

    app_config_set_defaults(cfg);
    return false;
}

bool app_config_save(const app_config_t *cfg)
{
    app_config_t tmp = *cfg;
    tmp.magic = APP_CONFIG_MAGIC;
    tmp.version = APP_CONFIG_VERSION;
    tmp.crc32 = crc32_calc((const uint8_t *)&tmp, offsetof(app_config_t, crc32));

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                            FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    FLASH_EraseInitTypeDef erase = {0};
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Sector = APP_CONFIG_FLASH_SECTOR;
    erase.NbSectors = 1;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3; /* питание платы 3.3В */

    uint32_t sector_error = 0;
    bool ok = (HAL_FLASHEx_Erase(&erase, &sector_error) == HAL_OK);

    if (ok)
    {
        const uint32_t *src = (const uint32_t *)&tmp;
        uint32_t words = sizeof(tmp) / sizeof(uint32_t);

        for (uint32_t i = 0; i < words; i++)
        {
            uint32_t addr = APP_CONFIG_FLASH_ADDR + i * sizeof(uint32_t);
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, src[i]) != HAL_OK)
            {
                ok = false;
                break;
            }
        }
    }

    HAL_FLASH_Lock();
    return ok;
}

static app_config_t g_config;

void app_config_init(void)
{
    if (!app_config_load(&g_config))
    {
        app_config_save(&g_config);
    }
}

const app_config_t *app_config_get(void)
{
    return &g_config;
}
