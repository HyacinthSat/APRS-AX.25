/*

This C program demonstrates the construction of AX.25 frames.
It is intended to serve as a reference for the communication standard design of HyacinthSat.

Part 2: AX.25 Frame Construction
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
uint16_t Stream_Len = 0;            // 比特流长度
uint8_t Bitstream[65536];           // 比特流
uint8_t Address_Counter = 0;        // 地址数量计数器
uint8_t Address_Quantity = 2;       // 地址数量，最小为2，即源站与目标

// 声明内部函数
int Construct_AX25_frame(char *, char *, char *, char *, char *, char *, char *);
int Bitstream_Append_Byte(uint8_t byte);
int Process_Path_Address(char *, char *);
int Process_Address(char *, char *);
int Get_Address_Num(char *);
int Calculate_CRC();
int Bit_Stuffing();
int Add_Frame_Sync();
int NRZI_Process();
int Print_Bitstream_bytes();

// AFSK 1200bd 的调制函数在其他程序内实现
extern int AFSK_1200_Modulation();

// 构建 AX.25 帧
int Construct_AX25_Frame(char *ToCall, char *ToSSID, char *FmCall, char *FmSSID, char *PathCall , char *PathSSID, char *INFO) {

    // 统计地址数量
    Get_Address_Num(PathCall);
    
    // 处理各个地址
    Process_Address(ToCall,ToSSID);
    Process_Address(FmCall,FmSSID);
    Process_Path_Address(PathCall,PathSSID);

    printf("\n");

    // 帧类型指示字段：UI 帧
    Bitstream_Append_Byte(0x03); printf(": 0x03\n");
    // 协议标识符字段：无上层协议
    Bitstream_Append_Byte(0xf0); printf(": 0xf0\n");

    printf("\n信息字段: \n");

    // 构建信息字段
    for(int i = 0; i < strlen(INFO); i++){
        Bitstream_Append_Byte(INFO[i]); printf(":%c\n", INFO[i]);
    }

    printf("\n");

    // 计算CRC
    Calculate_CRC();

    // 输出完整的比特流
    Print_Bitstream_bytes();

    // 数据链路层工作：位填充、添加帧同步标记、NRZI编码
    Bit_Stuffing();
    Add_Frame_Sync();
    NRZI_Process();

    // 物理层工作：将比特流送入AFSK 1200bd 调制函数
    AFSK_1200_Modulation();
    
    return 0;
}

// 从最低有效位开始将输入的字节添加到比特流中
int Bitstream_Append_Byte(uint8_t byte) {
    
    for (int i = 7; i >= 0; i--) { printf("%d", (byte >> i) & 1);} // 调试输出

    if (Stream_Len >= 65536) {
        printf("Error: Bitstream缓冲区溢出！\n");
        return -1;
    }

    for (int i =0; i < 8; i++){
        Bitstream[Stream_Len++] = ((byte >> i) & 0x01) ? 1 : 0;
    }
    return 0;
}

// 统计地址数量
int Get_Address_Num(char *Path) {
    // 计算 Path 中的地址数量，更新 Address_Quantity
    Address_Quantity = 2;
    if (!(strlen(Path) == 1 && Path[0] == ' ')) {
        // 路径数量为分隔符(,)数量 + 1
        Address_Quantity += 1;
        for (int i = 0; Path[i]; i++) {
            if (Path[i] == ',') {
                Address_Quantity++;
            }
        }
    }
}

// 处理呼号、SSID与控制字段，将其加入比特流
int Process_Address(char *CallSign, char *SSID) {

    // 地址计数器
    Address_Counter++;

    // 处理呼号
    printf("\n第 %d 段地址:\n", Address_Counter);
    for (int i = 0; i < 6; i++) {
        // 如果呼号长度不足6位，补充空格（0x20）
        char c = (i < strlen(CallSign)) ? CallSign[i] : 0x20;
        // 移位并添加到比特流
        Bitstream_Append_Byte(c << 1);
        printf(": %c\n", c);
    }

    // 定义拼装所需要的各个参数
    int C; int R; int S; int X; char Ctrl_Byte;
    R = 1;
    S = atoi(SSID);
    Ctrl_Byte = 0;
    
    // 根据地址计数器与地址数量处理控制信息
    if (Address_Counter == 1) {
        C = 1;  // 目标地址
    } else if (Address_Counter == 2) {
        C = 0;  // 源地址
    } else {
        C = 0;  // 路径地址，默认未中继
    }    
    X = (Address_Counter == Address_Quantity) ? 1 : 0;
    
    // 按照C、R、R、SSID、X的顺序拼接
    Ctrl_Byte |= (C & 0x01) << 7;
    Ctrl_Byte |= (R & 0x01) << 6;
    Ctrl_Byte |= (R & 0x01) << 5;
    Ctrl_Byte |= (S & 0x0F) << 1;
    Ctrl_Byte |= (X & 0x01) << 0;

    // 将拼接结果添加到比特流
    Bitstream_Append_Byte(Ctrl_Byte);
    printf(": SSID\n");

    return 0;
}

// 将路径地址分割后加入比特流
int Process_Path_Address(char *PathCall, char *PathSSID) {

    if (PathCall[0] == ' ' && PathCall[1] == '\0') {
        printf("\n无路径地址\n");
        return 0;
    }

    // 独立的进度记录
    char *saveptr1 = NULL;
    char *saveptr2 = NULL;

    // 使用 strtok_r 函数进行分割
    char *call_token = strtok_r(PathCall, ",", &saveptr1);
    char *ssid_token = strtok_r(PathSSID, ",", &saveptr2);

    while (call_token != NULL && ssid_token != NULL) {
        Process_Address(call_token, ssid_token);

        call_token = strtok_r(NULL, ",", &saveptr1);
        ssid_token = strtok_r(NULL, ",", &saveptr2);
    }

    return 0;
}

// 计算CRC值（CRC-16-CCITT算法，低位优先，多项式0x8408）
int Calculate_CRC(){

    uint16_t CRC = 0xFFFF;    // 初始化CRC寄存器
    uint8_t CRCL, CRCH;       // CRC低字节与高字节

    for (int i = 0; i < Stream_Len; i++){
        uint8_t shiftbit = 0x0001 & (uint8_t)CRC;    // 获取当前CRC寄存器最低位
        CRC >>= 1;                                   // 将CRC寄存器右移一位
        if (shiftbit != Bitstream[i]) {              // 如果输入位与CRC寄存器最低位不同
            CRC ^= 0x8408;                           // 将CRC寄存器值与多项式异或
        }
    }
        
    // 取反并依次添加到比特流
    CRC ^= 0xFFFF;
    CRCL = CRC & 0xFF;
    CRCH = (CRC >> 8) & 0xFF;

    Bitstream_Append_Byte(CRCL);
    Bitstream_Append_Byte(CRCH);
    printf(": CRC校验值 0x%04X\n", CRC); 

    return 0;
}

// 位填充：当连续出现5个1时在其后插入一个0
int Bit_Stuffing() {
    // 初始化计数器
    int Count = 0;

    for (int i = 0; i < Stream_Len; i++) {
        // 更新计数器
        if (Bitstream[i] == 1) { Count++; } else { Count = 0; }
        if (Count == 5) {
            // memmove(dst, src, len)函数用法：将从src开始的len个字节的数据复制到dst位置
            memmove(&Bitstream[i + 2], &Bitstream[i + 1], Stream_Len - (i + 1));
            Bitstream[i + 1] = 0;     // 在连续的五个1之后插入一个0
            Stream_Len++;             // 更新比特流长度
            i++; Count = 0;           // 跳过插入的0并重置计数器
        }
    }

    return 0;
}

// 插入同步标记 0x7E
int Add_Frame_Sync() {

    // 将比特流整体后移8位。为帧首的同步标记留出空间
    memmove(&Bitstream[8], &Bitstream[0], Stream_Len);
    
    for (int i = 0; i < 8; i++) {
        Bitstream[i] = (0x7E >> i) & 0x01;
        Bitstream[Stream_Len + 8 + i] = (0x7E >> i) & 0x01;
    }

    Stream_Len += 16;

    return 0;
}

// NRZI编码：0翻转信号，1保持信号
int NRZI_Process(){

    // 初始化状态
    int Status = 0;

    for (int i = 0; i < Stream_Len; i++) {
        if (Bitstream[i] == 0) {
            Status = !Status;  // 当输入0时翻转状态
        }
        Bitstream[i] = Status;  // 写入编码后结果
    }

    return 0;
}

// 调试用，分组打印比特流
int Print_Bitstream_bytes() {

    printf("Bitstream:\n");
    for (int i = 0; i < Stream_Len; i++) {
        printf("%d", Bitstream[i]);
        if ((i + 1) % 8 == 0) printf(" ");
    }
    printf("\n\n");

    return 0;
}

/*

每个地址固定占有七个字节。
前六个字节用于传输呼号，第七个字节用于传输控制字段与SSID，结构如下：

位数 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
作用 | C | R | R |      SSID     | X |

其中C为控制字段，R为保留位（填充1即可），X为终止指示符。
针对源站与目标地址，地址所属的C为0时为源站地址，C为1时为目标地址。
针对中继地址，C为0时表示该地址尚未完成中继，C为1时表示该地址已经完成了中继。

当X为1时，表示地址到此结束。

*/