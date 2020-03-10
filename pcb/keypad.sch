EESchema Schematic File Version 4
LIBS:pcb-cache
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 7 8
Title "Bric"
Date "2020-03-09"
Rev "R0.1"
Comp "EE300"
Comment1 "zephray@outlook.com"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text GLabel 6000 3600 0    50   Output ~ 0
SW_ST
Text GLabel 5500 4500 0    50   Output ~ 0
SW_RI
Text GLabel 6900 4300 0    50   Output ~ 0
SW_A
Text GLabel 6500 4700 0    50   Output ~ 0
SW_B
Text GLabel 4500 4500 0    50   Output ~ 0
SW_LE
Text GLabel 5000 4100 0    50   Output ~ 0
SW_UP
Text GLabel 5200 3600 0    50   Output ~ 0
SW_SE
Text GLabel 5000 4900 0    50   Output ~ 0
SW_DN
$Comp
L power:GND #PWR0288
U 1 1 5D28BC76
P 5400 4100
F 0 "#PWR0288" H 5400 3850 50  0001 C CNN
F 1 "GND" H 5405 3927 50  0000 C CNN
F 2 "" H 5400 4100 50  0001 C CNN
F 3 "" H 5400 4100 50  0001 C CNN
	1    5400 4100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0287
U 1 1 5D28B91D
P 4900 4500
F 0 "#PWR0287" H 4900 4250 50  0001 C CNN
F 1 "GND" H 4905 4327 50  0000 C CNN
F 2 "" H 4900 4500 50  0001 C CNN
F 3 "" H 4900 4500 50  0001 C CNN
	1    4900 4500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0286
U 1 1 5D28B509
P 5400 4900
F 0 "#PWR0286" H 5400 4650 50  0001 C CNN
F 1 "GND" H 5405 4727 50  0000 C CNN
F 2 "" H 5400 4900 50  0001 C CNN
F 3 "" H 5400 4900 50  0001 C CNN
	1    5400 4900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0285
U 1 1 5D28B128
P 5900 4500
F 0 "#PWR0285" H 5900 4250 50  0001 C CNN
F 1 "GND" H 5905 4327 50  0000 C CNN
F 2 "" H 5900 4500 50  0001 C CNN
F 3 "" H 5900 4500 50  0001 C CNN
	1    5900 4500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0284
U 1 1 5D28AD69
P 6900 4700
F 0 "#PWR0284" H 6900 4450 50  0001 C CNN
F 1 "GND" H 6905 4527 50  0000 C CNN
F 2 "" H 6900 4700 50  0001 C CNN
F 3 "" H 6900 4700 50  0001 C CNN
	1    6900 4700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0283
U 1 1 5D28AA10
P 7300 4300
F 0 "#PWR0283" H 7300 4050 50  0001 C CNN
F 1 "GND" H 7305 4127 50  0000 C CNN
F 2 "" H 7300 4300 50  0001 C CNN
F 3 "" H 7300 4300 50  0001 C CNN
	1    7300 4300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0282
U 1 1 5D289F76
P 5600 3600
F 0 "#PWR0282" H 5600 3350 50  0001 C CNN
F 1 "GND" H 5605 3427 50  0000 C CNN
F 2 "" H 5600 3600 50  0001 C CNN
F 3 "" H 5600 3600 50  0001 C CNN
	1    5600 3600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0281
U 1 1 5D288EE2
P 6400 3600
F 0 "#PWR0281" H 6400 3350 50  0001 C CNN
F 1 "GND" H 6405 3427 50  0000 C CNN
F 2 "" H 6400 3600 50  0001 C CNN
F 3 "" H 6400 3600 50  0001 C CNN
	1    6400 3600
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW701
U 1 1 5D2828BB
P 6200 3600
F 0 "SW701" H 6200 3885 50  0000 C CNN
F 1 "SW_Push" H 6200 3794 50  0000 C CNN
F 2 "footprints:contact" H 6200 3800 50  0001 C CNN
F 3 "~" H 6200 3800 50  0001 C CNN
	1    6200 3600
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW700
U 1 1 5D281FA3
P 5400 3600
F 0 "SW700" H 5400 3885 50  0000 C CNN
F 1 "SW_Push" H 5400 3794 50  0000 C CNN
F 2 "footprints:contact" H 5400 3800 50  0001 C CNN
F 3 "~" H 5400 3800 50  0001 C CNN
	1    5400 3600
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW703
U 1 1 5D27EABD
P 6700 4700
F 0 "SW703" H 6700 4985 50  0000 C CNN
F 1 "SW_Push" H 6700 4894 50  0000 C CNN
F 2 "footprints:contact" H 6700 4900 50  0001 C CNN
F 3 "~" H 6700 4900 50  0001 C CNN
	1    6700 4700
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW702
U 1 1 5D27E22C
P 7100 4300
F 0 "SW702" H 7100 4585 50  0000 C CNN
F 1 "SW_Push" H 7100 4494 50  0000 C CNN
F 2 "footprints:contact" H 7100 4500 50  0001 C CNN
F 3 "~" H 7100 4500 50  0001 C CNN
	1    7100 4300
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW706
U 1 1 5D27D8DE
P 5700 4500
F 0 "SW706" H 5700 4785 50  0000 C CNN
F 1 "SW_Push" H 5700 4694 50  0000 C CNN
F 2 "footprints:contact" H 5700 4700 50  0001 C CNN
F 3 "~" H 5700 4700 50  0001 C CNN
	1    5700 4500
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW707
U 1 1 5D27CCB2
P 5200 4900
F 0 "SW707" H 5200 5185 50  0000 C CNN
F 1 "SW_Push" H 5200 5094 50  0000 C CNN
F 2 "footprints:contact" H 5200 5100 50  0001 C CNN
F 3 "~" H 5200 5100 50  0001 C CNN
	1    5200 4900
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW705
U 1 1 5D27BD89
P 4700 4500
F 0 "SW705" H 4700 4785 50  0000 C CNN
F 1 "SW_Push" H 4700 4694 50  0000 C CNN
F 2 "footprints:contact" H 4700 4700 50  0001 C CNN
F 3 "~" H 4700 4700 50  0001 C CNN
	1    4700 4500
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW704
U 1 1 5D2797BD
P 5200 4100
F 0 "SW704" H 5200 4385 50  0000 C CNN
F 1 "SW_Push" H 5200 4294 50  0000 C CNN
F 2 "footprints:contact" H 5200 4300 50  0001 C CNN
F 3 "~" H 5200 4300 50  0001 C CNN
	1    5200 4100
	1    0    0    -1  
$EndComp
$EndSCHEMATC
