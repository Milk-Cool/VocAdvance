#include <gba.h>

#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "common.h"
#include "save.h"

#include <maxmod.h>
#include <string.h>
#include <stdarg.h>

#define INIT_FLAG 0x7fff
#define BPM_SAVE_ADDR 0x7ffe

#define TRACKS_N 4
#define TRACK_NOTES_N 512
static Note* tracks;

static uint32_t frame = 0;
static uint8_t track = 0;
static uint16_t pos_x = 0;
static uint8_t pos_y = 0;
static uint8_t bpm = 120;

static bool show_kb = false;
static uint8_t kb_x = 0;
static uint8_t kb_y = 0;
static bool playing = false;
static uint16_t playing_x[TRACKS_N];
static uint16_t playing_x_start;
static const char* kb[14][8] = {
	{ "a",    "i",    "u",    "e",    "o",    "n",    NULL,   NULL  },
	{ "ka",   "ki",   "ku",   "ke",   "ko",   "kya",  "kyu",  "kyo" },
	{ "sa",   "shi",  "su",   "se",   "so",   "sha",  "shu",  "sho" },
	{ "ta",   "chi",  "tsu",  "te",   "to",   "cha",  "chu",  "cho" },
	{ "na",   "ni",   "nu",   "ne",   "no",   "nya",  "nyu",  "nyo" },
	{ "ha",   "hi",   "fu",   "he",   "ho",   "hya",  "hyu",  "hyo" },
	{ "ma",   "mi",   "mu",   "me",   "mo",   "mya",  "myu",  "myo" },
	{ "ya",   "yu",   "yo",   "wa",   "wo",   NULL,   NULL,   NULL  },
	{ "ra",   "ri",   "ru",   "re",   "ro",   "rya",  "ryu",  "ryo" },
	{ "ga",   "gi",   "gu",   "ge",   "go",   "gya",  "gyu",  "gyo" },
	{ "za",   "ji",   "zu",   "ze",   "zo",   "ja",   "ju",   "jo"  },
	{ "da",   "ji",   "zu",   "de",   "do",   "ja",   "ju",   "jo"  },
	{ "ba",   "bi",   "bu",   "be",   "bo",   "bya",  "byu",  "byo" },
	{ "pa",   "pi",   "pu",   "pe",   "po",   "pya",  "pyu",  "pyo" },
};

