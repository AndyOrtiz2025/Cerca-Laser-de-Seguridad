# 🔴 Cerca Láser de Seguridad Perimetral

Prototipo de alarma perimetral con **Arduino UNO**. Un haz láser recorre el perímetro reflejándose en espejos hasta llegar a un sensor LDR. Si alguien interrumpe el haz, se activa una sirena.

Proyecto del curso **Arquitectura II**, Universidad Mariano Gálvez de Guatemala, Campus Huehuetenango.

## ✨ Características

- **Autocalibración adaptativa:** en cada armado mide la luz base (promedio de 10 lecturas) y calcula un umbral dinámico (`luz base + 300`). Así no le afectan los cambios de luz ambiental.
- **Filtro antirruido:** exige 2 lecturas seguidas sobre el umbral antes de activar la alarma.
- **Máquina de Estados Finitos (FSM):** `DESARMADO`, `ALINEACION`, `ARMADO` y `ALARMA`.
- **Pantalla LCD 16x2 I2C:** muestra el estado del sistema y los valores del sensor.
- **Sirena** con barrido de frecuencia de 600 a 1300 Hz.

## 🧰 Componentes

| Componente | Pin Arduino |
|---|---|
| Módulo sensor LDR | A0 |
| Módulo láser 650 nm | 8 |
| Pulsador | 2 (INPUT_PULLUP) |
| Buzzer pasivo | 9 |
| LCD 16x2 con módulo I2C | SDA → A4, SCL → A5 |

Además: espejos planos, protoboard y cables jumper.

## 🎮 Uso

| Acción | Resultado |
|---|---|
| Pulsación corta (< 2.5 s) en Desarmado | Calibra y arma el sistema tras una cuenta regresiva de 10 s |
| Pulsación larga (≥ 2.5 s) en Desarmado | Entra al modo alineación (muestra el valor del LDR) |
| Cualquier pulsación en Alineación | Desarma |
| Pulsación larga en Alarma | Apaga la alarma y desarma |

> 💡 Usa el modo alineación para ajustar los espejos antes de armar.

## ⚙️ Instalación

1. Instala la librería **LiquidCrystal_I2C** desde el Gestor de Librerías del Arduino IDE.
2. Abre el archivo `.ino` y súbelo al Arduino UNO.
3. Si la pantalla no muestra nada, cambia la dirección I2C de `0x27` a `0x3F` en el código.


## 👥 Integrantes

- Andy Ronaldo Ramírez Ortíz
- María Fernanda Orozco Alonzo
- Juan Eliezer Raymundo Lucas
