
#include "aw9120.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "cmsis_os.h"


#define AW9120_TRACE(s,...)  TRACE(s, ##__VA_ARGS__)

CMD_REGISTER_T cmd_register[ALL_LED_NUMBERS];
//CMD_REGISTER_T cmd_register[LED_NUMBERS]; /*use this delete */


static void Aw9120I2cWrite(uint8_t reg_add,uint16_t w_data,uint8_t data_length)
{
	uint32_t temp_ret;
	uint8_t temp_data[sizeof(uint16_t)]={0x00};

    temp_data[0]=(uint8_t)(w_data>>8);             /*high 8 bit*/
    temp_data[1]=(uint8_t)(w_data&0x00ff);        /*low 8 bit*/

	//AW9120_TRACE("AW9120 i2c1 write!!!write_data H=%2x,L=%2x",temp_data[0],temp_data[1]);
	temp_ret=I2c1WriteData(AW9120_I2C_ADDRESS,&reg_add,temp_data,data_length*2);
	if(0 == temp_ret)
	{
		return;
	}
	else
	{
		AW9120_TRACE("%s","AW9120_write error!\r\n");
	}
}

static void Aw9120I2cRead(uint8_t reg_add,uint16_t *r_data,uint8_t data_length)
{
	uint32_t temp_ret;
	uint8_t temp_read_data[0x40]={0x00};

   // temp_data[0]=(uint8_t)(w_data>>8)             /*high 8 bit*/
    //temp_data[1]=(uint8_t)(w_data&0x00ff);        /*low 8 bit*/

	AW9120_TRACE("AW9120 i2c1 read!!!SLAVE ADDRESS=%x",AW9120_I2C_ADDRESS);

	temp_ret=I2c1ReadData(AW9120_I2C_ADDRESS,&reg_add,temp_read_data,data_length*2);
	if(0 == temp_ret)
	{
	    for(uint8_t i=0;i<data_length*2;i=i+2)
	    {
	    r_data[i/2]=((uint16_t)temp_read_data[i]<<8)|((uint16_t)temp_read_data[i+1]);
	    }
		return;
	}
	else
	{
		AW9120_TRACE("%s","AW9120_read error!\r\n");
	}
}
/******************************************************************************/
/** read chip id
  *
  * @param[in]
  *
  * @return
  *
  * @note
  *
  *//*
  * History        :
  * 1.Date         : 2018/12/11
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
uint16_t Aw9120ReadChipId(void)
{
    uint16_t temp_id=0x00;
    Aw9120I2cRead(AW9120_CHIPID_RESET_REG,&temp_id,1);
    AW9120_TRACE("aw9120 id=%x",temp_id);
    return temp_id;
}

/******************************************************************************/
/** enable & disable led driver function
  *
  * @param[in] led_enable  TRUE enable led driver  FALSE disable led driver
  *
  * @return
  *
  *//*
  * History        :
  * 1.Date         : 2018/12/11
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void Aw9120EnableDisableLedDriver(BOOL led_enable)
{
    uint16_t temp_enable=0x00;

    Aw9120I2cRead(AW9120_GLOBAL_CTRL_REG,&temp_enable,1);
    if(led_enable)
    {
    temp_enable=temp_enable|0x0001;
    }
    else
    {
    temp_enable=temp_enable&0x0000;
    }
    Aw9120I2cWrite(AW9120_GLOBAL_CTRL_REG,temp_enable,1);
}
/******************************************************************************/
/** enable led all channels
  *
  * @param[in]
  *
  * @note   channel 1-12(bit0-bit11)
  *         channel 13-20(bit0-bit7)
  *//*
  * History        :
  * 1.Date         : 2018/12/12
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void Aw9120EnableLedChannels()
{
    uint16_t temp_led_channels_select1_12=0x0fff;/*channel 1-12(bit0-bit11)*/
    uint16_t temp_led_channels_select13_20=0x00ff; /*channel 13-20(bit0-bit7)*/

    Aw9120I2cWrite(AW9120_LED_DRIVER_ENABLE_REG1,temp_led_channels_select1_12,1);
    Aw9120I2cWrite(AW9120_LED_DRIVER_ENABLE_REG2,temp_led_channels_select13_20,1);

}
/******************************************************************************/
/** set every channels current
  *
  * @param[in] current_mode
  *
  * @return
  *
  *//*
  * History        :
  * 1.Date         : 2018/12/12
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void Aw9120SelectOutputCurrent(CURRENT_MODE_E current_mode)
{
    uint16_t temp_current_cfg=0x0000;
    uint16_t current_cfg_mask=(uint16_t)(current_mode);/*0001-3.5mA; 0002-7.0mA;0003-10.5mA;0004-14mA;0005-17.5mA;0006-21mA;0007-24.5mA*/
    for(uint8_t i=AW9120_MAX_OUTPUT_CURRENT_REG1;i<=AW9120_MAX_OUTPUT_CURRENT_REG5;i++)/*5registers*/
    {
        for(uint8_t j=0;j<4;j++)/*every register has 4 channels*/
        {
        Aw9120I2cRead(i,&temp_current_cfg,1);
        temp_current_cfg=temp_current_cfg|(current_cfg_mask<<(4*j));
        AW9120_TRACE("temp_current_cfg=%x",temp_current_cfg);
        Aw9120I2cWrite(i,temp_current_cfg,1);
        }
    }
}
/******************************************************************************/
/** SRAM program mode config function
  *
  * @param[in] program_mode
  *
  * @return
  *
  *//*
  * History        :
  * 1.Date         : 2018/12/12
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void Aw9120ProgramModeConfig(PROGRAM_MODE_E program_mode)
{
    //uint16_t temp_mode_cfg=0x0000;
    Aw9120I2cWrite(AW9120_PROGRAM_MODE_REG,(uint16_t)program_mode,1);
    AW9120_TRACE("AW9120_PROGRAM_MODE_REG=%x",program_mode);
}
/******************************************************************************/
/** program run mode config
  *
  * @param[in] program_run_mode
  *
  * @return
  *
  *//*
  * History        :
  * 1.Date         : 2018/12/12
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void Aw9120ProgramRunModeConfig(PROGRAM_RUN_MODE_E program_run_mode)
{
    Aw9120I2cWrite(AW9120_PROGRAM_RUN_MODE_REG,(uint16_t)program_run_mode,1);
    AW9120_TRACE("AW9120_PROGRAM_RUN_MODE_REG=%x",program_run_mode);
}
/******************************************************************************/
/** in SRAM mode  load program data via i2c
  *
  * @param[in] load_data
  *
  * @return
  *
  *//*
  * History        :
  * 1.Date         : 2018/12/13
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void Aw9120ProgramLoadingData(uint16_t load_data)
{
    Aw9120I2cWrite(AW9120_W_DATA_REG,load_data,1);
}

void Aw9120ProgramLoadingDataStructure(CMD_REGISTER_T program_data)
{
    Aw9120I2cWrite(AW9120_W_DATA_REG,program_data.set_pwm.register_cmd3,1);
    Aw9120I2cWrite(AW9120_W_DATA_REG,program_data.set_step_time.register_cmd1,1);
    Aw9120I2cWrite(AW9120_W_DATA_REG,program_data.set_fade_in_mode.register_cmd2,1);
    Aw9120I2cWrite(AW9120_W_DATA_REG,program_data.set_wait_time.register_cmd4,1);
    Aw9120I2cWrite(AW9120_W_DATA_REG,program_data.set_fade_out_mode.register_cmd2,1);
    Aw9120I2cWrite(AW9120_W_DATA_REG,program_data.set_wait_time.register_cmd4,1);
/**40*step_time****/
/**41*fade in***/
/**42*wait time***/
/**43*fade out***/
/**44*wait time***/
}

