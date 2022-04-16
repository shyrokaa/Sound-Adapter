#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <stdio.h>

#define wait_time 100
#define max_val 3303254

int main(void)
{
	//variable definitions

	cy_rslt_t result_red, result_green, result_blue, result_pot, result_uart;
	cyhal_pwm_t pwm_red, pwm_green, pwm_blue;

	int pot_value = 0;
	cyhal_adc_t adc_obj;
	cyhal_adc_channel_t adc_chan_0_obj;

	/* Initialize the device and board peripherals */
	result_red = cybsp_init() ;
	result_green = cybsp_init() ;
	result_blue = cybsp_init() ;
	result_pot = cybsp_init() ;
	if (result_red != CY_RSLT_SUCCESS){
		CY_ASSERT(0);
	}

	__enable_irq();

	result_uart = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
	CY_ASSERT(result_uart == CY_RSLT_SUCCESS);

	/* Initialize PWM on the supplied pin and assign a new clock */
	result_red = cyhal_pwm_init(&pwm_red, CYBSP_LED_RGB_RED, NULL);
	result_green = cyhal_pwm_init(&pwm_green, CYBSP_LED_RGB_GREEN, NULL);
	result_blue = cyhal_pwm_init(&pwm_blue, CYBSP_LED_RGB_BLUE, NULL);

	/* Start the PWM output */
	result_red = cyhal_pwm_start(&pwm_red);
	result_green = cyhal_pwm_start(&pwm_green);
	result_blue = cyhal_pwm_start(&pwm_blue);

	// ADC configuration structure
	const cyhal_adc_config_t ADCconfig ={
		.continuous_scanning = false,
		.resolution = 12,
		.average_count = 1,
		.average_mode_flags = 0,
		.ext_vref_mv = 0,
		.vneg = CYHAL_ADC_VNEG_VREF,
		.vref = CYHAL_ADC_REF_VDDA,
		.ext_vref = NC,
		.is_bypassed = false,
		.bypass_pin = NC
	};

	//PIN 10_6 IS THE POTENTIOMETER
	/* Initialize ADC. The ADC block which can connect to pin 10[6] is selected */
	result_pot = cyhal_adc_init(&adc_obj, P10_6, NULL);

	// Configure to use VDD as Vref
	result_pot = cyhal_adc_configure(&adc_obj, &ADCconfig);

	/* Initialize ADC channel, allocate channel number 0 to pin 10[6] as this is the first channel initialized */
	const cyhal_adc_channel_config_t channel_config = { .enable_averaging = false, .min_acquisition_ns = 220, .enabled = true };
	result_pot = cyhal_adc_channel_init_diff(&adc_chan_0_obj, &adc_obj, CYBSP_POT, CYHAL_ADC_VNEG, &channel_config);

	cyhal_pwm_set_duty_cycle(&pwm_red, 100, 10000);
	cyhal_pwm_set_duty_cycle(&pwm_green, 100, 10000);
	cyhal_pwm_set_duty_cycle(&pwm_blue, 100, 10000);

	printf("%VALOAREA INITIALA DE PE POTENTIOMETRU: d\n", &pot_value);

	while(true){
		pot_value = cyhal_adc_read_uv(&adc_chan_0_obj);
		int pwm_val = pot_value * 100 / max_val;


		printf("%d -> %d\n", pot_value, pwm_val);
		cyhal_pwm_set_duty_cycle(&pwm_red, 100 - pwm_val, 10000);
	}
}

/* [] END OF FILE */
