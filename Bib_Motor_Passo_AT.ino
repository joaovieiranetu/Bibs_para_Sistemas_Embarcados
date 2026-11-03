#include <avr/io.h>

void delay_motor(unsigned int ms){  //Função criada para configurar um delay em ms
  TCCR1B = 0;  //Para o Timer1 antes de começar a contagem
  TCCR1A = 0;  //Zera o registrador de configuração do Timer1

  for (unsigned int i = 0; i < ms; i++){  //Repete a contagem de 1ms até completar o tempo desejado
    TCNT1 = 0;  //Zera o contador para iniciar uma nova contagem
    TCCR1B = (1 << CS11);  //Configura o Timer1 com prescaler de 8
    while (TCNT1 < 2000);  //Espera 2000 contagens, sendo cada uma de 0,5us, formando 1ms
    TCCR1B = 0;  //Para a contagem do Timer1
  }
}

void Config_Motor(){  //Função criada para configurar os pinos utilizados pelo motor
  DDRD |= (1 << PD3);  //Configura o pino 3 como saída, responsável pela entrada IN1
  DDRD |= (1 << PD4);  //Configura o pino 4 como saída, responsável pela entrada IN2
  DDRD |= (1 << PD5);  //Configura o pino 5 como saída, responsável pela entrada IN3
  DDRB |= (1 << PB5);  //Configura o pino 13 como saída, responsável pela entrada IN4
}

void Sequencia_Direita(){  //Função criada para realizar a sequência de acionamento para a direita
  //Primeiro aciona IN1
  PORTD |= (1 << PD3);   //Coloca IN1 em HIGH
  PORTD &= ~(1 << PD4);  //Coloca IN2 em LOW
  PORTD &= ~(1 << PD5);  //Coloca IN3 em LOW
  PORTB &= ~(1 << PB5);  //Coloca IN4 em LOW
  delay_motor(5);  //Espera 5ms antes do próximo acionamento

  // Depois IN3
  PORTD |= (1 << PD5);   //Coloca IN3 em HIGH
  PORTD &= ~(1 << PD3);  //Coloca IN1 em LOW
  PORTD &= ~(1 << PD4);  //Coloca IN2 em LOW
  PORTB &= ~(1 << PB5);  //Coloca IN4 em LOW
  delay_motor(5);  //Espera 5ms antes do próximo acionamento

  // Depois IN2
  PORTD |= (1 << PD4);   //Coloca IN2 em HIGH
  PORTD &= ~(1 << PD3);  //Coloca IN1 em LOW
  PORTD &= ~(1 << PD5);  //Coloca IN3 em LOW
  PORTB &= ~(1 << PB5);  //Coloca IN4 em LOW
  delay_motor(5);  //Espera 5ms antes do próximo acionamento

  // E por fim IN4
  PORTB |= (1 << PB5);   //Coloca IN4 em HIGH
  PORTD &= ~(1 << PD3);  //Coloca IN1 em LOW
  PORTD &= ~(1 << PD4);  //Coloca IN2 em LOW
  PORTD &= ~(1 << PD5);  //Coloca IN3 em LOW
  delay_motor(5);  //Espera 5ms antes de finalizar a sequência
}

void Sequencia_Esquerda(){  //Função criada para realizar a sequência de acionamento para a esquerda
  // Primeiro acionamena IN4
  PORTB |= (1 << PB5);   //Coloca IN4 em HIGH
  PORTD &= ~(1 << PD3);  //Coloca IN1 em LOW
  PORTD &= ~(1 << PD4);  //Coloca IN2 em LOW
  PORTD &= ~(1 << PD5);  //Coloca IN3 em LOW
  delay_motor(5);  //Espera 5ms antes do próximo acionamento

  // Depois aciona IN2
  PORTD |= (1 << PD4);   //Coloca IN2 em HIGH
  PORTD &= ~(1 << PD3);  //Coloca IN1 em LOW
  PORTD &= ~(1 << PD5);  //Coloca IN3 em LOW
  PORTB &= ~(1 << PB5);  //Coloca IN4 em LOW
  delay_motor(5);  //Espera 5ms antes do próximo acionamento

  // Depois IN3
  PORTD |= (1 << PD5);   //Coloca IN3 em HIGH
  PORTD &= ~(1 << PD3);  //Coloca IN1 em LOW
  PORTD &= ~(1 << PD4);  //Coloca IN2 em LOW
  PORTB &= ~(1 << PB5);  //Coloca IN4 em LOW
  delay_motor(5);  //Espera 5ms antes do próximo acionamento

  // E por fim IN1
  PORTD |= (1 << PD3);   //Coloca IN1 em HIGH
  PORTD &= ~(1 << PD4);  //Coloca IN2 em LOW
  PORTD &= ~(1 << PD5);  //Coloca IN3 em LOW
  PORTB &= ~(1 << PB5);  //Coloca IN4 em LOW
  delay_motor(5);  //Espera 5ms antes de finalizar a sequência
}

void Girar_Direita(){  //Função criada para executar várias sequências de giro para a direita
  for (unsigned int i = 0; i < 8; i++){  //Repete a sequência 8 vezes
    Sequencia_Direita();  //Executa os 4 acionamentos para a direita
  }
}

void Girar_Esquerda(){  //Função criada para executar várias sequências de giro para a esquerda
  for (unsigned int i = 0; i < 8; i++){  //Repete a sequência 8 vezes
    Sequencia_Esquerda();  //Executa os 4 acionamentos para a esquerda
  }
}