static void Aw9120JumpProgramAddress(uint8_t address)
{
    Aw9120I2cWrite(AW9120_W_DATA_REG,(uint16_t)address,1);
}
/******************************************************************************/
/** program address set
  *
  * @param[in]
  *
  * @note AW9120_PROGRAM_ADDRESS_REG 0x7E
  *
  *//*
  * History        :
  * 1.Date         : 2018/12/13
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void Aw9120ProgramLoadingAddress(uint16_t program_load_add)
{
    //uint16_t temp_addr_cfg=0x0000;
    Aw9120I2cWrite(AW9120_PROGRAM_ADDRESS_REG,program_load_add,1);
}
/******************************************************************************/
/** select led control source     controlled by i2c or SRAM
  *
  * @param[in] BOOL is_i2c_control  TRUE chip controlled by external i2c
  *                                 FALSE controlled by SRAM
  * @note     led control source select i2c or SRAM   0--SRAM  1--i2c
  *
  *//*
  * History        :
  * 1.Date         : 2018/12/13
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void Aw9120ControlSourceSelectI2cOrSram(BOOL is_i2c_control)
{
    uint16_t temp_control_select1=0x0000; /*1-12channel bit0-bit11*/
    uint16_t temp_control_select2=0x0000; /*13-20channel bit0-bit7*/
    if(is_i2c_control)
    {
        temp_control_select1=0x0fff;
        temp_control_select2=0x00ff;
    }
    else
    {
    //0x0000  0x0000  default SRAM mode
    }
    Aw9120I2cWrite(AW9120_CTRL_SOURCE_SELE_REG1,temp_control_select1,1);
    Aw9120I2cWrite(AW9120_CTRL_SOURCE_SELE_REG2,temp_control_select2,1);
}

