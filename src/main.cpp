#include <stdio.h>
#include <algorithm>

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "ds4.h"

const uint PWM_FREQ = 1000;	 // PWM周波数（Hz）
const uint PWM_LEVEL = 1000; // PWMデューティサイクルの最大値

void setup_pwm(uint gpio_pin)
{
	// GPIOピンをPWM機能に設定
	gpio_set_function(gpio_pin, GPIO_FUNC_PWM);

	// PWMスライス番号の取得
	uint slice_num = pwm_gpio_to_slice_num(gpio_pin);

	// PWMのラップ値を設定
	pwm_set_wrap(slice_num, PWM_LEVEL);

	// PWMのデューティサイクルを設定（50%）
	pwm_set_chan_level(slice_num, pwm_gpio_to_channel(gpio_pin), 0);

	// PWMを有効にする
	pwm_set_enabled(slice_num, true);
}

void set_pwm_duty_cycle(uint gpio_pin, uint duty_cycle)
{
	uint slice_num = pwm_gpio_to_slice_num(gpio_pin);
	pwm_set_chan_level(slice_num, pwm_gpio_to_channel(gpio_pin), duty_cycle);
}

int main()
{
	ds4_setup();

	setup_pwm(13);
	setup_pwm(14);
	setup_pwm(15);

	uint duty_cycle = PWM_LEVEL / 2;
	bool increase = true;

	while (true)
	{
		tight_loop_contents();
		if (ds4_can_use())
		{
			bt_hid_state state = ds4_get_state();
			printf("buttons: %04x, l: %d,%d, r: %d,%d, l2,r2: %d,%d hat: %d\n", state.buttons, state.lx, state.ly, state.rx, state.ry, state.l2, state.r2, state.hat);
			uint clip1 = std::max(0, state.ly - 128);
			uint clip2 = std::max(0, state.ry - 128);
			uint clip3 = std::min(0, state.ry - 128) * -1;

			printf("clip1: %d, clip2: %d, clip3: %d\n", clip1, clip2, clip3);

			set_pwm_duty_cycle(13, clip1 / 128.0 * PWM_LEVEL);
			set_pwm_duty_cycle(14, clip2 / 128.0 * PWM_LEVEL);
			set_pwm_duty_cycle(15, clip3 / 128.0 * PWM_LEVEL);
		}
		sleep_ms(20); // スリープで変更の間隔を制御
	}

	return 0;
}
