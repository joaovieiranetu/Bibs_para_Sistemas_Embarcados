#include <avr/io.h> 
#include <Arduino.h> 

struct Sensor_ { //Criando uma estrutura do tipo Sensor_ que armazena os dados do DHT
  unsigned char bits[40]; //Armazena os 40 bits enviados pelo sensor

  unsigned char byte1; //Armazena o primeiro byte recebido do DHT
  unsigned char byte2; //Armazena o segundo byte recebido do DHT
  unsigned char byte3; //Armazena o terceiro byte recebido do DHT
  unsigned char byte4; //Armazena o quarto byte recebido do DHT
  unsigned char byte5; //Armazena o quinto byte recebido do DHT, que corresponde ao checksum

  float temp; //Armazena o valor da temperatura representado pelos bytes 3 e 4
};

struct Sensor_ DHT_; //Criando uma variável chamada DHT_ do tipo Sensor_

unsigned int tempo_HIGH_DHT; //Armazena o tempo que o pino fica em HIGH
unsigned int tempo_LOW_DHT; //Armazena o tempo que o pino fica em LOW

unsigned int bit_DHT; //Armazena o valor do bit recebido (0 ou 1)
unsigned char dados_validos_DHT; //Informa se os dados recebidos são válidos

void DHT_Inicio(void){ //Função criada para configurar o pino utilizado pelo sensor
  DDRB |= (1 << PB0); //Define o pino 8 como saída
  DDRB &= ~(1 << PB0); //Define o pino 8 como entrada, pois o DHT utiliza o mesmo pino para enviar e receber dados
  PORTB |= (1 << PB0); //Ativa o pull-up interno para manter o barramento em HIGH
}

void delay_inicio_DHT(void){ //Função criada para configurar o Timer1
  TCCR1A = 0; //Configura o Timer1 em modo normal
  TCCR1B = 0; //Para a contagem do Timer1 antes de configurar

  TCCR1B |= (1 << CS11); //Configura o Timer1 com prescaler de 8
}

void delay_us_DHT(unsigned int us){ //Função criada para realizar um delay em microssegundos
  TCNT1 = 0; //Começa a contagem em 0

  while (TCNT1 < (us * 2)){
    //Aguarda o timer contar o tempo estimado
    //Multiplica por 2, pois com prescaler de 8 cada contagem representa 0,5us
  }
}

void delay_ms_DHT(unsigned int ms){ //Função criada para realizar um delay em milissegundos
  unsigned int i; //Declara uma variável inteira sem sinal para controlar a contagem

  for (i = 0; i < ms; i++){ //Repete o loop a quantidade de vezes informada em ms
    delay_us_DHT(1000); //Executa 1000us, que equivale a 1ms
  }
}

void Start_Dht(void){ //Função que libera a solicitação de dados do DHT22/11  | Deixei desse jeito para não ficar estranho comeco
  dados_validos_DHT = 1; //Inicia a comunicação considerando que ainda não ocorreu erro

  DDRB |= (1 << PB0); //Configura o pino 8 como saída para enviar o sinal inicial
  PORTB &= ~(1 << PB0); //Coloca o pino 8 em LOW para iniciar a comunicação
  delay_ms_DHT(20); //Mantém o pino em LOW durante 20ms

  PORTB |= (1 << PB0); //Após os 20ms coloca o pino 8 em HIGH
  delay_us_DHT(40); //Espera um intervalo de 40us

  DDRB &= ~(1 << PB0); //Configura o pino 8 como entrada para receber os dados do DHT
  PORTB |= (1 << PB0); //Ativa o pull-up interno do pino 8

  //Espera o DHT colocar o barramento em LOW para iniciar a resposta
  TCNT1 = 0; //Zera o contador para medir o tempo de espera

  while (PINB & (1 << PB0)){ //Enquanto o PB0 estiver em HIGH, continua esperando o DHT colocar o barramento em LOW
    if (TCNT1 > 1000){ //Limite de aproximadamente 500us, pois cada contagem representa 0,5us
      dados_validos_DHT = 0; //Informa que o sensor não respondeu dentro do tempo esperado
      return; //Encerra a função
    }
  }

  //Espera o DHT finalizar o pulso LOW da resposta
  TCNT1 = 0; //Reinicia a contagem para medir o tempo em LOW

  while (!(PINB & (1 << PB0))){ //Enquanto o pino estiver em LOW, continua contando
    if (TCNT1 > 1000){ //Verifica se o tempo máximo foi ultrapassado
      dados_validos_DHT = 0; //Considera os dados inválidos
      return; //Encerra a função
    }
  }

  tempo_LOW_DHT = TCNT1; //Armazena o tempo que o pino permaneceu em LOW

  //Espera o DHT finalizar o pulso HIGH da resposta
  TCNT1 = 0; //Reinicia a contagem para medir o tempo em HIGH

  while (PINB & (1 << PB0)){ //Enquanto o pino estiver em HIGH, continua contando
    if (TCNT1 > 1000){ //Verifica se o tempo máximo foi ultrapassado
      dados_validos_DHT = 0; //Considera os dados inválidos
      return; //Encerra a função
    }
  }

  tempo_HIGH_DHT = TCNT1; //Armazena o tempo que o pino permaneceu em HIGH
}

