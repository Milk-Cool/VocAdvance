#include <gba.h>

#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "common.h"
#include <maxmod.h>
#include <string.h>
#include <stdarg.h>

static uint32_t frame = 0;
static uint8_t track = 0;
static uint16_t pos_x = 0;
static uint8_t pos_y = 0;
static uint8_t bpm = 120;

static Note tracks[4][512];

static void appendf(char* buf, uint16_t* idx, const char* format, ...) {
	va_list args;
	va_start(args, format);
	*idx += vsprintf(buf + *idx, format, args);
}
static char* buf = NULL;
static bool track_render_lock = false;
static void render_tracks() {
	uint16_t idx = 0;
	if(buf == NULL) buf = (char*)malloc(1024);
	for(int8_t i = (pos_y < 18 ? sizeof(tones) / sizeof(tones[0]) - 1 : sizeof(tones) / sizeof(tones[0]) - 18 - 1); i >= (int8_t)(pos_y < 18 ? sizeof(tones) / sizeof(tones[0]) - 18 : 0); i--) {
		appendf(buf, &idx, "\x1b[%d;0H    ", pos_y < 18 ? sizeof(tones) / sizeof(tones[0]) - i : sizeof(tones) / sizeof(tones[0]) - i - 18);
		appendf(buf, &idx, "\x1b[%d;0H", pos_y < 18 ? sizeof(tones) / sizeof(tones[0]) - i : sizeof(tones) / sizeof(tones[0]) - i - 18);
		for(char* c = tones[i]; *c; c++)
			appendf(buf, &idx, "%c", *c == 's' ? '#' : *c);

		appendf(buf, &idx, "\x1b[%d;4H=-------=-------=-------|", pos_y < 18 ? sizeof(tones) / sizeof(tones[0]) - i : sizeof(tones) / sizeof(tones[0]) - i - 18);
	}
	if(pos_y >= 18) for(int8_t i = 7; i < 18; i++)
		appendf(buf, &idx, "\x1b[%d;0H                            |", i + 1);
	for(uint16_t j = 0; j < sizeof(tracks[track]) / sizeof(tracks[track][0]); j++) {
		if(tracks[track][j].length == 0) continue;
		if(tracks[track][j].pos >= (pos_x / 24) * 24 + 24 || tracks[track][j].pos + tracks[track][j].length < (pos_x / 24) * 24) continue;
		if((pos_y < 18 && tracks[track][j].pitch < sizeof(tones) / sizeof(tones[0]) - 18)
			|| (pos_y >= 18 && tracks[track][j].pitch >= sizeof(tones) / sizeof(tones[0]) - 18)) continue;
		char max[32];
		memset(max, '>', 31);
		max[31] = 0;
		memcpy(max, syllables[tracks[track][j].syllable], strlen(syllables[tracks[track][j].syllable]));
		max[tracks[track][j].length] = 0;
		max[24 - tracks[track][j].pos % 24] = 0;
		appendf(buf, &idx, "\x1b[%hhu;%hhuH%s", pos_y < 18 ? sizeof(tones) / sizeof(tones[0]) - tracks[track][j].pitch : sizeof(tones) / sizeof(tones[0]) - tracks[track][j].pitch - 18, 4 + (tracks[track][j].pos % 24), max + ((pos_x / 24) * 24 > tracks[track][j].pos ? (pos_x / 24) * 24 - tracks[track][j].pos : 0));
	}
	appendf(buf, &idx, "\x1b[%hhu;%hhuH#", pos_y % 18 + 1, pos_x % 24 + 4);
	buf[idx] = 0;
	track_render_lock = true;
	iprintf("%s", buf);
	track_render_lock = false;
}
int keys_held;
static void vblank_handler() {
	mmVBlank();
	int keys_pressed, keys_released;
	if(!track_render_lock) {
		iprintf("\x1b[0;1HVocAdvance : track %hhu free %hhu ", track + 1, get_max_new_notes());
		iprintf("\x1b[19;1HBPM = %hhu  ", bpm);
		iprintf("\x1b[19;20HCUR = %hhu  ", pos_x / 8 + 1);
	}

	scanKeys();

	keys_pressed = keysDown();
	keys_released = keysUp();

	keys_held |= keys_pressed;
	keys_held &= ~keys_released;

    mmFrame();
	frame++;
}

int main() {

	consoleDemoInit();

	// ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

	// ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

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

	for(uint8_t i = 0; i < sizeof(tracks) / sizeof(tracks[0]); i++)
		for(uint16_t j = 0; j < sizeof(tracks[0]) / sizeof(tracks[0][0]); j++)
			tracks[i][j].length = 0;
	tracks[0][0].length = 8;
	tracks[0][0].pitch = 7;
	tracks[0][0].pos = 0;
	tracks[0][0].syllable = 2;
	tracks[0][1].length = 8;
	tracks[0][1].pitch = 7;
	tracks[0][1].pos = 8;
	tracks[0][1].syllable = 56;
	tracks[0][2].length = 8;
	tracks[0][2].pitch = 14;
	tracks[0][2].pos = 16;
	tracks[0][2].syllable = 2;
	tracks[0][3].length = 8;
	tracks[0][3].pitch = 14;
	tracks[0][3].pos = 24;
	tracks[0][3].syllable = 56;
	render_tracks();

	irqSet(IRQ_VBLANK, vblank_handler);
	irqEnable(IRQ_VBLANK);

	do {
		loop_engine();

		if((keys_held & KEY_LEFT) && pos_x != 0) { pos_x--; render_tracks(); }
		if((keys_held & KEY_RIGHT) && pos_x != UINT16_MAX) { pos_x++; render_tracks(); }
		if((keys_held & KEY_UP) && pos_y != 0) { pos_y--; render_tracks(); }
		if((keys_held & KEY_DOWN) && pos_y != 24) { pos_y++; render_tracks(); }
	} while( 1 );
}
