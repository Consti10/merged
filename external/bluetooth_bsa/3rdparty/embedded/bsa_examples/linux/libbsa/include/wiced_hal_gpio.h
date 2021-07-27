/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
*
* List of parameters and defined functions needed to access the
* General Purpose Input/Ouput (GPIO) driver.
*
*/

#ifndef __WICED_GPIO_H__
#define __WICED_GPIO_H__

#include "brcm_fw_types.h"

/**  \addtogroup GPIODriver GPIO
* \ingroup HardwareDrivers
* @{
* Defines a driver to facilitate interfacing with the GPIO pins.
*
* Use this driver to control the behavior of any desired pin, such as
* driving a 1 or a 0, or as part of other drivers such as controlling
* the chip-select (CS) line for the SPI driver.
*
*/

/******************************************************************************
*** Parameters.
***
*** The following parameters are used to configure the driver or define
*** return status. They are not modifiable.
******************************************************************************/

/// Pin output config
typedef enum
{
    GPIO_PIN_OUTPUT_LOW,
    GPIO_PIN_OUTPUT_HIGH,
} tGPIO_PIN_OUTPUT_CONFIG;

/// The following enum defines the constant values for the GPIO driver and
/// associated GPIOs, each of which has a set of configuration signals.
enum
{
    /// Trigger Type
    /// GPIO configuration bit 0, Interrupt type defines
    GPIO_EDGE_TRIGGER_MASK       = 0x0001,
    GPIO_EDGE_TRIGGER            = 0x0001,
    GPIO_LEVEL_TRIGGER           = 0x0000,

    /// Negative Edge Triggering
    /// GPIO configuration bit 1, Interrupt polarity defines
    GPIO_TRIGGER_POLARITY_MASK   = 0x0002,
    GPIO_TRIGGER_NEG             = 0x0002,

    /// Dual Edge Triggering
    /// GPIO configuration bit 2, single/dual edge defines
    GPIO_DUAL_EDGE_TRIGGER_MASK  = 0x0004,
    GPIO_EDGE_TRIGGER_BOTH       = 0x0004,
    GPIO_EDGE_TRIGGER_SINGLE     = 0x0000,


    /// Interrupt Enable
    /// GPIO configuration bit 3, interrupt enable/disable defines
    GPIO_INTERRUPT_ENABLE_MASK   = 0x0008,
    GPIO_INTERRUPT_ENABLE        = 0x0008,
    GPIO_INTERRUPT_DISABLE       = 0x0000,


    /// Interrupt Config
    /// GPIO configuration bit 0:3, Summary of Interrupt enabling type
    GPIO_EN_INT_MASK             = GPIO_EDGE_TRIGGER_MASK | GPIO_TRIGGER_POLARITY_MASK | GPIO_DUAL_EDGE_TRIGGER_MASK | GPIO_INTERRUPT_ENABLE_MASK,
    GPIO_EN_INT_LEVEL_HIGH       = GPIO_INTERRUPT_ENABLE | GPIO_LEVEL_TRIGGER,
    GPIO_EN_INT_LEVEL_LOW        = GPIO_INTERRUPT_ENABLE | GPIO_LEVEL_TRIGGER | GPIO_TRIGGER_NEG,
    GPIO_EN_INT_RISING_EDGE      = GPIO_INTERRUPT_ENABLE | GPIO_EDGE_TRIGGER,
    GPIO_EN_INT_FALLING_EDGE     = GPIO_INTERRUPT_ENABLE | GPIO_EDGE_TRIGGER | GPIO_TRIGGER_NEG,
    GPIO_EN_INT_BOTH_EDGE        = GPIO_INTERRUPT_ENABLE | GPIO_EDGE_TRIGGER | GPIO_EDGE_TRIGGER_BOTH,


