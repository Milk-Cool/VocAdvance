import { spawnSync } from "child_process";
import { join } from "path";
import { writeFileSync } from "fs";
import { toRomaji } from "wanakana";

const SYLLABLES = `か,あ が,い き,い ぎ,い く,う ぐ,う け,え げ,え こ,お ご,お さ,あ ざ,あ し,い じ,い す,う ず,う せ,え ぜ,え そ,お ぞ,お た,あ だ,あ ち,い つ,う て,え で,え と,お ど,お な,あ に,い ぬ,う ね,え の,お は,あ ば,あ ぱ,あ ひ,い び,い ぴ,い ふ,う ぶ,う ぷ,う へ,え べ,え ぺ,え ほ,お ぼ,お ぽ,お ま,あ み,い む,う め,え も,お や,あ ゆ,う よ,お ら,あ り,い る,う れ,え ろ,お わ,あ を,お あ,あ い,い う,う え,え お,お いぇ,え うぃ,い うぇ,え うぉ,お カ,あ ガ,あ キ,い ギ,い きぇ,え ぎぇ,え きゃ,あ ぎゃ,あ きゅ,う ぎゅ,う きょ,お ぎょ,お ク,う グ,う ケ,え ゲ,え コ,お ゴ,お しぇ,え じぇ,え しゃ,あ じゃ,あ しゅ,う じゅ,う しょ,お じょ,お すぃ,い ずぃ,い ちぇ,え ちゃ,あ ちゅ,う ちょ,お つぁ,あ つぃ,い つぇ,え つぉ,お てぃ,い でぃ,い てゅ,う でゅ,う とぅ,う どぅ,う にぇ,え にゃ,あ にゅ,う にょ,お ひぇ,え びぇ,え ぴぇ,え ひゃ,あ びゃ,あ ぴゃ,あ ひゅ,う びゅ,う ぴゅ,う ひょ,お びょ,お ぴょ,お ふぁ,あ ふぃ,い ふぇ,え ふぉ,お みぇ,え みゃ,あ みゅ,う みょ,お りぇ,え りゃ,あ りゅ,う りょ,お ん,ん`.split(" ").map(x => x.split(","));
const ONSETS = SYLLABLES.map(x => x[0]);
const VOWELS = Array.from(new Set(SYLLABLES.map(x => x[1])));
console.log(ONSETS, VOWELS);

const ONSET_DURATION = 250;
const VOWEL_DURATION = 1000;
const TRANSITION = 50;

for(const onset of ONSETS) {
    const inp = VOWELS.includes(onset) ? onset.repeat(20) : onset;
    const { stdout: wav1, stderr: err1 } = spawnSync("espeak-ng", ["--load", "-v", "voice.espeak", "-s", "70", "-g", "0", "-P", "0", "-p", process.env.PITCH || "50", "--stdout", inp]);
    process.stderr.write(err1);
    const duration = VOWELS.includes(onset) ? VOWEL_DURATION : ONSET_DURATION;
    const { stdout: wav2, stderr: err2 } = spawnSync("ffmpeg", ["-i", "-", "-af", `silenceremove=start_periods=1:start_duration=0:start_threshold=-100dB:detection=peak,aformat=dblp,areverse,silenceremove=start_periods=1:start_duration=0:start_threshold=-100dB:detection=peak,aformat=dblp,areverse${/*,afade=t=in:st=0:d=${TRANSITION / 1000}*/""},afade=t=out:st=${(duration - TRANSITION) / 1000}:d=${TRANSITION / 1000}`, "-to", (duration / 1000).toString(), "-f", "wav", "-ar", "8000", "-ac", "1", "-"], { input: wav1 });
    process.stderr.write(err2);
    writeFileSync(join("out", toRomaji(onset) + ".wav"), wav2);
}