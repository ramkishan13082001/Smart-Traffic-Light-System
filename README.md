# **🚦 Smart Traffic Monitoring System**

An advanced embedded systems project using the **PIC18F4580 Microcontroller** to implement a smart traffic control system with:

- ADC-based traffic density monitoring
- Dynamic traffic signal timing
- PWM-controlled streetlight brightness
- UART telemetry communication
- Hardware interrupt emergency handling
- Timer1-based non-blocking delays

This project demonstrates how multiple embedded peripherals work together in a real-time environment similar to an RTOS-style architecture.

---

# **📌 Project Overview**

This project simulates an intelligent traffic management system capable of adapting traffic signal behavior according to road density conditions.

The system continuously:

- Reads analog traffic density values
- Dynamically changes GREEN signal timing
- Controls streetlight brightness using PWM
- Sends live traffic logs through UART
- Handles emergency ambulance situations using hardware interrupts

The project combines several important embedded concepts into a single integrated application.

---

# **🎯 Project Objective**

The objective of this project is to design a smart embedded traffic controller that:

- Monitors traffic density continuously
- Adjusts signal timing dynamically
- Saves power using PWM streetlight control
- Sends real-time UART telemetry logs
- Responds instantly to emergency interrupts
- Demonstrates multitasking-style embedded programming

This project is ideal after learning:

- LCD interfacing
- ADC programming
- UART communication
- Timer programming
- PWM generation
- External interrupts
- Embedded C Programming

---

# **🧠 Embedded Concepts Used**

| Peripheral | Purpose |
|------------|----------|
| ADC | Reads traffic density sensor |
| UART | Sends traffic logs to PC terminal |
| Timer1 | Generates accurate hardware delays |
| INT0 Interrupt | Handles emergency override |
| PWM (CCP1) | Controls streetlight brightness |
| LCD Interface | Displays system status |

---

# **⚙️ Components Required**

| Component | Purpose |
|------------|----------|
| PIC18F4580 | Main controller |
| LM016L LCD | Displays traffic information |
| LM35 / Analog Sensor | Traffic density input |
| RED LED | Stop signal |
| YELLOW LED | Warning signal |
| GREEN LED | Go signal |
| Push Button | Emergency interrupt trigger |
| Buzzer | Emergency warning indication |
| L293D | PWM driver interface |
| DC Motor / LED | Streetlight simulation |
| MAX232 / USB-TTL | UART communication |
| 20 MHz Crystal | System clock |
| 22pF Capacitors | Crystal stabilization |

---

# **🔍 Proteus Components Search Keywords**

Use these exact names in Proteus:

```text
PIC18F4580
LM016L
LM35
BUTTON
BUZZER
L293D
MOTOR
VIRTUAL TERMINAL
CRYSTAL
CAP
```

---

# **📁 Project Files**

```text
README.md
main.c
Makefile
Proteus Project Files
Output.hex
Simulation Screenshot
```

---

# **🔌 Circuit Connections**

---
# 📷 Project Screenshot

## **🟢 LCD Connections**

| LCD Pin | PIC18F4580 Connection |
|----------|-----------------------|
| RS | RC4 |
| RW | GND |
| EN | RC5 |
| D0-D7 | PORTD |
| VDD | +5V |
| VSS | GND |

---

## **🔴 Traffic Signal LED Connections**

| LED | PIC Pin |
|-----|----------|
| RED LED | RA1 |
| YELLOW LED | RA2 |
| GREEN LED | RA3 |

---

## **🟡 Analog Sensor Connection**

| Sensor | PIC Pin |
|---------|----------|
| Traffic Density Sensor | RA0 (AN0) |

---

## **🔵 Emergency Interrupt Connection**

| Device | PIC Pin |
|---------|----------|
| Emergency Push Button | RB0 (INT0) |

---

## **🟣 PWM Streetlight Connection**

| Device | PIC Pin |
|---------|----------|
| PWM Output | RC2 (CCP1) |

---

## **🟠 UART Connections**

| UART Signal | PIC Pin |
|--------------|----------|
| TX | RC6 |
| RX | RC7 |

---

## **⚫ Buzzer Connection**

| Device | PIC Pin |
|---------|----------|
| Buzzer | RC0 |

---

# **⚠️ Important Design Decision**

The emergency button is connected to **RB0** because RB0 is the dedicated **INT0 external interrupt pin** of the PIC18F4580.

This allows:

- Immediate emergency detection
- Hardware-level interrupt response
- Instant traffic sequence interruption

To avoid pin conflicts:

- LCD control pins are moved to PORTC
- LCD data bus uses PORTD
- ADC input remains on RA0
- UART communication uses RC6 and RC7

---

# **💻 MPLAB X Project Creation**

---

## **Step 1: Create New Project**

Open MPLAB X IDE and go to:

