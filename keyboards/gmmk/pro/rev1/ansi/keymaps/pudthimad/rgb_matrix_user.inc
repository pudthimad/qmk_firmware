// !!! DO NOT ADD #pragma once !!! //
#include <print.h>
// Step 1.
// Declare custom effects using the RGB_MATRIX_EFFECT macro
// (note the lack of semicolon after the macro!)
RGB_MATRIX_EFFECT(mod_1)

// Step 2.
// Define effects inside the `RGB_MATRIX_CUSTOM_EFFECT_IMPLS` ifdef block
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static bool up = false;

static bool isSideLed(int led) {
    uint8_t sideLed[16] = {67,68,70,71,73,74,76,77,80,81,83,84,87,88,91,92};
    bool result = false;

    int i;
    for(i =0;i<sizeof(sideLed);i++){
        if(sideLed[i]==led){
            result = true;
            break;
        }
    }
    return result;
}

static HSV MY_CYCLE_UP_DOWN_math(HSV hsv, uint8_t i, uint8_t time) {
    if(up == false){
        hsv.h = g_led_config.point[i].y - time;
    }else{
        hsv.h = g_led_config.point[i].y + time;
    }
    hsv.s = 180;

    return hsv;
}

static HSV MY_SOLID_REACTIVE_SIMPLE_math(HSV hsv, uint16_t offset) {
    hsv.v = scale8(255 - offset, hsv.v);
    return hsv;
}
static HSV matrix_light[98];
// e.g: A simple effect, self-contained within a single method
static bool mod_1(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint16_t max_tick = 65535 / qadd8(rgb_matrix_config.speed, 1);
    uint8_t time = scale16by8(g_rgb_timer, qadd8(rgb_matrix_config.speed / 4, 1));
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        uint16_t tick = max_tick;
        // Reverse search to find most recent key hit
        for (int8_t j = g_last_hit_tracker.count - 1; j >= 0; j--) {
            if (g_last_hit_tracker.index[j] == i && g_last_hit_tracker.tick[j] < tick) {
                tick = g_last_hit_tracker.tick[j];
                break;
            }
        }
        if(tick<20){
            HSV defaultHSV = rgb_matrix_config.hsv;
            matrix_light[i] = defaultHSV;
            matrix_light[i].h = rand() % 255 + 1;
            matrix_light[i].s = 200;
        }
        uint16_t offset = scale16by8(tick, qadd8(rgb_matrix_config.speed, 1));
        RGB      rgb    = rgb_matrix_hsv_to_rgb(MY_SOLID_REACTIVE_SIMPLE_math(matrix_light[i], offset));
        if(isSideLed(i)){
            if(time == 255 && i == 67){
                up = up?false:true;
            }
            rgb = rgb_matrix_hsv_to_rgb(MY_CYCLE_UP_DOWN_math(rgb_matrix_config.hsv, i, time));
        }
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
