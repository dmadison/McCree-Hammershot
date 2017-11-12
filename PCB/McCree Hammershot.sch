EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:McCree-Hammershot
LIBS:McCree Hammershot-cache
EELAYER 26 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "McCree Hammershot Controller"
Date "2017-09-04"
Rev "v1.0.0"
Comp "Parts Not Included"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Arduino_Pro-Micro U1
U 1 1 59AC7CD7
P 5600 3550
F 0 "U1" H 5925 2825 60  0000 C CNN
F 1 "Arduino_Pro-Micro" H 5600 4281 60  0000 C CNN
F 2 "McCree-Hammershot:Arduino_Pro-Micro_Headers_Gaps" V 5200 3900 60  0001 C CNN
F 3 "" V 5200 3900 60  0000 C CNN
	1    5600 3550
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 J1
U 1 1 59AC7F98
P 6600 4650
F 0 "J1" V 6700 4700 50  0000 L CNN
F 1 "Trigger Conn" V 6700 4650 50  0000 R CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 6600 4650 50  0001 C CNN
F 3 "" H 6600 4650 50  0001 C CNN
	1    6600 4650
	0    1    1    0   
$EndComp
$Comp
L CONN_01X02 J2
U 1 1 59AC80CF
P 6950 4650
F 0 "J2" V 7050 4550 50  0000 L CNN
F 1 "Hammer Conn" V 7050 4700 50  0000 L CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 6950 4650 50  0001 C CNN
F 3 "" H 6950 4650 50  0001 C CNN
	1    6950 4650
	0    1    1    0   
$EndComp
$Comp
L CONN_01X03 J3
U 1 1 59AC812C
P 4700 4650
F 0 "J3" V 4800 4700 50  0000 L CNN
F 1 "Rotary Enc. Conn" V 4800 4650 50  0000 R CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 4700 4650 50  0001 C CNN
F 3 "" H 4700 4650 50  0001 C CNN
	1    4700 4650
	0    1    1    0   
$EndComp
$Comp
L CONN_01X04 J4
U 1 1 59AC85C5
P 7850 4650
F 0 "J4" V 7950 4500 50  0000 L CNN
F 1 "LED Conn" V 7950 4700 50  0000 L CNN
F 2 "Connectors_JST:JST_EH_B04B-EH-A_04x2.50mm_Straight" H 7850 4650 50  0001 C CNN
F 3 "" H 7850 4650 50  0001 C CNN
	1    7850 4650
	0    1    1    0   
$EndComp
Text Label 6450 3100 2    60   ~ 0
GND
Text Label 6250 4100 0    60   ~ 0
Trigger
Text Label 6250 4000 0    60   ~ 0
Hammer
Text Label 6550 4250 3    60   ~ 0
GND
Text Label 6900 4250 3    60   ~ 0
GND
Text Label 4700 4250 3    60   ~ 0
GND
Text Label 7700 4250 3    60   ~ 0
GND
Text Label 7900 4450 1    60   ~ 0
LED_Data
$Comp
L R R1
U 1 1 59ACA65A
P 6900 3700
F 0 "R1" V 7000 3750 50  0000 C CNN
F 1 "1M" V 6800 3700 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 6830 3700 50  0001 C CNN
F 3 "" H 6900 3700 50  0001 C CNN
	1    6900 3700
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 59ACA6E5
P 7250 3700
F 0 "R2" V 7350 3800 50  0000 C CNN
F 1 "1M" V 7150 3700 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7180 3700 50  0001 C CNN
F 3 "" H 7250 3700 50  0001 C CNN
	1    7250 3700
	0    1    1    0   
$EndComp
$Comp
L R R3
U 1 1 59ACA70B
P 7600 3700
F 0 "R3" V 7700 3800 50  0000 C CNN
F 1 "1M" V 7500 3700 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7530 3700 50  0001 C CNN
F 3 "" H 7600 3700 50  0001 C CNN
	1    7600 3700
	0    1    1    0   
$EndComp
Wire Wire Line
	6200 4100 6650 4100
Wire Wire Line
	6200 4000 7000 4000
Wire Wire Line
	6650 4100 6650 4450
Wire Wire Line
	6550 4450 6550 4250
Wire Wire Line
	7000 4000 7000 4450
Wire Wire Line
	6900 4450 6900 4250