    /// GPIO Output Buffer Control and Output Value Multiplexing Control
    /// GPIO configuration bit 4:5, and 14 output enable control and
    /// muxing control
    GPIO_INPUT_ENABLE            = 0x0000,
    GPIO_OUTPUT_DISABLE          = 0x0000,
    GPIO_OUTPUT_ENABLE           = 0x4000,
    GPIO_KS_OUTPUT_ENABLE        = 0x0001, //Keyscan Output enable
    GPIO_OUTPUT_FN_SEL_MASK      = 0x0000,
    GPIO_OUTPUT_FN_SEL_SHIFT     = 0,


    /// Global Input Disable
    /// GPIO configuration bit 6, "Global_input_disable" Disable bit
    /// This bit when set to "1" , P0 input_disable signal will control
    /// ALL GPIOs. Default value (after power up or a reset event) is "0".
    GPIO_GLOBAL_INPUT_ENABLE     = 0x0000,
    GPIO_GLOBAL_INPUT_DISABLE    = 0x0040,


    /// Pull-up/Pull-down
    /// GPIO configuration bit 9 and bit 10, pull-up and pull-down enable
    /// Default value is [0,0]--means no pull resistor.
    GPIO_PULL_UP_DOWN_NONE       = 0x0000,   //[0,0]
    GPIO_PULL_UP                 = 0x0400,   //[1,0]
    GPIO_PULL_DOWN               = 0x0200,   //[0,1]
    GPIO_INPUT_DISABLE           = 0x0600,   //[1,1] // input disables the GPIO

    /// Drive Strength
    /// GPIO configuration bit 11
    GPIO_DRIVE_SEL_MASK         = 0x0800,
    GPIO_DRIVE_SEL_LOWEST       = 0x0000,  // 2mA @ 1.8V
    GPIO_DRIVE_SEL_MIDDLE_0     = 0x0000,  // 4mA @ 3.3v
    GPIO_DRIVE_SEL_MIDDLE_1     = 0x0800,  // 4mA @ 1.8v
    GPIO_DRIVE_SEL_HIGHEST      = 0x0800,  // 8mA @ 3.3v


    /// Input Hysteresis
    /// GPIO configuration bit 13, hysteresis control
    GPIO_HYSTERESIS_MASK         = 0x2000,
    GPIO_HYSTERESIS_ON           = 0x2000,
    GPIO_HYSTERESIS_OFF          = 0x0000,
};

//! GPIO Numbers : last 8 are ARM GPIOs and rest are LHL GPIOs
typedef enum
{
    WICED_P00 = 0,  /**< LHL GPIO 0 */
    WICED_P01,      /**< LHL GPIO 1 */
    WICED_P02,      /**< LHL GPIO 2 */
    WICED_P03,      /**< LHL GPIO 3 */
    WICED_P04,      /**< LHL GPIO 4 */
    WICED_P05,      /**< LHL GPIO 5 */
    WICED_P06,      /**< LHL GPIO 6 */
    WICED_P07,      /**< LHL GPIO 7 */
    WICED_P08,      /**< LHL GPIO 8 */
    WICED_P09,      /**< LHL GPIO 9 */
    WICED_P10,      /**< LHL GPIO 10 */
    WICED_P11,      /**< LHL GPIO 11 */
    WICED_P12,      /**< LHL GPIO 12 */
    WICED_P13,      /**< LHL GPIO 13 */
    WICED_P14,      /**< LHL GPIO 14 */
    WICED_P15,      /**< LHL GPIO 15 */
    WICED_P16,      /**< LHL GPIO 16 */
    WICED_P17,      /**< LHL GPIO 17 */
    WICED_P18,      /**< LHL GPIO 18 */
    WICED_P19,      /**< LHL GPIO 19 */
    WICED_P20,      /**< LHL GPIO 20 */
    WICED_P21,      /**< LHL GPIO 21 */
    WICED_P22,      /**< LHL GPIO 22 */
    WICED_P23,      /**< LHL GPIO 23 */
    WICED_P24,      /**< LHL GPIO 24 */
    WICED_P25,      /**< LHL GPIO 25 */
    WICED_P26,      /**< LHL GPIO 26 */
    WICED_P27,      /**< LHL GPIO 27 */
    WICED_P28,      /**< LHL GPIO 28 */
    WICED_P29,      /**< LHL GPIO 29 */
    WICED_P30,      /**< LHL GPIO 30 */
    WICED_P31,      /**< LHL GPIO 31 */
    WICED_P32,      /**< LHL GPIO 32 */
    WICED_P33,      /**< LHL GPIO 33 */
    WICED_P34,      /**< LHL GPIO 34 */
    WICED_P35,      /**< LHL GPIO 35 */
    WICED_P36,      /**< LHL GPIO 36 */
    WICED_P37,      /**< LHL GPIO 37 */
    WICED_P38,      /**< LHL GPIO 38 */
    WICED_P39,      /**< LHL GPIO 39 */
    /* GPIO_00 to GPIO_07 are ARM GPIOs */
    WICED_GPIO_00,  /**< ARM GPIO 0 - 40 */
    WICED_GPIO_01,  /**< ARM GPIO 1 - 41 */
    WICED_GPIO_02,  /**< ARM GPIO 2 - 42 */
    WICED_GPIO_03,  /**< ARM GPIO 3 - 43 */
    WICED_GPIO_04,  /**< ARM GPIO 4 - 44 */
    WICED_GPIO_05,  /**< ARM GPIO 5 - 45 */
    WICED_GPIO_06,  /**< ARM GPIO 6 - 46 */
    WICED_GPIO_07,  /**< ARM GPIO 7 - 47 */
    MAX_NUM_OF_GPIO
}wiced_bt_gpio_numbers_t;

