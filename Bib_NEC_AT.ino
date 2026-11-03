#include <avr/io.h> 

unsigned int tempo_HIGH_NEC;  //Variável criada para armazenar o tempo que o pino fica em HIGH
unsigned int tempo_LOW_NEC;   //Variável criada para armazenar o tempo que o pino fica em LOW
unsigned char bit_NEC;       //Variável criada para armazenar o valor do bit recebido

unsigned char byte1_NEC;  //Variável criada para armazenar o endereço recebido
unsigned char byte2_NEC;  //Variável criada para armazenar o endereço invertido
unsigned char byte3_NEC;  //Variável criada para armazenar o comando recebido
unsigned char byte4_NEC;  //Variável criada para armazenar o comando invertido
unsigned char dados_validos_NEC;  //Variável criada para informar se os dados recebidos são válidos

void Config_pinos_NEC(){  //Função criada para configurar o pino do receptor infravermelho
  DDRD &= ~(1 << PD2);   //Define o PD2 como entrada para receber os sinais do controle
  PORTD &= ~(1 << PD2);  //Desabilita o pull-up interno do PD2
}

void Config_Timer(){  //Função criada para configurar o Timer1
  TCCR1A = 0;            //Configura o Timer1 no modo normal
  TCCR1B = 0;            //Limpa as configurações anteriores do Timer1
  TCCR1B = (1 << CS11);  //Configura um prescaler de 8, sendo cada contagem equivalente a 0,5us
  TCNT1 = 0;             //Zera o contador do Timer1
}

unsigned char Inicio_NEC(){  //Função criada para receber o pulso inicial de 9ms e o intervalo de 4,5ms
  while (PIND & (1 << PD2));  //Aguarda o PD2 ficar em LOW para iniciar a comunicação

  TCNT1 = 0;  //Zera o contador para medir o pulso inicial
  while (!(PIND & (1 << PD2))){  //Enquanto o pino estiver em LOW
    if (TCNT1 > 18000){  //Verifica se o pulso ultrapassou os 9ms (18000 x 0,5us)
      return 0;  //Encerra a função caso o tempo seja ultrapassado
    }
  }
  tempo_LOW_NEC = TCNT1;  //Armazena a quantidade de contagens do pulso inicial

  if (tempo_LOW_NEC == 18000){  //Verifica se o pulso inicial corresponde aos 9ms
    TCNT1 = 0;  //Zera o contador para medir o intervalo inicial
  }
  else{
    return 0;  //Encerra a função se o tempo estiver incorreto
  }

  while (PIND & (1 << PD2)){  //Enquanto o pino estiver em HIGH
    if (TCNT1 > 9000){  //Verifica se o intervalo ultrapassou os 4,5ms (9000 x 0,5us)
      return 0;  //Encerra a função caso o tempo seja ultrapassado
    }
  }
  tempo_HIGH_NEC = TCNT1;  //Armazena a quantidade de contagens do intervalo inicial

  if (tempo_HIGH_NEC == 9000){  //Verifica se o intervalo inicial corresponde aos 4,5ms
    return 1;  //Informa que o início da comunicação está correto
  }
  else{
    return 0;  //Encerra a função se o tempo estiver incorreto
  }
}

