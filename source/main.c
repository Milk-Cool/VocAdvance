#include <gba.h>

#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include <maxmod.h>

static void vblank_handler() {
	mmVBlank();
	int keys_pressed, keys_released;
	iprintf("\x1b[5;0Htick %d     ", mibis());	 
	iprintf("\x1b[6;0Hfree %hhu ", get_max_new_notes());	 

	scanKeys();

	keys_pressed = keysDown();
	keys_released = keysUp();
    mmFrame();
}

int main() {

	consoleDemoInit();

	// ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

	// ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH
	iprintf("\x1b[0;8HMaxMod Audio demo");
	iprintf("\x1b[3;0HHold A for ambulance sound");
	iprintf("\x1b[4;0HPress B for boom sound");

	init_engine();
	play_note("ki", 0, 0, 512);
	play_note("ra", 0, 512, 512);
	play_note("ki", 7, 1024, 512);
	play_note("ra", 7, 1536, 512);
	play_note("hi", 9, 2048, 512);
	play_note("ka", 9, 2560, 512);
	play_note("ru", 7, 3072, 512);
	play_note("o", 5, 4096, 512);
	play_note("so", 5, 4608, 512);
	play_note("ra", 4, 5120, 512);
	play_note("no", 4, 5632, 512);
	play_note("ho", 2, 6144, 512);
	play_note("shi", 2, 6656, 512);
	play_note("yo", 0, 7168, 4096);
	reset_timers();

	irqSet(IRQ_VBLANK, vblank_handler);
	irqEnable(IRQ_VBLANK);

	do {
		loop_engine();
	} while( 1 );
}