/******************************************************************************/
/** send cmd to control led effect by extern i2c
  *
  * @param[in] cmd_data
  *
  * @return
  *
  * @note cmd_data include pwm\step_time\fade_mode see the following function
  *     Aw9120LedSetFadeMode();Aw9120LedSetPwm();Aw9120LedSetStepTime()
  *      fill data to structure CMD_REGISTER_T  then Aw9120LedCmdSend();
  *//*
  * History        :
  * 1.Date         : 2018/12/13
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void Aw9120LedCmdSend(uint16_t cmd_data)
{
    Aw9120I2cWrite(Aw9120_LED_MD_REG,cmd_data,1);
}

/******************************************************************************/
/** init structure set the flag_bit as  defined value refer to datasheet
    Page18
  *
  * @param[in] para_cmd_register
  *
  * @return
  *
  *//*
  * History        :
  * 1.Date         : 2018/12/13
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void Aw9120InitStructure(CMD_REGISTER_T *para_cmd_register)
{
    para_cmd_register->set_fade_in_mode.flag_bit=0x03;
    para_cmd_register->set_fade_out_mode.flag_bit=0x03;
    para_cmd_register->set_pwm.flag_bit=0x05;
    para_cmd_register->set_step_time.flag_bit=0x04;
    para_cmd_register->set_wait_time.flag_bit=0x07;
    para_cmd_register->led_effect_mode=LED_IDLE_MODE;
    //AW9120_TRACE("AW9120 init structure=%x",cmd_register.set_fade_mode.register_cmd2);
}

/******************************************************************************/
/** set led fade mode (fade in or fade out)
  *
  * @param[in] channels  0-19 defines 1-20channel if set 0xff open all the 20 channels
  * @param[in] led_light  max pwm value
  * @param[in] is_fade_in fade_in or fade_out select? if 0 fade_out else fade in
  * @param[in] para_cmd_register
  *
  * @return
  *
  * @note usage:if select 5channel max pwm_value is 0x80,fade in mode
  *         Aw9120LedSetFadeMode(0x04,0x80,1,&cmd_register);
  **          *
  *           *
  *           *    *********   (128)
  *           *   *
  *           *  *
  *           * *
  *           ******************************** t(ms)
  *
  *
  *//*
  * History        :
  * 1.Date         : 2018/12/13
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void Aw9120LedSetFadeMode(uint8_t channels,uint8_t led_light,BOOL is_fade_in,CMD_REGISTER_T *para_cmd_register)
{
    if(is_fade_in) /*fade in*/
    {
    para_cmd_register->set_fade_in_mode.channel_bit=channels;
    para_cmd_register->set_fade_in_mode.dir_bit=0x01;   /*Dir: 1: Fade-in; 0: Fade-out*/
    para_cmd_register->set_fade_in_mode.light_bit=led_light;
    }
    else    /*fade out*/
    {
    para_cmd_register->set_fade_out_mode.channel_bit=channels;
    para_cmd_register->set_fade_out_mode.dir_bit=0x00;   /*Dir: 1: Fade-in; 0: Fade-out*/
    para_cmd_register->set_fade_out_mode.light_bit=led_light;
    }
}
/******************************************************************************/
/** set pwm max value
  *
  * @param[in] channels     led channel
  * @param[in] led_light    pwm value
  * @param[in] para_cmd_register
  *
  * @return
  *
  *//*
  * History        :
  * 1.Date         : 2018/12/17
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void Aw9120LedSetPwm(uint8_t channels,uint8_t led_light,CMD_REGISTER_T *para_cmd_register)
{
    para_cmd_register->set_pwm.channel_bit=channels;
    para_cmd_register->set_pwm.light_bit=led_light;
    //Aw9120LedCmdSend(para_cmd_register->set_pwm.register_cmd3);
}

#if 1
void Aw9120SetWaitTime(CMD_REGISTER_T *para_cmd_register,uint16_t wait_time)
{
    /*pre_bit=0 basic time=0.5ms; pre_bit=1 basic=16ms*/
    para_cmd_register->set_wait_time.pre_bit=0x00;
    para_cmd_register->set_wait_time.time_set_bit=wait_time;
    //Aw9120LedCmdSend(para_cmd_register->set_wait_time.register_cmd4);
}
#endif
/******************************************************************************/
/** set step time for every channel
  *
  * @param[in] channels 0-19 defines 1-20channel if set 0xff open all the 20 channels
  * @param[in] led_light  this used calculate step time
  * @param[in] para_cmd_register
  *
  * @return
  *
  * @note usage:if select 5channel step time is 2ms (led_light=0x03)  2ms=(led_light+1)*pre_bit
  *         Aw9120LedSetFadeMode(0x04,0x03,1,&cmd_register);
  **          *
  *           *
  *           *    *|********   (128)
  *           *   * |
  *           *  *  |(2*128)ms
  *           * *   |
  *           ******************************** t(ms)
  *
  *//*
  * History        :
  * 1.Date         : 2018/12/13
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void Aw9120LedSetStepTime(uint8_t channels,uint8_t led_light,CMD_REGISTER_T *para_cmd_register)
{
    para_cmd_register->set_step_time.channel_bit=channels;
    para_cmd_register->set_step_time.light_bit=led_light;  /*(led_light+1)*pre_bit*/
    para_cmd_register->set_step_time.pre_bit=0x00;         /*Pre: basic unit of time, 0: 0.5ms; 1: 16ms*/
    //Aw9120LedCmdSend(para_cmd_register->set_step_time.register_cmd1);
    //AW9120_TRACE("AW9120 SET STEP TIME=%x",cmd_register.set_step_time.register_cmd1);
}
/******************************************************************************/
/** aw9120 init function
  *
  * @param[in]
  *
  * @note
  *
  *//*
  * History        :
  * 1.Date         : 2018/12/14
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void Aw9120Init()
{
	if(AW9120_CHIP_ID==	Aw9120ReadChipId())
	{
    AW9120_TRACE("AW9120 led driver init begin!");
	Aw9120EnableDisableLedDriver(TRUE); /*enable led driver*/
	Aw9120EnableLedChannels();          /*enable all led channels*/
	Aw9120SelectOutputCurrent(CURRENT_3P5MA);   /*set led output led current*/
	Aw9120ControlSourceSelectI2cOrSram(0);      /*select led chip controlled by extern i2c*/
    for(uint8_t i=0;i<ALL_LED_NUMBERS;i++)
    {
	    Aw9120InitStructure(&cmd_register[i]);         /*init structure*/
	}
    }
    else
    {
    AW9120_TRACE("AW9120 led driver read id failed!");
    }
}

