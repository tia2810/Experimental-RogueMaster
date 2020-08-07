extern "C" {
    #include "main.h"
    #include "cmsis_os.h"
    #include "gpio.h"
    #include <stdio.h>
}

#define pinMode app_gpio_init
#define digitalWrite app_gpio_write
#define digitalRead app_gpio_read
#define EEMEM
#define delayMicroseconds delay_us
#define delay osDelay
#define byte uint8_t

#define OUTPUT GpioModeOutput
#define INPUT GpioModeInput
#define LOW false
#define HIGH true
