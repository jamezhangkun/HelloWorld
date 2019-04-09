/*******************************************************************************
            Copyright (C), 2018, GoerTek Technology Co., Ltd.
*******************************************************************************/
/**
  *  @defgroup	i2c_driver module name
  *  @brief 	i2c_driver
  *  @file 		i2c_driver
  *  @author 	jame.zhang
  *  @version 	1.0
  *  @date    	2018/11/6
  *  @{
  *//*
  *  FileName      : i2c_driver
  *  Description   : i2c driver
  *  FunctionList  :
              		I2C_Close
              		I2C_Init
              		I2C_Open
              		i2c_read_data
              		i2c_write_data
  * History        :
  * 1.Date         : 2018/11/6
  *    Author      : jame.zhang
  *    Modification: Created file
*******************************************************************************/

#include "cmsis_os.h"
#include "string.h"
#include "hal_timer.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "plat_types.h"

extern "C" {
#include "hal_i2c.h"
}
#include "i2c_driver.h"


//#define IIC_MUTEX
#define I2C_TRACE(s, ...)       TRACE(s, ##__VA_ARGS__)
#define I2C_DUMP8(x,y,z)        //DUMP8(x,y,z)
#define I2C_FAIL_TRY_TIMES (3)

#ifdef IIC_MUTEX
osMutexId iic_mutex_id = NULL;
osMutexDef(iic_mutex);

osMutexId iic_mutex_id1 = NULL;
osMutexDef(iic_mutex1);
#endif

static struct HAL_I2C_CONFIG_T i2c_config_1v8;  /*i2c 0*/
static struct HAL_I2C_CONFIG_T i2c_config_3v3;  /*i2c 1*/

