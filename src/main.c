/*
 * ADC -> PWM -> motor DC
 *
 * Potentiometrul de pe AN0 produce o valoare 0..1023.
 * Valoarea este transformata in duty-cycle pentru CCP1/RC2.
 */
#include <string.h>
#include <xc.h>
#define RA0 0b00000000
#define _XTAL_FREQ 4000000
#define TMR2_pre 4
#define TMR2_factor 0b00000001
#define TMR2_ON 0b00000100
long PWM_freq = 5000;

void init_ADC(char in){
//Intrare RA0 - AN0
ADCON0 = ADCON0 | in;
//Vref = 5V
VCFG1 = 0;
VCFG0 = 0;
//Intrarea RA0 analogica
PCFG3 = 1;
PCFG2 = 1;
PCFG1 = 1;
PCFG0 = 0;
//Aliniere la dreapta
ADFM = 1;
//Tacq = 12*Tad (12*1.2us)
ACQT2 =1;
ACQT1 =0;
ACQT0 =1;
//F = Fosc/32 TADC = 8us
ADCS2 = 0;
ADCS1 = 1;
ADCS0 = 0;
ADRESH = 0;
ADRESL = 0;
__delay_ms(1);
ADON = 1;}

unsigned int ADC_start(){
unsigned int ch=0;
//pornire conversie
GO = 1;
//terminare conversie
while(DONE);
ch = ((ADRESH  <<8) | ADRESL) ;
return (ch);}

void PWM_init(){
PR2 = (_XTAL_FREQ/(PWM_freq*4*TMR2_pre))-1;
CCP1CON = 0b00001100;
CCPR1L = 0x0;
T2CON = T2CON | TMR2_factor | TMR2_ON; //T2CON = 0b00000101
TRISCbits.RC2=0;
TMR2 = 0;
TMR2IF = 0;}

void PWM_duty(unsigned int duty){
duty =  ( (float)duty/1023) * (_XTAL_FREQ / (PWM_freq*TMR2_pre));
CCPR1L = (duty>>2);
duty = (duty << 8);
CCP1CON = CCP1CON  | (duty<<4); //CCP1CON=0b00b1b01100
while(!PIR1bits.TMR2IF);}

void main(void){
unsigned int  v = RA0, y; 
init_ADC(v);
PWM_init();
while(1){
y = ADC_start();
PWM_duty(y); // duty = 0...1023
__delay_ms(50); }
}