void Aw9120LedEffectTest()
{
//AW9120_TRACE("effect test add wait time!!!");
for(uint8_t i=0x00;i<LED_BAND_NUMBERS;i++)
{
	Aw9120LedSetStepTime(i,0x13,&cmd_register[i]);  /*set step time as (09+1)*0.5=5ms*/
    Aw9120LedSetFadeMode(i,(0x0f+i*20),1,&cmd_register[i]); /*fade in mode*/
    Aw9120LedCmdSend(cmd_register[i].set_step_time.register_cmd1);
    Aw9120LedCmdSend(cmd_register[i].set_fade_in_mode.register_cmd2);
    osDelay(200);
 }
 }
void Aw9120LedEffectTestFadeOut()
{
 #if 1

 for(uint8_t i=0x00;i<LED_BAND_NUMBERS;i++)
{
	Aw9120LedSetStepTime(i,0x13,&cmd_register[i]);  /*set step time as (09+1)*0.5=5ms*/
    Aw9120LedSetFadeMode(i,(0x0f+i*20),0,&cmd_register[i]); /*fade out mode*/
    Aw9120LedCmdSend(cmd_register[i].set_step_time.register_cmd1);
    Aw9120LedCmdSend(cmd_register[i].set_fade_out_mode.register_cmd2);

    osDelay(200);
 }
 #endif
}

