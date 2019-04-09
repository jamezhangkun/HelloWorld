#ifndef __AW9120_H
#define __AW9120_H
#ifdef __cplusplus
extern "C"{
#endif

#include "i2c_driver.h"
#include "xatypes.h"

typedef enum PROGRAM_MODE
{
    PROGRAM_VIA_I2C=0x0000,
    PROGRAM_RELOAD_RUN=0x0001,
    PROGRAM_RUN=0x0002,

    PROGRAM_UNDEFINED=0x0003

}PROGRAM_MODE_E;

typedef enum PROGRAM_RUN_MODE
{
    HOLD_MODE=0x0000,
    STEP_MODE=0x0001,
    RUN_MODE=0x0002,

    REPEAT_MODE=0x0003

}PROGRAM_RUN_MODE_E;

typedef enum CURRENT_MODE
{
    CURRENT_0MA=0x0000,
    CURRENT_3P5MA=0x0001,
    CURRENT_7P0MA=0x0002,
    CURRENT_10P5MA=0x0003,
    CURRENT_14MA=0x0004,
    CURRENT_17P5MA=0x0005,
    CURRENT_21P0MA=0x0006,
    CURRENT_24P5MA=0x0007


}CURRENT_MODE_E;

typedef enum LED_LIGHTING_MODE
{
    SINGLE_BREATH_MODE=0x00,
    ALL_LED_LIGHTING=0x01,
    ALL_LED_OFF,
    CIRCLE_LIGHT,
    CIRCLE_LIGHT_FADE_OUT,

    LED_BAND_BREATH,


    LED_IDLE_MODE
}LED_LIGHTING_MODE_E;

/*the following data structure defined for the cmd sending to chip by extern i2c*/
/*SETSTEPTMRI Pre Ch Im     1 *0 *0   *Ch[4:0] *Pre *- *Im[5:0]
*SETPWMI Ch Im              1 *0 *1   *Ch[4:0] *Im[7:0]
*RAMPI Dir Ch Im            1 *1 *Dir *Ch[4:0] *Im[7:0]*/
/*Set the RAMP step time*/
typedef union
{
    struct
    {
        uint16_t light_bit:6;      /*Im: RAMP step time = (Im +1)*Pre, 0~255*/
        uint16_t reserved_bit:1;
        uint16_t pre_bit:1;     /*Pre: basic unit of time, 0: 0.5ms; 1: 16ms*/
        uint16_t channel_bit:5; /*1-20channels*/
        uint16_t flag_bit:3;    /*100*/

    };
    uint16_t register_cmd1;
}SETSTEPTMRI_CMD_U1;

/*Set the Fade-in/Fade-out for specified steps*/
typedef union
{
    struct
    {
        uint16_t light_bit:8;       /*Im: the steps of Fade-in/Fade-out*/
        uint16_t channel_bit:5; /*1-20channels*/
        uint16_t dir_bit:1;     /*Dir: 1: Fade-in; 0: Fade-out*/
        uint16_t flag_bit:2;    /*11*/
    };
    uint16_t register_cmd2;
}RAMPI_CMD_U2;

/*Set the PWM brightness level */
typedef union
{
    struct
    {
        uint16_t light_bit:8;       /*Im: Brightness level, 0~255*/
        uint16_t channel_bit:5; /*1-20channels*/
        uint16_t flag_bit:3;   /*101*/
    };
    uint16_t register_cmd3;
}SETPWM_CMD_U3;

typedef union
{
    struct
    {
        uint16_t time_set_bit:10;       /*time set max=1023*/
        uint16_t pre_bit:1;     /*0:0.5ms; 1:16ms*/
        uint16_t flag_bit:5;   /*00111*/
    };
    uint16_t register_cmd4;
}WAITI_CMD_U4;


typedef struct CMD_REGISTER
{
    SETSTEPTMRI_CMD_U1 set_step_time;

    RAMPI_CMD_U2 set_fade_in_mode;
    RAMPI_CMD_U2 set_fade_out_mode;

    SETPWM_CMD_U3 set_pwm;
    WAITI_CMD_U4 set_wait_time;

    LED_LIGHTING_MODE_E led_effect_mode;

}CMD_REGISTER_T;

/*i2c slave address
*/
#define AW9120_I2C_ADDRESS  (0x2C)

/*chip id*/
#define AW9120_CHIP_ID        (0xB223)

/*number of led*/
#define LED_BAND_NUMBERS             (12)            /*12 led for led band */
#define ALL_LED_NUMBERS         (LED_BAND_NUMBERS+1) /*1 LED status  12 led band*/

/*reg address*/
/*Chip ID: 0xB223
Software Reset: write 0x55AA to IDRST, reset the whole device.*/
#define AW9120_CHIPID_RESET_REG         (0x00)
/*enable disable LED driver*/
#define AW9120_GLOBAL_CTRL_REG          (0x01)
#define AW9120_LED_DRIVER_ENABLE_REG1   (0x50) /*BIT0~BIT11*/
#define AW9120_LED_DRIVER_ENABLE_REG2   (0x51) /*BIT0~BIT7*/

#define AW9120_LED_EFFECT_CONFIG_REG    (0x52)
#define AW9120_PROGRAM_MODE_REG         (0x53)
#define AW9120_PROGRAM_RUN_MODE_REG     (0x54)

#define AW9120_CTRL_SOURCE_SELE_REG1    (0x55) /*select controlled by external or SRAM*/
#define AW9120_CTRL_SOURCE_SELE_REG2    (0x56)

#define AW9120_MAX_OUTPUT_CURRENT_REG1  (0x57)/*1-4CHANNEL*/
#define AW9120_MAX_OUTPUT_CURRENT_REG2  (0x58)/*5-8CHANNEL*/
#define AW9120_MAX_OUTPUT_CURRENT_REG3  (0x59)/*9-12CHANNEL*/
#define AW9120_MAX_OUTPUT_CURRENT_REG4  (0x5A)/*13-16CHANNEL*/
#define AW9120_MAX_OUTPUT_CURRENT_REG5  (0x5B)/*17-20CHANNEL*/

#define AW9120_PROGRAM_START_ADDRESS    (0x5F)

#define Aw9120_LED_MD_REG                (0x61)

#define AW9120_PROGRAM_ADDRESS_REG      (0x7E)  /*sram program address*/
#define AW9120_W_DATA_REG               (0x7F)  /*sram program data*/
#define AW9120_WRITE_PROTECTION_REG      (0x7D)



/*function*/
uint16_t Aw9120ReadChipId(void);
void Aw9120EnableDisableLedDriver(BOOL led_enable);
void Aw9120EnableLedChannels(void);
void Aw9120SelectOutputCurrent(CURRENT_MODE_E current_mode);
void Aw9120ProgramModeConfig(PROGRAM_MODE_E program_mode);
void Aw9120ProgramRunModeConfig(PROGRAM_RUN_MODE_E program_run_mode);
void Aw9120ProgramLoadingData(uint16_t load_data);
void Aw9120ProgramLoadingAddress(uint16_t program_load_add);

void Aw9120ControlSourceSelectI2cOrSram(BOOL is_i2c_control);
void Aw9120LedCmdSend(uint16_t cmd_data);


void Aw9120LedSetStepTime(uint8_t channels,uint8_t led_light,CMD_REGISTER_T *para_cmd_register);
void Aw9120LedSetPwm(uint8_t channels,uint8_t led_light,CMD_REGISTER_T *para_cmd_register);
void Aw9120LedSetFadeMode(uint8_t channels,uint8_t led_light,BOOL is_fade_in,CMD_REGISTER_T *para_cmd_register);
void Aw9120InitStructure(CMD_REGISTER_T *para_cmd_register);
void Aw9120Init(void);
void Aw9120LedLightingThreadHandler(const void *arg);
void Aw9120SetLedEffectMode(CMD_REGISTER_T *para_cmd_register,LED_LIGHTING_MODE_E led_effec_mode);

/*test*/
void Aw9120LedEffectTest(void);
void Aw9120LedEffectTestFadeOut(void);
void Aw9120LedEffectBreathTest(void);

/*SRAM test*/
void Aw9120SramLedBreath(void);
void Aw9120SramLedBandBreath(void);
void Aw9120SramAllLedLight(void);
void Aw9120SramTest(void);
void Aw9120SramLedEffectSellect(LED_LIGHTING_MODE_E effect_mode);

#ifdef __cplusplus
}
#endif

#endif
