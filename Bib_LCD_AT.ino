#include <avr/io.h>

void Config_pinos(){  //Função inicial para configurar os pinos do LCD como saída (RS, E e pinos de dados)
  DDRD |= (1 << PD7); //Configura o pino 7 como saída, responsável pelo RS
  DDRB |= (1 << PB1); //Configura o pino 9 como saída, responsável pelo Enable
  DDRB |= (1 << PB2); //Configura o pino 10 como saída, responsável pelo primeiro bit de dados (D4)
  DDRB |= (1 << PB3); //Configura o pino 11 como saída, responsável pelo segundo bit de dados (D5)
  DDRB |= (1 << PB4); //Configura o pino 12 como saída, responsável pelo terceiro bit de dados (D6)
  DDRB |= (1 << PB5); //Configura o pino 13 como saída, responsável pelo quarto bit de dados (D7)
}

void delay_LCD(unsigned int ms){  //Função criada para configurar um delay em ms
  TCCR1B = 0; //Para o Timer1 antes de iniciar uma nova contagem
  TCCR1A = 0; //Configura o Timer1 em modo normal

  for (unsigned int i = 0; i < ms; i++){  //delay_LCD(1000) = 1000 * 1ms = 1s
    TCNT1 = 0; //Zera o contador (TCNT1)
    TCCR1B = (1 << CS11); //Configura um prescaler de 8
    while (TCNT1 < 2000); //Faz (2000 * 0,5us) = 1000us = 1ms
    TCCR1B = 0; //Para a contagem do Timer1
  }
}

void Config_RS_Comando(){  //Função criada para configurar o RS para enviar um comando
  PORTD &= ~(1 << PD7); //Coloca o pino RS em LOW, informando que será enviado um comando
}

void Config_RS_Dado(){  //Função criada para configurar o RS para enviar um caractere da tabela ASCII
  PORTD |= (1 << PD7); //Coloca o pino RS em HIGH, informando que será enviado um dado
}

void Pulso_do_Enable(){  //Função criada para gerar um pulso no Enable, informando ao LCD que os dados estão prontos
  PORTB |= (1 << PB1); //Coloca o Enable em HIGH
  delay_LCD(1); //Mantém o Enable em HIGH durante 1ms
  PORTB &= ~(1 << PB1); //Coloca o Enable em LOW, finalizando o pulso
}

void Enviar_bits(unsigned char dados){  //Função criada para enviar 4 bits pelos pinos PB2, PB3, PB4 e PB5
  if (dados & (1 << 0)){ //Verifica se o bit 0 da variável dados é igual a 1
    PORTB |= (1 << PB2); //Se for 1, coloca o pino PB2 em HIGH
  }
  else { //Se o bit 0 for igual a 0
    PORTB &= ~(1 << PB2); //Coloca o pino PB2 em LOW
  }

  if (dados & (1 << 1)){ //Verifica se o bit 1 da variável dados é igual a 1
    PORTB |= (1 << PB3); //Se for 1, coloca o pino PB3 em HIGH
  }
  else {
    PORTB &= ~(1 << PB3); //Se for 0, coloca o pino PB3 em LOW
  }

  if (dados & (1 << 2)){ //Verifica se o bit 2 da variável dados é igual a 1
    PORTB |= (1 << PB4); //Se for 1, coloca o pino PB4 em HIGH
  }
  else {
    PORTB &= ~(1 << PB4); //Se for 0, coloca o pino PB4 em LOW
  }

  if (dados & (1 << 3)){ //Verifica se o bit 3 da variável dados é igual a 1
    PORTB |= (1 << PB5); //Se for 1, coloca o pino PB5 em HIGH
  }
  else {
    PORTB &= ~(1 << PB5); //Se for 0, coloca o pino PB5 em LOW
  }
}