void Aw9120LedEffectBreathTest()
{
	Aw9120LedSetStepTime(0x0c,0x01,&cmd_register[0x0c]);  /*set step time as (09+1)*0.5=5ms*/
    Aw9120LedSetFadeMode(0x0c,255,1,&cmd_register[0x0c]); /*fade in mode*/
    Aw9120LedCmdSend(cmd_register[0x0c].set_step_time.register_cmd1);
    Aw9120LedCmdSend(cmd_register[0x0c].set_fade_in_mode.register_cmd2);

    osDelay(755);
	Aw9120LedSetStepTime(0x0c,0x01,&cmd_register[0x0c]);  /*set step time as (09+1)*0.5=5ms*/
    Aw9120LedSetFadeMode(0x0c,255,0,&cmd_register[0x0c]); /*fade out mode*/
    Aw9120LedCmdSend(cmd_register[0x0c].set_step_time.register_cmd1);
    Aw9120LedCmdSend(cmd_register[0x0c].set_fade_out_mode.register_cmd2);
    osDelay(755);
}

void Aw9120LedEffectAllLightOnOff(BOOL is_all_on)
{
    if(is_all_on)
    {
        for(uint8_t i=0x00;i<LED_BAND_NUMBERS;i++)
        {
        	Aw9120LedSetStepTime(i,0x01,&cmd_register[i]);  /*set step time as (09+1)*0.5=5ms*/
            Aw9120LedSetFadeMode(i,255,1,&cmd_register[i]); /*all on with full light*/
            Aw9120LedCmdSend(cmd_register[i].set_step_time.register_cmd1);
            Aw9120LedCmdSend(cmd_register[i].set_fade_in_mode.register_cmd2);
        }
    }
    else
    {
        for(uint8_t i=0x00;i<LED_BAND_NUMBERS;i++)
        {
        	Aw9120LedSetStepTime(i,0x01,&cmd_register[i]);  /*set step time as (09+1)*0.5=5ms*/
            Aw9120LedSetFadeMode(i,255,0,&cmd_register[i]); /*all off*/
            Aw9120LedCmdSend(cmd_register[i].set_step_time.register_cmd1);
            Aw9120LedCmdSend(cmd_register[i].set_fade_out_mode.register_cmd2);
        }
    }
}

void Aw9120SetLedEffectMode(CMD_REGISTER_T *para_cmd_register,LED_LIGHTING_MODE_E led_effec_mode)
{
    para_cmd_register->led_effect_mode=led_effec_mode;
}


