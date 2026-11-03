#include <Arduino.h>
#include <avr/io.h>

#define TIMEOUT 60000 //Tempo máximo de espera de 30ms (60000 x 0,5us)

// Pino 4 para o Trigger e o Pino 3 para o Echo

void Ultrassom_Inicio(){ //Função criada para configurar os pinos do sensor e o Timer1
  //Definir os pinos 4 e 3 como saída e entrada
  DDRD |= (1 << PD4);   //Definindo o pino 4 como saída para enviar o Trigger
  DDRD &= ~(1 << PD3);  //Definindo o pino 3 como entrada para receber o Echo

  PORTD &= ~(1 << PD4);  //Inicia o pino 4 (Trigger) em LOW
  PORTD &= ~(1 << PD3);  //Desativa o pull-up do pino Echo

  TCCR1B = 0; //Para o Timer1 antes de realizar a configuração
  TCCR1A = 0; //Configura o Timer1 em modo normal
  TCNT1 = 0;  //Zera o contador para iniciar uma nova contagem
}

void Trig_Config(){ //Função criada para enviar o pulso de 10us pelo Trigger
  TCCR1B = 0; //Para o Timer1 antes de zerar o contador
  TCCR1A = 0; //Configura o Timer1 em modo normal
  TCNT1 = 0;  //Zera o contador

  PORTD |= (1 << PD4); //Coloca o Trigger em nível alto para iniciar a medição

  TCCR1B = (1 << CS11); //Inicia o Timer1 com prescaler de 8

  while(TCNT1 < 20); //Aguarda 10us (20 x 0,5us)

  PORTD &= ~(1 << PD4); //Após esse tempo coloca o Trigger em LOW

  TCCR1B = 0; //O Timer1 para de contar
}

unsigned char Echo_inicio(){ //Função criada para aguardar o início do sinal Echo
  TCCR1B = 0; //Para o Timer1 antes de iniciar a contagem
  TCNT1 = 0;  //Zera o contador

  TCCR1B = (1 << CS11); //Inicia o Timer1 com prescaler de 8

  while(!(PIND & (1 << PD3))){ //Enquanto o Echo estiver em LOW, continua aguardando

    if(TCNT1 >= TIMEOUT){ //Verifica se o tempo máximo de 30ms foi atingido
      TCCR1B = 0; //Para a contagem do Timer1
      return 0; //Retorna zero indicando que o Echo não respondeu
    }
  }

  TCCR1B = 0; //Para o Timer1 após receber o sinal Echo
  return 1; //Retorna 1 indicando que o Echo passou para HIGH
}

unsigned int Echo_leitura(){ //Função criada para medir o tempo que o Echo permanece em HIGH
  TCCR1B = 0; //Para o Timer1 antes de realizar a leitura
  TCNT1 = 0;  //Zera a contagem

  TCCR1B = (1 << CS11); //Inicia o Timer1 com prescaler de 8

  while(PIND & (1 << PD3)){ //Enquanto o Echo estiver em HIGH, continua contando

    if(TCNT1 >= TIMEOUT){ //Verifica se o Echo permaneceu em HIGH por 30ms
      TCCR1B = 0; //Para a contagem do Timer1
      return 0; //Retorna zero indicando que ocorreu timeout
    }
  }

  TCCR1B = 0; //Para o Timer1 após finalizar a contagem
  return TCNT1; //Retorna a contagem acumulada no TCNT1
}

float tempo(){ //Função criada para transformar a contagem do Timer1 em microssegundos
  float time; //Variável criada para armazenar o tempo medido

  time = (Echo_leitura() * 0.5); //Converte a contagem do Timer1 para microssegundos
  return time; //Retorna o tempo em microssegundos
}

float distancia(){ //Função criada para calcular a distância em centímetros
  float dist; //Variável criada para armazenar a distância calculada

  Trig_Config(); //Envia um pulso de 10us pelo Trigger

  if(Echo_inicio() == 0){ //Verifica se ocorreu timeout esperando o início do Echo
    return -1; //Retorna -1 caso o sensor não tenha respondido
  }

  dist = ((tempo() * 0.0343) / 2); //Calcula a distância em centímetros usando a velocidade do som

  if(dist > 0){ //Verifica se a distância calculada é maior que zero
    return dist; //Retorna a distância calculada
  }

  return -1; //Retorna -1 caso tenha ocorrido erro na leitura
}
