/* Copyright 2021 Sleepdealer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "langpuca.h"
bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
    if (!process_record_user(keycode, record)) {
        return false;
    }
    switch (keycode) {
        case MC_00:
            if (record->event.pressed) {
                SEND_STRING("00");
            }
            break;
    }
    return true;
}

bool encoder_update_kb(uint8_t index, bool clockwise) {
    if (!encoder_update_user(index, clockwise)) { return false; }
    if (clockwise) {
        tap_code_delay(KC_VOLU, 10);  // Right
    } else {
        tap_code_delay(KC_VOLD, 10);  // Left
    }
    return false;
}



// OLED
#ifdef OLED_ENABLE
__attribute__((weak)) oled_rotation_t oled_init_user(oled_rotation_t rotation) { return OLED_ROTATION_270; }

//variables for use with langtons ant
uint8_t testArray[32][32] = {0};
uint8_t antX = 15;
uint8_t antY = 15;
uint8_t antFacing = 0;

bool oled_task_kb(void) {
    if (!oled_task_user()) { return false; }
	// WPM-responsive animation stuff here
	#    define IDLE_FRAMES 2
	#    define ANIM_FRAME_DURATION 200  // how long each frame lasts in ms
	#    define ANIM_SIZE 636  // number of bytes in array, minimize for adequate firmware size, max is 1024
    static uint32_t anim_timer = 0;
    
    void animation_phase(void) {
		//rotation
		switch(testArray[antX][antY]){
			case 0: //turn left
				antFacing = antFacing - 1;
				if(antFacing > 3) antFacing = 3;
				break;
			case 1: //turn right
				antFacing = antFacing + 1;
				if(antFacing > 3) antFacing = 0;
		}
		//flip color
		testArray[antX][antY] = (testArray[antX][antY] + 1)%2;
		//movement forward
		switch(antFacing){
			case 0: //pointing up
				antY = antY-1;
				if(antY > 31) antY = 31;
				break;
			case 1: //pointing right
				antX = antX+1;
				if(antX > 31) antX = 0;
				break;
			case 2: //pointing down
				antY = antY+1;
				if(antY > 31) antY = 0;
				break;
			case 3: //pointing left
				antX = antX-1;
				if(antX > 31) antX = 31;
		}
		//draw
		//white on black efficient drawing
		/*
		switch(testArray[antX][antY]){
			case 0:
				oled_write_pixel(antX,antY,true);
				break;
			case 1:
				oled_write_pixel(antX,antY,false);
		}
		*/
		for(uint8_t i=0;i<32;i++){
			for(uint8_t j=0;j<32;j++){
				switch(testArray[i][j]){
					case 0:
						oled_write_pixel(i,j,true);
						break;
					case 1:
						oled_write_pixel(i,j,false);
				}
			}
		}	

    }
    if (timer_elapsed32(anim_timer) > ANIM_FRAME_DURATION) {
        anim_timer = timer_read32();
        animation_phase();
    }
	//numlock info and puca name
    oled_set_cursor(0, 6);
    oled_write_P(PSTR("PUCA\nPAD\n"), false);
    oled_write_P(PSTR("-----\n"), false);
    // Host Keyboard Layer Status
    oled_write_P(PSTR("NUM: "), false);
	
	led_t led_state = host_keyboard_led_state();
	oled_write_P(led_state.num_lock ? PSTR("ON\n") : PSTR("OFF\n"), false);

    return true;
}
#endif