/*i2c1 chg pins config --3v3
*/
const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_i2c_chg[] = {
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_I2C_M1_SCL, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_I2C_M1_SDA, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
};
/*i2c0 dt pins config --1v8
*/
const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_i2c_dt[] = {
        {HAL_IOMUX_PIN_P1_6, HAL_IOMUX_FUNC_I2C_M0_SCL, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P1_7, HAL_IOMUX_FUNC_I2C_M0_SDA, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
};


static void I2cSet(enum HAL_I2C_ID_T i2c_id)
{
if(HAL_I2C_ID_0==i2c_id)
{
hal_iomux_set_i2c0(); /*1v8*/
}
else
{
hal_iomux_set_i2c1(); /*3v3*/
}
}


/******************************************************************************/
/** i2c init function
  *
  * @return null
  *
  *//*
  * History        :
  * 1.Date         : 2018/11/6
  *   Author       : jame.zhang
  *   Modification : Created function

*******************************************************************************/
void I2cInit(void)
{
	I2C_TRACE("%s","I2C Init!\r\n");
#ifdef IIC_MUTEX
    if (!iic_mutex_id)
	{
        iic_mutex_id = osMutexCreate((osMutex(iic_mutex)));
    }
#endif
	I2cSet(HAL_I2C_ID_0);
// hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)cfg_hw_i2c_chg, ARRAY_SIZE(cfg_hw_i2c_chg));
    i2c_config_1v8.mode = HAL_I2C_API_MODE_TASK;
    i2c_config_1v8.use_dma  = 0;
    i2c_config_1v8.use_sync = 1;
    i2c_config_1v8.speed = 400000;
    i2c_config_1v8.as_master = 1;

}

/******************************************************************************/
/** i2c1 init function
  *
  * @return
  *
  *//*
  * History        :
  * 1.Date         : 2018/11/26
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void I2c1Init(void)
{
	I2C_TRACE("%s","I2C1 Init!\r\n");
#ifdef IIC_MUTEX
    if (!iic_mutex_id1)
	{
        iic_mutex_id1 = osMutexCreate((osMutex(iic_mutex1)));
    }
#endif
	I2cSet(HAL_I2C_ID_1);

    //hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)cfg_hw_i2c_dt, sizeof(cfg_hw_i2c_dt)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
    i2c_config_3v3.mode = HAL_I2C_API_MODE_TASK;
    i2c_config_3v3.use_dma  = 0;
    i2c_config_3v3.use_sync = 1;
    i2c_config_3v3.speed = 100000;
    i2c_config_3v3.as_master = 1;

	I2C_TRACE("%s","JAMEjame****jameI2C1 100KHZ!\r\n");
}

/******************************************************************************/
/** i2c open function,used before communicating
  *
  * @return
  *
  *//*
  * History        :
  * 1.Date         : 2018/11/6
  *   Author       : jame.zhang
  *   Modification : Created function

*******************************************************************************/
void I2cOpen(void)
{
#ifdef IIC_MUTEX
    osStatus temp_os_status;
    temp_os_status = osMutexWait(iic_mutex_id, osWaitForever);
    if (osErrorISR == temp_os_status )
	{
        ASSERT(0, "%s Not allowed in ISR", __func__);
	}
#endif
	I2C_TRACE("I2C Open!!");
    hal_i2c_open(HAL_I2C_ID_0, &i2c_config_1v8);

#ifdef IIC_MUTEX
    osMutexRelease(iic_mutex_id);
#endif
}
/******************************************************************************/
/** i2c1 open function
  *
  * @return
  *
  *//*
  * History        :
  * 1.Date         : 2018/11/26
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void I2c1Open(void)
{
#ifdef IIC_MUTEX
    osStatus temp_os_status;
	I2C_TRACE("I2C1 Open!!");

    temp_os_status = osMutexWait(iic_mutex_id1, osWaitForever);
    if (osErrorISR == temp_os_status )
	{
        ASSERT(0, "%s Not allowed in ISR", __func__);
	}
#endif

    hal_i2c_open(HAL_I2C_ID_1, &i2c_config_3v3);

#ifdef IIC_MUTEX
    osMutexRelease(iic_mutex_id1);
#endif
}

/******************************************************************************/
/** i2c close function
  *
  * @return
  *
  *//*
  * History        :
  * 1.Date         : 2018/11/6
  *   Author       : jame.zhang
  *   Modification : Created function

*******************************************************************************/
void I2cClose(void)
{
#ifdef IIC_MUTEX
    osStatus temp_os_status;

    temp_os_status = osMutexWait(iic_mutex_id, osWaitForever);
    if (osErrorISR == temp_os_status)
	{
        ASSERT(0, "%s Not allowed in ISR", __func__);
    }
#endif

    hal_i2c_close(HAL_I2C_ID_0);

#ifdef IIC_MUTEX
    osMutexRelease(iic_mutex_id);
#endif
}
/******************************************************************************/
/** i2c1 close function
  *
  * @return
  *
  *//*
  * History        :
  * 1.Date         : 2018/11/26
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
void I2c1Close(void)
{
#ifdef IIC_MUTEX
    osStatus temp_os_status;

    temp_os_status = osMutexWait(iic_mutex_id1, osWaitForever);
    if (osErrorISR == temp_os_status)
	{
        ASSERT(0, "%s Not allowed in ISR", __func__);
    }
#endif

    hal_i2c_close(HAL_I2C_ID_1);

#ifdef IIC_MUTEX
    osMutexRelease(iic_mutex_id1);
#endif
}
/******************************************************************************/
/** i2c write data function
  *
  * @param[in] slave_addr  device address got from chip datasheet
  * @param[in] reg_addr    register address to write
  * @param[in] write_data  data to write into register
  * @param[in] data_len    data length
  *
  * @return  i2c error code
  *
  * @note
  *
  *//*
  * History        :
  * 1.Date         : 2018/11/6
  *   Author       : jame.zhang
  *   Modification : Created function

*******************************************************************************/
uint32_t I2cWriteData(uint32_t slave_addr, uint8_t *reg_addr, uint8_t *write_data, uint8_t data_len)
{
    uint32_t temp_i2c_error_code;
    uint8_t temp_send_buf[32]={0}; /*modify to allocate dynamically*/

    ASSERT((data_len+1)<=(uint8_t)sizeof(temp_send_buf), "%s buffer overflow", __func__);

#ifdef IIC_MUTEX
    osStatus temp_os_status;

    temp_os_status = osMutexWait(iic_mutex_id, osWaitForever);
    if (osErrorISR == temp_os_status)
	{
        ASSERT(0, "%s Not allowed in ISR", __func__);
    }
#endif

    temp_send_buf[0] = *reg_addr;
    memcpy(temp_send_buf+1, write_data, data_len);
	I2C_TRACE("%x%x",temp_send_buf[0],temp_send_buf[1]);
/*try 3 times when I2C wirte fail.*/
    for(uint8_t i=0; i<I2C_FAIL_TRY_TIMES; i++)
    {
    temp_i2c_error_code=hal_i2c_send(HAL_I2C_ID_0, slave_addr, temp_send_buf, 1, data_len, 0, NULL);
        if(0 == temp_i2c_error_code)
        {
        break;
        }
        osDelay(5);
    }

#ifdef IIC_MUTEX
    osMutexRelease(iic_mutex_id);
#endif
    return temp_i2c_error_code;
}

/******************************************************************************/
/** i2c1 write data function
  *
  * @param[in] slave_addr
  * @param[in] reg_addr
  * @param[in] write_data
  * @param[in] data_len
  *
  * @return
  *
  * @note
  *
  *//*
  * History        :
  * 1.Date         : 2018/11/26
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
uint32_t I2c1WriteData(uint32_t slave_addr, uint8_t *reg_addr, uint8_t *write_data, uint8_t data_len)
{
    uint32_t temp_i2c_error_code;
    uint8_t temp_send_buf[32]={0}; /*modify to allocate dynamically*/

    ASSERT((data_len+1)<=(uint8_t)sizeof(temp_send_buf), "%s buffer overflow", __func__);

#ifdef IIC_MUTEX
    osStatus temp_os_status;

    temp_os_status = osMutexWait(iic_mutex_id1, osWaitForever);
    if (osErrorISR == temp_os_status)
	{
        ASSERT(0, "%s Not allowed in ISR", __func__);
    }
#endif

    temp_send_buf[0] = *reg_addr;
    memcpy(temp_send_buf+1, write_data, data_len);
	//TRACE("temp_send_buf[0]=%x--temp_send_buf[1]=%x",temp_send_buf[0],temp_send_buf[1]);
/*try 3 times when I2C wirte fail.*/
    for(uint8_t i=0; i<I2C_FAIL_TRY_TIMES; i++)
    {
    temp_i2c_error_code=hal_i2c_send(HAL_I2C_ID_1, slave_addr, temp_send_buf, 1, data_len, 0, NULL);
        if(0 == temp_i2c_error_code)
        {
        break;
        }
        osDelay(5);
    }

#ifdef IIC_MUTEX
    osMutexRelease(iic_mutex_id1);
#endif
    return temp_i2c_error_code;
}