//! possible functions to be brought out through LHL GPIO's
typedef enum
{
    WICED_GPIO = 0,           /**< LHL GPIO (default functionality) */
    WICED_I2C_1_SCL,          /**< I2C 1 Clock */
    WICED_I2C_1_SDA,          /**< I2C 1 Data */
    RESERVED_1,               /**< Reserved for Future */
    RESERVED_2,               /**< Reserved for Future */
    WICED_SPI_1_CLK,          /**< SPI 1 Clock */
    WICED_SPI_1_CS,           /**< SPI 1 Chip Select */
    WICED_SPI_1_MOSI,         /**< SPI 1 Master Out Slave In */
    WICED_SPI_1_MISO,         /**< SPI 1 Slave In Master Out */
    RESERVED_3,               /**< Reserved for Future */
    RESERVED_4,               /**< Reserved for Future */
    RESERVED_5,               /**< Reserved for Future */
    RESERVED_6,               /**< Reserved for Future */
    WICED_SPI_2_CLK,          /**< SPI 2 Clock */
    WICED_SPI_2_CS,           /**< SPI 2 Chip Select */
    WICED_SPI_2_MOSI,         /**< SPI 2 Master Out Slave In */
    WICED_SPI_2_MISO,         /**< SPI 2 Slave In Master Out */
    RESERVED_7,               /**< Reserved for Future */
    RESERVED_8,               /**< Reserved for Future */
    RESERVED_9,               /**< Reserved for Future */
    RESERVED_10,               /**< Reserved for Future */
    RESERVED_11,               /**< Reserved for Future */
    RESERVED_12,               /**< Reserved for Future */
    RESERVED_13,               /**< Reserved for Future */
    RESERVED_14,               /**< Reserved for Future */
    RESERVED_15,               /**< Reserved for Future */
    WICED_SWDCK,              /**< SWD Clock */
    WICED_SWDIO,              /**< SWD Data */
    WICED_UART_1_TXD,         /**< HCI UART TX */
    WICED_UART_1_RXD,         /**< HCI UART RX */
    WICED_UART_1_CTS,         /**< HCI UART CTS */
    WICED_UART_1_RTS,         /**< HCI UART RTS */
    WICED_UART_2_TXD,         /**< PUART TX */
    WICED_UART_2_RXD,         /**< PUART RX */
    WICED_UART_2_CTS,         /**< PUART CTS */
    WICED_UART_2_RTS,         /**< PUART RTS */
    RESERVED_16,              /**< Reserved for Future */
    RESERVED_17,              /**< Reserved for Future */
    RESERVED_18,              /**< Reserved for Future */
    RESERVED_19,              /**< Reserved for Future */
    WICED_I2S_1_IN,           /**< I2S 1 DATA IN */
    WICED_I2S_1_OUT,          /**< I2S 1 DATA OUT */
    WICED_I2S_1_WS,           /**< I2S 1 Word Selectt*/
    WICED_I2S_1_CLK,          /**< I2S 1 Clock */
    RESERVED_20,              /**< Reserved for Future */
    RESERVED_21,              /**< Reserved for Future */
    RESERVED_22,              /**< Reserved for Future */
    RESERVED_23,              /**< Reserved for Future */
    RESERVED_24,              /**< Reserved for Future */
    RESERVED_25,              /**< Reserved for Future */
    RESERVED_26,              /**< Reserved for Future */
    RESERVED_27,              /**< Reserved for Future */
    RESERVED_28,              /**< Reserved for Future */
    RESERVED_29,              /**< Reserved for Future */
    RESERVED_30,              /**< Reserved for Future */
    RESERVED_31,              /**< Reserved for Future */
    WICED_PCM_CLK_I2S_CLK,    /**< PCM Clock/I2S 2 Clock */
    WICED_PCM_SYNC_I2S_WS,    /**< PCM Sync/I2S 2 Word Select */
    WICED_PCM_OUT_I2S_DO,     /**< PCM Data Out/ I2S Data Out */
    WICED_PCM_IN_I2S_DI,      /**< PCM Data In/ I2S Data In */
    WICED_GCI_SECI_IN,        /**< GCI */
    WICED_GCI_SECI_OUT,       /**< GCI */
    RESERVED_32,               /**< Reserved for Future */
    RESERVED_33,               /**< Reserved for Future */
    WICED_ACLK_0,
    WICED_ACLK_1,
    WICED_KSO0,
    WICED_KSO1,
    WICED_KSO2,
    WICED_KSO3,
    WICED_KSO4,
    WICED_KSO5,
    WICED_KSO6,
    WICED_KSO7,
    WICED_KSO8,
    WICED_KSO9,
    WICED_KSO10,
    WICED_KSO11,
    WICED_KSO12,
    WICED_KSO13,
    WICED_KSO14,
    WICED_KSO15,
    WICED_KSO16,
    WICED_KSO17,
    WICED_KSO18,
    WICED_KSO19,
    WICED_TX_PD,
    WICED_TX_PD_TILDA,
    WICED_PA_RAMP,
    WICED_TX_FSM,             /**< TX FSM */
    WICED_RX_FSM,             /**< RX FSM */
    WICED_TX_PU,              /**< TX PU */
    WICED_RX_PU,              /**< RX PU */
    WICED_EPA_CTX,
    WICED_EPA_CRX,
    WICED_EPA_CSD,
    WICED_SPI_1_CLK_SLAVE,
    WICED_SPI_1_CS_SLAVE,
    WICED_SPI_1_MOSI_SLAVE,
    WICED_SPI_1_MISO_SLAVE,
    WICED_PWM0,
    WICED_PWM1,
    WICED_PWM2,
    WICED_PWM3,
    WICED_PWM4,
    WICED_PWM5,
    WICED_UNAVAILABLE = 0xFF  /**< Invalid functionality for error check */
} wiced_bt_gpio_function_t;

