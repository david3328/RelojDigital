/*
Incluimos las librerías:
TimerOne para ejecutar una funcion transcurrido un tiempo determinado
LiquidCrystal para controlar el LCD 16x2
*/

#include <TimerOne.h>
#include <LiquidCrystal.h>

//Ejecutamos el LiquidCrystal en la variable lcd configurando los pines. 
LiquidCrystal lcd(12, 10, 5, 4, 3, 2);

//Declaramos las constates que son los pines de los pulsadores y el buzzer
const int PULSADOR_MODO = 9;
const int PULSADOR_UP = 8;
const int PULSADOR_DOWN = 7;
const int BUZZER = 6;

//Inicializamos las variables horas, minutos, segundos, y actualizar
int horas=0;
int minutos=0;
volatile int segundos=0;
volatile boolean actualizar=true;

/*
Incializamos las horas y minutos de la alarma, además que empiece apagada
El contador es, para que pasado un tiempo la alarme se apague automaticamente
*/

int horas_alarma=0;
int minutos_alarma=0;
boolean alarmaOn=false;
int contador_alarma=0;

/*
El arreglo  texto, me permite crear un arreglo para darle un formato a la hora mostrada
La variable modo me permite manejar los distintos modos que  tendremos:
  1-Modo '0' : Nos muestra la hora solamente
  2-Modo '1' : Configuraremos la hora
  3-Modo '2' : Configuramos los minutos
  4-Modo '3' : Configuramos la hora de la alarma
  5-Modo '4' : Configuramos los minutos de la alarma
  6-Modo '5' : Activamos o desactivamos la alarma

La constante N nos indica los modos que tenemos (6 modos)
*/
char texto[10];
int modo = 0;
const int N=6;


void setup(){
  //Incializamos el lcd y limpiamoz la pantalla
  lcd.begin(16,2);
  lcd.clear();

  //Colocamos los pulsadores como entrada y el buzzer como salida
  pinMode(PULSADOR_MODO, INPUT);
  pinMode(PULSADOR_UP, INPUT);
  pinMode(PULSADOR_DOWN, INPUT);
  pinMode(BUZZER, OUTPUT);

  //Con la libreria TimerOne inicializamos que se ejecute una funcion cada segundo
  //La función a realizar es manejadoraTimer
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(manejadoraTimer);
}

/*
La funcion manejadoraTimer me permite contar cada segundo
Adicional mente si la alarma está sonando cuenta cuanto tiempo lleva sonando la alarma
*/
void manejadoraTimer(){
    segundos++;                     //aumenta los segundos
    if(digitalRead(BUZZER)==HIGH){
      contador_alarma++;            //Si la alarma está activa incrementa los segundos que pasan
    }
    actualizar=true;                
}

/*
Actualizar reloj va contando los segundo que están pasando
aumentando los minutos y horas segun corresponda

Los minutos incremente con el cociente de una división entera.
Al ser la división entre 60, cada 60 segundos incrementara un minuto

Los segundos toma el valor del residuo de su valor en 60
Al pasar 60 segundos los segundos tomaría el valor de 0

Las horas tendrían la misma lógica anterior, solo que con los minutos
Las horas al final tendría el valor del residuo de su valor en 24 
*/

void actualizarReloj(){
  minutos += segundos / 60; 
  segundos = segundos % 60;

  horas += minutos / 60;
  minutos = minutos % 60;

  horas = horas % 24;
}
/*
La funcion incrementar recibe dos valores:
  1- Tipo que puede ser horas ('h) o minutos('m')
  2- Un valor númerico que incrementa su valor en 1,
El número ingresado incrementa en una unidad
Luego compara si lo ingresado son horas o minutos (con la variable tipo)
Al ser horas, toma el valor del residuo del numero en 24
Al ser minutos, toma el valor del residuo del numero en 60
Finalmente devuelve el número
*/
int incrementar(char tipo, int num){
  num++;
  num = tipo=='h'? num%24 : num%60;
  actualizar = true;
  return num;  
}

/*
La funcion decrementar recibe dos valores:
  1- Tipo que puede ser horas ('h) o minutos('m')
  2- Un valor númerico que decrementa su valor en 1,
El número ingresado decrementa en una unidad
Luego compara si el número es drecrementado es negativo
  Despues compara si lo ingresado son horas o minutos (con la variable tipo)
  Al ser horas, toma el valor de 23
  Al ser minutos, toma el valor de 59
Finalmente devuelve el número
*/
int decrementar(char tipo, int num){
  num--;
  if(num<0){
    num = tipo=='h'? 23 : 59;
  }
  actualizar = true;
  return num;  
}

/*
Verifica en que modo se encuentra el sistema
De estar en modo '0' no parpadea el cursor
En modo '1': parpadea el cursor en las horas
En modo '2': parpadea el cursor en los minutos
En modo '3': parpadea el cursor en las horas de la alarma
En modo '4': parpadea el cursor en los minutos de la alarma
En modo '5': parpadea el cursor en la opcion Apagado(OFF) o Encendido(ON) de la alarma 
*/
void fijarCursorModo(){
  switch(modo){
    case 0: 
      lcd.noBlink();
    break;
    case 1: 
      lcd.setCursor(4, 0);
      lcd.blink();
    break;
    case 2: 
      lcd.setCursor(7, 0);
      lcd.blink();
    break;
    case 3: 
      lcd.setCursor(4, 1);
      lcd.blink();
    break;  
    case 4: 
      lcd.setCursor(7, 1);
      lcd.blink();
    break;
    case 5:
      lcd.setCursor(10, 1);
      lcd.blink();
    break;
  }
}

