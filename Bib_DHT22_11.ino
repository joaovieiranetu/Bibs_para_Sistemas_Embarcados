#include <avr/io.h>  //Permite configurarmos os registradores
#include <Arduino.h> //Biblioteca para chamar a função Serial 

struct Sensor_ {    //Criando uma estrutura do tipo Sensor_ que armazena os dados do DHT
  unsigned char bits[40];

  unsigned char byte1; //armazena o primeiro byte recebido do DHT
  unsigned char byte2; //armazena o segundo byte recebido do DHT
  unsigned char byte3; //armazena o terceiro byte recebido do DHT
  unsigned char byte4; //armazena o quarto byte recebido do DHT
  unsigned char byte5; //armazena o quinto byte recebido do DHT, que corresponde ao checksum

  float temp;  //armazena o valor da temperatura  que é representado pelo byte 3 e 4
};

struct Sensor_ DHT_; //Criando uma variável chamada DHT_ do tipo Sensor

unsigned int tempo_HIGH; //Variável criada para armazenar o tempo que o pino fica em HIGH 
unsigned int tempo_LOW; //Variável criada para armazenar o tempo que o pino fica em LOW

unsigned int bit; //Variável criada para armazenar o valor do bit (0 ou 1)
unsigned char dados_validos; //Variável criada para informar se os dados recebidos são válidos

int main(void){

  DDRB |= (1 << PB0); //Define o pino 8 do MCU como saída
  DDRB &= ~(1 << PB0); //Define o pino 8 do MCU como entrada
  //Esse passo é importante pois o DHT usa o mesmo pino tanto como entrada quanto como saída

  Serial.begin(9600);

  while(1){
   delay_inicio(); //inicializa o Timer 1 para armazenar os tempos e as contagens
   Start_Dht(); //inicializa a comunicação do DHT com o microcontrolador
   Dados_sensor(); //lê os 40 bits enviados pelo DHT divididos por 0 e 1 dependendo dos valores provenientes da função Timer_interation
   Montar_bytes(); //Faz a organização dos bits em cada byte dps verifica o Checksum de acordo com o somatório dos bytes
   
   if (dados_validos == 1){   
      Temperatura();  //Se os dados forem verdadeiros ele chama a função temperatura
      Serial.print("Temperatura: ");  //Imprime por meio do serial o valor da temperatura
      Serial.print(DHT_.temp, 1);  //Ele imprime o valor da temperatura que é o valor da temperatura relacionado ao tipo de estrutura DHT_
      Serial.println(" C");   //Imprime °C 
    }
    else{
      Serial.println("Dados invalidos");
    }
    delay_ms(2000);
  }
}


void delay_inicio(void)
{
    // Timer1 configurado como Normal Mode
    TCCR1A = 0;
    TCCR1B = 0;

    // Prescaler de 8
    TCCR1B |= (1 << CS11);
}


void delay_us(unsigned int us){  //Essa função vai retornar o parametro us como um inteiro sem sinal

  TCNT1 = 0; //Começa a contagem em 0

  while (TCNT1 < (us * 2)){
    //Aguarda o timer contar o tempo estimado. Multiplica por 2, pois como o prescaler é de 8 ele vai representar um ciclo a cada 0,5us
  }

}


void delay_ms(unsigned int ms) //Retorna um parametro de ms como um inteiro sem sinal
{
  unsigned int i;   //Declara uma variável inteira e sem sinal para armazenar o valor em ms
  for (i=0; i < ms; i++){    //Realiza o loop for a quantidade de vezes que o ms for declarado

    delay_us(1000);    // 1000us que equivale a 1ms
  }
}


void Start_Dht(void){ //Função que Libera a Solicitação de Dados do DHT22/11
  DDRB |= (1 << PB0);  //Inicialmente o pino 8 começa como saída (Nível HIGH)
  PORTB &= ~(1 << PB0); //Aqui ele desliga o pino 8 para que o DHT entenda que pode começar a comunicação
  delay_ms(20); //O DHT se mantem desligado por 20ms

  PORTB |= (1 << PB0); //Após os 20ms ele coloca o pino 8 em HIGH
  delay_us(40); //E espera por um tempo de 40us

  DDRB &= ~(1 << PB0); //Após o intervalo de 40us o pino 8 deve se tornar entrada para receber os dados do DHT
  PORTB |= (1 << PB0); //Ativa o pull-up interno do pino 8

  //Espera o DHT colocar o barramento em LOW para iniciar a resposta
  TCNT1 = 0;

  while (PINB & (1 << PB0)){  //Enquanto o PB0 estiver em HIGH, continua esperando o DHT colocar o barramento em LOW
    if (TCNT1 > 1000){   //Limite de ~500us, pois o prescaler 8 gera 0,5us por contagem
      dados_validos = 0;
      return;
    }
  }

  //Espera o DHT finalizar o pulso LOW da resposta
  TCNT1 = 0;

  while (!(PINB & (1 << PB0))){
    if (TCNT1 > 1000){   
      dados_validos = 0; //Se demorou demais, considera que os dados não são válidos.
      return;
    }
  }

  tempo_LOW = TCNT1;

  //Espera o DHT finalizar o pulso HIGH da resposta
  TCNT1 = 0;

  while (PINB & (1 << PB0)){
    if (TCNT1 > 1000){
      dados_validos = 0;
      return;
    }
  }

  tempo_HIGH = TCNT1;
}