// only 8bit readwrites allowed
static uint16_t pos(Note* note) {
	uint16_t x;
	memcpy(&x, (uint8_t*)&note->pos, sizeof(uint16_t));
	return x;
}
static void appendf(char* buf, uint16_t* idx, const char* format, ...) {
	va_list args;
	va_start(args, format);
	*idx += vsprintf(buf + *idx, format, args);
}
static char* buf = NULL;
static bool ui_render_lock = false;
static void render_ui() {
	uint16_t idx = 0;
	if(buf == NULL) buf = (char*)malloc(1024);
	if(show_kb) {
		for(uint8_t y = 0; y < sizeof(kb) / sizeof(kb[0]); y++) {
			uint8_t len = 0;
			for(uint8_t x = 0; x < sizeof(kb[y]) / sizeof(kb[y][0]); x++) {
				if(kb[y][x] == NULL) break;
				appendf(buf, &idx, "\x1b[%hhu;%hhuH%c%s", y + 1, len, kb_x == x &&  kb_y == y ? '#' : ' ', kb[y][x]);
				len += strlen(kb[y][x]) + 1;
			}
			appendf(buf, &idx, "                              ");
		}
		for(uint8_t y = 0; y < 4; y++) appendf(buf, &idx, "                              ");
	} else {
		for(int8_t i = (pos_y < 18 ? sizeof(tones) / sizeof(tones[0]) - 1 : sizeof(tones) / sizeof(tones[0]) - 18 - 1); i >= (int8_t)(pos_y < 18 ? sizeof(tones) / sizeof(tones[0]) - 18 : 0); i--) {
			appendf(buf, &idx, "\x1b[%d;0H    ", pos_y < 18 ? sizeof(tones) / sizeof(tones[0]) - i : sizeof(tones) / sizeof(tones[0]) - i - 18);
			appendf(buf, &idx, "\x1b[%d;0H", pos_y < 18 ? sizeof(tones) / sizeof(tones[0]) - i : sizeof(tones) / sizeof(tones[0]) - i - 18);
			for(char* c = tones[i]; *c; c++)
				appendf(buf, &idx, "%c", *c == 's' ? '#' : *c);

			appendf(buf, &idx, "\x1b[%d;4H=-------=-------=-------|", pos_y < 18 ? sizeof(tones) / sizeof(tones[0]) - i : sizeof(tones) / sizeof(tones[0]) - i - 18);
		}
		if(pos_y >= 18) for(int8_t i = 7; i < 18; i++)
			appendf(buf, &idx, "\x1b[%d;0H                            |", i + 1);
		for(uint16_t j = 0; j < TRACK_NOTES_N; j++) {
			if(tracks[(uint16_t)track * TRACK_NOTES_N + j].length == 0) break;
			if(!(
				(pos(&tracks[(uint16_t)track * TRACK_NOTES_N + j]) >= (pos_x / 24) * 24 && pos(&tracks[(uint16_t)track * TRACK_NOTES_N + j]) < (pos_x / 24) * 24 + 24)
				|| (pos(&tracks[(uint16_t)track * TRACK_NOTES_N + j]) + tracks[(uint16_t)track * TRACK_NOTES_N + j].length >= (pos_x / 24) * 24 && pos(&tracks[(uint16_t)track * TRACK_NOTES_N + j]) + tracks[(uint16_t)track * TRACK_NOTES_N + j].length < (pos_x / 24) * 24 + 24)
			)) continue;
			if((pos_y < 18 && tracks[(uint16_t)track * TRACK_NOTES_N + j].pitch < sizeof(tones) / sizeof(tones[0]) - 18)
				|| (pos_y >= 18 && tracks[(uint16_t)track * TRACK_NOTES_N + j].pitch >= sizeof(tones) / sizeof(tones[0]) - 18)) continue;
			char max[32];
			memset(max, '>', 31);
			max[31] = 0;
			memcpy(max, syllables[tracks[(uint16_t)track * TRACK_NOTES_N + j].syllable], strlen(syllables[tracks[(uint16_t)track * TRACK_NOTES_N + j].syllable]));
			max[tracks[(uint16_t)track * TRACK_NOTES_N + j].length] = 0;
			if((pos_x / 24) * 24 <= pos(&tracks[(uint16_t)track * TRACK_NOTES_N + j])) max[24 - pos(&tracks[(uint16_t)track * TRACK_NOTES_N + j]) % 24] = 0;
			appendf(buf, &idx, "\x1b[%hhu;%hhuH%s", pos_y < 18 ? sizeof(tones) / sizeof(tones[0]) - tracks[(uint16_t)track * TRACK_NOTES_N + j].pitch : sizeof(tones) / sizeof(tones[0]) - tracks[(uint16_t)track * TRACK_NOTES_N + j].pitch - 18, 4 + ((pos_x / 24) * 24 > pos(&tracks[(uint16_t)track * TRACK_NOTES_N + j]) ? 0 : pos(&tracks[(uint16_t)track * TRACK_NOTES_N + j]) % 24), max + ((pos_x / 24) * 24 > pos(&tracks[(uint16_t)track * TRACK_NOTES_N + j]) ? (pos_x / 24) * 24 - pos(&tracks[(uint16_t)track * TRACK_NOTES_N + j]) : 0));
		}
		appendf(buf, &idx, "\x1b[%hhu;%hhuH#", pos_y % 18 + 1, pos_x % 24 + 4);
	}
	buf[idx] = 0;
	ui_render_lock = true;
	iprintf("%s", buf);
	ui_render_lock = false;
}
int keys_held;
static void vblank_handler() {
	mmVBlank();
	int keys_pressed, keys_released;
	if(!ui_render_lock) {
		iprintf("\x1b[0;0HVocAdvance : track %hhu free %hhu ", track + 1, get_max_new_notes());
		iprintf("\x1b[19;1HBPM = %hhu  ", bpm);
		uint8_t cur = pos_x / 8 + 1;
		iprintf("\x1b[19;20HCUR = %hhu", cur);
		if(cur < 10) iprintf("%s", " ");
		if(cur < 100) iprintf("%s", " ");
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

	iprintf("\x1b[2J");
	iprintf("\x1b[2J");

	init_engine();
	reset_timers();

	tracks = (Note*)SAVE_ADDR; // no malloc

	uint8_t init_flag;
	save_read(INIT_FLAG, &init_flag, 1);
	if(init_flag != 143) {
		for(uint8_t i = 0; i < TRACKS_N; i++)
			for(uint16_t j = 0; j < TRACK_NOTES_N; j++)
				tracks[(uint16_t)i * TRACK_NOTES_N + j].length = 0;
		init_flag = 143;
		save_write(INIT_FLAG, &init_flag, 1);
		save_write(BPM_SAVE_ADDR, &bpm, 1);
	} else {
		save_read(BPM_SAVE_ADDR, &bpm, 1);
	}

	render_ui();

	irqSet(IRQ_VBLANK, vblank_handler);
	irqEnable(IRQ_VBLANK);

	do {
		loop_engine();

		bool to_reset_timers = false;
		if(show_kb) {
			if(keys_held & KEY_LEFT) {
				if(kb_x == 0) kb_x = 7;
				else kb_x--;
				while(kb[kb_y][kb_x] == NULL) kb_x--;
				render_ui();
			} else if(keys_held & KEY_RIGHT) {
				if(kb_x == 7) kb_x = 0;
				else kb_x++;
				while(kb[kb_y][kb_x] == NULL) kb_x--;
				render_ui();
			} else if(keys_held & KEY_DOWN) {
				if(kb_y == 13) kb_y = 0;
				else kb_y++;
				while(kb[kb_y][kb_x] == NULL) kb_x--;
				render_ui();
			} else if(keys_held & KEY_UP) {
				if(kb_y == 0) kb_y = 13;
				else kb_y--;
				while(kb[kb_y][kb_x] == NULL) kb_x--;
				render_ui();
			} else if(keys_held & KEY_A) {
				for(uint16_t i = 0; i < TRACK_NOTES_N; i++)
					if(tracks[(uint16_t)track * TRACK_NOTES_N + i].length == 0 || pos(&tracks[(uint16_t)track * TRACK_NOTES_N + i]) > pos_x) {
						bool flag = false;
						for(uint8_t j = 0; j < sizeof(syllables) / sizeof(syllables[0]); j++)
							if(!strcmp(syllables[j], kb[kb_y][kb_x])) {
								memmove(&tracks[(uint16_t)track * TRACK_NOTES_N + i + 1], &tracks[(uint16_t)track * TRACK_NOTES_N + i], sizeof(Note) * (TRACK_NOTES_N - i - 1));
								flag = true;
								tracks[(uint16_t)track * TRACK_NOTES_N + i].syllable = j;
								tracks[(uint16_t)track * TRACK_NOTES_N + i].length = 4;
								tracks[(uint16_t)track * TRACK_NOTES_N + i].pitch = sizeof(tones) / sizeof(tones[0]) - pos_y - 1;
								memcpy((uint8_t*)&tracks[(uint16_t)track * TRACK_NOTES_N + i].pos, &pos_x, sizeof(uint16_t)); // only 8bit readwrites allowed
								break;
							}
						if(flag) break;
					}
				show_kb = false;
				render_ui();
			} else if(keys_held & KEY_B) {
				show_kb = false;
				render_ui();
			}
		} else {
			if(keys_held & KEY_A) {
				bool flag = false;
				for(uint16_t i = 0; i < TRACK_NOTES_N; i++) {
					if(tracks[(uint16_t)track * TRACK_NOTES_N + i].length == 0) break;
					if(pos_y != sizeof(tones) / sizeof(tones[0]) - tracks[(uint16_t)track * TRACK_NOTES_N + i].pitch - 1
						|| pos_x < pos(&tracks[(uint16_t)track * TRACK_NOTES_N + i])
						|| pos_x >= pos(&tracks[(uint16_t)track * TRACK_NOTES_N + i]) + tracks[(uint16_t)track * TRACK_NOTES_N + i].length) continue;
					memmove(&tracks[(uint16_t)track * TRACK_NOTES_N + i], &tracks[(uint16_t)track * TRACK_NOTES_N + i + 1], sizeof(Note) * (TRACK_NOTES_N - i - 1));
					tracks[(track + 1) * TRACK_NOTES_N - 1].length = 0;
					flag = true;
					break;
				}
				if(!flag) {
					show_kb = true;
				}
				keys_held &= ~KEY_A; // just in case
				render_ui();
			}
			if(((keys_held & KEY_LEFT) || (keys_held & KEY_RIGHT)) && (keys_held & KEY_B)) {
				for(uint16_t i = 0; i < TRACK_NOTES_N; i++) {
					if(tracks[(uint16_t)track * TRACK_NOTES_N + i].length == 0) break;
					if(pos_y != sizeof(tones) / sizeof(tones[0]) - tracks[(uint16_t)track * TRACK_NOTES_N + i].pitch - 1
						|| pos_x < pos(&tracks[(uint16_t)track * TRACK_NOTES_N + i])
						|| pos_x >= pos(&tracks[(uint16_t)track * TRACK_NOTES_N + i]) + tracks[(uint16_t)track * TRACK_NOTES_N + i].length) continue;
					if(((keys_held & KEY_RIGHT) && tracks[(uint16_t)track * TRACK_NOTES_N + i].length >= 24)) continue;
					if(keys_held & KEY_LEFT)
						tracks[(uint16_t)track * TRACK_NOTES_N + i].length--;
					else
						tracks[(uint16_t)track * TRACK_NOTES_N + i].length++;
					break;
				}
				render_ui();
			}
			if((keys_held & KEY_L) && (keys_held & KEY_B)) {
				if(pos_x >= 8) pos_x -= 8;
				render_ui();
			}
			if((keys_held & KEY_R) && (keys_held & KEY_B)) {
				if(pos_x < UINT16_MAX - 8) pos_x += 8;
				render_ui();
			}
			if(keys_held & KEY_SELECT) {
				if(keys_held & KEY_LEFT) { bpm--; keys_held &= ~KEY_LEFT; save_write(BPM_SAVE_ADDR, &bpm, 1); }
				if(keys_held & KEY_RIGHT) { bpm++; keys_held &= ~KEY_RIGHT; save_write(BPM_SAVE_ADDR, &bpm, 1); }
				if(keys_held & KEY_DOWN) { bpm -= 10; keys_held &= ~KEY_DOWN; save_write(BPM_SAVE_ADDR, &bpm, 1); }
				if(keys_held & KEY_UP) { bpm += 10; keys_held &= ~KEY_UP; save_write(BPM_SAVE_ADDR, &bpm, 1); }
			} else {
				if((keys_held & KEY_LEFT) && !(keys_held & KEY_B) && pos_x != 0) { pos_x--; render_ui(); }
				if((keys_held & KEY_RIGHT) && !(keys_held & KEY_B) && pos_x != UINT16_MAX) { pos_x++; render_ui(); }
				if((keys_held & KEY_UP) && pos_y != 0) { pos_y--; render_ui(); }
				if((keys_held & KEY_DOWN) && pos_y != 24) { pos_y++; render_ui(); }
			}
			if(keys_held & KEY_START) {
				playing = !playing;
				if(playing) {
					if(keys_held & KEY_B) { playing_x_start = 0; for(uint8_t i = 0; i < TRACKS_N; i++) playing_x[i] = 0; }
					else { playing_x_start = pos_x; for(uint8_t i = 0; i < TRACKS_N; i++) playing_x[i] = pos_x; }
					to_reset_timers = true;
				}
				keys_held &= ~KEY_START;
			}
			if((keys_held & KEY_L) && !(keys_held & KEY_B)) {
				keys_held &= ~KEY_L;
				if(track == 0) track = TRACKS_N - 1;
				else track--;
				render_ui();
			}
			if((keys_held & KEY_R) && !(keys_held & KEY_B)) {
				keys_held &= ~KEY_R;
				if(track == TRACKS_N - 1) track = 0;
				else track++;
				render_ui();
			}
		}

		if(playing && get_max_new_notes() > 0) {
			bool played = false;
			for(uint8_t i = 0; i < TRACKS_N; i++) {
				bool flag = false;
				for(uint16_t j = 0; j < TRACK_NOTES_N; j++) {
					if(tracks[(uint16_t)i * TRACK_NOTES_N + j].length == 0) break;
					if(pos(&tracks[(uint16_t)i * TRACK_NOTES_N + j]) < playing_x[i]) continue;
					if(pos(&tracks[(uint16_t)i * TRACK_NOTES_N + j]) < playing_x_start) continue;
					play_note(syllables[tracks[(uint16_t)i * TRACK_NOTES_N + j].syllable], tracks[(uint16_t)i * TRACK_NOTES_N + j].pitch - 7,
						(pos(&tracks[(uint16_t)i * TRACK_NOTES_N + j]) - playing_x_start) * (7680.0f / bpm),
						tracks[(uint16_t)i * TRACK_NOTES_N + j].length * (7680.0f / bpm));
					playing_x[i] = pos(&tracks[(uint16_t)i * TRACK_NOTES_N + j]) + 1;
					played = true;
					if(get_max_new_notes() == 0) { flag = true; break; }
				}
				if(flag) break;
			}
			if(!played) playing = false;
		}
		if(to_reset_timers) reset_timers();
	} while(true);
}
