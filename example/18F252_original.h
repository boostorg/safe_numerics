//////// Standard Header file for the PIC18F252 device ////////////////
#device PIC18F252
#nolist
//////// Program memory: 16384x16  Data RAM: 1536  Stack: 31
//////// I/O: 23   Analog Pins: 5
//////// Data EEPROM: 256
//////// C Scratch area: 00   ID Location: 2000
//////// Fuses: LP,XT,HS,RC,EC,EC_IO,H4,RC_IO,PROTECT,NOPROTECT,OSCSEN
//////// Fuses: NOOSCSEN,NOBROWNOUT,BROWNOUT,WDT1,WDT2,WDT4,WDT8,WDT16,WDT32
//////// Fuses: WDT64,WDT128,WDT,NOWDT,BORV20,BORV27,BORV42,BORV45,PUT,NOPUT
//////// Fuses: CCP2C1,CCP2B3,NOSTVREN,STVREN,NODEBUG,DEBUG,NOLVP,LVP,WRT
//////// Fuses: NOWRT,NOWRTD,WRTD,NOWRTB,WRTB,WRTC,NOWRTC,CPD,NOCPD,CPB
//////// Fuses: NOCPB,EBTR,NOEBTR,EBTRB,NOEBTRB
//////// 
////////////////////////////////////////////////////////////////// I/O
// Discrete I/O Functions: SET_TRIS_x(), OUTPUT_x(), INPUT_x(),
//                         PORT_B_PULLUPS(), INPUT(),
//                         OUTPUT_LOW(), OUTPUT_HIGH(),
//                         OUTPUT_FLOAT(), OUTPUT_BIT()
// Constants used to identify pins in the above are:

#define PIN_A0  31744
#define PIN_A1  31745
#define PIN_A2  31746
#define PIN_A3  31747
#define PIN_A4  31748
#define PIN_A5  31749
#define PIN_A6  31750

#define PIN_B0  31752
#define PIN_B1  31753
#define PIN_B2  31754
#define PIN_B3  31755
#define PIN_B4  31756
#define PIN_B5  31757
#define PIN_B6  31758
#define PIN_B7  31759

#define PIN_C0  31760
#define PIN_C1  31761
#define PIN_C2  31762
#define PIN_C3  31763
#define PIN_C4  31764
#define PIN_C5  31765
#define PIN_C6  31766
#define PIN_C7  31767

////////////////////////////////////////////////////////////////// Useful defines
#define FALSE 0
#define TRUE 1

#define BYTE int
#define BOOLEAN short int

#define getc getch
#define fgetc getch
#define getchar getch
#define putc putchar
#define fputc putchar
#define fgets gets
#define fputs puts

////////////////////////////////////////////////////////////////// Control
// Control Functions:  RESET_CPU(), SLEEP(), RESTART_CAUSE()
// Constants returned from RESTART_CAUSE() are:

#define WDT_TIMEOUT      4     
#define MCLR_FROM_SLEEP  8     
#define NORMAL_POWER_UP  12    
#define BROWNOUT_RESTART 14    

////////////////////////////////////////////////////////////////// Timer 0
// Timer 0 (AKA RTCC)Functions: SETUP_COUNTERS() or SETUP_TIMER0(),
//                              SET_TIMER0() or SET_RTCC(),
//                              GET_TIMER0() or GET_RTCC()
// Constants used for SETUP_TIMER0() are:
#define RTCC_INTERNAL   0
#define RTCC_EXT_L_TO_H 32
#define RTCC_EXT_H_TO_L 48

#define RTCC_DIV_1      8
#define RTCC_DIV_2      0
#define RTCC_DIV_4      1
#define RTCC_DIV_8      2
#define RTCC_DIV_16     3
#define RTCC_DIV_32     4
#define RTCC_DIV_64     5
#define RTCC_DIV_128    6
#define RTCC_DIV_256    7

#define RTCC_OFF        0x80  

#define RTCC_8_BIT      0x40  