void Aw9120LedLightingThreadHandler(const void *arg)
{
static BOOL enter_onetime_flag1=TRUE;
while(0)
{
    if(LED_IDLE_MODE==cmd_register[0].led_effect_mode)
    {
        if(enter_onetime_flag1)
        {
        TRACE("Led driver disable!");
        Aw9120EnableDisableLedDriver(FALSE);
        enter_onetime_flag1=FALSE;
        }
    }
    else
    {
        Aw9120EnableDisableLedDriver(TRUE);
        enter_onetime_flag1=TRUE;
    switch(cmd_register[0].led_effect_mode)
    {
        case SINGLE_BREATH_MODE:
        {
            Aw9120LedEffectBreathTest();
            TRACE("Breath Mode!!");
            break;
        }
        case ALL_LED_LIGHTING:
        {
            Aw9120LedEffectAllLightOnOff(TRUE);
            TRACE("All on Mode!!");
            break;
        }
        case CIRCLE_LIGHT:
        {
            Aw9120LedEffectTest();
            TRACE("Fade in Mode!!");
            break;
        }
        case CIRCLE_LIGHT_FADE_OUT:
        {
            Aw9120LedEffectTestFadeOut();
            TRACE("Fade out Mode!!");
            break;
        }
        case ALL_LED_OFF:
        {
            Aw9120LedEffectAllLightOnOff(FALSE);
            TRACE("All off Mode!!");
            break;
        }

        default:
            break;
        }
        }
    }
}


/*The following codes are used for testing led SRAM mode*/

void Aw9120SramLedBandBreath(void)
{
    AW9120_TRACE("*****led band breath cmd data*******");

    /*cmd data store in cmd_register*/
    for(uint8_t i=0;i<LED_BAND_NUMBERS;i++)
    {
        Aw9120LedSetPwm(i,0x00,&cmd_register[i]); /*set pwm value 0 turn off band led*/
        Aw9120LedSetStepTime(i,0x03,&cmd_register[i]);/*led13 set step time 2ms*/
        Aw9120LedSetFadeMode(i, 0x80, 1, &cmd_register[i]);
        Aw9120SetWaitTime(&cmd_register[i],1000);
        Aw9120LedSetFadeMode(i, 0x80, 0, &cmd_register[i]);
        Aw9120SetWaitTime(&cmd_register[i],1000);

        TRACE("%x %x %x %x",
        cmd_register[i].set_step_time.register_cmd1,
        cmd_register[i].set_fade_in_mode.register_cmd2,
        cmd_register[i].set_wait_time.register_cmd4,
        cmd_register[i].set_fade_out_mode.register_cmd2);
    }

    /*load program starting at address =0x0000*/
    Aw9120ProgramLoadingAddress(0x0000);

    /*load cmd data begin*/
     /*address 0-11      set pwm 0;*/
    /*address 12-23      step time;
      address 24-35     fade in;
      address 36         wait time;
      address 37-48     fade out;
      address 49        wait time;
      address 50        jump address0x18(24)*/
    for(uint8_t index=0x00;index<LED_BAND_NUMBERS;index++)
    {
    Aw9120ProgramLoadingData(cmd_register[index].set_pwm.register_cmd3);
    }
    for(uint8_t index=0x00;index<LED_BAND_NUMBERS;index++)
    {
    Aw9120ProgramLoadingData(cmd_register[index].set_step_time.register_cmd1);
    }
    for(uint8_t index=0x00;index<LED_BAND_NUMBERS;index++)
    {
    Aw9120ProgramLoadingData(cmd_register[index].set_fade_in_mode.register_cmd2);
    }
    Aw9120ProgramLoadingData(cmd_register[0].set_wait_time.register_cmd4);
    for(uint8_t index=0x00;index<LED_BAND_NUMBERS;index++)
    {
    Aw9120ProgramLoadingData(cmd_register[index].set_fade_out_mode.register_cmd2);
    }
    Aw9120ProgramLoadingData(cmd_register[0].set_wait_time.register_cmd4);


    Aw9120JumpProgramAddress(0x18); /*jump program start address address 24*/
/*load cmd data end*/


}



