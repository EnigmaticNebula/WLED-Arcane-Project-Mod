#include <math.h>

// #define NOTE_C0 (16*(8192/7902))
// #define NOTE_C1 (33*(8192/7902))
// #define NOTE_C2 (65*(8192/7902))
// #define NOTE_C3 (131*(8192/7902))
// #define NOTE_C4 (262*(8192/7902))
// #define NOTE_C5 (523*(8192/7902))
// #define NOTE_C6 (1047*(8192/7902))
// #define NOTE_C7 (2093*(8192/7902))
// #define NOTE_C8 (4186*(8192/7902))

// #define NOTE_CS0 (17*(8192/7902))
// #define NOTE_CS1 (35*(8192/7902))
// #define NOTE_CS2 (69*(8192/7902))
// #define NOTE_CS3 (139*(8192/7902))
// #define NOTE_CS4 (277*(8192/7902))
// #define NOTE_CS5 (554*(8192/7902))
// #define NOTE_CS6 (1109*(8192/7902))
// #define NOTE_CS7 (2217*(8192/7902))
// #define NOTE_CS8 (4435*(8192/7902))

// #define NOTE_Db0 (17*(8192/7902))
// #define NOTE_Db1 (35*(8192/7902))
// #define NOTE_Db2 (69*(8192/7902))
// #define NOTE_Db3 (139*(8192/7902))
// #define NOTE_Db4 (277*(8192/7902))
// #define NOTE_Db5 (554*(8192/7902))
// #define NOTE_Db6 (1109*(8192/7902))
// #define NOTE_Db7 (2217*(8192/7902))
// #define NOTE_Db8 (4435*(8192/7902))

// #define NOTE_D0 (18*(8192/7902))
// #define NOTE_D1 (37*(8192/7902))
// #define NOTE_D2 (73*(8192/7902))
// #define NOTE_D3 (147*(8192/7902))
// #define NOTE_D4 (294*(8192/7902))
// #define NOTE_D5 (587*(8192/7902))
// #define NOTE_D6 (1175*(8192/7902))
// #define NOTE_D7 (2349*(8192/7902))
// #define NOTE_D8 (4699*(8192/7902))

// #define NOTE_DS0 (19*(8192/7902))
// #define NOTE_DS1 (39*(8192/7902))
// #define NOTE_DS2 (78*(8192/7902))
// #define NOTE_DS3 (156*(8192/7902))
// #define NOTE_DS4 (311*(8192/7902))
// #define NOTE_DS5 (622*(8192/7902))
// #define NOTE_DS6 (1245*(8192/7902))
// #define NOTE_DS7 (2489*(8192/7902))
// #define NOTE_DS8 (4978*(8192/7902))

// #define NOTE_Eb0 (19*(8192/7902))
// #define NOTE_Eb1 (39*(8192/7902))
// #define NOTE_Eb2 (78*(8192/7902))
// #define NOTE_Eb3 (156*(8192/7902))
// #define NOTE_Eb4 (311*(8192/7902))
// #define NOTE_Eb5 (622*(8192/7902))
// #define NOTE_Eb6 (1245*(8192/7902))
// #define NOTE_Eb7 (2489*(8192/7902))
// #define NOTE_Eb8 (4978*(8192/7902))

// #define NOTE_E0 (21*(8192/7902))
// #define NOTE_E1 (41*(8192/7902))
// #define NOTE_E2 (82*(8192/7902))
// #define NOTE_E3 (165*(8192/7902))
// #define NOTE_E4 (330*(8192/7902))
// #define NOTE_E5 (659*(8192/7902))
// #define NOTE_E6 (1319*(8192/7902))
// #define NOTE_E7 (2637*(8192/7902))
// #define NOTE_E8 (5274*(8192/7902))

// #define NOTE_F0 (22*(8192/7902))
// #define NOTE_F1 (44*(8192/7902))
// #define NOTE_F2 (87*(8192/7902))
// #define NOTE_F3 (175*(8192/7902))
// #define NOTE_F4 (349*(8192/7902))
// #define NOTE_F5 (698*(8192/7902))
// #define NOTE_F6 (1397*(8192/7902))
// #define NOTE_F7 (2794*(8192/7902))
// #define NOTE_F8 (5588*(8192/7902))

// #define NOTE_FS0 (23*(8192/7902))
// #define NOTE_FS1 (46*(8192/7902))
// #define NOTE_FS2 (93*(8192/7902))
// #define NOTE_FS3 (185*(8192/7902))
// #define NOTE_FS4 (370*(8192/7902))
// #define NOTE_FS5 (740*(8192/7902))
// #define NOTE_FS6 (1480*(8192/7902))
// #define NOTE_FS7 (2960*(8192/7902))
// #define NOTE_FS8 (5920*(8192/7902))

