EESchema Schematic File Version 2
LIBS:R
LIBS:L
LIBS:C
LIBS:POWER
LIBS:DA_POWER
LIBS:FET
LIBS:SPICE
LIBS:VD
LIBS:SWITCH
LIBS:MAXIM
LIBS:deep-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 7
Title ""
Date "1 apr 2015"
Rev ""
Comp "(c) DepthGuru, Samara, RU, OpenHardware license"
Comment1 "(by) Dmitry Ponyatov <dponyatov@gmail.com>"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 5300 3600 2    60   Input ~ 0
VCC5
Text HLabel 3750 4300 0    60   Input ~ 0
RX_RS232
Text HLabel 3750 4200 0    60   Output ~ 0
TX_RS232
Text HLabel 5000 4700 0    60   Input ~ 0
GND_RS232
Text HLabel 5050 4300 2    60   Input ~ 0
RX_TTL
Text HLabel 5050 4200 2    60   Output ~ 0
TX_TTL
Text HLabel 3900 4700 0    60   Input ~ 0
GND_TTL
$Comp
L MAX232 D1
U 1 1 55229849
P 4400 3900
AR Path="/551BBEAB/55229849" Ref="D1"  Part="1" 
AR Path="/551BB4F9/55229849" Ref="D2"  Part="1" 
F 0 "D2" H 4400 4350 60  0000 C CNN
F 1 "MAX232" H 4400 3350 60  0000 C CNN
F 2 "" H 4400 3900 60  0000 C CNN
F 3 "" H 4400 3900 60  0000 C CNN
	1    4400 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 3600 3950 3600
Wire Wire Line
	2800 3600 2700 3600
Wire Wire Line
	2700 3600 2700 3800
Wire Wire Line
	2700 3800 3950 3800
Wire Wire Line
	3950 3900 2600 3900
Wire Wire Line
	2100 4000 3950 4000
$Comp
L 0 #GND6
U 1 1 55229965
P 5250 3800
AR Path="/551BBEAB/55229965" Ref="#GND6"  Part="1" 
AR Path="/551BB4F9/55229965" Ref="#GND12"  Part="1" 
F 0 "#GND12" H 5250 3700 40  0001 C CNN
F 1 "0" H 5250 3730 40  0000 C CNN
F 2 "" H 5250 3800 60  0000 C CNN
F 3 "" H 5250 3800 60  0000 C CNN
	1    5250 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 3700 5250 3700
Wire Wire Line
	5250 3700 5250 3750
Wire Wire Line
	4850 3600 5300 3600
$Comp
L 0 #GND4
U 1 1 55229AF8
P 4100 4800
AR Path="/551BBEAB/55229AF8" Ref="#GND4"  Part="1" 
AR Path="/551BB4F9/55229AF8" Ref="#GND10"  Part="1" 
F 0 "#GND10" H 4100 4700 40  0001 C CNN
F 1 "0" H 4100 4730 40  0000 C CNN
F 2 "" H 4100 4800 60  0000 C CNN
F 3 "" H 4100 4800 60  0000 C CNN
	1    4100 4800
	1    0    0    -1  
$EndComp
$Comp
L 0 #GND5
U 1 1 55229B0F
P 5200 4800
AR Path="/551BBEAB/55229B0F" Ref="#GND5"  Part="1" 
AR Path="/551BB4F9/55229B0F" Ref="#GND11"  Part="1" 
F 0 "#GND11" H 5200 4700 40  0001 C CNN
F 1 "0" H 5200 4730 40  0000 C CNN
F 2 "" H 5200 4800 60  0000 C CNN
F 3 "" H 5200 4800 60  0000 C CNN
	1    5200 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 4700 5200 4700
Wire Wire Line
	5200 4700 5200 4750
Wire Wire Line
	3900 4700 4100 4700
Wire Wire Line
	4100 4700 4100 4750
Wire Wire Line
	3750 4200 3950 4200
Wire Wire Line
	3750 4300 3950 4300
Wire Wire Line
	4850 4300 5050 4300
Wire Wire Line
	4850 4200 5050 4200
$Comp
L CP C5
U 1 1 5522A126
P 5200 2850
AR Path="/551BBEAB/5522A126" Ref="C5"  Part="1" 
AR Path="/551BB4F9/5522A126" Ref="C11"  Part="1" 
F 0 "C11" H 5200 3100 60  0000 C CNN
F 1 "1uF" H 5350 2750 60  0000 C CNN
F 2 "" H 5275 2850 60  0000 C CNN
F 3 "" H 5275 2850 60  0000 C CNN
	1    5200 2850
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 5522A215
P 5200 3250
AR Path="/551BBEAB/5522A215" Ref="C6"  Part="1" 
AR Path="/551BB4F9/5522A215" Ref="C12"  Part="1" 
F 0 "C12" H 5100 3350 60  0000 C CNN
F 1 "11pF" H 5350 3150 60  0000 C CNN
F 2 "" H 5275 3250 60  0000 C CNN
F 3 "" H 5275 3250 60  0000 C CNN
	1    5200 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 2850 4950 2850
