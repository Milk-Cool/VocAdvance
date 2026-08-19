import { spawnSync } from "child_process";
import { join } from "path";
import { writeFileSync } from "fs";
import { toRomaji } from "wanakana";

const SYLLABLES = `か,あ が,い き,い ぎ,い く,う ぐ,う け,え げ,え こ,お ご,お さ,あ ざ,あ し,い じ,い す,う ず,う せ,え ぜ,え そ,お ぞ,お た,あ だ,あ ち,い つ,う て,え で,え と,お ど,お な,あ に,い ぬ,う ね,え の,お は,あ ば,あ ぱ,あ ひ,い び,い ぴ,い ふ,う ぶ,う ぷ,う へ,え べ,え ぺ,え ほ,お ぼ,お ぽ,お ま,あ み,い む,う め,え も,お や,あ ゆ,う よ,お ら,あ り,い る,う れ,え ろ,お わ,あ を,お あ,あ い,い う,う え,え お,お しゃ,あ じゃ,あ しゅ,う じゅ,う しょ,お じょ,お ちゃ,あ ちゅ,う ちょ,お にゃ,あ にゅ,う にょ,お ひゃ,あ びゃ,あ ぴゃ,あ ひゅ,う びゅ,う ぴゅ,う ひょ,お びょ,お ぴょ,お みゃ,あ みゅ,う みょ,お りゃ,あ りゅ,う りょ,お ぎゃ,あ ぎゅ,う ぎょ,お きゃ,あ きゅ,う きょ,お ん,ん`.split(" ").map(x => x.split(","));
const ONSETS = SYLLABLES.map(x => x[0]);
const VOWELS = Array.from(new Set(SYLLABLES.map(x => x[1])));
console.log(ONSETS, VOWELS);
const TEMPLATE = `name Japanese
language ja
phonemes ja

intonation 4
pitch %p %p`;

const ONSET_DURATION = 250;
const VOWEL_DURATION = 1000;
const TRANSITION = 50;

for(const [name, pitch] of [["f3", 174.61], ["fs3", 185], ["g3", 196], ["gs3", 207.65], ["a3", 220], ["as3", 233.08], ["b3", 246.94], ["c4", 261.64], ["cs4", 277.18], ["d4", 293.67], ["ds4", 311.13], ["e4", 329.63], ["f4", 349.23], ["fs4", 369.99], ["g4", 392], ["gs4", 415.3], ["a4", 440], ["as4", 466.16], ["b4", 493.88], ["c5", 523.25], ["cs5", 554.37], ["d5", 587.33], ["ds5", 622.25], ["e5", 659.26], ["f5", 698.46]])
    for(const onset of ONSETS) {
        writeFileSync("voice.espeak", TEMPLATE.replaceAll("%p", Math.round(pitch).toString()));
        const inp = VOWELS.includes(onset) ? onset.repeat(20) : onset;
        const { stdout: wav1, stderr: err1 } = spawnSync("espeak-ng", ["--load", "-v", "voice.espeak", "-s", "70", "-g", "0", "-P", "0", "-p", process.env.PITCH || "50", "--stdout", inp]);
        process.stderr.write(err1);
        const duration = VOWELS.includes(onset) ? VOWEL_DURATION : ONSET_DURATION;
        const { stdout: wav2, stderr: err2 } = spawnSync("ffmpeg", ["-i", "-", "-af", `silenceremove=start_periods=1:start_duration=0:start_threshold=-100dB:detection=peak,aformat=dblp,areverse,silenceremove=start_periods=1:start_duration=0:start_threshold=-100dB:detection=peak,aformat=dblp,areverse${/*,afade=t=in:st=0:d=${TRANSITION / 1000}*/""},afade=t=out:st=${(duration - TRANSITION) / 1000}:d=${TRANSITION / 1000}`, "-to", (duration / 1000).toString(), "-f", "wav", "-ar", "8000", "-ac", "1", "-"], { input: wav1 });
        process.stderr.write(err2);
        writeFileSync(join("out", name + "_" + toRomaji(onset) + ".wav"), wav2);
    }