// #define NOTE_Gb0 (23*(8192/7902))
// #define NOTE_Gb1 (46*(8192/7902))
// #define NOTE_Gb2 (93*(8192/7902))
// #define NOTE_Gb3 (185*(8192/7902))
// #define NOTE_Gb4 (370*(8192/7902))
// #define NOTE_Gb5 (740*(8192/7902))
// #define NOTE_Gb6 (1480*(8192/7902))
// #define NOTE_Gb7 (2960*(8192/7902))
// #define NOTE_Gb8 (5920*(8192/7902))

// #define NOTE_G0 (25*(8192/7902))
// #define NOTE_G1 (49*(8192/7902))
// #define NOTE_G2 (98*(8192/7902))
// #define NOTE_G3 (196*(8192/7902))
// #define NOTE_G4 (392*(8192/7902))
// #define NOTE_G5 (784*(8192/7902))
// #define NOTE_G6 (1568*(8192/7902))
// #define NOTE_G7 (3136*(8192/7902))
// #define NOTE_G8 (6272*(8192/7902))

// #define NOTE_GS0 (26*(8192/7902))
// #define NOTE_GS1 (52*(8192/7902))
// #define NOTE_GS2 (104*(8192/7902))
// #define NOTE_GS3 (208*(8192/7902))
// #define NOTE_GS4 (415*(8192/7902))
// #define NOTE_GS5 (831*(8192/7902))
// #define NOTE_GS6 (1661*(8192/7902))
// #define NOTE_GS7 (3322*(8192/7902))
// #define NOTE_GS8 (6645*(8192/7902))

// #define NOTE_Ab0 (26*(8192/7902))
// #define NOTE_Ab1 (52*(8192/7902))
// #define NOTE_Ab2 (104*(8192/7902))
// #define NOTE_Ab3 (208*(8192/7902))
// #define NOTE_Ab4 (415*(8192/7902))
// #define NOTE_Ab5 (831*(8192/7902))
// #define NOTE_Ab6 (1661*(8192/7902))
// #define NOTE_Ab7 (3322*(8192/7902))
// #define NOTE_Ab8 (6645*(8192/7902))

// #define NOTE_A0 (28*(8192/7902))
// #define NOTE_A1 (55*(8192/7902))
// #define NOTE_A2 (110*(8192/7902))
// #define NOTE_A3 (220*(8192/7902))
// #define NOTE_A4 (440*(8192/7902))
// #define NOTE_A5 (880*(8192/7902))
// #define NOTE_A6 (1760*(8192/7902))
// #define NOTE_A7 (3520*(8192/7902))
// #define NOTE_A8 (7040*(8192/7902))

// #define NOTE_AS0 (29*(8192/7902))
// #define NOTE_AS1 (58*(8192/7902))
// #define NOTE_AS2 (117*(8192/7902))
// #define NOTE_AS3 (233*(8192/7902))
// #define NOTE_AS4 (466*(8192/7902))
// #define NOTE_AS5 (932*(8192/7902))
// #define NOTE_AS6 (1865*(8192/7902))
// #define NOTE_AS7 (3729*(8192/7902))
// #define NOTE_AS8 (7459*(8192/7902))

// #define NOTE_Bb0 (29*(8192/7902))
// #define NOTE_Bb1 (58 *(8192/7902))
// #define NOTE_Bb2 (117*(8192/7902))
// #define NOTE_Bb3 (233*(8192/7902))
// #define NOTE_Bb4 (466*(8192/7902))
// #define NOTE_Bb5 (932*(8192/7902))
// #define NOTE_Bb6 (1865*(8192/7902))
// #define NOTE_Bb7 (3729*(8192/7902))
// #define NOTE_Bb8 (7459*(8192/7902))

// #define NOTE_B0 (31*(8192/7902))
// #define NOTE_B1 (62*(8192/7902))
// #define NOTE_B2 (123*(8192/7902))
// #define NOTE_B3 (247*(8192/7902))
// #define NOTE_B4 (494*(8192/7902))
// #define NOTE_B5 (988*(8192/7902))
// #define NOTE_B6 (1976*(8192/7902))
// #define NOTE_B7 (3951*(8192/7902))
// #define NOTE_B8 (7902*(8192/7902))


// These tones are multiplied by 8192/7902.
// 8192 is the highest value for 13 bit resolution
// 7902 is the highest frequency note, B8


#define NOTE_C0 17
#define NOTE_C1 34
#define NOTE_C2 67
#define NOTE_C3 136
#define NOTE_C4 272
#define NOTE_C5 542
#define NOTE_C6 1085
#define NOTE_C7 2170
#define NOTE_C8 4340

