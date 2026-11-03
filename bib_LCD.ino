void Config_pinos(){  //Função incial para configurar os pinos como saída todos (RS, E, pinos digitais...)
  DDRD |= (1<<PD2);
  DDRD |= (1<<PD3);
  DDRD |= (1<<PD4);
  DDRD |= (1<<PD5);
  DDRD |= (1<<PD6);
  DDRD |= (1<<PD7);
}

void meu_delay(unsigned int ms){  //Função criada para configurar um delay em ms
  TCCR1B = 0;
  TCCR1A = 0;

  for (unsigned int i = 0; i < ms; i++){  //meu_delay(1000) = 1000 * 1ms = 1s
    TCNT1 = 0;  //Zera o contador (TCNT1)
    TCCR1B = (1<<CS11);  //Configura um prescaller de 8
    while (TCNT1 < 2000);  //Faz (2000 * 0,5us) = 1000us = 1ms
    TCCR1B = 0;  //Para a contagem do Timer1
  }
}

void Config_RS_Comando(){  //Função criada para configurar o RS para enviar um comando (ex: Clean)
  PORTD &= ~(1 << PD2);
}

void Config_RS_Dado(){  //Função criada para configurar o RS para enviar um caractere da tabela ASCII (ex: A)
  PORTD |= (1 << PD2);
}

void Pulso_do_Enable(){  //Função criada para configurar o Enable, de forma que essa subida e descida diz ao LCD que os dados estão prontos
  PORTD |= (1 << PD3);  //Ele é ativado (HIGH)
  meu_delay(1);         //Conta um tempo em HIGH, usando o meu_delay por 1ms
  PORTD &= ~(1 << PD3); //Depois desse tempo ele cai pra LOW
}

void Enviar_bits(unsigned char dados){  //Função criada para enviar os bits, enfileirando em bit0, 1, 2 e 3
  if (dados & (1 << 0)){     //Verifica se o bit 0 da variável dados é igual a 1
    PORTD |= (1 << PD4);    //Se for 1, coloca o pino PD4 em HIGH
  }                          
  else {                   //Se o bit 0 for igual a 0
    PORTD &= ~(1 << PD4);  //Coloca o pino PD4 em LOW
}

  if (dados & (1 << 1)){   //Tudo se repete
    PORTD |= (1 << PD5);
  }
  else {
    PORTD &= ~(1 << PD5);
  }

  if (dados & (1 << 2)){
    PORTD |= (1 << PD6);
  }
  else {
    PORTD &= ~(1 << PD6);
  }

  if (dados & (1 << 3)){
    PORTD |= (1 << PD7);
  }
  else {
    PORTD &= ~(1 << PD7);
  }
}

void Separar_bits(unsigned char Caractere){  //Função criada para separar os 4 bits mais significativos
  unsigned char part_1; //Função criada para armazenar esses bits
  part_1 = (Caractere >> 4);  //Pega o caractere (dado) e desloca para a direita afim de armazenar na variável os bits mais significativos
  Enviar_bits(part_1);   //Depois envia por meio da função enviar bits os dados armazenados
  Pulso_do_Enable();     //Depois dá um pulso Enable para o LCD receber os bits

  unsigned char part_2;    //Ainda na função, uma variável part_2 é criada para armazenar os ultimos 4 bits menos significativos
  part_2 = (Caractere & 0b00001111);  //Pega o caractere (dado) e compara com uma Mask para encontrar os bits menos significativos e armazenar na variável
  Enviar_bits(part_2);  //Envia os ultimos 4bits de dados por meio da função Enviar_bits
  Pulso_do_Enable();    //Depois dá um pulso Enable para o LCD receber os bits
}

void Limpar_Tela(){  //Função criada para limpar todos os caracteres da tela do LCD
  Config_RS_Comando();  //Configura o RS em LOW para enviar um comando
  Separar_bits(0x01);   //Envia o comando para limpar a tela
  meu_delay(2);         //Aguarda 2ms para o LCD terminar a limpeza
}

void Ligar_Display(){  //Função criada para ligar o display sem mostrar o cursor
  Config_RS_Comando();  //Configura o RS em LOW para enviar um comando
  Separar_bits(0x0C);   //Liga o display, deixando o cursor invisível e sem piscar
  meu_delay(1);         //Aguarda o LCD processar o comando
}

void Config_Escrita(){  //Função criada para configurar a escrita da esquerda para a direita
  Config_RS_Comando();  //Configura o RS em LOW para enviar um comando
  Separar_bits(0x06);   //Faz o cursor avançar automaticamente após cada caractere
  meu_delay(1);         //Aguarda o LCD processar o comando
}

void Escrever_Caractere(unsigned char Caractere){  //Função criada para escrever um caractere no LCD
  Config_RS_Dado();       //Configura o RS em HIGH para enviar um dado
  Separar_bits(Caractere); //Envia os 8 bits do caractere em duas partes
  meu_delay(1);           //Aguarda o LCD processar o caractere
}

void Inicio_LCD(){  //Função criada para inicializar o LCD no modo de 4 bits
  Config_pinos();  //Chama a função Config_pinos que aciona os pinos como saída
  PORTD &= ~(1 << PD3);   //Coloca o pino 3 (Enable) em LOW
  meu_delay(50);         //Espera 50ms para o LCD estabilizar após ser ligado
  Config_RS_Comando();  //Configura o RS em LOW para que o LCD interprete os próximos bits como comandos

  //Sequência inicial do LCD
  Enviar_bits(0b0011);   //Envie os bits 0011 para configurar o modo 4 bits
  Pulso_do_Enable();     //Gera um pulso no Enable para o LCD receber os bits enviados
  meu_delay(5);          //Espera 5ms antes de enviar o próximo comando

  Enviar_bits(0b0011);   //Envia novamente o mesmo comando
  Pulso_do_Enable();
  meu_delay(1);      //Porém, aqui ele aguarda mais 1ms

  Enviar_bits(0b0011);  //Envia novamente o mesmo comando
  Pulso_do_Enable();
  meu_delay(1);  //Espera mais 1ms

  //Configura o LCD para trabalhar no modo de 4 bits
  Enviar_bits(0b0010);
  Pulso_do_Enable();
  meu_delay(1);

  //Configura 4 bits, 2 linhas e caracteres 5x8
  Separar_bits(0x28);
  meu_delay(1);

  //Desliga temporariamente o display durante a configuração
  Separar_bits(0x08);
  meu_delay(1);

  //Limpa a tela do LCD
  Limpar_Tela();

  //Configura a escrita automática
  Config_Escrita();

  //Liga o display
  Ligar_Display();
}
//-------------------------------------------------------------------------------------------
//Lógica para testar a bib do Display LDC:
void setup(){
  Inicio_LCD();  //Inicializa o LCD no modo de 4 bits
}

void loop(){
  Escrever_Caractere('O');  //Envia o caractere O para o LCD
  Escrever_Caractere('L');  //Envia o caractere L para o LCD
  Escrever_Caractere('A');  //Envia o caractere A para o LCD

  meu_delay(2000);  //Espera 2 segundos com a palavra OLA na tela

  Limpar_Tela();    //Limpa a tela e retorna o cursor para o início
}
//------------------------------------------------------------------------------------------