// Constants used for SETUP_COUNTERS() are the above
// constants for the 1st param and the following for
// the 2nd param:

////////////////////////////////////////////////////////////////// WDT
// Watch Dog Timer Functions: SETUP_WDT() or SETUP_COUNTERS() (see above)
//                            RESTART_WDT()
//
#define WDT_ON      0x100   
#define WDT_OFF     0       

////////////////////////////////////////////////////////////////// Timer 1
// Timer 1 Functions: SETUP_TIMER_1, GET_TIMER1, SET_TIMER1
// Constants used for SETUP_TIMER_1() are:
//      (or (via |) together constants from each group)
#define T1_DISABLED         0
#define T1_INTERNAL         0x85
#define T1_EXTERNAL         0x87
#define T1_EXTERNAL_SYNC    0x83

#define T1_CLK_OUT          8

#define T1_DIV_BY_1         0
#define T1_DIV_BY_2         0x10
#define T1_DIV_BY_4         0x20
#define T1_DIV_BY_8         0x30

////////////////////////////////////////////////////////////////// Timer 2
// Timer 2 Functions: SETUP_TIMER_2, GET_TIMER2, SET_TIMER2
// Constants used for SETUP_TIMER_2() are:
#define T2_DISABLED         0
#define T2_DIV_BY_1         4
#define T2_DIV_BY_4         5
#define T2_DIV_BY_16        6

////////////////////////////////////////////////////////////////// Timer 3
// Timer 3 Functions: SETUP_TIMER_3, GET_TIMER3, SET_TIMER3
// Constants used for SETUP_TIMER_3() are:
//      (or (via |) together constants from each group)
#define T3_DISABLED         0
#define T3_INTERNAL         0x85
#define T3_EXTERNAL         0x87
#define T3_EXTERNAL_SYNC    0x83

#define T3_DIV_BY_1         0
#define T3_DIV_BY_2         0x10
#define T3_DIV_BY_4         0x20
#define T3_DIV_BY_8         0x30

////////////////////////////////////////////////////////////////// CCP
// CCP Functions: SETUP_CCPx, SET_PWMx_DUTY
// CCP Variables: CCP_x, CCP_x_LOW, CCP_x_HIGH
// Constants used for SETUP_CCPx() are:
#define CCP_OFF                         0
#define CCP_CAPTURE_FE                  4
#define CCP_CAPTURE_RE                  5
#define CCP_CAPTURE_DIV_4               6
#define CCP_CAPTURE_DIV_16              7
#define CCP_COMPARE_SET_ON_MATCH        8
#define CCP_COMPARE_CLR_ON_MATCH        9
#define CCP_COMPARE_INT                 0xA
#define CCP_COMPARE_INT_AND_TOGGLE      0x2       
#define CCP_COMPARE_RESET_TIMER         0xB
#define CCP_PWM                         0xC
#define CCP_PWM_PLUS_1                  0x1c
#define CCP_PWM_PLUS_2                  0x2c
#define CCP_PWM_PLUS_3                  0x3c
#define CCP_USE_TIMER3                  0x100       
long CCP_1;
#byte   CCP_1    =                      0xfbe       
#byte   CCP_1_LOW=                      0xfbe       
#byte   CCP_1_HIGH=                     0xfbf       
long CCP_2;
#byte   CCP_2    =                      0xfbb       
#byte   CCP_2_LOW=                      0xfbb       
#byte   CCP_2_HIGH=                     0xfbc       
////////////////////////////////////////////////////////////////// SPI
// SPI Functions: SETUP_SPI, SPI_WRITE, SPI_READ, SPI_DATA_IN
// Constants used in SETUP_SSP() are:
#define SPI_MASTER       0x20
#define SPI_SLAVE        0x24
#define SPI_L_TO_H       0
#define SPI_H_TO_L       0x10
#define SPI_CLK_DIV_4    0
#define SPI_CLK_DIV_16   1
#define SPI_CLK_DIV_64   2
#define SPI_CLK_T2       3
#define SPI_SS_DISABLED  1