#define NOTE_CS0 18
#define NOTE_CS1 36
#define NOTE_CS2 72
#define NOTE_CS3 144
#define NOTE_CS4 287
#define NOTE_CS5 574
#define NOTE_CS6 1150
#define NOTE_CS7 2298
#define NOTE_CS8 4598

#define NOTE_Db0 18
#define NOTE_Db1 36
#define NOTE_Db2 72
#define NOTE_Db3 144
#define NOTE_Db4 287
#define NOTE_Db5 574
#define NOTE_Db6 1150
#define NOTE_Db7 2298
#define NOTE_Db8 4598

#define NOTE_D0 19
#define NOTE_D1 38
#define NOTE_D2 76
#define NOTE_D3 152
#define NOTE_D4 305
#define NOTE_D5 609
#define NOTE_D6 1218
#define NOTE_D7 2435
#define NOTE_D8 4871

#define NOTE_DS0 20
#define NOTE_DS1 40
#define NOTE_DS2 81
#define NOTE_DS3 162
#define NOTE_DS4 322
#define NOTE_DS5 645
#define NOTE_DS6 1291
#define NOTE_DS7 2580
#define NOTE_DS8 5161

#define NOTE_Eb0 20
#define NOTE_Eb1 40
#define NOTE_Eb2 81
#define NOTE_Eb3 162
#define NOTE_Eb4 322
#define NOTE_Eb5 645
#define NOTE_Eb6 1291
#define NOTE_Eb7 2580
#define NOTE_Eb8 5161

#define NOTE_E0 22
#define NOTE_E1 43
#define NOTE_E2 85
#define NOTE_E3 171
#define NOTE_E4 342
#define NOTE_E5 683
#define NOTE_E6 1367
#define NOTE_E7 2734
#define NOTE_E8 5468

#define NOTE_F0 23
#define NOTE_F1 46
#define NOTE_F2 90
#define NOTE_F3 181
#define NOTE_F4 362
#define NOTE_F5 724
#define NOTE_F6 1448
#define NOTE_F7 2897
#define NOTE_F8 5793

#define NOTE_FS0 24
#define NOTE_FS1 48
#define NOTE_FS2 96
#define NOTE_FS3 192
#define NOTE_FS4 384
#define NOTE_FS5 767
#define NOTE_FS6 1534
#define NOTE_FS7 3069
#define NOTE_FS8 6137

#define NOTE_Gb0 24
#define NOTE_Gb1 48
#define NOTE_Gb2 96
#define NOTE_Gb3 192
#define NOTE_Gb4 384
#define NOTE_Gb5 767
#define NOTE_Gb6 1534
#define NOTE_Gb7 3069
#define NOTE_Gb8 6137

#define NOTE_G0 26
#define NOTE_G1 51
#define NOTE_G2 102
#define NOTE_G3 203
#define NOTE_G4 406
#define NOTE_G5 813
#define NOTE_G6 1626
#define NOTE_G7 3251
#define NOTE_G8 6502

#define NOTE_GS0 27
#define NOTE_GS1 54
#define NOTE_GS2 108
#define NOTE_GS3 216
#define NOTE_GS4 430
#define NOTE_GS5 861
#define NOTE_GS6 1722
#define NOTE_GS7 3444
#define NOTE_GS8 6889

#define NOTE_Ab0 27
#define NOTE_Ab1 54
#define NOTE_Ab2 108
#define NOTE_Ab3 216
#define NOTE_Ab4 430
#define NOTE_Ab5 861
#define NOTE_Ab6 1722
#define NOTE_Ab7 3444
#define NOTE_Ab8 6889

#define NOTE_A0 29
#define NOTE_A1 57
#define NOTE_A2 114
#define NOTE_A3 228
#define NOTE_A4 456
#define NOTE_A5 912
#define NOTE_A6 1825
#define NOTE_A7 3649
#define NOTE_A8 7298

#define NOTE_AS0 30
#define NOTE_AS1 60
#define NOTE_AS2 121
#define NOTE_AS3 242
#define NOTE_AS4 483
#define NOTE_AS5 966
#define NOTE_AS6 1933
#define NOTE_AS7 3866
#define NOTE_AS8 7733

#define NOTE_Bb0 30
#define NOTE_Bb1 60
#define NOTE_Bb2 121
#define NOTE_Bb3 242
#define NOTE_Bb4 483
#define NOTE_Bb5 966
#define NOTE_Bb6 1933
#define NOTE_Bb7 3866
#define NOTE_Bb8 7733

#define NOTE_B0 32
#define NOTE_B1 64
#define NOTE_B2 128
#define NOTE_B3 256
#define NOTE_B4 512
#define NOTE_B5 1024
#define NOTE_B6 2048
#define NOTE_B7 4096
#define NOTE_B8 8192

