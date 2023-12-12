/*
 * funcoes.h
 *
 *  Created on: Dec 12, 2023
 *      Author: gacla
 */

#ifndef FUNCOES_H_
#define FUNCOES_H_

void configLED(void);
void configADC10(void);
void configPIN(void);
void configPWM(void);
void configBTN(void);
void configCLK(void);
void servo_esquerda(void);
void servo_direta(void);

void configCLK(void){
    // Ajusta o clock Para 1MHz
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    __delay_cycles(1000);  // Aguarda estabiliza��o do clock
}

void configBTN(void){
    //Configurando o Button
    P1DIR &= ~BUTTON;       // Configura a dire��o individual do pino (Entrada -> bit = 0)
    P1REN |= BUTTON;        // Habilita o resistor pull-up/pull-down
    P1OUT |= BUTTON;        // Define pull-up
    P1IES |= BUTTON;        // Controla a borda de sensibilidade � iterrup��o da porta
    P1IE |= BUTTON;         // Habilita a interrup��o
    P1IFG= 0X00;            // Desliga todas as flags de P1
}

void configPWM(void){
    TACTL = TASSEL_2 + MC_1;      // SMCLK submain clock, Upmode
    TACCTL1 = OUTMOD_7;         // CCR1 selection reset-set
}

void configPIN(void){
    // Configura��o do Port 2
    P2DIR = 0x00;   // Configura todos os pinos do Port 2 como entrada
    P2OUT = 0x00;   // Configura todos os pinos como sa�da de baixa imped�ncia

    // Configura��o do Port 3
    P3DIR = 0x00;   // Configura todos os pinos do Port 3 como entrada
    P3OUT = 0x00;   // Configura todos os pinos como sa�da de baixa imped�ncia

    //Saida para o Servo Motor
    P1DIR |= BIT6;
    P1SEL |= BIT6;

    configLED();
}

void configLED(void) {
    P1DIR |= LED_RED;
    P1OUT &= ~LED_RED;

    P1DIR |= LED_GREEN;
    P1OUT &= ~LED_GREEN;
}

void configADC10(void) {
    ADC10AE0 |= LDR1_BIT + LDR2_BIT; // Configura os pinos dos LDRs

    ADC10CTL1 |= INCH_5;        // Configura o primeiro canal (LDR1)
    ADC10CTL1 |= SHS_0;
    ADC10CTL1 |= ADC10DIV_7;
    ADC10CTL1 |= ADC10SSEL_0;
    ADC10CTL1 |= CONSEQ_3;

    ADC10CTL0 = SREF_0;
    ADC10CTL0 |= ADC10SHT_0;
    ADC10CTL0 |= ADC10ON;
    ADC10CTL0 |= ADC10IE;
}

void servo_esquerda(void){
    TACCR0 = 20000;         //PWM period
    for (i=350; i<=2350; i++) {
        TACCR1 = i;
        __delay_cycles(1000);
    }
    //TACCR1 = 2350;
}

void servo_direta(void){
    TACCR0 = 20000;  //PWM period
    for (i=2350; i>=350; i--) {
        TACCR1 = i;
        __delay_cycles(1000);
    }
    //  TACCR1 = 350;
}

#endif /* FUNCOES_H_ */
