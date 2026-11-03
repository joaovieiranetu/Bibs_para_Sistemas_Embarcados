
#include <avr/io.h>

void USART_Init(unsigned int baud){  //Função criada para configurar a comunicação Serial
  unsigned long ubrr = (16000000UL / (16UL * baud)) - 1;  //Calcula o valor do registrador UBRR

  UCSR0A = 0;  //Configura a USART no modo de velocidade normal

  UBRR0H = (unsigned char)(ubrr >> 8);  //Armazena os bits mais significativos
  UBRR0L = (unsigned char)ubrr;  //Armazena os bits menos significativos

  UCSR0B = (1 << RXEN0) | (1 << TXEN0);  //Habilita a recepção RX e a transmissão TX
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  //Configura 8 bits, sem paridade e 1 bit de parada
}

void USART_Transmit(unsigned char data){  //Função criada para transmitir um byte pela Serial
  while (!(UCSR0A & (1 << UDRE0)));  //Espera o registrador de transmissão ficar disponível

  UDR0 = data;  //Coloca o byte no registrador para ser transmitido
}

unsigned char USART_Receive(){  //Função criada para receber um byte pela Serial
  while (!(UCSR0A & (1 << RXC0)));  //Espera até que um byte seja recebido

  return UDR0;  //Retorna o byte recebido
}

void serial_print_char(char letra){  //Função criada para enviar uma letra pela Serial
  USART_Transmit(letra);  //Envia o código ASCII da letra utilizando a função USART_Transmit
}

char serial_receive_char(){  //Função criada para receber uma letra pela Serial
  char letra;  //Variável criada para armazenar a letra recebida

  letra = USART_Receive();  //Recebe o código ASCII da letra pela USART

  return letra;  //Retorna a letra recebida
}

char int_char(unsigned int numero){  //Função criada para transformar um dígito numérico em caractere ASCII
  return numero + '0';  //Soma o número ao código ASCII do caractere '0'
}

void serial_print_int(unsigned int numero){  //Função criada para separar e enviar um número inteiro pela Serial

  unsigned int unidade;        //Variável criada para armazenar a unidade
  unsigned int dezena;         //Variável criada para armazenar a dezena
  unsigned int centena;        //Variável criada para armazenar a centena
  unsigned int milhar;         //Variável criada para armazenar o milhar
  unsigned int dezena_milhar;  //Variável criada para armazenar a dezena de milhar

  char caractere_unidade;        //Variável criada para armazenar a unidade como caractere
  char caractere_dezena;         //Variável criada para armazenar a dezena como caractere
  char caractere_centena;        //Variável criada para armazenar a centena como caractere
  char caractere_milhar;         //Variável criada para armazenar o milhar como caractere
  char caractere_dezena_milhar;  //Variável criada para armazenar a dezena de milhar como caractere

  unidade = numero % 10;  //Separa a unidade
  dezena = (numero / 10) % 10;  //Separa a dezena
  centena = (numero / 100) % 10;  //Separa a centena
  milhar = (numero / 1000) % 10;  //Separa o milhar
  dezena_milhar = (numero / 10000) % 10;  //Separa a dezena de milhar

  caractere_unidade = int_char(unidade);  //Converte a unidade para ASCII
  caractere_dezena = int_char(dezena);  //Converte a dezena para ASCII
  caractere_centena = int_char(centena);  //Converte a centena para ASCII
  caractere_milhar = int_char(milhar);  //Converte o milhar para ASCII
  caractere_dezena_milhar = int_char(dezena_milhar);  //Converte a dezena de milhar para ASCII

  USART_Transmit(caractere_dezena_milhar);  //Envia a dezena de milhar
  USART_Transmit(caractere_milhar);  //Envia o milhar
  USART_Transmit(caractere_centena);  //Envia a centena
  USART_Transmit(caractere_dezena);  //Envia a dezena
  USART_Transmit(caractere_unidade);  //Envia a unidade
}

unsigned int serial_receive_int(){  //Função criada para receber um número inteiro pela Serial
  unsigned int numero = 0;  //Variável criada para armazenar o número recebido
  char caractere;  //Variável criada para armazenar cada caractere recebido
  unsigned char iniciou = 0;  //Variável criada para identificar se algum dígito já foi recebido

  while (1){  //Executa enquanto o número estiver sendo recebido
    caractere = USART_Receive();  //Recebe um caractere pela Serial

    if (caractere >= '0' && caractere <= '9'){  //Verifica se o caractere recebido é um dígito numérico
      numero = numero * 10 + (caractere - '0');  //Converte o caractere para número e acrescenta ao valor anterior
      iniciou = 1;  //Indica que a recepção do número começou
    }

    if ((caractere == '\n' || caractere == '\r') && iniciou){  //Verifica se o usuário pressionou Enter após digitar um número
      break;  //Encerra a recepção do número
    }
  }

  return numero;  //Retorna o número inteiro recebido
}
