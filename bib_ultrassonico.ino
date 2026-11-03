
#include <Arduino.h>
#include <avr/io.h>

#define TIMEOUT 60000 //Tempo máximo de espera de 30ms (60000 x 0,5us)

// Pino 4 para o Trigger e o Pino 3 para o Echo

unsigned char erro = 0; //Flag para identificar erro na leitura (0 = normal, 1 = erro)

void Ultrassom_Inicio(){
  //Definir os pinos 4 e 3 como saída e entrada
  DDRD |= (1 << PD4);   //Definindo o pino 4 como saída
  DDRD &= ~(1 << PD3);  //Definindo o pino 3 como entrada

  PORTD &= ~(1 << PD4);  //Inicia o pino 4 (Trigger) em LOW
  PORTD &= ~(1 << PD3);  //Desativa o pull-up do pino Echo

  TCCR1B = 0; //Para o Timer1
  TCCR1A = 0; //Configura o Timer1 em modo normal
  TCNT1 = 0;  //Zera o contador

  erro = 0; //Inicia a flag sem erro
}

void Trig_Config(){
  TCCR1B = 0; //Para o Timer1 antes de zerar o contador
  TCCR1A = 0; //Configura o Timer1 em modo normal
  TCNT1 = 0;  //Zera o contador

  PORTD |= (1 << PD4); //Coloca o Trigger em nível alto

  TCCR1B = (1 << CS11); //Inicia o Timer1 com prescaler de 8

  while(TCNT1 < 20); //Aguarda 10us (20 x 0,5us)

  PORTD &= ~(1 << PD4); //Após esse tempo o Trigger desliga

  TCCR1B = 0; //O Timer1 para de contar
}

void Echo_inicio(){
  TCCR1B = 0; //Para o Timer1 antes de iniciar a contagem
  TCNT1 = 0;  //Zera o contador

  TCCR1B = (1 << CS11); //Inicia o Timer1 com prescaler de 8

  //Enquanto o Echo estiver em LOW e não houver erro, continua aguardando
  while(!(PIND & (1 << PD3)) && erro == 0){

    if(TCNT1 >= TIMEOUT){ //Verifica se o tempo máximo de espera foi atingido
      erro = 1; //Indica que ocorreu timeout
    }
  }

  TCCR1B = 0; //Para o Timer1
}

unsigned int Echo_leitura(){
  TCCR1B = 0; //Para o Timer1
  TCNT1 = 0;  //Zera a contagem

  TCCR1B = (1 << CS11); //Inicia o Timer1 com prescaler de 8

  //Enquanto o Echo estiver em HIGH e não houver erro, continua a contagem
  while((PIND & (1 << PD3)) && erro == 0){

    if(TCNT1 >= TIMEOUT){ //Verifica se o Echo permaneceu em HIGH por 30ms
      erro = 1; //Indica que ocorreu timeout
    }
  }

  TCCR1B = 0; //Para o Timer1

  if(erro == 1){
    return 0; //Retorna zero caso tenha ocorrido erro
  }

  return TCNT1; //Retorna a contagem acumulada no TCNT1
}

float tempo(){  //Cria uma função tempo que cria uma variável time para armazenar a contagem do timer em microsegundos
  float time;

  time = (Echo_leitura() * 0.5); //Converte a contagem do Timer1 para microssegundos

  return time; //Retorna o tempo em microssegundos
}

float distancia(){  //Cria uma função distancia do tipo ponto flutuante, pois o valor pode ser em formato decimal
  float dist; //Variável para representar a distância

  erro = 0; //Limpa a flag antes de iniciar uma nova leitura

  Trig_Config(); //Envia um pulso de 10us pelo Trigger

  Echo_inicio(); //Aguarda o Echo passar para HIGH

  if(erro == 0){ //Verifica se o Echo respondeu corretamente

    dist = ((tempo() * 0.0343) / 2); //Calcula a distância em centímetros

    if(erro == 0 && dist > 0){
      return dist; //Retorna a distância calculada
    }
  }

  return -1; //Retorna -1 caso tenha ocorrido erro na leitura
}

void setup(){
  Serial.begin(9600); //Inicia a comunicação serial em 9600 baud
  Ultrassom_Inicio(); //Configura os pinos e o Timer1
}

void loop(){
  float dist = distancia(); //Realiza a leitura da distância

  if(dist >= 0){ //Verifica se a leitura foi válida
    Serial.print("Distancia: ");
    Serial.print(dist); //Exibe a distância com duas casas decimais
    Serial.println(" cm");
  }
  else{
    Serial.println("Erro na leitura"); //Exibe mensagem caso ocorra timeout
  }

  delay(250); //Aguarda 250ms antes da próxima leitura
}
