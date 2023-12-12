#include <msp430.h>

//Constantes utilizadas
#define TRUE        1
#define LED_RED     BIT0
#define LED_GREEN   BIT6
#define BUTTON      BIT3
#define LDR1_BIT    BIT5
#define LDR2_BIT    BIT7

//Declaração das Funções
void configLED(void);
void configADC10(void);
void configPIN(void);
void configPWM(void);
void configBTN(void);
void configCLK(void);
void servo_esquerda(void);
void servo_direta(void);

//Global
int i;

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

//Funções
void configCLK(void){
    // Ajusta o clock Para 1MHz
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    __delay_cycles(1000);  // Aguarda estabilização do clock
}

void configBTN(void){
    //Configurando o Button
    P1DIR &= ~BUTTON;       // Configura a direção individual do pino (Entrada -> bit = 0)
    P1REN |= BUTTON;        // Habilita o resistor pull-up/pull-down
    P1OUT |= BUTTON;        // Define pull-up
    P1IES |= BUTTON;        // Controla a borda de sensibilidade à iterrupção da porta
    P1IE |= BUTTON;         // Habilita a interrupção
    P1IFG= 0X00;            // Desliga todas as flags de P1
}

void configPWM(void){
    TACTL = TASSEL_2 + MC_1;      // SMCLK submain clock, Upmode
    TACCTL1 = OUTMOD_7;         // CCR1 selection reset-set
}

void configPIN(void){
    // Configuração do Port 2
    P2DIR = 0x00;   // Configura todos os pinos do Port 2 como entrada
    P2OUT = 0x00;   // Configura todos os pinos como saída de baixa impedância

    // Configuração do Port 3
    P3DIR = 0x00;   // Configura todos os pinos do Port 3 como entrada
    P3OUT = 0x00;   // Configura todos os pinos como saída de baixa impedância

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