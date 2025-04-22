# AX.25

本仓库用于示范 AX.25 数据帧的构建，以备后续通信制式设计的参考需求。

## 目录  

- [APRS Formatte](https://github.com/HyacinthSat/AX.25/blob/main/APRS_Formatte.c): 预处理数据
- [AX25 Construct](https://github.com/HyacinthSat/AX.25/blob/main/AX25_Construct.c): 构建 AX.25 帧
- [AFSK Modulator](https://github.com/HyacinthSat/AX.25/blob/main/AFSK_Modulator.c): 生成 AFSK 基带信号
- [WAV Encapsulation](https://github.com/HyacinthSat/AX.25/blob/main/WAV_Encapsulation.c): 将信号封装入 WAV 容器

## 编译  

```
gcc APRS_Formatte.c AX25_Construct.c AFSK_Modulator.c WAV_Encapsulation.c -o aprs
```

## 用法  

用法:  
```
./aprs <'ToCall-SSID'> <'FmCall-SSID'> <'Path1-SSID,Path2-SSID'> <'INFO'> <'Output Filename'>
```  

例如:  
```
./aprs 'BG7ZDQ' 'BI4PYM-1' 'RS0ISS,WIDE2-2' 'Test Message.' 'Output.wav'
```  

因协议的限制，呼号长度不得超过六位，路径地址不得超过八个。  
若无路径，填写空格即可。  

## 注意

- 目前仅保证支持gcc编译器。  
- 目前没有实现对APRS格式的完整支持。  

## 许可证  

本仓库在 MIT 许可证下发布。  
这意味着：

- 任何人均可自由使用、复制、修改本仓库中的内容，包括个人和商业用途。

- 允许以原始或修改后的形式分发本仓库的内容，但须附带原许可证声明，确保后续用户了解其授权条款。

- 本仓库内容按“现状（as-is）”提供，不附带任何明示或暗示的担保，使用者需自行承担风险。

- 仓库贡献者无需对使用本内容造成的任何损失或问题负责。

- 其余未说明但未尽的事项。更多信息请参见：[LICENSE](https://github.com/HyacinthSat/AX.25/blob/main/LICENSE)

**注意**：该许可证条款仅应用于本项目组所著之内容（除特有说明外），在其之外的各类内容均遵循源许可证或版权要求。