void Time_iteration(void){ //Função criada para medir os tempos de interação entre os sinais | Deixei com esse nome pra não ficar estranho o interacao

  TCNT1 = 0; //O valor da contagem inicia em 0

  while (PINB & (1 << PB0)){ //Enquanto o valor lido pelo registrador for HIGH
    tempo_HIGH_DHT = TCNT1; //Armazena a quantidade de contagens do Timer1

    //Armazena o tempo que o pino ficou em HIGH
  }

  TCNT1 = 0; //Reinicia a contagem para medir o tempo em LOW

  while (!(PINB & (1 << PB0))){ //Enquanto o valor lido pelo registrador for LOW
    tempo_LOW_DHT = TCNT1; //Armazena a quantidade de contagens do Timer1

    //Armazena o tempo que o pino ficou em LOW
  }
}

void Dados_sensor(void){ //Função criada para ler os 40 bits e identificar se cada pulso representa 0 ou 1

  if (dados_validos_DHT == 0){ //Verifica se ocorreu algum erro durante o início da comunicação
    return; //Não realiza a leitura caso o sensor não tenha respondido
  }

  for (int i = 0; i < 40; i++){ //Executa o loop 40 vezes, pois o DHT envia 40 bits de dados

    TCNT1 = 0; //Inicia a contagem do tempo em LOW

    //Espera o início do pulso HIGH do bit
    while (!(PINB & (1 << PB0))){ //Enquanto o pino estiver em LOW, continua contando

      if (TCNT1 > 1000){ //Verifica se o tempo máximo de espera foi ultrapassado
        dados_validos_DHT = 0; //Informa que ocorreu um erro na leitura
        return; //Encerra a função
      }
    }

    tempo_LOW_DHT = TCNT1; //Armazena as contagens do LOW na variável tempo_LOW_DHT

    if (tempo_LOW_DHT >= 60){ //Se o tempo em LOW for aproximadamente 30us ou mais, continua a leitura do bit
      TCNT1 = 0; //Reinicia o Timer1 para medir o tempo em HIGH

      while (PINB & (1 << PB0)){ //Enquanto o pino estiver em HIGH, continua contando

        if (TCNT1 > 300){ //Verifica se o pulso HIGH ultrapassou aproximadamente 150us
          dados_validos_DHT = 0; //Informa que ocorreu um erro na leitura
          return; //Encerra a função
        }
      }

      tempo_HIGH_DHT = TCNT1; //Armazena as contagens do HIGH na variável tempo_HIGH_DHT

      if (tempo_HIGH_DHT > 55){ //Se o tempo em HIGH for maior que 55 contagens, considera que o bit enviado foi 1
        bit_DHT = 1; //Armazena o valor 1
      }
      else{ //Senão, considera que o bit enviado foi 0
        bit_DHT = 0; //Armazena o valor 0
      }

      DHT_.bits[i] = bit_DHT; //Armazena o bit recebido na posição correspondente dos 40 bits
    }
    else{
      dados_validos_DHT = 0; //Informa que o tempo em LOW não foi o esperado
      return; //Encerra a função
    }
  }
}

void Montar_bytes(void){ //Função criada para organizar os 40 bits em 5 bytes e verificar o checksum

  if (dados_validos_DHT == 0){ //Verifica se ocorreu algum erro durante a leitura dos bits
    return; //Não monta os bytes caso os dados estejam inválidos
  }

  unsigned char byte; //Cria uma variável byte do tipo char sem sinal

  for (int j = 0; j < 5; j++){ //Executa o loop 5 vezes para montar os bytes 1, 2, 3, 4 e 5
    byte = 0; //Inicia o byte em 0

    for (int i = 0; i < 8; i++){ //Executa o loop 8 vezes para organizar os bits de cada byte
      byte = byte << 1; //Desloca os bits uma posição para a esquerda
      byte |= DHT_.bits[(j * 8) + i]; //Adiciona o bit correspondente à posição dos 40 bits
    }

    if (j == 0){ //Quando j for 0, armazena o primeiro byte
      DHT_.byte1 = byte;
    }
    else if (j == 1){ //Quando j for 1, armazena o segundo byte
      DHT_.byte2 = byte;
    }
    else if (j == 2){ //Quando j for 2, armazena o terceiro byte
      DHT_.byte3 = byte;
    }
    else if (j == 3){ //Quando j for 3, armazena o quarto byte
      DHT_.byte4 = byte;
    }
    else if (j == 4){ //Quando j for 4, armazena o quinto byte
      DHT_.byte5 = byte;
    }
  }

  unsigned char Checksum = DHT_.byte1 + DHT_.byte2 + DHT_.byte3 + DHT_.byte4; //Soma os quatro primeiros bytes para calcular o checksum

  if (Checksum == DHT_.byte5){ //Compara o checksum calculado com o quinto byte enviado pelo DHT
    dados_validos_DHT = 1; //Informa que os dados recebidos são válidos
  }
  else{
    dados_validos_DHT = 0; //Informa que os dados recebidos são inválidos
  }
}

void Temperatura(void){ //Função criada para calcular a temperatura recebida do sensor
  unsigned int valor_temp; //Variável criada para armazenar o valor da temperatura

  valor_temp = (256 * DHT_.byte3) + DHT_.byte4; //Junta os bytes 3 e 4 para formar o valor da temperatura

  if (valor_temp > 0){ //Verifica se o valor recebido é maior que zero
    DHT_.temp = valor_temp / 10; //Converte o valor recebido para graus Celsius
  }
  else{
    DHT_.temp = 0; //Caso o valor recebido seja zero, armazena zero na temperatura
  }
}