void Aw9120SramLedBreath(void)
{
    /*cmd data store in cmd_register*/
    Aw9120LedSetPwm(0x0c,0x00,&cmd_register[0x0c]);/*set led pwm 00*/
    Aw9120LedSetStepTime(0x0c,0x03,&cmd_register[0x0c]);/*led13 set step time 2ms*/
    Aw9120LedSetFadeMode(0x0c, 0x80, 1, &cmd_register[0x0c]);
    Aw9120SetWaitTime(&cmd_register[0x0c],1000);
    Aw9120LedSetFadeMode(0x0c, 0x80, 0, &cmd_register[0x0c]);
    Aw9120SetWaitTime(&cmd_register[0x0c],1000);

   TRACE("%x %x %x %x",
    cmd_register[0x0c].set_step_time.register_cmd1,
    cmd_register[0x0c].set_fade_in_mode.register_cmd2,
    cmd_register[0x0c].set_fade_out_mode.register_cmd2,
    cmd_register[0x0c].set_wait_time.register_cmd4);

    /*program start address 52 load program starting at address =0x34(52)*/
    Aw9120ProgramLoadingAddress(0x34);
    /**52 pwm =0**/
    /**53*step_time****/
    /**54*fade in***/
    /**55*wait time***/
    /**56*fade out***/
    /**57*wait time***/
    /**58*jump address 0x36***/
    Aw9120ProgramLoadingDataStructure(cmd_register[0x0c]); /*load cmd data*/

    Aw9120JumpProgramAddress((0x34+0x01)); /*program address+1*/

}

void Aw9120SramAllLedOnByPwm()
{
    for(uint8_t i=0x00;i<LED_BAND_NUMBERS;i++)
    {
    Aw9120LedSetPwm(i,0xff,&cmd_register[i]);/*set led pwm 0xff*/
    }

    /*address 60-71 pwm =0xff; */

    /*program start address 60 load program starting at address =0x3c*/
    Aw9120ProgramLoadingAddress(0x3C);
    for(uint8_t i=0;i<LED_BAND_NUMBERS;i++)
    {
     Aw9120ProgramLoadingData(cmd_register[i].set_pwm.register_cmd3);
    }
    Aw9120JumpProgramAddress((0x3C));/*jump address 60*/

}
void Aw9120SramAllLedOffByPwm()
{
    for(uint8_t i=0x00;i<LED_BAND_NUMBERS;i++)
    {
    Aw9120LedSetPwm(i,0x00,&cmd_register[i]);/*set led pwm 00*/
    }
    /*address 75-86 pwm =0; */

    /*program start address 75 load program starting at address =0x48*/
    Aw9120ProgramLoadingAddress(0x4B);
    for(uint8_t i=0;i<LED_BAND_NUMBERS;i++)
    {
     Aw9120ProgramLoadingData(cmd_register[i].set_pwm.register_cmd3);
    }
    Aw9120JumpProgramAddress((0x4B));/*jump address 75*/

}

#if 0

void Aw9120SramAllLedLight(void)
{

    for(uint8_t i=0x00;i<LED_BAND_NUMBERS;i++)
    {
        Aw9120LedSetStepTime(i,0x00,&cmd_register[i]);/*led set step time 0.5ms*/
        Aw9120LedSetFadeMode(i,0xff, 1, &cmd_register[i]);/*set led fade in*/
    }

    /*program start address 60 load program starting at address =0x3c*/
    Aw9120ProgramLoadingAddress(0x3C);

        /*address 60-71 step time;
        address 72-83 fade in
        address 84 jump address */

    for(uint8_t i=0;i<LED_BAND_NUMBERS;i++)
    {
     Aw9120ProgramLoadingData(cmd_register[i].set_step_time.register_cmd1);
    }
    for(uint8_t i=0;i<LED_BAND_NUMBERS;i++)
    {
     Aw9120ProgramLoadingData(cmd_register[i].set_fade_in_mode.register_cmd2);
    }
     Aw9120JumpProgramAddress((0x48));/*jump address 72*/

}

