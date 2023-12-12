#include <msp430.h>
#include "declaracoes.h"
#include "funcoes.h"

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;

    __enable_interrupt();

    configCLK();
    configPIN();
    configADC10();
    configPWM();
    configBTN();

    while(TRUE) {

        ADC10CTL0 &= ~ENC;

        while (ADC10CTL1 & ADC10BUSY); //Espera até que a conversão seja concluída

        ADC10CTL0 |= ENC + ADC10SC; //Inicia manualmente a próxima conversão

        __bis_SR_register(CPUOFF);

        __delay_cycles(1000);  // Adiciona um pequeno atraso entre as conversões
    }
}

//Interrupções
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
    int resultado1 = ADC10MEM;
    ADC10CTL1 ^= INCH_5;        // Troca para o próximo canal (LDR2)
    ADC10CTL0 |= ADC10SC;       // Inicia a próxima conversão
    int resultado2 = ADC10MEM;

    if (resultado1 >= resultado2) {
        P1OUT &= ~LED_GREEN;    //Apaga o verde
        P1OUT |= LED_RED;       //Liga o LED vermelho
        servo_esquerda();
    } else {
        P1OUT &= ~LED_RED;      //Apaga o vermelho
        P1OUT |= LED_GREEN;     //Liga o LED verde
        servo_direta();
    }

    ADC10CTL0 &= ~ADC10IFG;     // Limpa a flag de interrupção do ADC10
    __bic_SR_register_on_exit(CPUOFF);
}

#pragma vector=PORT1_VECTOR
__interrupt void interrupt_reset_servo(void){
    if(P1IFG & BUTTON){
        P1OUT |= LED_RED;       //Liga o LED vermelho
        servo_esquerda();
        servo_direta();
        TACCR0 = 20000;
        for (i=350; i<=1350; i++) {
            TACCR1 = i;
            __delay_cycles(500);
        }
        P1IFG &= ~BUTTON;
        P1OUT &= ~LED_RED;
    }
}
