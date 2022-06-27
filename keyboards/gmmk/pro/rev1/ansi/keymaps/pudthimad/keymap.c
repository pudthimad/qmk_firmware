/* Copyright 2021 Glorious, LLC <salman@pcgamingrace.com>
   Copyright 2021 Jonavin

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include "rgb_matrix_map.h"
#include "jonavin.h"
#include "print.h"


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

//      ESC      F1       F2       F3       F4       F5       F6       F7       F8       F9       F10      F11      F12	    PrintScreen    Rotary(Mute)
//      ~        1        2        3        4        5        6        7        8        9        0         -       (=)	     BackSpc           Del
//      Tab      Q        W        E        R        T        Y        U        I        O        P        [        ]        \                 PgUp
//      Caps     A        S        D        F        G        H        J        K        L        ;        "                 Enter             PgDn
//      Sh_L              Z        X        C        V        B        N        M        ,        .        ?                 Sh_R     Up       End
//      Ct_L     Win_L    Alt_L                               SPACE                               Alt_R    FN       Ct_R     Left     Down     Right


    [_BASE] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_PSCR,          KC_MUTE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,          KC_DEL,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,          KC_PGUP,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,           KC_PGDN,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT, KC_UP,   KC_END,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, MO(_FN1),KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [_FN1] = LAYOUT(
        _______, KC_MYCM, KC_WHOM, KC_CALC, KC_MSEL, KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP, KC_MUTE, KC_VOLD, KC_VOLU, _______, KC_CALC,          _______,
        _______, RGB_MOD1,_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          RGB_TOG,
        _______, _______, RGB_VAI, _______, _______, _______, _______, KC_PSCR, KC_SLCK, KC_PAUS, _______, _______, _______, RESET,            KC_HOME,
        KC_CAPS, _______, RGB_VAD, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          KC_END,
        _______,          RGB_NITE,RGB_HUI, _______, _______, _______, KC_NLCK, _______, RGB_TOD, RGB_TOI, _______,          _______, RGB_MOD, _______,
        _______, KC_WINLCK, _______,                            _______,                          _______, _______, _______, RGB_SPD, RGB_RMOD, RGB_SPI
    ),
};

#if defined(ENCODER_ENABLE) && !defined(ENCODER_DEFAULTACTIONS_ENABLE)     // Encoder Functionality when not using userspace defaults
    void encoder_action_rgbhue(bool clockwise) {
        if (clockwise)
            rgblight_increase_hue_noeeprom();
        else
            rgblight_decrease_hue_noeeprom();
    }

    bool encoder_update_user(uint8_t index, bool clockwise) {
        uint8_t mods_state = get_mods();
        if (mods_state & MOD_BIT(KC_LSFT) ) { // If you are holding L shift, encoder changes layers
            encoder_action_layerchange(clockwise);
        } else if (mods_state & MOD_BIT(KC_RSFT) ) { // If you are holding R shift, Page up/dn
            unregister_mods(MOD_BIT(KC_RSFT));
            encoder_action_navpage(clockwise);
            register_mods(MOD_BIT(KC_RSFT));
        } else if (mods_state & MOD_BIT(KC_LCTL)) {  // if holding Left Ctrl, navigate next/prev word
            encoder_action_navword(clockwise);
        } else if (mods_state & MOD_BIT(KC_RCTL)) {  // if holding Right Ctrl, change rgb hue/colour
            encoder_action_rgbhue(clockwise);
        } else if (mods_state & MOD_BIT(KC_LALT)) {  // if holding Left Alt, change media next/prev track
            encoder_action_mediatrack(clockwise);
        } else  {
            switch(get_highest_layer(layer_state)) {
            case _FN1:
                #ifdef IDLE_TIMEOUT_ENABLE
                    timeout_update_threshold(clockwise);
                #endif
                break;
            default:
                encoder_action_volume(clockwise);       // Otherwise it just changes volume
                break;
            }
        }
        return true;
    }
#endif // ENCODER_ENABLE && !ENCODER_DEFAULTACTIONS_ENABLE

#ifdef RGB_MATRIX_ENABLE
    // Capslock, Scroll lock and Numlock  indicator on Left side lights.
    void rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
        if (get_rgb_nightmode()) rgb_matrix_set_color_all(RGB_OFF);
        if (IS_HOST_LED_ON(USB_LED_SCROLL_LOCK)) {
            rgb_matrix_set_color(LED_L1, RGB_GREEN);
            rgb_matrix_set_color(LED_L2, RGB_GREEN);
        }

        if (IS_HOST_LED_ON(USB_LED_CAPS_LOCK)) {
            rgb_matrix_set_color(LED_L5, RGB_RED);
            rgb_matrix_set_color(LED_L6, RGB_RED);
            rgb_matrix_set_color(LED_L7, RGB_RED);
        }
        if (keymap_config.no_gui) {
            rgb_matrix_set_color(LED_LWIN, RGB_RED);  //light up Win key when disabled
        }
        switch(get_highest_layer(layer_state)){  // special handling per layer
        case _FN1:  // on Fn layer select what the encoder does when pressed
            rgb_matrix_set_color(LED_R2, RGB_RED);
            rgb_matrix_set_color(LED_R3, RGB_RED);
            rgb_matrix_set_color(LED_R4, RGB_RED);
            rgb_matrix_set_color(LED_FN, RGB_RED); //FN key
            break;
        case _LOWER:
            for (uint8_t i=0; i<ARRAYSIZE(LED_LIST_NUMPAD); i++) {
                rgb_matrix_set_color(LED_LIST_NUMPAD[i], RGB_MAGENTA);
            }
            rgb_matrix_set_color(LED_R4, RGB_MAGENTA);
            rgb_matrix_set_color(LED_R5, RGB_MAGENTA);
            rgb_matrix_set_color(LED_R6, RGB_MAGENTA);
            break;
        case _RAISE:
            rgb_matrix_set_color(LED_R6, RGB_GREEN);
            rgb_matrix_set_color(LED_R7, RGB_GREEN);
            rgb_matrix_set_color(LED_R8, RGB_GREEN);
            break;
#ifdef COLEMAK_LAYER_ENABLE
       case _COLEMAK:
            for (uint8_t i=0; i<ARRAYSIZE(LED_SIDE_RIGHT); i++) {
                rgb_matrix_set_color(LED_SIDE_RIGHT[i], RGB_BLUE);
            }
            break;
#endif
        default:
            break;
        }
    }

    void suspend_power_down_user(void) {
        rgb_matrix_set_suspend_state(true);
    }

    void suspend_wakeup_init_user(void) {
        rgb_matrix_set_suspend_state(false);
    }
#endif

void keyboard_post_init_keymap(void) {
    // keyboard_post_init_user() moved to userspace
    debug_enable=true;
    #ifdef RGB_MATRIX_ENABLE
        rgb_matrix_mode(RGB_MATRIX_CUSTOM_mod_1);
        activate_rgb_nightmode(false);  // Set to true if you want to startup in nightmode, otherwise use Fn + Z to toggle
    #endif
}
