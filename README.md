# 🏎️ Team Hard - Control Remoto Híbrido (ESP32 + PIC)

Este repositorio contiene el código fuente del control remoto y el sistema de telemetría para un vehículo a control remoto ("El Carro"). El sistema utiliza una arquitectura distribuida combinando microcontroladores ESP32 y PIC18F4550, comunicándose de forma inalámbrica mediante ESP-NOW y Bluetooth Low Energy (BLE).

## 🧠 Arquitectura del Sistema

El flujo de comunicación es bidireccional (Modo Espejo) y funciona de la siguiente manera:
`Celular (RemoteXY) <-> ESP1 (Master) <-> PIC1 =======(ESP-NOW)======= ESP2 (Carro) <-> PIC2`

### Componentes Principales:
* **ESP1 (Master / Control Remoto):** Se conecta por Bluetooth al celular usando la interfaz de **RemoteXY**. Traduce las órdenes del usuario y las envía físicamente al PIC1. También escucha al PIC1 y dispara las tramas por ESP-NOW.
* **PIC18F4550 (Procesador Físico 1):** Recibe datos crudos del ESP1 por UART, ejecuta lógicas de hardware y devuelve estados y tiempos de ejecución.
* **ESP2 (WROOM / Nodo Carro):** Actúa como receptor inalámbrico de las órdenes del Master, desempaqueta la información y la inyecta de forma cruda al PIC2.
* **Módulo de Freno de Emergencia:** Sistema de seguridad ("Freno Sexual") que detiene el equipo en caso de pérdida de conexión.

## ✨ Características y Modos de Operación

El sistema se controla mediante la app de RemoteXY (vía BLE) y cuenta con los siguientes modos:
1. **🕹️ Modo 0 (Joystick):** Control analógico proporcional de Avance y Giro.
2. **➕ Modo 1 (D-PAD):** Control digital de movimientos precisos. Incluye la activación del sistema de grabación de rutas (Dua Lipa Mode 🎤).
3. **🔄 Modo 2 (Replay):** Reproducción de secuencias guardadas en la EEPROM.
4. **❔ Modo 3:** [Espacio para futura expansión del Team Hard].

Adicionalmente, cuenta con feedback visual a través de un **LED NeoPixel** que indica el estado de la conexión y el modo activo.

## 🛠️ Tecnologías y Librerías

Este proyecto está desarrollado bajo el framework de **Arduino** utilizando **PlatformIO** como entorno de construcción.

**Librerías principales:**
* `RemoteXY`: Para la interfaz gráfica por Bluetooth.
* `esp_now`: Para la comunicación de ultra baja latencia entre los ESP32.
* `Adafruit NeoPixel`: Para el control de indicadores RGB.
* Librerías personalizadas (`Esp_Chisme` y `Freno Sexual`) construidas para encapsular la lógica de radio y seguridad.

## 🚀 Instalación y Uso

1. Clona este repositorio: `git clone https://github.com/tu-usuario/nombre-del-repo.git`
2. Abre la carpeta del proyecto (`Device_1` o `Device_2`) en **VS Code** con la extensión de **PlatformIO**.
3. Asegúrate de tener instaladas las librerías requeridas (PlatformIO las descargará automáticamente según el `platformio.ini`).
4. Para el **Master (ESP32-S3)**: Usa el entorno `esp32-s3-n16r8`.
5. Para el **Carro (ESP32 WROOM)**: Usa el entorno `esp32dev`.
6. Compila y sube el firmware a cada placa.
7. Enciende las placas, abre la app de RemoteXY en tu teléfono, busca el dispositivo Bluetooth **"Team Hard"** y ¡arranca!

## ⚠️ Consideraciones de Hardware
* La comunicación UART entre el ESP32 (3.3V) y el PIC18F4550 (5V) requiere **obligatoriamente** un divisor de tensión en la línea TX del PIC hacia el RX del ESP32 para evitar quemar el puerto.
* No cruzar los cables (Crossover) al conectar el ESP y el PIC en este PCB en particular según el diseño físico.