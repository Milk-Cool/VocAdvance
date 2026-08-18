#include "engine.h"
#include <gba.h>
#include <maxmod.h>
#include <string.h>
#include <math.h>

#include "common.h"

#include "soundbank.h"
#include "soundbank_bin.h"

#define TM2D (*(volatile unsigned short*)0x04000108)
#define TM2CNT (*(volatile unsigned short*)0x0400010A)
#define TM3D (*(volatile unsigned short*)0x0400010C)
#define TM3CNT (*(volatile unsigned short*)0x0400010E)

void init_engine() {
	irqInit();

	irqSet(IRQ_VBLANK, mmVBlank);
	irqEnable(IRQ_VBLANK);
    
    mmInitDefault((mm_addr)soundbank_bin, 8);

    TM2D = 0;
    TM3D = 0;
    TM2CNT = 0x0080;
    TM3CNT = 0x0084;
}
uint32_t mibis() {
    return (((uint32_t)TM3D << 16) | TM2D) >> 14;
}
void reset_timers() {
    TM2D = 0;
    TM3D = 0;
}
typedef struct {
    const char* name;
    mm_word id;
} Syllable;
static Syllable syllables[] = {
    { "a", SFX_A },
    { "ba", SFX_BA },
    { "be", SFX_BE },
    { "bi", SFX_BI },
    { "bo", SFX_BO },
    { "bu", SFX_BU },
    { "bya", SFX_BYA },
    { "bye", SFX_BYE },
    { "byo", SFX_BYO },
    { "byu", SFX_BYU },
    { "cha", SFX_CHA },
    { "che", SFX_CHE },
    { "chi", SFX_CHI },
    { "cho", SFX_CHO },
    { "chu", SFX_CHU },
    { "da", SFX_DA },
    { "dei", SFX_DEI },
    { "de", SFX_DE },
    { "deyu", SFX_DEYU },
    { "dou", SFX_DOU },
    { "do", SFX_DO },
    { "e", SFX_E },
    { "fua", SFX_FUA },
    { "fuo", SFX_FUO },
    { "fu", SFX_FU },
    { "fye", SFX_FYE },
    { "fyi", SFX_FYI },
    { "ga", SFX_GA },
    { "ge", SFX_GE },
    { "gi", SFX_GI },
    { "go", SFX_GO },
    { "gu", SFX_GU },
    { "gya", SFX_GYA },
    { "gye", SFX_GYE },
    { "gyo", SFX_GYO },
    { "gyu", SFX_GYU },
    { "ha", SFX_HA },
    { "he", SFX_HE },
    { "hi", SFX_HI },
    { "ho", SFX_HO },
    { "hya", SFX_HYA },
    { "hye", SFX_HYE },
    { "hyo", SFX_HYO },
    { "hyu", SFX_HYU },
    { "ie", SFX_IE },
    { "i", SFX_I },
    { "ja", SFX_JA },
    { "je", SFX_JE },
    { "ji", SFX_JI },
    { "jo", SFX_JO },
    { "ju", SFX_JU },
    { "ka", SFX_KA },
    { "ke", SFX_KE },
    { "ki", SFX_KI },
    { "ko", SFX_KO },
    { "ku", SFX_KU },
    { "kya", SFX_KYA },
    { "kye", SFX_KYE },
    { "kyo", SFX_KYO },
    { "kyu", SFX_KYU },
    { "ma", SFX_MA },
    { "me", SFX_ME },
    { "mi", SFX_MI },
    { "mo", SFX_MO },
    { "mu", SFX_MU },
    { "mya", SFX_MYA },
    { "mye", SFX_MYE },
    { "myo", SFX_MYO },
    { "myu", SFX_MYU },
    { "na", SFX_NA },
    { "ne", SFX_NE },
    { "ni", SFX_NI },
    { "no", SFX_NO },
    { "nu", SFX_NU },
    { "n", SFX_N },
    { "nya", SFX_NYA },
    { "nye", SFX_NYE },
    { "nyo", SFX_NYO },
    { "nyu", SFX_NYU },
    { "o", SFX_O },
    { "pa", SFX_PA },
    { "pe", SFX_PE },
    { "pi", SFX_PI },
    { "po", SFX_PO },
    { "pu", SFX_PU },
    { "pya", SFX_PYA },
    { "pye", SFX_PYE },
    { "pyo", SFX_PYO },
    { "pyu", SFX_PYU },
    { "ra", SFX_RA },
    { "re", SFX_RE },
    { "ri", SFX_RI },
    { "ro", SFX_RO },
    { "ru", SFX_RU },
    { "rya", SFX_RYA },
    { "rye", SFX_RYE },
    { "ryo", SFX_RYO },
    { "ryu", SFX_RYU },
    { "sa", SFX_SA },
    { "se", SFX_SE },
    { "sha", SFX_SHA },
    { "she", SFX_SHE },
    { "shi", SFX_SHI },
    { "sho", SFX_SHO },
    { "shu", SFX_SHU },
    { "so", SFX_SO },
    { "sui", SFX_SUI },
    { "su", SFX_SU },
    { "ta", SFX_TA },
    { "tei", SFX_TEI },
    { "te", SFX_TE },
    { "teyu", SFX_TEYU },
    { "tou", SFX_TOU },
    { "to", SFX_TO },
    { "tsua", SFX_TSUA },
    { "tsue", SFX_TSUE },
    { "tsui", SFX_TSUI },
    { "tsuo", SFX_TSUO },
    { "tsu", SFX_TSU },
    { "ue", SFX_UE },
    { "ui", SFX_UI },
    { "uo", SFX_UO },
    { "u", SFX_U },
    { "wa", SFX_WA },
    { "wo", SFX_WO },
    { "ya", SFX_YA },
    { "yo", SFX_YO },
    { "yu", SFX_YU },
    { "za", SFX_ZA },
    { "ze", SFX_ZE },
    { "zo", SFX_ZO },
    { "zui", SFX_ZUI },
    { "zu", SFX_ZU }
};
typedef struct {
    mm_sound_effect sfx;
    mm_sfxhand handle;
    uint32_t start;
    uint16_t duration;
    int16_t pitch;
    bool started;
    bool playing;
} PlayingNote;
static PlayingNote playing_notes[16];
static uint8_t playing_idx;
static void add_note(mm_sound_effect sfx, uint32_t start_mibis, uint16_t duration_mibis, uint16_t pitch) {
    PlayingNote* ptr = &playing_notes[playing_idx++ % (sizeof(playing_notes) / sizeof(playing_notes[0]))];
    ptr->sfx = sfx;
    ptr->start = start_mibis;
    ptr->duration = duration_mibis;
    ptr->pitch = pitch;
    ptr->started = false;
    ptr->playing = true;
}
static mm_sound_effect find(const char* lyric) {
    for(uint16_t i = 0; i < sizeof(syllables) / sizeof(syllables[0]); i++) {
        if(!strcmp(lyric, syllables[i].name)) return (mm_sound_effect){
            { syllables[i].id },
            (int)(1.0f * (1<<10)),
            0,
            255,
            128
        };
    }
    return (mm_sound_effect){
        { SFX_TE },
        (int)(1.0f * (1<<10)),
        0,
        255,
        128
    }; // safe default
};
void play_note(const char* lyric, int16_t pitch, uint32_t start_mibis, uint16_t duration_mibis) {
    char cvowel = lyric[strlen(lyric) - 1];
    const char vowel[2] = { cvowel, 0 };
    if(!strcmp(lyric, vowel)) {
        mm_sound_effect sfx = find(lyric);
        add_note(sfx, start_mibis, duration_mibis, pitch);
    } else {
        mm_sound_effect sfx_onset = find(lyric);
        add_note(sfx_onset, start_mibis, duration_mibis < ONSET_TIME ? duration_mibis : ONSET_TIME, pitch);
        mm_sound_effect sfx_vowel = find(vowel);
        uint32_t vowel_mibis = start_mibis + ONSET_TIME - TRANSITION_TIME;
        add_note(sfx_vowel, vowel_mibis, (duration_mibis < ONSET_TIME ? 0 : duration_mibis - ONSET_TIME) + TRANSITION_TIME, pitch);
    }
}
void play_note_now(const char* lyric, int16_t pitch, uint32_t duration_mibis) {
    play_note(lyric, pitch, mibis(), duration_mibis);
}
void loop_engine() {
    uint32_t cur = mibis();
    for(uint8_t i = 0; i < sizeof(playing_notes) / sizeof(playing_notes[0]); i++) {
        PlayingNote* ptr = &playing_notes[i];
        if(!ptr->playing) continue;
        if(cur < ptr->start) continue;
        if(!ptr->started) {
            ptr->handle = mmEffectEx(&ptr->sfx);
            mmEffectRate(ptr->handle, RATE(ptr->pitch));
            ptr->started = true;
        }
        if(cur >= ptr->start + ptr->duration) {
            mmEffectCancel(ptr->handle);
            ptr->playing = false;
        }
    }
}