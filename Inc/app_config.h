#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

#define APP_CONFIG_EXTRA_INFO_LEN   64

/* Конфигурация приложения, хранимая во flash. Значения этой структуры
   влияют на поведение исполняемого кода (см. app_config_load()). */
typedef struct
{
    uint32_t magic;
    uint32_t version;
    char     extra_info[APP_CONFIG_EXTRA_INFO_LEN];
    uint32_t crc32; /* считается по всем полям выше, должен идти последним */
} app_config_t;

/* Заполняет структуру значениями по умолчанию (используется, если во flash
   нет валидного конфига). */
void app_config_set_defaults(app_config_t *cfg);

/* Читает конфиг из flash. Если магия или CRC не совпали (чистая flash или
   первый запуск), возвращает значения по умолчанию и false. */
bool app_config_load(app_config_t *cfg);

/* Стирает сектор flash, отведённый под конфиг, и записывает cfg целиком.
   Возвращает false при ошибке стирания/программирования. */
bool app_config_save(const app_config_t *cfg);

/* Вызывать один раз при старте прошивки. Читает конфиг из flash в
   внутренний экземпляр; если он пуст/повреждён, сразу же записывает туда
   значения по умолчанию, чтобы в дальнейшем во flash всегда лежал
   валидный конфиг. */
void app_config_init(void);

/* Текущий конфиг (заполняется в app_config_init()). */
const app_config_t *app_config_get(void);

#endif /* APP_CONFIG_H_ */