unsigned char Receber_bits(){  //Função criada para receber os 32 bits e armazenar os quatro bytes
  byte1_NEC = 0;  //Zera o primeiro byte, responsável pelo endereço
  byte2_NEC = 0;  //Zera o segundo byte, responsável pelo endereço invertido
  byte3_NEC = 0;  //Zera o terceiro byte, responsável pelo comando
  byte4_NEC = 0;  //Zera o quarto byte, responsável pelo comando invertido

  for (unsigned char i = 0; i < 32; i++){  //Repete a leitura para os 32 bits do NEC
    TCNT1 = 0;  //Zera o contador para medir o pulso LOW

    while (!(PIND & (1 << PD2))){  //Enquanto o pino estiver em LOW
      if (TCNT1 > 1125){  //Verifica se o pulso ultrapassou os 562,5us (1125 x 0,5us)
        return 0;  //Encerra a função caso o tempo seja ultrapassado
      }
    }
    tempo_LOW_NEC = TCNT1;  //Armazena a quantidade de contagens do pulso LOW

    if (tempo_LOW_NEC == 1125){  //Verifica se o pulso corresponde aos 562,5us
      TCNT1 = 0;  //Zera o contador para medir o intervalo HIGH
    }
    else{
      return 0;  //Encerra a função se o tempo estiver incorreto
    }

    while (PIND & (1 << PD2)){  //Enquanto o pino estiver em HIGH
      if (TCNT1 > 3375){  //Verifica se o intervalo ultrapassou os 1687,5us (3375 x 0,5us)
        return 0;  //Encerra a função caso o tempo seja ultrapassado
      }
    }
    tempo_HIGH_NEC = TCNT1;  //Armazena a quantidade de contagens do intervalo HIGH

    if (tempo_HIGH_NEC == 1125){  //Verifica se o intervalo corresponde aos 562,5us
      bit_NEC = 0;  //Armazena o bit 0
    }
    else if (tempo_HIGH_NEC == 3375){  //Verifica se o intervalo corresponde aos 1687,5us
      bit_NEC = 1;  //Armazena o bit 1
    }
    else{
      return 0;  //Encerra a função se o tempo estiver incorreto
    }

    if (bit_NEC == 1){  //Verifica se o bit recebido é igual a 1
      if (i < 8){  //Verifica se o bit pertence ao primeiro byte
        byte1_NEC |= (1 << i);  //Coloca o bit 1 na posição correspondente do primeiro byte
      }
      else if (i < 16){  //Verifica se o bit pertence ao segundo byte
        byte2_NEC |= (1 << (i - 8));  //Coloca o bit 1 na posição correspondente do segundo byte
      }
      else if (i < 24){  //Verifica se o bit pertence ao terceiro byte
        byte3_NEC |= (1 << (i - 16));  //Coloca o bit 1 na posição correspondente do terceiro byte
      }
      else{  //Caso o bit pertença ao quarto byte
        byte4_NEC |= (1 << (i - 24));  //Coloca o bit 1 na posição correspondente do quarto byte
      }
    }
  }

  TCNT1 = 0;  //Zera o contador para medir o pulso final
  while (!(PIND & (1 << PD2))){  //Enquanto o pino estiver em LOW
    if (TCNT1 > 1125){  //Verifica se o pulso final ultrapassou os 562,5us
      return 0;  //Encerra a função caso o tempo seja ultrapassado
    }
  }
  tempo_LOW_NEC = TCNT1;  //Armazena a quantidade de contagens do pulso final

  if (tempo_LOW_NEC == 1125){  //Verifica se o pulso final corresponde aos 562,5us
    return 1;  //Informa que os 32 bits foram recebidos corretamente
  }
  else{
    return 0;  //Encerra a função se o tempo estiver incorreto
  }
}

void Receber_NEC(){  //Função criada para receber e organizar os dados do protocolo NEC
  dados_validos_NEC = 0;  //Indica que os dados ainda não são válidos

  if (Inicio_NEC() == 0){  //Verifica se o pulso e o intervalo inicial estão corretos
    return;  //Encerra a função caso ocorra algum erro
  }

  if (Receber_bits() == 0){  //Verifica se os 32 bits foram recebidos corretamente
    return;  //Encerra a função caso ocorra algum erro
  }

  if (((unsigned int)byte1_NEC + byte2_NEC == 255) && ((unsigned int)byte3_NEC + byte4_NEC == 255)){  //Verifica se a soma dos bytes com seus inversos é igual a 255
    dados_validos_NEC = 1;  //Informa que os dados recebidos são válidos
  }
  else{
    dados_validos_NEC = 0;  //Informa que os dados recebidos são inválidos
  }
}
