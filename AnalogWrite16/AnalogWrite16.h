#include "wiring_private.h"

void analogWrite16(uint32_t pin, uint32_t value) {
	PinDescription pinDesc = g_APinDescription[pin];
	uint32_t attr = pinDesc.ulPinAttribute;

	if (attr & (PIN_ATTR_PWM_E|PIN_ATTR_PWM_F|PIN_ATTR_PWM_G)) {
		uint32_t tcNum = GetTCNumber(pinDesc.ulPWMChannel);
		uint8_t tcChannel = GetTCChannelNumber(pinDesc.ulPWMChannel);
		static bool tcEnabled[TCC_INST_NUM+TC_INST_NUM];

		if(attr & PIN_ATTR_PWM_E)
			pinPeripheral(pin, PIO_TIMER);
		else if(attr & PIN_ATTR_PWM_F)
			pinPeripheral(pin, PIO_TIMER_ALT);
		else if(attr & PIN_ATTR_PWM_G)
			pinPeripheral(pin, PIO_TCC_PDEC);

		if (!tcEnabled[tcNum]) {
			tcEnabled[tcNum] = true;
			GCLK->PCHCTRL[GCLK_CLKCTRL_IDs[tcNum]].reg = GCLK_PCHCTRL_GEN_GCLK0_Val | (1 << GCLK_PCHCTRL_CHEN_Pos); //use clock generator 0

			// Set PORT
			if (tcNum >= TCC_INST_NUM) {
				// -- Configure TC
				Tc* TCx = (Tc*) GetTC(pinDesc.ulPWMChannel);

				//reset
				TCx->COUNT16.CTRLA.bit.SWRST = 1;
				while (TCx->COUNT16.SYNCBUSY.bit.SWRST);

				// Disable TCx
				TCx->COUNT16.CTRLA.bit.ENABLE = 0;
				while (TCx->COUNT16.SYNCBUSY.bit.ENABLE);
				// Set Timer counter Mode to 16 bits, normal PWM, prescaler 1
				TCx->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV1;
				TCx->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_NPWM;

				while (TCx->COUNT16.SYNCBUSY.bit.CC0);
				// Set the initial value
				TCx->COUNT16.CC[tcChannel].reg = (uint16_t) value;
				while (TCx->COUNT16.SYNCBUSY.bit.CC0 || TCx->COUNT16.SYNCBUSY.bit.CC1);
				// Enable TCx
				TCx->COUNT16.CTRLA.bit.ENABLE = 1;
				while (TCx->COUNT16.SYNCBUSY.bit.ENABLE);
			}
			else {
				// -- Configure TCC
				Tcc* TCCx = (Tcc*) GetTC(pinDesc.ulPWMChannel);

				TCCx->CTRLA.bit.SWRST = 1;
				while (TCCx->SYNCBUSY.bit.SWRST);

				// Disable TCCx
				TCCx->CTRLA.bit.ENABLE = 0;
				while (TCCx->SYNCBUSY.bit.ENABLE);
				// Set prescaler to 1
				TCCx->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV1 | TCC_CTRLA_PRESCSYNC_GCLK;

				// Set TCx as normal PWM
				TCCx->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;
				while ( TCCx->SYNCBUSY.bit.WAVE );

				while (TCCx->SYNCBUSY.bit.CC0 || TCCx->SYNCBUSY.bit.CC1);
				// Set the initial value
				TCCx->CC[tcChannel].reg = (uint32_t) value;
				while (TCCx->SYNCBUSY.bit.CC0 || TCCx->SYNCBUSY.bit.CC1);
				// Set PER to maximum counter value (resolution : 0xFFFF)
				TCCx->PER.reg = 0xFFFF;
				while (TCCx->SYNCBUSY.bit.PER);
				// Enable TCCx
				TCCx->CTRLA.bit.ENABLE = 1;
				while (TCCx->SYNCBUSY.bit.ENABLE);
			}
		}
		else {
			if (tcNum >= TCC_INST_NUM) {
				Tc* TCx = (Tc*) GetTC(pinDesc.ulPWMChannel);
				TCx->COUNT16.CC[tcChannel].reg = (uint16_t) value;
				while (TCx->COUNT16.SYNCBUSY.bit.CC0 || TCx->COUNT16.SYNCBUSY.bit.CC1);
			} 
			else {
				Tcc* TCCx = (Tcc*) GetTC(pinDesc.ulPWMChannel);
				while (TCCx->SYNCBUSY.bit.CTRLB);
				while (TCCx->SYNCBUSY.bit.CC0 || TCCx->SYNCBUSY.bit.CC1);
				TCCx->CCBUF[tcChannel].reg = (uint32_t) value;
				while (TCCx->SYNCBUSY.bit.CC0 || TCCx->SYNCBUSY.bit.CC1);
				TCCx->CTRLBCLR.bit.LUPD = 1;
				while (TCCx->SYNCBUSY.bit.CTRLB);
			}
		}
	}
}
