#define DIREITA   0x1C
#define ESQUERDA  0x18

#define CURSOR_LIGADO    2
#define CURSOR_DESLIGADO 0

#define BLINK_LIGADO     1
#define BLINK_DESLIGADO  0

#define BL_LIGADO    0x08
#define BL_DESLIGADO 0x00

#ifdef PCF8574A
   #define pcf_addres_write 0x70 | (A2_A1_A0 << 1)  
   #define pcf_addres_read  0x71 | (A2_A1_A0 << 1) 
#else  //PCF8574
   #define pcf_addres_write 0x40 | (A2_A1_A0 << 1)   
   #define pcf_addres_read  0x41 | (A2_A1_A0 << 1)  
#endif

#ifdef LCD20X4
   #define linha1_inicio 0x00
   #define linha1_fim    0x13  
   
   #define linha2_inicio 0x40
   #define linha2_fim    0x53  
   
   #define linha3_inicio 0x14
   #define linha3_fim    0x27  
   
   #define linha4_inicio 0x54
   #define linha4_fim    0x67

   #define linha1_inicio_comando 0x80
   #define linha2_inicio_comando 0xC0
   #define linha3_inicio_comando 0x94
   #define linha4_inicio_comando 0xD4
#else //LCD16X2
   #define linha1_inicio 0x00
   #define linha1_fim    0x27  
   
   #define linha2_inicio 0x40
   #define linha2_fim    0x67  

   #define linha1_inicio_comando 0x80
   #define linha2_inicio_comando 0xC0
#endif

int backlight_lcd_i2c = 0x08, lcd_i2c_linha = 1;

