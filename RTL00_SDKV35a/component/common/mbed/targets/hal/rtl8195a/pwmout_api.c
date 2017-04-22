 /* mbed Microcontroller Library
  *******************************************************************************
  * Copyright (c) 2014, Realtek Semiconductor Corp.
  * All rights reserved.
  *
  *******************************************************************************
  *  bug fixing: pvvx
  */
     
#include "device.h"
#include "objects.h"
#include "pinmap.h"
//#include <rtl_lib.h>

#if DEVICE_PWMOUT

#ifdef CONFIG_PWM_EN
#include "pwmout_api.h"
#include "objects.h"

extern u32 gTimerRecord;
     
const PinMap PinMap_PWM[] = {
    {PB_4,  RTL_PIN_PERI(PWM0, 0, S0), RTL_PIN_FUNC(PWM0, S0)},
    {PB_5,  RTL_PIN_PERI(PWM1, 1, S0), RTL_PIN_FUNC(PWM1, S0)},
    {PB_6,  RTL_PIN_PERI(PWM2, 2, S0), RTL_PIN_FUNC(PWM2, S0)},
    {PB_7,  RTL_PIN_PERI(PWM3, 3, S0), RTL_PIN_FUNC(PWM3, S0)},

    {PC_0,  RTL_PIN_PERI(PWM0, 0, S1), RTL_PIN_FUNC(PWM0, S1)},
    {PC_1,  RTL_PIN_PERI(PWM1, 1, S1), RTL_PIN_FUNC(PWM1, S1)},
    {PC_2,  RTL_PIN_PERI(PWM2, 2, S1), RTL_PIN_FUNC(PWM2, S1)},
    {PC_3,  RTL_PIN_PERI(PWM3, 3, S1), RTL_PIN_FUNC(PWM3, S1)},

    {PD_3,  RTL_PIN_PERI(PWM0, 0, S2), RTL_PIN_FUNC(PWM0, S2)},
    {PD_4,  RTL_PIN_PERI(PWM1, 1, S2), RTL_PIN_FUNC(PWM1, S2)},
    {PD_5,  RTL_PIN_PERI(PWM2, 2, S2), RTL_PIN_FUNC(PWM2, S2)},
    {PD_6,  RTL_PIN_PERI(PWM3, 3, S2), RTL_PIN_FUNC(PWM3, S2)},

    {PE_0,  RTL_PIN_PERI(PWM0, 0, S3), RTL_PIN_FUNC(PWM0, S3)},
    {PE_1,  RTL_PIN_PERI(PWM1, 1, S3), RTL_PIN_FUNC(PWM1, S3)},
    {PE_2,  RTL_PIN_PERI(PWM2, 2, S3), RTL_PIN_FUNC(PWM2, S3)},
    {PE_3,  RTL_PIN_PERI(PWM3, 3, S3), RTL_PIN_FUNC(PWM3, S3)},

    {NC,    NC,     0}
};
 
int pwmout_init(pwmout_t* obj, PinName pin)
{
    uint32_t peripheral;
    u32 pwm_idx;
    u32 pin_sel;

    DBG_PWM_INFO("%s: Init PWM for pin(0x%x)\n", __FUNCTION__, pin);
    
    // Get the peripheral name from the pin and assign it to the object
    peripheral = pinmap_peripheral(pin, PinMap_PWM);

    if (unlikely(peripheral == NC)) {
        DBG_PWM_ERR("%s: Cannot find matched pwm for this pin(0x%x)\n", __FUNCTION__, pin);
        return -1;
    }

    pwm_idx = RTL_GET_PERI_IDX(peripheral);
    pin_sel = RTL_GET_PERI_SEL(peripheral);

    obj->pwm_idx = pwm_idx;
    obj->pin_sel = pin_sel;
    obj->period = 0;
    obj->pulse = 0;
    rtl_memset((void *)&obj->pwm_hal_adp, 0, sizeof(HAL_PWM_ADAPTER));
    if (HAL_OK != HAL_Pwm_Init(&obj->pwm_hal_adp, pwm_idx, pin_sel)) {
        DBG_PWM_ERR("pwmout_init Err!\n");
        return -1;
    }
//    pwmout_period_us(obj, 20000); // 20 ms per default
//    HAL_Pwm_Enable(&obj->pwm_hal_adp);
    return 0;
}

void pwmout_free(pwmout_t* obj) 
{
    HAL_Pwm_Disable(&obj->pwm_hal_adp);
    gTimerRecord &= ~(1 << obj->pwm_hal_adp.gtimer_id);
}

void pwmout_period_us(pwmout_t* obj, uint32_t us)
{
	obj->period = us;
    HAL_Pwm_SetDuty(&obj->pwm_hal_adp, us, obj->pulse);
}

void pwmout_pulsewidth_us(pwmout_t* obj, uint32_t us)
{
	obj->pulse = us;
	if(us > obj->period) obj->period = us;
    HAL_Pwm_SetDuty(&obj->pwm_hal_adp, obj->period, us);
}

#endif // #ifdef CONFIG_PWM_EN
#endif
