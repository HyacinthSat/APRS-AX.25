/*

This C program demonstrates the construction of AX.25 frames.
It is intended to serve as a reference for the communication standard design of HyacinthSat.

Part 1: Part 1: APRS Information Formatting and Preprocessing
Version: 0.0.1    Date: April 22, 2025

Developer & Acknowledgments:
    BG7ZDQ - Implemented the program
    BI4PYM - Suggestions & review

License: MIT License

*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

// 定义全局变量
char *filename;

// 声明内部函数
int Preprocessing(char *, char *, char *, char *);
int Parameter_Separation(const char *, char *, char *);

// 程序总入口点
int main(int argc, char *argv[]) {

    // 检查参数
    if (argc != 6 || strcmp(argv[1], "--help") == 0) {
        printf("用法: ./aprs <'ToCall-SSID'> <'FmCall-SSID'> <'Path1-SSID,Path2-SSID'> <'INFO'> <'Output Filename'>\n");
        printf("例如: ./aprs 'BG7ZDQ' 'BI4PYM-1' 'RS0ISS,WIDE2-2' 'Test Message.' 'Output.wav'\n");
        printf("若无路径，填写空格即可\n");
        return 1;
    }

    filename = argv[5];

    // 调用预处理函数
    return Preprocessing(argv[1], argv[2], argv[3], argv[4]);
}


// 参数预处理函数
int Preprocessing(char *To, char *Fm, char *Path, char *INFO) {
    
    // 呼号存储区
    char ToCall[7] = {0};
    char FmCall[7] = {0};
    char PathCall[64] = {0}; 

    // SSID存储区
    char ToSSID[3] = {0};
    char FmSSID[3] = {0};
    char PathSSID[16] = {0};
    
    // 分离目标呼号及其 SSID
    if (Parameter_Separation(To, ToCall, ToSSID) != 0) return 1;

    // 分离源站呼号及其 SSID
    if (Parameter_Separation(Fm, FmCall, FmSSID) != 0) return 1;

    // 分离路径呼号及其 SSID（多个路径）
    char *Token = strtok(Path, ",");
    int Path_Counter = 0;

    while (Token != NULL) {
        char call[7] = {0};  // 暂存单个路径的呼号
        char ssid[3] = {0};  // 暂存单个路径的SSID

        // 分离路径呼号及其 SSID
        if (Parameter_Separation(Token, call, ssid) != 0) return 1;

        // 拼接到路径呼号缓冲区，用逗号分隔
        strcat(PathCall, call);
        strcat(PathCall, ",");
        
        // 拼接到路径呼号缓冲区，用逗号分隔
        strcat(PathSSID, ssid);
        strcat(PathSSID, ",");

        Path_Counter = Path_Counter + 1;
        Token = strtok(NULL, ",");
    }

    // 检查路径数量
    if (Path_Counter > 8) { printf("路径数量不能超过8个！\n"); return 1;}

    // 移除末尾多余的逗号
    if (strlen(PathCall) > 0) PathCall[strlen(PathCall) - 1] = '\0';
    if (strlen(PathSSID) > 0) PathSSID[strlen(PathSSID) - 1] = '\0';

    // 调试信息
    printf("\n");
    printf("FmCall:    %s-%s\n", ToCall, ToSSID);
    printf("ToCall:    %s-%s\n", FmCall, FmSSID);
    printf("------------------------\n");
    printf("PathCall:  %s\n", PathCall);
    printf("PathSSID:  %s\n", PathSSID);
    printf("------------------------\n");
    printf("INFO:      %s\n\n", INFO);

    // 开始构建 AX.25 帧
    Construct_AX25_Frame(ToCall, ToSSID, FmCall, FmSSID, PathCall, PathSSID, INFO);

    return 0;
}

// 参数分离函数
int Parameter_Separation(const char *Input, char *CallSign_buf, char *SSID_buf) {
    
    const char *dash_ptr = strchr(Input, '-');

    if (dash_ptr) {
        // 检查呼号长度是否正确
        size_t call_len = dash_ptr - Input;
        if (call_len > 6) {
            printf("Error: %s \n呼号长度应在6位以内！\n", Input);
            return 1;
        }

        strncpy(CallSign_buf, Input, call_len);
        CallSign_buf[call_len] = '\0';

        strncpy(SSID_buf, dash_ptr + 1, 2);
        SSID_buf[2] = '\0';

        // 检查 SSID 是否在有效范围内
        int ssid = atoi(SSID_buf);
        if (ssid < 0 || ssid > 15) {
            printf("Error: %s \nSSID 不合法！有效范围：0 ~ 15\n", Input);
            return -1;
        }

    } else {
        // 未添加SSID时默认为0
        if (strlen(Input) > 7) {
            printf("Error: %s \n呼号长度应在6位以内！\n", Input);
            return -1;
        }
        strncpy(CallSign_buf, Input, 6);
        CallSign_buf[6] = '\0';
        strcpy(SSID_buf, "0");
    }

    return 0;
}