void inicializa_lcd_i2c() {
   i2c_start();                        // Inicia a comunicação
   i2c_write(pcf_addres_write);        // Envia o endereco do PCF + bit WRITE
   i2c_write(0x2C);                    // Configur o LCD para 4 bits/BL ON/En 1
   i2c_write(0x28);                    // En 0 para a instrução ser exec.
   
   delay_us(40);                       // Vericar depois o tempo de 
                                       //demora para os bits serem enviados.
   i2c_write(0x2C);                    // Envia o primeiro nible da 
   i2c_write(0x28);                    // instrução "Function Set".
   i2c_write(0x8C);                    // Envia o segundo nible da instrução.
   i2c_write(0x88);                    // Ficando 4 bits, 2 linhas, fonte 5*8.
   
   delay_us(40); 

   i2c_write(0x0C);                    // Envia o primeiro nible da instrução 
   i2c_write(0x08);                    // "Display ON/OFF control".
   i2c_write(0xCC);                    // Envia o segundo nible da instrução.
   i2c_write(0xC8);                    // Ficando display ligado, 
                                       // cursor desligado, blink desligado. 
   delay_us(1); 

   i2c_write(0x0C);                    // Envia o primeiro nible da instrução 
   i2c_write(0x08);                    // "Entry Mode Set".
   i2c_write(0x6C);                    // Envia o segundo nible da instrução.
   i2c_write(0x68);                    // Ficando incrementa cursor a cada 
                                       // escrita (posicao+=1), Scrol do LCD
                                       // desabilitado;
                                       // se ficar abilitado a cada caractere 
                                       // escrito o display faz o scroll.
   delay_us(40); 
   
   i2c_stop();
}
void enviar_comando_lcd_i2c(char caractere) {
   int ms_nibble_e, ls_nibble_e, ms_nibble, ls_nibble;
   
   ms_nibble = caractere & 0xF0;       // nibble + significativo do caractere.
   ls_nibble = caractere & 0x0F;       // nibble - significativo do caractere.
   swap(ls_nibble);                    // Swap para pois os bits usados são os 
                                       // mais significativos (D7: D4).
   ms_nibble |= backlight_lcd_i2c;     // Adicio o BL ao nibble + significativo
   ls_nibble |= backlight_lcd_i2c;     // Adicio o BL ao nibble - significativo

   ms_nibble_e = ms_nibble | 0x04;     // nibble + significativo En 1
   ls_nibble_e = ls_nibble | 0x04;     // nibble - significativo En 1
   
   i2c_start();
   i2c_write(pcf_addres_write); 
     
   i2c_write(ms_nibble_e);             // Envia o 1° nibble com "E" setado
   i2c_write(ms_nibble);               // Envia o 1° nibble com "E" zerado
   
   i2c_write(ls_nibble_e);             // Envia o 2° nibble com "E" setado
   i2c_write(ls_nibble);               // Envia o 2° nibble com "E" zerado

   i2c_stop();
   delay_us(44); 
}
void enviar_caractere_lcd_i2c(int caractere) {
   int ms_nibble_e, ls_nibble_e, ms_nibble, ls_nibble;

   ms_nibble = caractere & 0xF0;       // nibble + significativo do caractere.
   ls_nibble = caractere & 0x0F;       // nibble - significativo do caractere.
   swap(ls_nibble);                    // Swap pois os bits usados são os mais 
                                       // significativos (D7: D4).
   
   ms_nibble |= backlight_lcd_i2c;     // Adicio o BL ao nibble + significativo
   ls_nibble |= backlight_lcd_i2c;     // Adicio o BL ao nibble - significativo
   ms_nibble++;                        // seta o bit RS
   ls_nibble++;                        // seta o bit RS

   ms_nibble_e = ms_nibble | 0x05;     // nibble + significativo En e RS = 1
   ls_nibble_e = ls_nibble | 0x05;     // nibble - significativo En e RS = 1
   
   i2c_start();
   i2c_write(pcf_addres_write); 
     
   i2c_write(ms_nibble_e);             // Envia o 1° nibble com RS e En 1
   i2c_write(ms_nibble);               // Envia o 1° nibble com RS 1 e E 0
   
   i2c_write(ls_nibble_e);             // Envia o 2° nibble com RS e E 1
   i2c_write(ls_nibble);               // Envia o 2° nibble com RS 0 e E 0
   
   i2c_stop();  
   delay_us(44);
}
void limpar_lcd_i2c() {
   enviar_comando_lcd_i2c(0x01);
   delay_ms(2);
}
int ler_endereco_cursor() { 
   int aux, aux2, endereco;
   
   aux = 0xF2 | backlight_lcd_i2c;     // Seta os pinos do PCF referentes ao       
   aux2 = aux;                         // D7:D4 do LCD E=0 | RW=1 | RS=0.
   
   i2c_start();
   i2c_write(pcf_addres_write);
   i2c_write(aux);                     // Seta os pinos do PCF referentes ao 
                                       // D7:D4 do LCD E=0 | RW=1 | RS=0.
   i2c_write(aux | 0x04);              // En=1 para o LCD disponibilizar o 
                                       // nibble + significativo.
   i2c_start();
   i2c_write(pcf_addres_read);
   endereco = 0x70 & i2c_read(0);      // Lê o nibble + singnificativo usando 
                                       // a máscara 0x70 (bit + significativo 
                                       // é o Busy Flag - BF).
   i2c_start();
   i2c_write(pcf_addres_write);        
   i2c_write(aux);                     // Zera o pino Enable do LCD
   i2c_write(aux | 0x04);              // Seta o pino Enable do LCD para ele 
                                       // disponibilizar o nibble + signfi.
   i2c_start();
   i2c_write(pcf_addres_read);
   aux = 0xF0 & i2c_read(0);           // Lê o nibble menos singnificativo 
                                       // usando a máscara 0xF0.
   i2c_start();
   i2c_write(pcf_addres_write); 
   i2c_write(aux2);                    // Primeiro zera o pino "E"
   aux2 &= 0xF8;
   i2c_write(aux2);                    // E então zera o pino RW para assim 
                                       // sair do modo de leitura.
   i2c_stop();
   
   swap(aux);                          // Posiciona o nibble - significativo 
                                       // no seu lugar correto.
   endereco |= aux;                    // OR entre o nibble - significativo
                                       //  e o + significativo.
   delay_us(45);
   return(endereco);                   // Retorna o endereço atual do cursor.
}
void escrever_lcd_i2c(char caractere) {
   int end_cursor;
   
   if(caractere == '\n') {             // Posiciona o cursor no inicio da 
                                       // próxima linha atual.
      #ifdef LCD20X4                   // LCD 20 x 4 está sendo usado    
         switch(lcd_i2c_linha) {
            case 1:
               end_cursor = linha2_inicio_comando;
               lcd_i2c_linha = 2;
               break;
            
            case 2:
               end_cursor = linha3_inicio_comando;
               lcd_i2c_linha = 3;
               break; 
            
            case 3:
               end_cursor = linha4_inicio_comando;
               lcd_i2c_linha = 4;
               break;
            
            case 4:
               end_cursor = linha1_inicio_comando;
               lcd_i2c_linha = 1;
               break;
         }
         
      #else                            // LCD 16 x 2 está sendo usado
         switch(lcd_i2c_linha) {
            case 1:
               end_cursor = linha2_inicio_comando;
               lcd_i2c_linha = 2;
               break;
            
            case 2:
               end_cursor = linha1_inicio_comando;
               lcd_i2c_linha = 1;
               break; 
         }
      #endif                              
   
      enviar_comando_lcd_i2c(end_cursor);
   }
   else if(caractere == '\r') {        // Posiciona o cursor no inicio da 
                                       // linha atual.
      #ifdef LCD20X4                   // LCD 20 x 4 está sendo usado    
         switch(lcd_i2c_linha) {
            case 1:
               end_cursor = linha1_inicio_comando;
               break;
            
            case 2:
               end_cursor = linha2_inicio_comando;
               break; 
            
            case 3:
               end_cursor = linha3_inicio_comando;
               break;
            
            case 4:
               end_cursor = linha4_inicio_comando;
               break;
         }      
      
      #else                            // LCD 16 x 2 está sendo usado 
         switch(lcd_i2c_linha) {
            case 1:
               end_cursor = linha1_inicio_comando;
               break;
            
            case 2:
               end_cursor = linha2_inicio_comando;
               break; 
         }
   
      #endif                              
   
      enviar_comando_lcd_i2c(end_cursor);
   }
   else if (caractere == '\a') {             // Posiciona o cursor no 
      end_cursor = linha1_inicio_comando;    // início da primeira linha.
      enviar_comando_lcd_i2c(end_cursor);
      lcd_i2c_linha = 1;
   }
   else if (caractere == '\f') {             // limpa o LCD e posiciona o
      enviar_comando_lcd_i2c(0x01);          // cursor no início da 1ª linha.
      delay_ms(2);
      lcd_i2c_linha=1;
   }
   else {     
      enviar_caractere_lcd_i2c(caractere);
   }  
}
void lcd_i2c_irpara_xy(int coluna, int linha) {
   int comando;
   
   #ifdef LCD20X4
      if(linha == 1) {
         comando = coluna - 1;
         comando += linha1_inicio_comando; 
      }
      else if(linha == 2) {
         comando = coluna - 1;
         comando += linha2_inicio_comando; 
      }
      else if(linha == 3) {
         comando = coluna - 1;
         comando += linha3_inicio_comando; 
      }
      else if(linha == 4) {
         comando = coluna - 1;
         comando += linha4_inicio_comando; 
      }       
   #else
      if(linha == 1) {
         comando = coluna - 1;
         comando += linha1_inicio_comando; 
      }
      else if(linha == 2) {
         comando = coluna - 1;
         comando += linha2_inicio_comando; 
      }   
   #endif
   
   enviar_comando_lcd_i2c(comando);
}
void lcd_i2c_cursor(int estado_cursor) {
   estado_cursor |= 0x0C;                                                   
   enviar_comando_lcd_i2c(estado_cursor);
}
void lcd_i2c_backlight(int estado_backlight) {
   backlight_lcd_i2c = estado_backlight;
   
   i2c_start();
   i2c_write(pcf_addres_write); 
   i2c_write(estado_backlight);
   i2c_stop();
}
void escrever_cgram_lcd_i2c(int posicao, int vetor[8]) {
   int i;
   
   posicao *= 8;                          // Ajusta endereço>char0=0, char1=8..
   posicao |= 0x40;                       // Comando "set cgram address"
   enviar_comando_lcd_i2c(posicao);       // Envia comando para operação 
                                          // de escritar acontecer na CGRAM.
   for(i=0;i<8;++i) {
      enviar_caractere_lcd_i2c(vetor[i]); // envia os 8 bytes correspondentes 
   }                                      // ao caractere que será desenhado.
   
   enviar_comando_lcd_i2c(linha1_inicio_comando);  // Retorna para escritas na 
}                                                  // DDRAM cursor col 1 lin 1
int ler_caractere_lcd_i2c(int coluna, int linha) {
   int comando, aux, aux2, caractere;
   
   #ifdef LCD20X4
      if(linha == 1) {
         comando = coluna - 1;
         comando += linha1_inicio_comando; 
      }
      else if(linha == 2) {
         comando = coluna - 1;
         comando += linha2_inicio_comando; 
      }
      else if(linha == 3) {
         comando = coluna - 1;
         comando += linha3_inicio_comando; 
      }
      else if(linha == 4) {
         comando = coluna - 1;
         comando += linha4_inicio_comando; 
      }       
   #else
      if(linha == 1) {
         comando = coluna - 1;
         comando += linha1_inicio_comando; 
      }
      else if(linha == 2) {
         comando = coluna - 1;
         comando += linha2_inicio_comando; 
      }   
   #endif
   
   enviar_comando_lcd_i2c(comando);    // posiciona o cursor na posição 
                                       // onde a leitura será realizada.
   aux = 0xF3 | backlight_lcd_i2c;     // Seta os pinos do PCF referentes     
   aux2 = aux;                         // ao D7:D4 do LCD E=0 | RW=1 | RS=1.
   
   i2c_start();
   i2c_write(pcf_addres_write);
   i2c_write(aux);                     // Seta os pinos do PCF referentes ao 
                                       // D7:D4 do LCD E=0 | RW=1 | RS=0.
   i2c_write(aux | 0x04);              // Agora seta o Enable para o LCD o
                                       // liberar o nibble + significativo.
   i2c_start();
   i2c_write(pcf_addres_read);
   caractere = 0xF0 & i2c_read(0);     // Lê o nibble mais singnificativo 
                                       // usando a máscara 0xF0.
   i2c_start();
   i2c_write(pcf_addres_write);        
   i2c_write(aux);                     // Zera o pino Enable do LCD
   i2c_write(aux | 0x04);              // Seta o pino Enable do LCD para ele
                                       // liberar o nibble - significativo.
   
   i2c_start();
   i2c_write(pcf_addres_read);
   aux = 0xF0 & i2c_read(0);           // Lê o nibble menos singnificativo 
                                       // usando a máscara 0xF0.
   i2c_start();
   i2c_write(pcf_addres_write); 
   i2c_write(aux2);                    // Primeiro zera o pino "E"
   aux2 &= 0xF8;
   i2c_write(aux2);                    // E então zera o pino RW para assim 
                                       // sair do modo de leitura.
   i2c_stop();
   delay_us(45);
   
   swap(aux);                          // Posiciona o nibble - significativo 
                                       // no seu lugar correto.
   caractere |= aux;                   // OR entre o nibble - significativo 
                                       // e o + significativo.
   return(caractere);                  // Retorna o Char lido.  
   
}

void escrever_caractere_desenhado(int posicao_cgram) {
   enviar_caractere_lcd_i2c(posicao_cgram);
}

void deslocar_lcd_i2c(int comando) {
   enviar_comando_lcd_i2c(comando);
}

void lcd_i2c_posicao_original() {
   enviar_comando_lcd_i2c(0x02);
   delay_ms(2);
}
