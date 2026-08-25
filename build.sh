#!/bin/bash

voice() {
    echo BUILD $1
    make clean
    VOICE=vb_$1 make
    mv VocAdvance.gba VocAdvance-$1.gba
}
voice a1
voice m1
voice f1
voice f2