void Time_iteration(void){ //Função de tempo de interação entre os tempos

  TCNT1 = 0;   //O valor da contagem inicia em 0

  while (PINB & (1 << PB0)){  //Enquanto o valor lido pelo registrador for HIGH

    tempo_HIGH = TCNT1;  //O valor armazenado na variável tempo é a quantidade de contagens (lá do registrador do Timer1)

    //Armazena na variável o tempo que o pino ficou em HIGH
  }

  TCNT1 = 0;    //O valor da contagem recomeça em 0 para não alterar os valores do Timer_ HIGH e LOW

  while (!(PINB & (1 << PB0))){ //Enquanto o valor lido pelo registrador for LOW

    tempo_LOW = TCNT1;   //O valor armazenado na variável tempo é a quantidade de contagens tbm

    //Armazena na variável o tempo que o pino fica em LOW
  }
}


void Dados_sensor(void){ //Nessa função ele vai executar o loop de 40 bits e medir a duração dos pulsos para transformar cada pulso em 0 ou 1.

  dados_validos = 1;

  for (int i = 0; i < 40; i++){   //Cria um loop for para executar 40 vezes, pois são 40 bits de dados

      TCNT1 = 0;  //inicia o tempo_LOW em 0

      //Espera o início do pulso HIGH do bit
      while (!(PINB & (1 << PB0))){

        if (TCNT1 > 1000){
          dados_validos = 0;
          return;
        }
      }

      tempo_LOW = TCNT1;   //Armazena as contagens do LOW na variável tempo_LOW

      if (tempo_LOW >= 60){   //Se o tempo_LOW for aproximadamente 30us ou mais, ele inicia a lógica para verificar se foi enviado o bit 1 ou 0
        TCNT1 = 0;  //Reinicia o Timer1 para começar a medir o tempo_HIGH

        while (PINB & (1 << PB0)){

          if (TCNT1 > 300){
            dados_validos = 0;
            return;
          }
        }

        tempo_HIGH = TCNT1;   //Armazena as contagens do HIGH na variável tempo_HIGH

        if (tempo_HIGH > 55){    //Se o tempo_HIGH foi maior que 55 então o bit enviado foi 1
          bit = 1;
        }
        else{  //Senão o bit enviado foi 0
          bit = 0;
        }
        DHT_.bits[i] = bit; //Armazena o bit recebido na posição correspondente dos 40 bits
      }
      else{
        dados_validos = 0;
        return;
      }
  }
}


void Montar_bytes(void){   //Função criada para armazenar os bits em cada byte até o 5 que é o somatório dos outros 4

  unsigned char byte;   //Cria variável byte do tipo char sem sinal

  for (int j = 0; j < 5; j++){ //Vai fazer o loop 5 vezes para armazenar os bytes 1,2,3,4 e 5
    byte = 0; //inicia o byte em 0

    for (int i = 0; i < 8; i++){  //Esse loop for vai deslocar os bits 8 vezes conforme ele sendo 0 ou 1
      byte = byte << 1;   //Desloca o bit para a esquerda após cada iteração
      byte |= DHT_.bits[(j * 8) + i];  //O valor do byte recebe o bit correspondente à posição dos 40 bits
    }

    if (j == 0){     //Se a iteração for 1 o byte1 vai ser a lógica byte respectiva
      DHT_.byte1 = byte;
    }
    else if (j == 1){
      DHT_.byte2 = byte;
    }
    else if (j == 2){
      DHT_.byte3 = byte;
    }
    else if (j == 3){
      DHT_.byte4 = byte;
    }
    else if (j == 4){
      DHT_.byte5 = byte;
    }
  }

  unsigned char Checksum = DHT_.byte1 + DHT_.byte2 + DHT_.byte3 + DHT_.byte4; //Soma os quatro primeiros bytes para calcular o checksum

  if (Checksum == DHT_.byte5){ //Compara o checksum calculado com o checksum enviado pelo DHT
    dados_validos = 1; //Dados válidos
  }
  else{
    dados_validos = 0; //Dados inválidos
  }
}

void Temperatura(void){  //Função temperatura
  unsigned int valor_temp;
  valor_temp = (256 * DHT_.byte3) + DHT_.byte4;  //Junta o byte 3 e 4 para formar o valor de temperatura

  if (valor_temp > 0){ //Verifica se o valor recebido é maior que zero
    DHT_.temp = valor_temp / 10; //Converte o valor recebido para graus Celsius
  }
  else{
    DHT_.temp = 0; //Caso o valor recebido seja zero, armazena zero na temperatura
  }
}

