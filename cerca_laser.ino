Código Fuente
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- ASIGNACIÓN DE PINES ---
const int PIN_BOTON = 2;   // Pulsador con INPUT_PULLUP
const int PIN_LASER = 8;   // Diodo Láser de 5V
const int PIN_BUZZER = 9;  // Buzzer para tonos y sirena
const int PIN_LDR = A0;    // Entrada analógica del sensor LDR

// Dirección I2C típica: 0x27 o 0x3F (Pantalla 16 columnas x 2 filas)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- MÁQUINA DE ESTADOS FINITOS ---
enum EstadoSistema { DESARMADO, ALINEACION, ARMADO, ALARMA };
EstadoSistema estadoActual = DESARMADO;

// --- VARIABLES DE CALIBRACIÓN Y FILTRADO ---
int umbralLuzDinamico = 500; // Se autocalcula en el armado
unsigned long tiempoInicioBoton = 0;
bool botonPresionadoAntes = false;
int contadorInterrupciones = 0; // Filtro de confirmación antirruido

void setup() {
  pinMode(PIN_LASER, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_BOTON, INPUT_PULLUP);

  digitalWrite(PIN_LASER, LOW);

  lcd.init();
  lcd.backlight();
  mostrarPantallaDesarmado();
}

void loop() {
  int lecturaLDR = analogRead(PIN_LDR);
  bool estadoBoton = (digitalRead(PIN_BOTON) == LOW); // LOW = Presionado

  // --- LÓGICA DE DETECCIÓN DE PULSACIÓN (SHORT / LONG PRESS) ---
  if (estadoBoton && !botonPresionadoAntes) {
    tiempoInicioBoton = millis();
    botonPresionadoAntes = true;
  } 
  else if (!estadoBoton && botonPresionadoAntes) {
    unsigned long duracionPulsacion = millis() - tiempoInicioBoton;
    botonPresionadoAntes = false;

    // LONG PRESS (>= 2.5s): Entra a Alineación o Desarma
    if (duracionPulsacion >= 2500) {
      if (estadoActual == DESARMADO) {
        estadoActual = ALINEACION;
        lcd.clear();
      } else if (estadoActual == ALINEACION || estadoActual == ALARMA) {
        desarmarSistema();
      }
    } 
    // SHORT PRESS (< 2.5s): Inicia Armado con cuenta de 10s
    else {
      if (estadoActual == DESARMADO) {
        iniciarSecuenciaArmado();
      } else if (estadoActual == ALINEACION) {
        desarmarSistema();
      }
    }
  }

  // --- MÁQUINA DE ESTADOS DEL SISTEMA ---
  switch (estadoActual) {
    case DESARMADO:
      digitalWrite(PIN_LASER, LOW);
      noTone(PIN_BUZZER);
      break;

    case ALINEACION:
      digitalWrite(PIN_LASER, HIGH);
      noTone(PIN_BUZZER);
      lcd.setCursor(0, 0);
      lcd.print("MODO ALINEACION ");
      lcd.setCursor(0, 1);
      lcd.print("LDR Val: ");
      lcd.print(lecturaLDR);
      lcd.print("    ");
      delay(100);
      break;

    case ARMADO:
      digitalWrite(PIN_LASER, HIGH);
      
      // Evaluación de intrusión mediante el umbral autocalibrado
      if (lecturaLDR > umbralLuzDinamico) {
        contadorInterrupciones++;
        if (contadorInterrupciones >= 2) { // Exige 2 confirmaciones seguidas
          estadoActual = ALARMA;
          lcd.clear();
          lcd.print("!! ALERTA !!");
          lcd.setCursor(0, 1);
          lcd.print("INTRUSO DETECT.");
        }
      } else {
        contadorInterrupciones = 0; // Reinicia el contador si la luz vuelve a la normalidad
      }
      break;

    case ALARMA:
      digitalWrite(PIN_LASER, HIGH);
      ejecutarSirena();
      break;
  }
}

// --- FUNCIONES AUXILIARES ---

void iniciarSecuenciaArmado() {
  lcd.clear();
  lcd.print("CALIBRANDO...");
  
  digitalWrite(PIN_LASER, HIGH); 
  delay(300);

  // Muestreo analógico para autocalibración adaptativa
  long sumaLuz = 0;
  for (int k = 0; k < 10; k++) {
    sumaLuz += analogRead(PIN_LDR);
    delay(50);
  }
  int luzBase = sumaLuz / 10;

  // Asignación de margen seguro (+300 puntos sobre el valor base)
  umbralLuzDinamico = luzBase + 300;
  if (umbralLuzDinamico > 950) umbralLuzDinamico = 950; // Límite máximo del umbral

  contadorInterrupciones = 0;

  // Cuenta regresiva de 10 segundos requerida
  lcd.clear();
  lcd.print("ARMANDO EN:");
  for (int i = 10; i > 0; i--) {
    lcd.setCursor(12, 0);
    if (i < 10) lcd.print(" "); // Formato visual de un solo dígito
    lcd.print(i);
    tone(PIN_BUZZER, 1200, 100);
    delay(1000);
  }

  estadoActual = ARMADO;
  lcd.clear();
  lcd.print("SISTEMA ARMADO");
  lcd.setCursor(0, 1);
  lcd.print("U:");
  lcd.print(umbralLuzDinamico);
  lcd.print(" B:");
  lcd.print(luzBase);
}

void desarmarSistema() {
  estadoActual = DESARMADO;
  noTone(PIN_BUZZER);
  digitalWrite(PIN_LASER, LOW);
  contadorInterrupciones = 0;
  mostrarPantallaDesarmado();
}

void mostrarPantallaDesarmado() {
  lcd.clear();
  lcd.print("SIST. DESARMADO");
  lcd.setCursor(0, 1);
  lcd.print("       UMG      "); // Encabezado institucional UMG
}

void ejecutarSirena() {
  for (int f = 600; f <= 1300; f += 25) {
    tone(PIN_BUZZER, f);
    delay(3);
  }
  for (int f = 1300; f >= 600; f -= 25) {
    tone(PIN_BUZZER, f);
    delay(3);
  }
}
