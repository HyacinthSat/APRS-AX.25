/*

This C program demonstrates the construction of AX.25 frames.
It is intended to serve as a reference for the communication standard design of HyacinthSat.

Part 5: Header File
Version: 0.0.1    Date: April 22, 2025

Developer & Acknowledgments:
    BG7ZDQ - Implemented the program
    BI4PYM - Suggestions & review

License: MIT License

*/

#ifndef HEADER_H
#define HEADER_H

// 声明程序全局函数
int Construct_AX25_Frame(char *, char *, char *, char *, char *, char *, char *);

int AFSK_300_Modulation();
int AFSK_600_Modulation();
int AFSK_1200_Modulation();
int AFSK_2400_Modulation();

int WAV_Initialization();
int WAV_Finalization();
int WAV_Write(double, double);

#endif