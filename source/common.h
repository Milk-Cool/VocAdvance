#pragma once

#define TRANSITION_TIME 51
#define TRANSITION_TIME_2 26
#define ONSET_TIME 256
#define VOWEL_TIME 1024

#define RATE(x) (1024 * pow(2, (x) / 12.0f))
#define CRATE(a, x) ((a) * pow(2, (x) / 12.0f))