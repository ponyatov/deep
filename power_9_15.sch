EESchema Schematic File Version 2  date Срд 01 Апр 2015 14:37:43
LIBS:R
LIBS:L
LIBS:C
LIBS:POWER
LIBS:DA_POWER
LIBS:FET
LIBS:SPICE
LIBS:VD
LIBS:SWITCH
LIBS:deep-cache
EELAYER 25  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 7 7
Title "Вторичный источник питания 9..15В"
Date "1 apr 2015"
Rev ""
Comp "(c) DepthGuru, Samara, RU, OpenHardware license"
Comment1 "(by) Dmitry Ponyatov <dponyatov@gmail.com>"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	4150 2200 3600 2200
Wire Wire Line
	3600 2200 3600 2450
Wire Wire Line
	4150 1000 2100 1000
Wire Wire Line
	2100 2200 2400 2200
Wire Wire Line
	2400 2200 2400 2450
$Comp
L 0 #GND02
U 1 1 551BC10C
P 3600 2500
F 0 "#GND02" H 3600 2400 40  0001 C CNN
F 1 "0" H 3600 2430 40  0000 C CNN
	1    3600 2500
	1    0    0    -1  
$EndComp
$Comp
L 0 #GND03
U 1 1 551BC107
P 2400 2500
F 0 "#GND03" H 2400 2400 40  0001 C CNN
F 1 "0" H 2400 2430 40  0000 C CNN
	1    2400 2500
	1    0    0    -1  
$EndComp
Text HLabel 4150 1000 0    60   Output ~ 0
PW_BYPASS
Text HLabel 4150 2200 0    60   Output ~ 0
PW_GND
Text HLabel 2100 2200 0    60   Input ~ 0
GND_IN
Text HLabel 4150 1350 0    60   Output ~ 0
PW_5v
Text HLabel 4150 1700 0    60   Output ~ 0
PW_3v
Text HLabel 2100 1000 0    60   Input ~ 0
PW_IN
$EndSCHEMATC
