#pragma once


/*


Pin mapping Pico:
=================

----+---------------+-------------------------------------------+----------------------
PIN | Arduino       |   Raspberry PI PICO                       | Application 
    | DXX : default | GPXX : SPI        : I2C       : UART      | pin assignment
----+---------------+-------------------------------------------+----------------------
1   | D0  : SER1_TX | GP0  : SPI0_RX    : I2C0_SDA  : UART0_TX  | DEBUG_SER_TX res.
2   | D1  : SER1_RX | GP1  : SPI0_CSn   : I2C0_SCL  : UART0_RX  | DEBUG_SER_RX res.
3   |---------------|------ GND --------------------------------|------ GND -----------
4   | D2  :         | GP2  : SPI0_SCK   : I2C1_SDA  : UART0_CTS | 
5   | D3  :         | GP3  : SPI0_TX    : I2C1_SCL  : UART0_RTS | 
6   | D4  :WIRE0_SDA| GP4  : SPI0_RX    : I2C0_SDA  : UART1_TX  | I2C_SDA res.
7   | D5  :WIRE0_SCL| GP5  : SPI0_CSn   : I2C0_SCL  : UART1_RX  | I2C_SCL res.
8   |---------------|------ GND --------------------------------|------ GND -----------
9   | D6  :         | GP6  : SPI0_SCK   : I2C1_SDA  : UART1_CTS | Button 1
10  | D7  :         | GP7  : SPI0_TX    : I2C1_SCL  : UART1_RTS | 
11  | D8  : SER2_TX | GP8  : SPI1_RX    : I2C0_SDA  : UART1_TX  | 
12  | D9  : SER2_RX | GP9  : SPI1_CSn   : I2C0_SCL  : UART1_RX  | 
13  |---------------|------ GND --------------------------------|------ GND -----------
14  | D10 :         | GP10 : SPI1_SCK   : I2C1_SDA  : UART1_CTS | 
15  | D11 :         | GP11 : SPI1_TX    : I2C1_SCL  : UART1_RTS | 
16  | D12 :SPI1_MISO| GP12 : SPI1_RX    : I2C0_SDA  : UART0_TX  | WS2812B Line DO  Recomended by adafruit for best performance
17  | D13 :SPI1_CS  | GP13 : SPI1_CSn   : I2C0_SCL  : UART0_RX  | 
18  |---------------|------ GND --------------------------------|------ GND -----------
19  | D14 :SPI1_SCK | GP14 : SPI1_SCK   : I2C1_SDA  : UART0_CTS | 
20  | D15 :SPI1_MOSI| GP15 : SPI1_TX    : I2C1_SCL  : UART0_RTS | 
----+---------------+-------------------------------------------+----------------------
----+---------------+-------------------------------------------+----------------------
21  | D16 :SPI0_MISO| GP16 : SPI0_RX    : I2C0_SDA  : UART0_TX  | 
22  | D17 :SPI0_CS0 | GP17 : SPI0_CSn   : I2C0_SCL  : UART0_RX  | 
23  |---------------|------ GND --------------------------------|------ GND -----------
24  | D18 :SPI0_SCK | GP18 : SPI0_SCK   : I2C1_SDA  : UART0_CTS | 
25  | D19 :SPI0_MOSI| GP19 : SPI0_TX    : I2C1_SCL  : UART0_RTS | 
26  | D20 :         | GP20 : SPI0_RX    : I2C0_SDA  :           | 
27  | D21 :         | GP21 : SPI0_CSn   : I2C0_SCL  :           | 
28  |---------------|------ GND --------------------------------|------ GND -----------
29  | D22 :         | GP22 : SPI0_SCK   :           : UART1_CTS | 
30  |---------------|------ RUN --------------------------------|------ RUN -----------
31  | A0  :WIRE1_SDA| GP26 : SPI1_SCK   : I2C0_SDA  : UART1_CTS |       A0
32  | A1  :WIRE1_SCL| GP27 : SPI1_TX    : I2C0_SCL  : UART1_RTS | 
33  |---------------|------ AGND -------------------------------|------ AGND ----------
34  | A2  :         | GP28 : SPI1_SCK   : I2C0_SDA  : UART1_CTS | 
35  |---------------|------ AREF -------------------------------|------ AREF ----------
36  |---------------|------ 3V3_OUT ----------------------------|------ 3V3_OUT -------
37  |---------------|------ 3V3_EN -----------------------------|------ 3V3_EN --------
38  |---------------|------ GND --------------------------------|------ GND -----------
39  |---------------|------ V_SYS ------------------------------|------ V_SYS ---------
40  |---------------|------ V_BUS ------------------------------|------ V_BUS ---------
----+---------------+-------------------------------------------+----------------------


*/

////////////////////////////////////////
// button
#define PIN_BUTTON0         6


////////////////////////////////////////
// WS2812B
#define PIN_WS2812B         12


////////////////////////////////////////
// ADC / Random
#define PIN_ADC0           31




////////////////////////////////////////
// I2C Display
/* defined in common.h for PICO board :
// I2C
#define PIN_WIRE0_SDA       4
#define PIN_WIRE0_SCL       5
*/







