#include <SPI.h>
#include <Ethernet.h> 
#include <SD.h>
/*pinos digitais 10, 11, 12 e 13 no Uno e nos pinos 50, 51 e 52 no Mega. Em ambas as placas, o pino 10 é usado para selecionar o W5100 e o pino 4 para o cartão SD. Esses pinos não podem ser usados ​​para E/S geral. 
No Mega, o pino SS de hardware, 53, não é usado para selecionar o W5100 ou o cartão SD, mas deve ser mantido como uma saída ou a interface SPI não funcionará.*/

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 68, 59);                       
EthernetServer server(80);                             

String requisicao_do_navegador;
String parametro_da_requisicao;

const int REED = 6;  // Constantes / Resistior Pull-Up ligado com um botao na I/O 6 // pino 7 jampeado direto com o reset // USADO NO PLUVIOMETRO
int val = 0;
int old_val = 0;
int REEDCOUNT = 0;
void (*funcReset)() = 0;  //funcao de reiniciar o arduino
const int chipSelect = 4;  // guardar dados pluviometricos no cartao SD
int sensorValue;  //variavel para gravar dados no cartao SD



void setup()
{
    Ethernet.begin(mac, ip); 
    server.begin();    
    Serial.begin(9600); // initializa a comunicaçao serial
    pinMode(REED, INPUT_PULLUP); // Vai ler os pulsos do pluviometro de bascula       
    pinMode(8,OUTPUT);
    pinMode(9,OUTPUT);
      while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Inicializando cartao SD...");
  Serial.print("\n");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Falha no cartao, ou nao esta presente");
    // don't do anything more:
    while (1);
  }
  Serial.println("Cartao SD inicializado.");
}



void loop(){
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

                              // visualiza o pluviometro pelo monitor serial
                              val = digitalRead(REED);      //Leia o status do swtich Reed
                              if ((val == LOW) && (old_val == HIGH)){    //Verifique se o status mudou
                                    delay(10);                   // Atraso colocado para lidar com qualquer "salto" no switch.
                                    REEDCOUNT = REEDCOUNT + 1;   //Adicione 1 à contagem de pontas de balde
                                    old_val = val;              //Tornar o valor antigo igual ao valor atual
                                    Serial.print("Medida de chuva (contagem): ");
                                    Serial.print(REEDCOUNT); 
                                    Serial.println(" pulso");
                                    Serial.print("Medida de chuva (calculado): ");
                                    Serial.print(REEDCOUNT*0.25); 
                                    Serial.println(" mm");
                                      File dataFile = SD.open("datalog.txt", FILE_WRITE);
            //                        if((dataFile) && (REEDCOUNT > 1)){
             //                         Serial.print("Teste");
             //                         String dataString = "";
             //                         dataString += "Medida de chuva (contagem): ";
             //                       }
                                                                               
                                                                               if (dataFile){
                                                                                  String dataString = ""; 
                                                                                  dataString += "Medida de chuva (contagem): ";
                                                                                  dataString += String(REEDCOUNT);
                                                                                  dataString += " pulso";
                                                                                  dataString += "\n";
                                                                                  dataString += "Medida de chuva (calculado): ";
                                                                                  dataString += String(REEDCOUNT*0.25);
                                                                                  dataString += " mm";
                                                                                  }
String dataString = "";
                                                                                  dataFile.print(dataString);
                                                                                  dataFile.close();
                                                                                  Serial.println("Dados salvos no cartao SD");
                                                                             
                                                                              
                                }                                                              
                                else {
                                      old_val = val;              //Se o status não mudou, não faça nada
                                    }

                                    String dataString = "";
/*
//codigo gpt
                                                                        if (dataFile){
                                                                          dataString += "Medida de chuva (contagem): ";
                                                                          dataString += String(REEDCOUNT);
                                                                          dataString += " pulso";
                                                                          dataString += "\n";
                                                                          dataString += "Medida de chuva (calculado): ";
                                                                          dataString += String(REEDCOUNT*0.25);
                                                                          dataString += " mm";


                                                                          dataFile.print(dataString);
                                                                          dataFile.close();
                                                                          Serial.println("Dados salvos no cartao SD");
                                                                        }
                                                                        
                                                                       // else {
                                                                          //Serial.println("Erro na abertura do arquivo");
                                                                       // }
*/

//delay(1000);
 /*
                    leia três sensores e anexe à string:
                    for (int analogPin = 0; analogPin < 3; analogPin++) {
                      int sensor = analogRead(analogPin);
                      dataString += String(sensor);
                      if (analogPin < 2) {
                        dataString += ",";
                      }
                    }

                    // abra o arquivo. observe que apenas um arquivo pode ser aberto por vez,
                    // então você tem que fechar este antes de abrir outro.
*/

/*
                    // se o arquivo estiver disponível, escreva nele:
                    if (dataFile) {
                      dataFile.println(dataString);
                      dataFile.close();
                      // print to the serial port too:
                      Serial.println(dataString);
                    }
                    //se o arquivo não estiver aberto, aparecerá um erro:
                    else {
                      Serial.println("Erro ao abrir o arquivo datalog.txt");
                    }
*/

 EthernetClient client = server.available(); // initializa o servidor

    if (client) { 
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) { 
                char c = client.read(); 
                requisicao_do_navegador += c;  
                
                if (c == '\n' && currentLineIsBlank ) 
                {     
                        if (definindo_a_requisicao(&requisicao_do_navegador)) {
                        parametro_da_requisicao = pegar_parametro_da_requisicao(&requisicao_do_navegador);
                        //Serial.println(parametro_da_requisicao);
                                                 
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connection: close");
                        client.println();
                        
                        //Conteudo da Página HTML
                        client.println("<!DOCTYPE html>");
                        client.println("<html>");
                        client.println("<head>");
                        client.println("<meta http-equiv='refresh' content='5'>"); // atualiza a pagina a cada ** segundos
                        client.println("<link rel='icon' type='image/png' href='https://cdn.icon-icons.com/icons2/2107/PNG/512/file_type_homeassistant_icon_130543.png'/>"); //DEFINIÇÃO DO ICONE DA PÁGINA
                        client.println("<title>Dickel's Home Remote</title>"); //ESCREVE O TEXTO NA PÁGINA
                        client.println("</head>");
                        client.println("<body style=background-color:#1C1C1C>"); //DEFINE A COR DE FUNDO DA PÁGINA
                        client.println("<center><font color=#00BFFF><h1>Dickel's Home Remote</font></center></h1>"); //ESCREVE "Dickel's Home Remote" 
                        client.println("<hr/>");                         
                        client.println("<h1><font color=#8A2BE2>Saidas Digitais</h1>");
                        
                        client.println("<form method=\"get\">");

                        saida(client);
                        
                        client.println("<br/>");
                        saida2(client);
                        client.println("</form>");
                        client.println("<h1><font color=#8A2BE2>Dados Pluviometricos</h1>");
                                  // Imprime na pagina web
                                  client.print("<font color=#FF0000>Medida de chuva (contagem): ");
                                  client.print(REEDCOUNT);
                                  client.println(" pulso");
                                  client.println("<br/>");
                                  client.print("<font color=#FF0000>Medida de chuva (calculado): ");
                                  client.print(REEDCOUNT * 0.25);
                                  client.println(" mm");
                                  client.println("<br/>");
                                  client.println("<br/>");
                                  // Rodapé com botoes
                                  client.println("<input type='button' value='Zerar contador pluviometrico' onclick='REEDCOUNT = 0'>");
                                  client.println("<input type='button' value='Restart' onclick='funcReset = 0';'>");
                                  
                        client.println("</form>");
                        client.println("</body>");
                        client.println("</html>");
                        } else {
                        client.println("HTTP/1.1 200 OK");
                    }
                   
                    requisicao_do_navegador = "";    
                    break;
                }
                
                if (c == '\n') {
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    currentLineIsBlank = false;
                }
            }
        } 
        delay(1);     
        client.stop(); 
    } 
}

