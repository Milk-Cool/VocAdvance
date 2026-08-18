#include <gba.h>

#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include <maxmod.h>

static void vblank_handler() {
	mmVBlank();
	int keys_pressed, keys_released;
	iprintf("\x1b[5;0H%d", mibis());	 

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
	play_note("pa", -12 + 0, 0, 512);
	play_note("ke", -12 + 2, 512, 512);
	play_note("mi", -12 + 4, 1024, 512);
	play_note("do", -12 + 5, 1536, 512);
	play_note("bu", -12 + 7, 2048, 512);
	play_note("yo", -12 + 9, 2560, 512);
	play_note("ne", -12 + 11, 3072, 512);
	play_note("go", -12 + 12, 3584, 512);

	irqSet(IRQ_VBLANK, vblank_handler);
	irqEnable(IRQ_VBLANK);

	do {
		loop_engine();
	} while( 1 );
}