/******************************************************************************/
/** i2c read data function
  *
  * @param[in] slave_addr        device address got from chip datasheet
  * @param[in] reg_addr			 register address to be read
  * @param[in] read_data_bffer	 data to read from register
  * @param[in] read_data_len	 data length
  *
  * @return i2c error code
  *
  * @note
  *
  *//*
  * History        :
  * 1.Date         : 2018/11/6
  *   Author       : jame.zhang
  *   Modification : Created function

*******************************************************************************/
uint32_t I2cReadData(uint8_t slave_addr, uint8_t *reg_addr, uint8_t *read_data_bffer, uint8_t read_data_len)
{
    uint8_t temp_read_buf[32]={0}; /*modify to allocate dynamically*/
    uint32_t temp_i2c_error_code;

    ASSERT((read_data_len+1)<=(uint8_t)sizeof(temp_read_buf), "%s buffer overflow", __func__);

#ifdef IIC_MUTEX
    osStatus temp_os_status;

    temp_os_status = osMutexWait(iic_mutex_id, osWaitForever);
    if (osErrorISR == temp_os_status)
	{
        ASSERT(0, "%s Not allowed in ISR", __func__);
    }
#endif

    temp_read_buf[0] = *reg_addr;
    for(uint8_t i=0; i<I2C_FAIL_TRY_TIMES; i++)
    {
    temp_i2c_error_code = hal_i2c_recv(HAL_I2C_ID_0, slave_addr, temp_read_buf, 1, read_data_len, 1, 0, NULL);
        if(0 == temp_i2c_error_code)
        {
            memcpy(read_data_bffer, temp_read_buf+1,read_data_len);
            break;
        }
        osDelay(5);
    }

#ifdef IIC_MUTEX
    osMutexRelease(iic_mutex_id);
#endif

    return temp_i2c_error_code;
}

/******************************************************************************/
/** i2c1 read data function
  *
  * @param[in] slave_addr
  * @param[in] reg_addr
  * @param[in] read_data_bffer
  * @param[in] read_data_len
  *
  * @return
  *
  * @note
  *
  *//*
  * History        :
  * 1.Date         : 2018/11/26
  *   Author       : i2c1 init function
  *   Modification : Created function

*******************************************************************************/
uint32_t I2c1ReadData(uint8_t slave_addr, uint8_t *reg_addr, uint8_t *read_data_bffer, uint8_t read_data_len)
{
    uint8_t temp_read_buf[32]={0}; /*modify to allocate dynamically*/
    uint32_t temp_i2c_error_code;

    ASSERT((read_data_len+1)<=(uint8_t)sizeof(temp_read_buf), "%s buffer overflow", __func__);

#ifdef IIC_MUTEX
    osStatus temp_os_status;

    temp_os_status = osMutexWait(iic_mutex_id1, osWaitForever);
    if (osErrorISR == temp_os_status)
	{
        ASSERT(0, "%s Not allowed in ISR", __func__);
    }
#endif

    temp_read_buf[0] = *reg_addr;
    for(uint8_t i=0; i<I2C_FAIL_TRY_TIMES; i++)
    {
    temp_i2c_error_code = hal_i2c_recv(HAL_I2C_ID_1, slave_addr, temp_read_buf, 1, read_data_len, 1, 0, NULL);
        if(0 == temp_i2c_error_code)
        {
            memcpy(read_data_bffer, temp_read_buf+1,read_data_len);
            break;
        }
        osDelay(5);
    }

#ifdef IIC_MUTEX
    osMutexRelease(iic_mutex_id1);
#endif

    return temp_i2c_error_code;
}





/** @}*/