Wire Wire Line
	4600 3800 4600 4450
Wire Wire Line
	4800 4450 4800 3900
Wire Wire Line
	4700 4450 4700 4250
Wire Wire Line
	7800 3700 7800 4450
Wire Wire Line
	8000 3200 8000 4450
Wire Wire Line
	6200 3100 6450 3100
Wire Wire Line
	7700 4450 7700 4250
Wire Wire Line
	6200 3500 7000 3500
Wire Wire Line
	7400 3700 7450 3700
Wire Wire Line
	7050 3700 7100 3700
Wire Wire Line
	6650 3700 6750 3700
Wire Wire Line
	7750 3700 7800 3700
NoConn ~ 6200 3200
NoConn ~ 5000 3000
NoConn ~ 5000 3100
Wire Wire Line
	5000 3200 4900 3200
Wire Wire Line
	4900 3200 4900 3300
Wire Wire Line
	4900 3300 5000 3300
Wire Wire Line
	4900 3250 4600 3250
Connection ~ 4900 3250
Text Label 4600 3250 0    60   ~ 0
GND
NoConn ~ 5000 4100
NoConn ~ 5000 4000
NoConn ~ 5000 3700
NoConn ~ 5000 3600
$Comp
L MPU6050 U2
U 1 1 59ACCF11
P 3950 3650
F 0 "U2" H 4150 3100 60  0000 L CNN
F 1 "MPU6050" H 3750 4200 60  0000 L CNN
F 2 "McCree-Hammershot:MPU6050_Breakout" H 3150 3600 60  0001 C CNN
F 3 "" H 3150 3600 60  0001 C CNN
	1    3950 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 3200 6600 3200
Wire Wire Line
	6600 3200 6600 3300
Wire Wire Line
	6600 3300 6200 3300
NoConn ~ 6200 3000
Text Label 6250 3300 0    60   ~ 0
+5V
Text Label 8000 4450 1    60   ~ 0
+5V
Text Label 7800 4450 1    60   ~ 0
CapSense
Text Label 4600 4450 1    60   ~ 0
EncoderB
Text Label 4800 4450 1    60   ~ 0
EncoderA
Text Label 7000 4450 1    60   ~ 0
Hammer
Text Label 6650 4450 1    60   ~ 0
Trigger
Wire Wire Line
	3450 3300 3200 3300
Text Label 3200 3300 0    60   ~ 0
+5V
Wire Wire Line
	3450 3400 3200 3400
Text Label 3200 3400 0    60   ~ 0
GND
Wire Wire Line
	3450 3500 3200 3500
Wire Wire Line
	3450 3600 3200 3600
Text Label 3200 3500 0    60   ~ 0
SCL
Text Label 3200 3600 0    60   ~ 0
SDA
Wire Wire Line
	5000 3400 4600 3400
Wire Wire Line
	5000 3500 4600 3500
Text Label 4600 3500 0    60   ~ 0
SCL
Text Label 4600 3400 0    60   ~ 0
SDA
NoConn ~ 3450 3700
NoConn ~ 3450 3800
NoConn ~ 3450 3900
NoConn ~ 3450 4000
$Comp
L R R4
U 1 1 59ACF27C
P 7450 3400
F 0 "R4" V 7550 3500 50  0000 C CNN
F 1 "470" V 7350 3400 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7380 3400 50  0001 C CNN
F 3 "" H 7450 3400 50  0001 C CNN
	1    7450 3400
	0    1    1    0   
$EndComp
Wire Wire Line
	7600 3400 7900 3400
Wire Wire Line
	4800 3900 5000 3900
Wire Wire Line
	5000 3800 4600 3800
Wire Wire Line
	6200 3900 7800 3900
Connection ~ 7800 3900
Text Label 6250 3900 0    60   ~ 0
CapSense
NoConn ~ 6200 3800
NoConn ~ 6200 3700
Wire Wire Line
	6650 3400 6200 3400
NoConn ~ 6200 3600
Wire Wire Line
	6650 3400 6650 3700
Text Label 6200 3400 0    60   ~ 0
CapSource
Wire Wire Line
	7900 3400 7900 4450
Wire Wire Line
	7300 3400 7000 3400
Wire Wire Line
	7000 3400 7000 3500
$EndSCHEMATC