//! Possible return values from wiced_hal_gpio_select_function(...), Callers only need to check for the
//! GPIO_FAILURE case since any other status means success
typedef enum GPIO_STATUS_e
{
    GPIO_FAILURE, //!< The requested pin and function mapping is not supported by hardware
    GPIO_SUCCESS, //!< The requested pin and function mapping is complete, The pin was previously not used and the function was previously not mapped
    GPIO_REMAPPED,//!< The requested pin and function mapping is complete, The pin was previously used by another function, that function was disabled and the new function applied
    GPIO_MOVED    //!< The requested pin and function mapping is complete, The requested function was already mapped to a different pin, that pin was disabled and the function moved to the new pin
} wiced_bt_gpio_select_status_t;

/*! GPIO Active Level HIGH */
#define WICED_GPIO_ACTIVE_HIGH      1

/*! GPIO Active Level LOW */
#define WICED_GPIO_ACTIVE_LOW       0

/******************************************************************************
*** Function prototypes and defines.
******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// Initializes the GPIO driver and its private values.
/// Also programs all GPIOs to be ready for use. This must be invoked before
/// accessing any GPIO driver services, typically at boot.
/// This is independent of other drivers and must be one of the first to
/// be initialized.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_gpio_init(void);

///////////////////////////////////////////////////////////////////////////////
/// Convert a given schematic pin number (i.e. P5) to the internal port/pin
/// representation used by various other drivers such as PSPI and PUART. There
/// could be up to 40 GPIO pins on a board so the numbering is P0 - P39.
///
/// \param pin - The desired pin number from the schematic. Ex: P<pin>
///
/// \return Returns the port and pin representation encoded into a byte.
/// Value is 0xFF if the input parameter is out of range.
///////////////////////////////////////////////////////////////////////////////
UINT8 wiced_hal_gpio_pin_to_port_pin(UINT8 pin);

///////////////////////////////////////////////////////////////////////////////
/// Configures a GPIO pin.
///
/// Note that the GPIO output value is programmed before
/// the GPIO is configured. This ensures that the GPIO will activate with the
/// correct external value. Also note that the output value is always
/// written to the output register regardless of whether the GPIO is configured
/// as input or output.
///
/// Enabling interrupts here isn't sufficient; you also want to
/// register with registerForMask().
///
/// All input parameter values must be in range or the function will
/// have no effect.
///
/// \param pin    - pin id (0-39).
/// \param config - Gpio configuration. See Parameters section above for
/// possible values.
///
/// For example, to enable interrupts for all edges, with a pull-down,
/// you could use:
/// \verbatim
/// GPIO_EDGE_TRIGGER | GPIO_EDGE_TRIGGER_BOTH |
/// GPIO_INTERRUPT_ENABLE_MASK | GPIO_PULL_DOWN_MASK
/// \endverbatim
///
/// \param outputVal - output value.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_gpio_configure_pin(UINT32 pin, UINT32 config,
                                                UINT32 outputVal);

///////////////////////////////////////////////////////////////////////////////
/// Retrieve the current configuration of the specified pin.
///
/// All input parameter values must be in range or the function will
/// have no effect.
///
/// \param pin  - pin id (0-39).
///
/// \return Configuration of specified pin.
///////////////////////////////////////////////////////////////////////////////
UINT16 wiced_hal_gpio_get_pin_config(UINT32 pin);


///////////////////////////////////////////////////////////////////////////////
/// Sets the output value of a pin. Note that the pin must already be
/// configured as output or this will have no visible effect.
///
/// All input parameter values must be in range or the function will
/// have no effect.
///
/// \param pin  - pin id (0-39).
/// \param val  - the output value
///    -   0        - pin will be set to 0
///    -   non-zero - pin will be set to 1
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_gpio_set_pin_output(UINT32 pin, UINT32 val);


///////////////////////////////////////////////////////////////////////////////
/// Get the programmed output value of a pin. Note that this does not return
/// the current pin value. It returns the value that the pin would attempt to
/// drive if it is configured as output.
///
/// \param pin  - pin id (0-39).
///
/// \return Returns 1 if output port of the pin is configured 1, likewise
/// for 0. Returns 0xFF if the input parameters are out of range.
///////////////////////////////////////////////////////////////////////////////
UINT32 wiced_hal_gpio_get_pin_output(UINT32 pin);

///////////////////////////////////////////////////////////////////////////////
/// Read the current value at a pin. Note that for this to be valid, the pin
/// must be configured with input enabled.
///
/// \param pin  - pin id (0-39).
///
/// \return  Returns 1 if the pin is high, 0 if the pin is low, and 0xFF if
/// the input parameters are out of range.
///////////////////////////////////////////////////////////////////////////////
UINT32 wiced_hal_gpio_get_pin_input_status(UINT32 pin);

///////////////////////////////////////////////////////////////////////////////
/// Get the interrupt status of a pin
///
/// \param pin  - pin id (0-39).
///
/// \return  Returns 1 if an interrupt (programmed edge) was detected at the
/// pin, 0 if not. Returns 0xFF if the input parameters are out of range.
///////////////////////////////////////////////////////////////////////////////
UINT32 wiced_hal_gpio_get_pin_interrupt_status(UINT32 pin);

///////////////////////////////////////////////////////////////////////////////
/// Register a function for notification of changes to a pin (via interrupt).
///
/// Note that this is independent of configuring the pin for
/// interrupts; a call to configurePin() is also required.
///
/// Also note that once registered, you CANNOT UNREGISTER; registration is
/// meant to be a startup activity. To stop receiving notifications,
/// re-configure the pin and disable the interrupt using
/// configurePin().
///
/// Example:
///
/// \verbatim
/// void gpioIntTestCb(void *data);
///
/// UINT16 gpioIntMasks[GPIO_NUM_PORTS] = {0};
/// wiced_hal_gpio_configurePin(27,
///     (GPIO_INPUT_ENABLE|GPIO_PULL_DOWN|GPIO_EN_INT_RISING_EDGE),
///      GPIO_PIN_OUTPUT_LOW);
/// wiced_hal_gpio_register_pin_for_interrupt(WICED_P01, gpio_test_cb, NULL);
/// \endverbatim
///
/// \param masks - Points to an array of NUM_PORTS.
///
/// \param userfn - Points to the function to call when the interrupt
/// comes .Below is the description of the arguments received by the call back.
///
///     void* user_data  - User data provided when interrupt is being registered
///                        using wiced_hal_gpio_register_pin_for_interrupt(...)
///
///     uint8_t value    - Number of the pin causing the interrupt
///
/// (!) Note that the function does not need to clear the interrupt
/// status; this will be done automatically.
///
/// (!) Note that the function will be called ONCE per interrupt, not once per
/// pin (this makes a difference if multiple pins toggle at the same time).
///
/// \param userdata - Will be passed back to userfn as-is. Typically NULL.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_gpio_register_pin_for_interrupt(UINT16 pin,
                               void (*userfn)(void*, UINT8), void* userdata);


///////////////////////////////////////////////////////////////////////////////
/// Clear the interrupt status of a pin manually.
///
/// \param pin  - pin id (0-39).
///
/// \return  Returns 1 if successful, 0xFF if the input parameters are
/// out of range.
///////////////////////////////////////////////////////////////////////////////
UINT32 wiced_hal_gpio_clear_pin_interrupt_status(UINT32 pin);

///////////////////////////////////////////////////////////////////////////////
/// Configures all gpios to be INPUT DISABLED.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_gpio_disable_all_inputs(void);

/////////////////////////////////////////////////////////////////////////////////////
///  save the LHL GPIO configuration in AON memeory to reenforce in slimboot
///  This is a must if we want to wake up in SDS by external LHL GPIO interrupts
///  \param pin - pin id (0-39)
///  \param config - Gpio configuration.
///  \result: TRUE - successful save/update; FALSE - run out of entries, not able to save or invalid pin
/////////////////////////////////////////////////////////////////////////////////////
BOOL32 wiced_hal_gpio_slimboot_reenforce_cfg(UINT8 pin, UINT16 config);

#define wiced_hal_gpio_slimboot_reenforce_outputpin_value()
/**
 *  \brief Configure a GPIO pin to have the chosen functionality.
 *
 *  \param [in] pin (0-39) (Only LHL GPIO supported for now)
 *  \param [in] function Chosen functionality (refer wiced_bt_gpio_function_t for valid options)
 *  \return refer wiced_bt_gpio_select_status_t for possible return status.
 *
 */
wiced_bt_gpio_select_status_t wiced_hal_gpio_select_function(wiced_bt_gpio_numbers_t pin, wiced_bt_gpio_function_t function);

/* @} */

#endif // __WICED_GPIO_H__