Wire Wire Line
	4950 2850 4950 3600
Connection ~ 4950 3600
Wire Wire Line
	5050 3250 4950 3250
Connection ~ 4950 3250
$Comp
L 0 #GND7
U 1 1 5522A281
P 5500 3400
AR Path="/551BBEAB/5522A281" Ref="#GND7"  Part="1" 
AR Path="/551BB4F9/5522A281" Ref="#GND13"  Part="1" 
F 0 "#GND13" H 5500 3300 40  0001 C CNN
F 1 "0" H 5500 3330 40  0000 C CNN
F 2 "" H 5500 3400 60  0000 C CNN
F 3 "" H 5500 3400 60  0000 C CNN
	1    5500 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 2850 5500 2850
Wire Wire Line
	5500 2850 5500 3350
Wire Wire Line
	5350 3250 5500 3250
Connection ~ 5500 3250
Wire Wire Line
	2600 3900 2600 3600
Wire Wire Line
	2600 3600 2450 3600
Wire Wire Line
	2150 3600 2100 3600
Wire Wire Line
	2100 3600 2100 4000
$Comp
L CP C1
U 1 1 5522AAC1
P 2300 3600
AR Path="/551BBEAB/5522AAC1" Ref="C1"  Part="1" 
AR Path="/551BB4F9/5522AAC1" Ref="C7"  Part="1" 
F 0 "C7" H 2300 3850 60  0000 C CNN
F 1 "1uF" H 2150 3500 60  0000 C CNN
F 2 "" H 2375 3600 60  0000 C CNN
F 3 "" H 2375 3600 60  0000 C CNN
	1    2300 3600
	-1   0    0    -1  
$EndComp
$Comp
L CP C2
U 1 1 5522AAEE
P 2950 3600
AR Path="/551BBEAB/5522AAEE" Ref="C2"  Part="1" 
AR Path="/551BB4F9/5522AAEE" Ref="C8"  Part="1" 
F 0 "C8" H 2950 3850 60  0000 C CNN
F 1 "1uF" H 2800 3500 60  0000 C CNN
F 2 "" H 3025 3600 60  0000 C CNN
F 3 "" H 3025 3600 60  0000 C CNN
	1    2950 3600
	-1   0    0    -1  
$EndComp
$Comp
L CP C3
U 1 1 5522AD13
P 3300 3100
AR Path="/551BBEAB/5522AD13" Ref="C3"  Part="1" 
AR Path="/551BB4F9/5522AD13" Ref="C9"  Part="1" 
F 0 "C9" H 3300 3350 60  0000 C CNN
F 1 "1uF" H 3300 2850 60  0000 C CNN
F 2 "" H 3375 3100 60  0000 C CNN
F 3 "" H 3375 3100 60  0000 C CNN
	1    3300 3100
	1    0    0    -1  
$EndComp
$Comp
L CP C4
U 1 1 5522ADAC
P 3800 3100
AR Path="/551BBEAB/5522ADAC" Ref="C4"  Part="1" 
AR Path="/551BB4F9/5522ADAC" Ref="C10"  Part="1" 
F 0 "C10" H 3800 3350 60  0000 C CNN
F 1 "1uF" H 3800 2850 60  0000 C CNN
F 2 "" H 3875 3100 60  0000 C CNN
F 3 "" H 3875 3100 60  0000 C CNN
	1    3800 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 3700 3600 3700
Wire Wire Line
	3600 3700 3600 3100
Wire Wire Line
	3600 3100 3650 3100
Wire Wire Line
	3950 4100 3500 4100
Wire Wire Line
	3500 4100 3500 3100
Wire Wire Line
	3500 3100 3450 3100
$Comp
L 0 #GND2
U 1 1 5522AF14
P 3050 3200
AR Path="/551BBEAB/5522AF14" Ref="#GND2"  Part="1" 
AR Path="/551BB4F9/5522AF14" Ref="#GND8"  Part="1" 
F 0 "#GND8" H 3050 3100 40  0001 C CNN
F 1 "0" H 3050 3130 40  0000 C CNN
F 2 "" H 3050 3200 60  0000 C CNN
F 3 "" H 3050 3200 60  0000 C CNN
	1    3050 3200
	1    0    0    -1  
$EndComp
$Comp
L 0 #GND3
U 1 1 5522AF2F
P 4050 3200
AR Path="/551BBEAB/5522AF2F" Ref="#GND3"  Part="1" 
AR Path="/551BB4F9/5522AF2F" Ref="#GND9"  Part="1" 
F 0 "#GND9" H 4050 3100 40  0001 C CNN
F 1 "0" H 4050 3130 40  0000 C CNN
F 2 "" H 4050 3200 60  0000 C CNN
F 3 "" H 4050 3200 60  0000 C CNN
	1    4050 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 3100 4050 3100
Wire Wire Line
	4050 3100 4050 3150
Wire Wire Line
	3150 3100 3050 3100
Wire Wire Line
	3050 3100 3050 3150
$EndSCHEMATC