void Aw9120SramAllLedOff(void)
{

    for(uint8_t i=0x00;i<LED_BAND_NUMBERS;i++)
    {
        Aw9120LedSetStepTime(i,0x00,&cmd_register[i]);/*led set step time 0.5ms*/
        Aw9120LedSetFadeMode(i,0x00, 0, &cmd_register[i]);/*set led fade out*/
    }

/*program address 87 load program starting at address =0x57*/
    Aw9120ProgramLoadingAddress(0x57);
    /*address 87-98 step time;
      address 99-110 fade in
      address 111 jump address 99*/

    for(uint8_t i=0;i<LED_BAND_NUMBERS;i++)
    {
     Aw9120ProgramLoadingData(cmd_register[i].set_step_time.register_cmd1);
    }
    for(uint8_t i=0;i<LED_BAND_NUMBERS;i++)
    {
     Aw9120ProgramLoadingData(cmd_register[i].set_fade_in_mode.register_cmd2);
    }
     Aw9120JumpProgramAddress(0x63); /*jump address 99*/

}

#endif

void Aw9120SramProgramConfigBeforeLoading(void)
{
    AW9120_TRACE("*****led SRAM program config before loading*******");
	Aw9120ControlSourceSelectI2cOrSram(0); /*controlled by SRAM*/
	Aw9120ProgramModeConfig(PROGRAM_VIA_I2C);  /*load program by i2c*/
    Aw9120ProgramRunModeConfig(HOLD_MODE);  /*HOLD mode*/
}




/*
address 0x00  led band breath
address 0x34  single led breath
address 0x3C  all led on
address 0x57  all led off
*/

void Aw9120SramProgramStartRunAddressSet(uint16_t pc_start_address)
{
    Aw9120I2cWrite(AW9120_PROGRAM_START_ADDRESS,pc_start_address,1);
}

/*load cmd data to sram*/
void Aw9120SramTest(void)
{
    Aw9120SramProgramConfigBeforeLoading();

    /*load cmd data to sram*/
    Aw9120SramLedBandBreath();
    Aw9120SramLedBreath();
    //Aw9120SramAllLedLight();
   // Aw9120SramAllLedOff();
   Aw9120SramAllLedOnByPwm();
   Aw9120SramAllLedOffByPwm();
    /*end load cmd data*/

    /*set program run start address*/
    Aw9120SramProgramStartRunAddressSet(0x0000);
    /*from the start address begin running*/
    Aw9120ProgramRunModeConfig(RUN_MODE);
	Aw9120ProgramModeConfig(PROGRAM_RELOAD_RUN);
}

void Aw9120SramLedEffectSellect(LED_LIGHTING_MODE_E effect_mode)
{
    //Aw9120ProgramRunModeConfig(HOLD_MODE);
    switch(effect_mode)
    {
        case SINGLE_BREATH_MODE:
        {
            /*set program run start address*/
            Aw9120SramProgramStartRunAddressSet(0x34);
            /*from the start address begin running*/
            break;
        }
        case LED_BAND_BREATH:
        {
            /*set program run start address*/
            //Aw9120SramAllLedOff();
            Aw9120SramProgramStartRunAddressSet(0x0000);
            /*from the start address begin running*/
            break;
        }
        case ALL_LED_LIGHTING:
        {
            /*set program run start address*/
            Aw9120SramProgramStartRunAddressSet(0x3C);
            /*from the start address begin running*/
            break;
        }
        case ALL_LED_OFF:
        {
            /*set program run start address*/
            Aw9120SramProgramStartRunAddressSet(0x4B);
            /*from the start address begin running*/
            break;
        }
        default:
        break;
    }
    AW9120_TRACE("delete hold mode led effect case = %x",effect_mode);
    Aw9120ProgramRunModeConfig(RUN_MODE);
	Aw9120ProgramModeConfig(PROGRAM_RELOAD_RUN);

}

/*end SRAM mode test*/



