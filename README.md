# STM32F446_seed

## Плата

Компактная плата STM32F446RE от WeAct (форм-фактор Black Pill, 512 КиБ flash, кварц HSE 8 МГц).

Важно: это **не** Nucleo-F446RE — распиновка отличается:

- Пользовательский светодиод: **PB2** (на Nucleo — PA5), задаётся в `Inc/led_task.h`
- USB OTG_FS: PA11 (D-), PA12 (D+)

В Makefile используется линкер-скрипт от Nucleo-F446RE (`STM32F446RETX_FLASH.ld` из STM32CubeF4) — это осознанный выбор: скрипт зависит только от чипа (STM32F446RE: 512 КиБ flash, 128 КиБ RAM), а не от платы, поэтому шаблон от Nucleo подходит без изменений.

## Инструменты сборки

Проект собирается с помощью GNU toolchain для ARM Cortex-M3:

- `arm-none-eabi-gcc`

Makefile использует эти утилиты напрямую, поэтому они должны быть установлены и доступны в переменной `PATH`.

### Debian / Ubuntu / Debian 13

Установите GNU ARM embedded toolchain и OpenOCD для прошивки:

```bash
sudo apt update
sudo apt install gcc-arm-none-eabi gdb-multiarch openocd make
```

Если нужно только собрать прошивку, достаточно установить `gcc-arm-none-eabi`.

### Проверка инструментов

```bash
arm-none-eabi-gcc --version
gdb-multiarch --version
openocd --version
```

## Сборка

```bash
make
```

Альтернативно можно использовать xpm:

```bash
xpm run build
```

## Прошивка

```bash
make flash
```

Альтернативно можно использовать xpm:

```bash
xpm run flash
```

Цель `flash` использует `openocd` для записи прошивки в STM32 через ST-Link. Поэтому `openocd` должен быть установлен и доступен в `PATH`.

## Отладка

Для запуска серверной части отладки можно использовать OpenOCD напрямую:

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "gdb_port 3333; telnet_port 4444; init; reset halt;"
```

Альтернативно можно запустить тот же сервер через xpm:

```bash
xpm run monitor
```

Эта команда запускает OpenOCD в режиме отладочного сервера. В VS Code клиент подключается к этому серверу через конфигурацию `launch.json`.

## Подключение библиотек

В проекте используются следующие библиотеки:

- `libs/tinyusb` — USB стек TinyUSB
- `libs/STM32CubeF4` — HAL и CMSIS для STM32F4
- `libs/FreeRTOS` — ядро FreeRTOS

```bash
git submodule update --init --recursive
```

## Тестирование джойстика в Linux

```bash
sudo jstest /dev/input/js0
grep -i -A5 "STM32F446" /proc/bus/input/devices
```

## Виртуальный COM-порт и протокол команд

Помимо HID-джойстика устройство предоставляет виртуальный COM-порт (USB CDC) для управления контроллером с компьютера. В Linux порт появляется как `/dev/ttyACM0`.

Подключение:

```bash
picocom /dev/ttyACM0
```

Скорость указывать не нужно — для USB CDC она формальна. Выход из picocom: `Ctrl+A`, затем `Ctrl+X`.

Формат команд:

```
команда:параметры\n
```

Концом команды считается `\n` или `\r` (работает и с Windows-стилем `\r\n`). Каждый введённый символ эхлится обратно. Ответы завершаются `\r\n`.

Реализованные команды:

| Команда | Ответ | Назначение |
|---------|-------|------------|
| `Test`  | `Ok`  | Проверка работоспособности контроллера |

Неизвестная команда возвращает `Err:unknown command`.

Приём устроен так (`Src/cmd_task.c`): байты складываются в кольцевой буфер на 1024 байта, по символу конца строки задача-обработчик `CmdTask` пробуждается через уведомление FreeRTOS (`xTaskNotifyGive`), извлекает строку и выполняет команду. Новые команды добавляются в `cmd_process_line()`.

## Модернизация конфигурации джойстика

Для изменения структуры HID-джойстика редактируйте `Src/usb_descriptors.c` и `Inc/tusb_config.h`.

- `Src/usb_descriptors.c` отвечает за HID-репорт дескриптор: количество кнопок, осей и размер репорта.
- `Inc/tusb_config.h` задаёт размер буфера `CFG_TUD_HID_EP_BUFSIZE` и другие параметры TinyUSB.

После изменений пересоберите проект командой `make` и прошейте заново. Такие правки позволяют адаптировать устройство под другую комбинацию кнопок или данные контроллера.
