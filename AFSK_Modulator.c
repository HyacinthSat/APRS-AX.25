/*

This C program demonstrates the construction of AX.25 frames.
It is intended to serve as a reference for the communication standard design of HyacinthSat.

Part 3: AFSK baseband Modulator
Version: 0.0.1    Date: April 22, 2025

Developer & Acknowledgments:
    BG7ZDQ - Implemented the program
    BI4PYM - Suggestions & review

License: MIT License

*/

#include <stdio.h>
#include <stdint.h>
#include "header.h"

// 获取外部变量
extern uint16_t Stream_Len;
extern uint8_t Bitstream[65536];

// 声明内部函数
int AFSK_300_Modulation();
int AFSK_600_Modulation();
int AFSK_1200_Modulation();
int AFSK_2400_Modulation();
int AFSK_Modulator(double SPACE_TONE, double MARK_TONE, uint16_t BAUD_RATE);

// AFSK 300bd 基带调制函数
int AFSK_300_Modulation(){

    double SPACE_TONE = 1900;   // 空号 (0)
    double MARK_TONE = 1600;    // 传号 (1)
    uint16_t BAUD_RATE = 300;   // 波特率

    return AFSK_Modulator(SPACE_TONE, MARK_TONE, BAUD_RATE);
}

// AFSK 600bd 基带调制函数
int AFSK_600_Modulation(){

    double SPACE_TONE = 1930;   // 空号 (0)
    double MARK_TONE = 1470;    // 传号 (1)
    uint16_t BAUD_RATE = 600;   // 波特率

    return AFSK_Modulator(SPACE_TONE, MARK_TONE, BAUD_RATE);
}

// AFSK 1200bd 基带调制函数
int AFSK_1200_Modulation(){

    double SPACE_TONE = 2200;   // 空号 (0)
    double MARK_TONE = 1200;    // 传号 (1)
    uint16_t BAUD_RATE = 1200;  // 波特率
    
    return AFSK_Modulator(SPACE_TONE, MARK_TONE, BAUD_RATE);
}

// AFSK 2400bd 基带调制函数
int AFSK_2400_Modulation(){

    double SPACE_TONE = 2600;   // 空号 (0)
    double MARK_TONE = 800;     // 传号 (1)
    uint16_t BAUD_RATE = 2400;  // 波特率

    return AFSK_Modulator(SPACE_TONE, MARK_TONE, BAUD_RATE);
}

// AFSK 基础基带调制函数
int AFSK_Modulator(double SPACE_TONE, double MARK_TONE, uint16_t BAUD_RATE){

    double symbol_duration = 1000.0 / BAUD_RATE;

    printf("APRS %d bd\n", BAUD_RATE);
    WAV_Initialization();

    for (int i = 0; i < Stream_Len; i++){
        if (Bitstream[i] == 1){
            WAV_Write(MARK_TONE, symbol_duration);
        } else {
            WAV_Write(SPACE_TONE, symbol_duration);
        }  
    }

    WAV_Finalization();
    return 0;
}