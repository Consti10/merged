/*
 * $ Copyright Cypress Semiconductor $
 */

#pragma once

#include "wiced.h"
#include "wiced_hal_gpio.h"

//! Number of LEDs available on the platform.
// LED_2 is pre-configured, others are configurable via SuperMux
typedef enum
{
    WICED_PLATFORM_LED_2,  //!< LED 2 (default, WICED_P26)
    WICED_PLATFORM_LED_1,  //!< LED 1
    WICED_PLATFORM_LED_3,  //!< LED 3
    WICED_PLATFORM_LED_4,  //!< LED 4
    WICED_PLATFORM_LED_5,  //!< LED 5
    WICED_PLATFORM_LED_6,  //!< LED 6
    WICED_PLATFORM_LED_7,  //!< LED 7
    WICED_PLATFORM_LED_8,  //!< LED 8
    WICED_PLATFORM_LED_MAX //!< Max LED for error check
} wiced_platform_led_number_t;

//! Number of Buttons available on the platform.
// BUTTON_1 is pre-configured, others are configurable via SuperMux
typedef enum
{
    WICED_PLATFORM_BUTTON_1,  //!< BUTTON 1
    WICED_PLATFORM_BUTTON_2,  //!< BUTTON 2
    WICED_PLATFORM_BUTTON_3,  //!< BUTTON 3
    WICED_PLATFORM_BUTTON_4,  //!< BUTTON 4
    WICED_PLATFORM_BUTTON_5,  //!< BUTTON 5
    WICED_PLATFORM_BUTTON_6,  //!< BUTTON 6
    WICED_PLATFORM_BUTTON_7,  //!< BUTTON 7
    WICED_PLATFORM_BUTTON_8,  //!< BUTTON 8
    WICED_PLATFORM_BUTTON_MAX //!< Max button for error check
} wiced_platform_button_number_t;


//! Number of GPIO available on the platform.
// Configurable via SuperMux
typedef enum
{
    WICED_PLATFORM_GPIO_1,  //!< GPIO 1
    WICED_PLATFORM_GPIO_2,  //!< GPIO 2
    WICED_PLATFORM_GPIO_3,  //!< GPIO 3
    WICED_PLATFORM_GPIO_4,  //!< GPIO 4
    WICED_PLATFORM_GPIO_5,  //!< GPIO 5
    WICED_PLATFORM_GPIO_6,  //!< GPIO 6
    WICED_PLATFORM_GPIO_7,  //!< GPIO 7
    WICED_PLATFORM_GPIO_8,  //!< GPIO 8
    WICED_PLATFORM_GPIO_MAX //!< Max GPIO for error check
} wiced_platform_gpio_number_t;

//! Possible interrupt configuration for platform buttons
typedef enum
{
    WICED_PLATFORM_BUTTON_BOTH_EDGE = GPIO_EN_INT_BOTH_EDGE,   //!< indicates that app. should receive interrupt on both edges
    WICED_PLATFORM_BUTTON_RISING_EDGE = GPIO_EN_INT_RISING_EDGE, //!< indicates that app. should receive interrupt only for rising edge
    WICED_PLATFORM_BUTTON_FALLING_EDGE = GPIO_EN_INT_FALLING_EDGE,//!< indicates that app. should receive interrupt only for falling edge
} wiced_platform_button_interrupt_edge_t;

//! List of pins available on the platform
enum wiced_platform_pins
{
    PLATFORM_GPIO_0,
    PLATFORM_GPIO_1,
    PLATFORM_GPIO_2,
    PLATFORM_GPIO_3,
    PLATFORM_GPIO_4,
    PLATFORM_GPIO_5,
    PLATFORM_GPIO_6,
    PLATFORM_GPIO_7,
    PLATFORM_GPIO_8,
    PLATFORM_GPIO_9,
    PLATFORM_GPIO_10,
    PLATFORM_GPIO_11,
    PLATFORM_GPIO_12,
    PLATFORM_GPIO_13,
    PLATFORM_GPIO_14,
    PLATFORM_GPIO_15,
    PLATFORM_GPIO_MAX_PINS
};

/**
 * configuration for the platform GPIOs
 */
typedef struct
{
    wiced_bt_gpio_numbers_t gpio_pin; /**< WICED GPIO pin */
    wiced_bt_gpio_function_t functionality; /**< chosen functionality for the pin */
}
wiced_platform_gpio_t;

/**
 * Configuration for platform LEDs
 */
typedef struct
{
    wiced_bt_gpio_numbers_t* gpio; /**< WICED GPIO pin */
    uint32_t config; /**< configuration like GPIO_PULL_DOWN,GPIO_PULL_UP etc., */
    uint32_t default_state; /**< GPIO_PIN_OUTPUT_HIGH/GPIO_PIN_OUTPUT_LOW */
}
wiced_platform_led_config_t;

/**
 * Configuration for platform Buttons
 */
typedef struct
{
    wiced_bt_gpio_numbers_t* gpio; /**< WICED GPIO pin */
    uint32_t config; /**< configuration like GPIO_PULL_DOWN,GPIO_PULL_UP etc., interrupt is configured through wiced_platform_register_button_callback(...) */
    uint32_t default_state; /**< GPIO_PIN_OUTPUT_HIGH/GPIO_PIN_OUTPUT_LOW */
    uint32_t button_pressed_value; /**< Button pressed value */
}
wiced_platform_button_config_t;

/**
 * Configuration for platform GPIOs
 */
typedef struct
{
    wiced_bt_gpio_numbers_t* gpio; /**< WICED GPIO pin */
    uint32_t config; /**< configuration like GPIO_PULL_DOWN,GPIO_PULL_UP etc., interrupt is configured through wiced_platform_register_button_callback(...) */
    uint32_t default_state; /**< GPIO_PIN_OUTPUT_HIGH/GPIO_PIN_OUTPUT_LOW */
}
wiced_platform_gpio_config_t;


/*! pin for button 1 */
#define WICED_GPIO_PIN_BUTTON_1       WICED_P01

/*! configuration settings for button, x can be GPIO_EN_INT_RISING_EDGE or GPIO_EN_INT_FALLING_EDGE or GPIO_EN_INT_BOTH_EDGE */
#define WICED_GPIO_BUTTON_SETTINGS(x)                       ( GPIO_INPUT_ENABLE | GPIO_PULL_DOWN | x )

/*! pin for LED 1 */
#define WICED_GPIO_PIN_LED_1     WICED_P25   /* CYBT_483039_EVAL does not have LED 1 on board. Please connect to external LED circuit */
/*! pin for LED 2 */
#define WICED_GPIO_PIN_LED_2     WICED_P26

/*! Max. supported baudrate by this platform */
#define HCI_UART_MAX_BAUD       1000000

/*! default baud rate is 115200, it is suggested to modify this value to 1000000 when traces are enabled to avoid latency */
#define HCI_UART_DEFAULT_BAUD   115200

/* utility functions */
void wiced_platform_register_button_callback(wiced_platform_button_number_t button, void (*userfn)(void*, UINT8), void* userdata, wiced_platform_button_interrupt_edge_t trigger_edge);
void wiced_platform_init(void);
uint32_t wiced_platform_get_button_pressed_value(wiced_platform_button_number_t button);