```text
File → New Project
```

---

## **Step 2: Select Project Type**

Choose:

```text
Microchip Embedded → Standalone Project
```

---

## **Step 3: Select Device**

Choose:

```text
PIC18F4580
```

---

## **Step 4: Select Compiler**

Choose:

```text
XC8 Compiler
```

---

## **Step 5: Add Source File**

Create:

```text
main.c
```

Paste the project code into this file.

---

# **🧠 Project Working Logic**

The firmware performs the following operations sequentially:

1. Initializes LCD, ADC, UART, PWM, and Timer peripherals
2. Reads traffic density using ADC
3. Compares traffic level against threshold
4. Extends GREEN signal timing during heavy traffic
5. Adjusts PWM brightness dynamically
6. Sends traffic status through UART
7. Executes traffic light sequence
8. Continuously monitors emergency interrupt

---

# **🚦 Traffic Control Flow**

```text
Power ON
   ↓
Initialize Peripherals
   ↓
Read Traffic Density
   ↓
Density > Threshold ?
 ┌─────────────┬──────────────┐
 │ YES         │ NO           │
 ↓             ↓
Heavy Traffic  Normal Traffic
 ↓             ↓
GREEN = 10s    GREEN = 5s
 ↓             ↓
PWM = 100%     PWM = 30%
 ↓
Send UART Logs
 ↓
Execute Signal Sequence
(GREEN → YELLOW → RED)
 ↓
Emergency Interrupt?
 ┌─────────────┐
 │ YES         │
 ↓
Force RED Signal
Activate Buzzer
Send UART Alert
```

---

# **📄 Main Firmware Features**

---

## **ADC-Based Density Monitoring**

The ADC continuously reads analog values from AN0.

```c
int density = adc_read();
```

If traffic density exceeds the threshold:

```c
if(density > 600)
```

the system:

- Extends GREEN signal duration
- Increases streetlight brightness
- Sends HIGH traffic UART logs

---

## **Hardware Interrupt Emergency Handling**

Emergency button connected to RB0 triggers INT0 interrupt.

```c
void __interrupt() ISR()
{
    if(INTCONbits.INT0IF)
```

The interrupt instantly:

- Stops normal sequence
- Forces RED signal
- Activates buzzer
- Sends emergency UART alert

---

## **PWM Streetlight Brightness Control**

PWM is generated using CCP1 module on RC2.

```c
pwm_set_duty(1023);
```

Brightness levels:

| Traffic Condition | PWM Duty |
|-------------------|-----------|
| Normal Traffic | Low Brightness |
| Heavy Traffic | Maximum Brightness |

---

## **UART Telemetry Communication**

UART continuously transmits traffic logs.

Example:

```text
TRAFFIC: NORMAL
TRAFFIC: HIGH
ALERT: AMBULANCE DETECTED
```

This enables external monitoring through a PC terminal.

---

## **Timer1 Non-Blocking Delay**

Timer1 generates accurate hardware delays.

Unlike software delays:

- CPU remains responsive
- Interrupts can instantly preempt execution
- Better real-time behavior is achieved

---

# **▶️ How to Build the Project**

Click the:

```text
Hammer Icon
```

in MPLAB X IDE.

Generated HEX file location:

```text
dist/default/production
```

---

# **🧪 Proteus Simulation Steps**

1. Open Proteus
2. Place all required components
3. Complete circuit connections
4. Load generated HEX file into PIC18F4580
5. Run simulation

---

# **✅ Expected LCD Output**

---

## **Startup Screen**

```text
SYSTEM BOOTING..
```

---

## **Normal Traffic**

```text
TRAFFIC: NORMAL
SIGNAL: GREEN
```

---

## **Heavy Traffic**

```text
TRAFFIC: HEAVY
SIGNAL: GREEN
```

---

## **Emergency Override**

```text
EMERGENCY OVERRIDE
```

---

# **💻 Expected UART Output**

```text
============================
SMART TRAFFIC SYSTEM BOOTED
============================

TRAFFIC: NORMAL
TRAFFIC: HIGH
ALERT: AMBULANCE DETECTED
```

---

# **📚 Concepts Covered**

- Embedded C Programming
- ADC Programming
- UART Communication
- LCD Interfacing
- PWM Generation
- Timer1 Programming
- External Interrupt Handling
- RTOS-Style Embedded Design
- PIC18F4580 Peripheral Programming

---

# **🚀 Future Improvements**

Possible enhancements include:

- Automatic ambulance RF detection
- IoT cloud traffic monitoring
- GSM emergency notification
- Multi-road traffic synchronization
- Camera-based density analysis
- AI-based traffic optimization
- GPS-assisted emergency routing

---

# **👨‍💻 Author**

**Ramkishan**  
Embedded Systems | Real-Time Firmware Development | PIC Microcontrollers