void saida(EthernetClient cl)
{
        
        if (parametro_da_requisicao.indexOf("P8=1") > -1) 
        { 
           digitalWrite(8, HIGH);
           cl.println("<input type=\"checkbox\" name=\"P8\" value=\"1\" onclick=\"submit();\" checked ><font color=#FF0000> Pino digital 8");
        } 
        else 
        {
           digitalWrite(8, LOW);
           cl.println("<input type=\"checkbox\" name=\"P8\" value=\"1\" onclick=\"submit();\" ><font color=#00FF00> Pino digital 8");
        }
        
}

void saida2(EthernetClient cl)
{
        
        if (parametro_da_requisicao.indexOf("P9=1") > -1) 
        { 
           digitalWrite(9, HIGH);
           cl.println("<input type=\"checkbox\" name=\"P9\" value=\"1\" onclick=\"submit();\" checked ><font color=#FF0000> Pino digital 9");
        } 
        else 
        {
           digitalWrite(9, LOW);
           cl.println("<input type=\"checkbox\" name=\"P9\" value=\"1\" onclick=\"submit();\" ><font color=#00FF00> Pino digital 9");
        }
        
}


String pegar_parametro_da_requisicao(String *requisicao) {
int pos_inicial, pos_final;
String parametro;

  
  pos_inicial = (*requisicao).indexOf("GET") + 3;
  pos_final = (*requisicao).indexOf("HTTP/") - 1;
  parametro = (*requisicao).substring(pos_inicial,pos_final);
  parametro.trim();
 
  return parametro;
}

bool definindo_a_requisicao(String *requisicao) {
String parametro;
bool requisicao_desejada = false;

  parametro = pegar_parametro_da_requisicao(requisicao);

  if (parametro == "/") {
     requisicao_desejada = true;
  }

  if (parametro.substring(0,2) == "/?") {
     requisicao_desejada = true;
  }  

  return requisicao_desejada;

}