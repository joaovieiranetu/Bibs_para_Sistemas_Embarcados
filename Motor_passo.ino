void meu_delay(unsigned int ms){  //Função criada para configurar um delay em ms
  TCCR1B = 0;  
  TCCR1A = 0;

  for (unsigned int i = 0; i < ms; i++){   //meu_delay(1000) = 1000 * 1ms = 1s , ou seja, ele executa o loop for enquanto a i definido for menor que o valor em ms
    TCNT1 = 0;  //Zera o contador (TCNT1)
    TCCR1B = (1<<CS11);   //Configura um prescaller de 8
    while (TCNT1 < 2000);  //Faz (2000 * 0,5us)  = 1000
    TCCR1B = 0; //Para a contagem do Timer1
  }
}

void Config_Motor(){
  DDRD |= (1 << PD2); //Configura os pinos 2,3, 4 e5 como saídas para acionar respectivamente: IN1,IN2,IN3 e IN4
  DDRD |= (1 << PD3);
  DDRD |= (1 << PD4);
  DDRD |= (1 << PD5);
}

void Sequencia_Direita(){
  // IN1
  PORTD |= (1 << PD2);
  PORTD &= ~(1 << PD3);
  PORTD &= ~(1 << PD4);
  PORTD &= ~(1 << PD5);
  meu_delay(5);

  // IN3
  PORTD |= (1 << PD4);
  PORTD &= ~(1 << PD2);
  PORTD &= ~(1 << PD3);
  PORTD &= ~(1 << PD5);
  meu_delay(5);

  // IN2
  PORTD |= (1 << PD3);
  PORTD &= ~(1 << PD2);
  PORTD &= ~(1 << PD4);
  PORTD &= ~(1 << PD5);
  meu_delay(5);

  // IN4
  PORTD |= (1 << PD5);
  PORTD &= ~(1 << PD2);
  PORTD &= ~(1 << PD3);
  PORTD &= ~(1 << PD4);
  meu_delay(5);
}

void Sequencia_Esquerda(){
  // IN4
  PORTD |= (1 << PD5);
  PORTD &= ~(1 << PD2);
  PORTD &= ~(1 << PD3);
  PORTD &= ~(1 << PD4);
  meu_delay(5);

  // IN2
  PORTD |= (1 << PD3);
  PORTD &= ~(1 << PD2);
  PORTD &= ~(1 << PD4);
  PORTD &= ~(1 << PD5);
  meu_delay(5);

  // IN3
  PORTD |= (1 << PD4);
  PORTD &= ~(1 << PD2);
  PORTD &= ~(1 << PD3);
  PORTD &= ~(1 << PD5);
  meu_delay(5);

  // IN1
  PORTD |= (1 << PD2);
  PORTD &= ~(1 << PD3);
  PORTD &= ~(1 << PD4);
  PORTD &= ~(1 << PD5);
  meu_delay(5);
}

void Girar_Direita(){
  for (unsigned int i = 0; i < 8; i++){
    Sequencia_Direita();
  }
}

void Girar_Esquerda(){
  for (unsigned int i = 0; i < 8; i++){
    Sequencia_Esquerda();
  }
}