#define SPI_SAMPLE_AT_END 0x8000
#define SPI_XMIT_L_TO_H  0x4000

////////////////////////////////////////////////////////////////// ADC
// ADC Functions: SETUP_ADC(), SETUP_ADC_PORTS() (aka SETUP_PORT_A),
//                SET_ADC_CHANNEL(), READ_ADC()
// Constants used in SETUP_ADC_PORTS() are:
#define NO_ANALOGS             0x86         // None
#define ALL_ANALOG             0x80         // A0 A1 A2 A3 A5 E0 E1 E2 Ref=Vdd
#define ANALOG_RA3_REF         0x81         // A0 A1 A2 A5 E0 E1 E2 Ref=A3
#define A_ANALOG               0x82         // A0 A1 A2 A3 A5 Ref=Vdd
#define A_ANALOG_RA3_REF       0x83         // A0 A1 A2 A5 Ref=A3
#define RA0_RA1_RA3_ANALOG     0x84         // A0 A1 A3 Ref=Vdd
#define RA0_RA1_ANALOG_RA3_REF 0x85         // A0 A1 Ref=A3
#define ANALOG_RA3_RA2_REF              0x88   // A0 A1 A5 E0 E1 E2 Ref=A2,A3
#define ANALOG_NOT_RE1_RE2              0x89   // A0 A1 A2 A3 A5 E0 Ref=Vdd
#define ANALOG_NOT_RE1_RE2_REF_RA3      0x8A   // A0 A1 A2 A5 E0 Ref=A3
#define ANALOG_NOT_RE1_RE2_REF_RA3_RA2  0x8B   // A0 A1 A5 E0 Ref=A2,A3
#define A_ANALOG_RA3_RA2_REF            0x8C   // A0 A1 A5 Ref=A2,A3
#define RA0_RA1_ANALOG_RA3_RA2_REF      0x8D   // A0 A1 Ref=A2,A3
#define RA0_ANALOG                      0x8E   // A0
#define RA0_ANALOG_RA3_RA2_REF          0x8F   // A0 Ref=A2,A3
// Constants used for SETUP_ADC() are:
#define ADC_OFF                0              // ADC Off
#define ADC_CLOCK_DIV_2        1
#define ADC_CLOCK_DIV_4    0x101  
#define ADC_CLOCK_DIV_8     0x41
#define ADC_CLOCK_DIV_16   0x141  
#define ADC_CLOCK_DIV_32    0x81
#define ADC_CLOCK_DIV_64   0x181  
#define ADC_CLOCK_INTERNAL  0xc1              // Internal 2-6us

// Constants used in READ_ADC() are:
#define ADC_START_AND_READ     7   // This is the default if nothing is specified
#define ADC_START_ONLY         1
#define ADC_READ_ONLY          6

////////////////////////////////////////////////////////////////// INT
// Interrupt Functions: ENABLE_INTERRUPTS(), DISABLE_INTERRUPTS(),
//                      EXT_INT_EDGE()
//
// Constants used in EXT_INT_EDGE() are:
#define L_TO_H              0x40
#define H_TO_L                 0
// Constants used in ENABLE/DISABLE_INTERRUPTS() are:
#define GLOBAL                    0xF2C0
#define INT_RTCC                  0xF220
#define INT_TIMER0                0xF220
#define INT_TIMER1                0x9D01
#define INT_TIMER2                0x9D02
#define INT_TIMER3                0xA002
#define INT_EXT                   0xF210
#define INT_EXT1                  0xF008
#define INT_EXT2                  0xF010
#define INT_RB                    0xF208
#define INT_AD                    0x9D40
#define INT_RDA                   0x9D20
#define INT_TBE                   0x9D10
#define INT_SSP                   0x9D08
#define INT_CCP1                  0x9D04
#define INT_CCP2                  0xA001
#define INT_LOWVOLT               0xA004
#define INT_EEPROM                0xA010

#list