void loop(){

  /*
    Al presionar el primer pulsador, que es de MODO,
    Incrementa para entrar en los diferentes modos
    El modo toma el valor de su residuo con el total de modos que existen
      Esto sirve para reiniciar el modo cuando esté ya esté en el ultimo
    Según el modo que este llama a la función fijarCursorModo
  */
  if(digitalRead(PULSADOR_MODO)==HIGH){
    modo++;
    modo = modo % N;
    fijarCursorModo();
    while(digitalRead(PULSADOR_MODO)==HIGH);
  }
  
  //Comprobar si funciona apagar el buzzer
  /*
    Si la alarma se encuentra activa en caso de presionar cualquier pulsador, se apaga la alarma
    El contador de la alarma se reinicia 
  */
  if((digitalRead(PULSADOR_MODO)==HIGH) || (digitalRead(PULSADOR_UP)==HIGH) || (digitalRead(PULSADOR_DOWN)==HIGH)){
    if(digitalRead(BUZZER)==HIGH){
      digitalWrite(BUZZER, LOW);
      contador_alarma=0;  
    }
    while((digitalRead(PULSADOR_MODO)==HIGH) || (digitalRead(PULSADOR_UP)==HIGH) || (digitalRead(PULSADOR_DOWN)==HIGH));
  }

  /*
  Segun el modo en que nos encontremos tendremos funciones para los otros pulsadores
  En los 4 primeros modos:
    El pulsador Up: me permite incrementar minutos u horas, para lo cual se llama a la función incrementar
    El pulsador Down: me permite decrementar minutos u horas, para lo cual se llama a la función decrementar
  En el caso 5:
    El pulsador Up: me permite Apagar/Encender la alarma
    El pulsador Down queda inutilizable
  */
  switch(modo){
    //RELOJ
    case 1: 
      if(digitalRead(PULSADOR_UP)==HIGH){
        horas = incrementar('h',horas);
        while(digitalRead(PULSADOR_UP)==HIGH);
      }
      if(digitalRead(PULSADOR_DOWN)==HIGH){
        horas = decrementar('h',horas);
        while(digitalRead(PULSADOR_DOWN)==HIGH);
      }
    break;
    case 2: 
    if(digitalRead(PULSADOR_UP)==HIGH){
        minutos = incrementar('m',minutos);
        while(digitalRead(PULSADOR_UP)==HIGH);
      }
      if(digitalRead(PULSADOR_DOWN)==HIGH){
        minutos = decrementar('m',minutos);
        while(digitalRead(PULSADOR_DOWN)==HIGH);
      }
    break;
    //ALARMA
    case 3: 
      if(digitalRead(PULSADOR_UP)==HIGH){
        horas_alarma = incrementar('h',horas_alarma);
        while(digitalRead(PULSADOR_UP)==HIGH);
      }
      if(digitalRead(PULSADOR_DOWN)==HIGH){
        horas_alarma = decrementar('h',horas_alarma);
        while(digitalRead(PULSADOR_DOWN)==HIGH);
      }
    break;
    case 4: 
      if(digitalRead(PULSADOR_UP)==HIGH){
        minutos_alarma = incrementar('m',minutos_alarma);
        while(digitalRead(PULSADOR_UP)==HIGH);
      }
      if(digitalRead(PULSADOR_DOWN)==HIGH){
        minutos_alarma = decrementar('m',minutos_alarma);
        while(digitalRead(PULSADOR_DOWN)==HIGH);
      }
    break;
    case 5:    
      if(digitalRead(PULSADOR_UP)==HIGH){
        alarmaOn=!alarmaOn;
        actualizar=true;
        while(digitalRead(PULSADOR_UP)==HIGH);
      }
    break;
  }

  if(actualizar == true){
    actualizarReloj();
    lcd.clear();
    lcd.noBlink();
    //RELOJ
    lcd.setCursor(4,0);
    sprintf(texto, "%02d:%02d:%02d",horas,minutos, segundos); 
    lcd.print(texto);
    lcd.setCursor(4,1);
    sprintf(texto, "%02d:%02d",horas_alarma,minutos_alarma); 
    lcd.print(texto);
    String estado_alarma = alarmaOn? " ON" : " OFF";
    lcd.print(estado_alarma);
    /*
      Si la alarma se encuentra activada 
        Verifica si las horas y minutos actuales coinciden con las programadas
        De ser las mismas, aciva la alarma
      Compara el contador de la alarma, si es igual a 60 
      Recordar que el contador solo aumenta si la alarma está sonando, de lo contrario se encontraría en 0.
      Si el contador es igual, dejaría de sonar la alarma y el contador se reinicia en 0;
    */
    if(alarmaOn ==true){
      if(segundos==0 && horas == horas_alarma && minutos == minutos_alarma){
        digitalWrite(BUZZER, HIGH);
      }
    }        
    if(contador_alarma==60){
      digitalWrite(BUZZER, LOW);
      contador_alarma=0;
    }  
    actualizar = false;
    fijarCursorModo();
  }  
}