void Separar_bits(unsigned char Caractere){  //Função criada para separar os 8 bits em duas partes de 4 bits
  unsigned char part_1; //Variável criada para armazenar os 4 bits mais significativos
  part_1 = (Caractere >> 4); //Desloca o caractere 4 posições para a direita, obtendo os bits mais significativos
  Enviar_bits(part_1); //Envia os 4 bits mais significativos para os pinos de dados
  Pulso_do_Enable(); //Gera um pulso no Enable para o LCD receber os bits

  unsigned char part_2; //Variável criada para armazenar os 4 bits menos significativos
  part_2 = (Caractere & 0b00001111); //Utiliza uma máscara para manter somente os 4 bits menos significativos
  Enviar_bits(part_2); //Envia os últimos 4 bits para os pinos de dados
  Pulso_do_Enable(); //Gera outro pulso no Enable para o LCD receber os bits
}

void Limpar_Tela(){  //Função criada para limpar todos os caracteres da tela do LCD
  Config_RS_Comando(); //Configura o RS em LOW para enviar um comando
  Separar_bits(0x01); //Envia o comando 0x01, responsável por limpar a tela
  delay_LCD(2); //Aguarda 2ms para o LCD terminar a limpeza
}

void Ligar_Display(){  //Função criada para ligar o display sem mostrar o cursor
  Config_RS_Comando(); //Configura o RS em LOW para enviar um comando
  Separar_bits(0x0C); //Liga o display, deixando o cursor invisível e sem piscar
  delay_LCD(1); //Aguarda o LCD processar o comando
}

void Config_Escrita(){  //Função criada para configurar a escrita da esquerda para a direita
  Config_RS_Comando(); //Configura o RS em LOW para enviar um comando
  Separar_bits(0x06); //Faz o cursor avançar automaticamente após cada caractere
  delay_LCD(1); //Aguarda o LCD processar o comando
}

void Escrever_Caractere(unsigned char Caractere){  //Função criada para escrever um caractere no LCD
  Config_RS_Dado(); //Configura o RS em HIGH para enviar um dado
  Separar_bits(Caractere); //Envia os 8 bits do caractere em duas partes de 4 bits
  delay_LCD(1); //Aguarda o LCD processar o caractere
}

void Inicio_LCD(){  //Função criada para inicializar o LCD no modo de 4 bits
  Config_pinos(); //Chama a função Config_pinos para configurar os pinos como saída
  PORTB &= ~(1 << PB1); //Coloca o pino 9 (Enable) em LOW
  delay_LCD(50); //Espera 50ms para o LCD estabilizar após ser ligado
  Config_RS_Comando(); //Configura o RS em LOW para que o LCD interprete os próximos bits como comandos

  //Sequência inicial do LCD
  Enviar_bits(0b0011); //Envia os bits 0011 para iniciar a configuração do LCD
  Pulso_do_Enable(); //Gera um pulso no Enable para o LCD receber os bits enviados
  delay_LCD(5); //Espera 5ms antes de enviar o próximo comando

  Enviar_bits(0b0011); //Envia novamente os bits 0011
  Pulso_do_Enable(); //Gera outro pulso no Enable
  delay_LCD(1); //Aguarda mais 1ms

  Enviar_bits(0b0011); //Envia novamente os bits 0011
  Pulso_do_Enable(); //Gera outro pulso no Enable
  delay_LCD(1); //Espera mais 1ms

  //Configura o LCD para trabalhar no modo de 4 bits
  Enviar_bits(0b0010); //Envia os bits 0010 para selecionar o modo de 4 bits
  Pulso_do_Enable(); //Gera um pulso para o LCD receber essa configuração
  delay_LCD(1); //Aguarda o LCD processar o comando

  //Configura 4 bits, 2 linhas e caracteres 5x8
  Separar_bits(0x28); //Envia o comando 0x28 para configurar o formato de funcionamento
  delay_LCD(1); //Aguarda o LCD processar o comando

  //Desliga temporariamente o display durante a configuração
  Separar_bits(0x08); //Envia o comando 0x08 para desligar o display
  delay_LCD(1); //Aguarda o LCD processar o comando

  //Limpa a tela do LCD
  Limpar_Tela(); //Envia o comando responsável por limpar todos os caracteres

  //Configura a escrita automática
  Config_Escrita(); //Configura o cursor para avançar da esquerda para a direita

  //Liga o display
  Ligar_Display(); //Liga o display sem mostrar o cursor
}
