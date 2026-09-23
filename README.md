# 🎛️ Controlul turației unui motor DC prin PWM

### ADC-to-PWM DC Motor Speed Control — PIC18F8722

---

# 🇷🇴 Română

## Despre proiect

Proiectul implementează un sistem simplu de control al vitezei unui motor DC folosind perifericele hardware ale microcontrolerului **PIC18F8722**.

Un **potențiometru conectat la AN0/RA0** furnizează o tensiune analogică. Aceasta este convertită de ADC-ul intern într-o valoare digitală pe **10 biți**, în intervalul `0–1023`.

Valoarea ADC este apoi mapată către duty cycle-ul unui semnal **PWM de 5 kHz**, generat de modulul **CCP1 pe RC2**.

În acest mod, poziția potențiometrului controlează direct nivelul PWM și, implicit, viteza motorului DC.

### 🔄 Fluxul de funcționare

```text
┌───────────────┐
│ Potențiometru  │
└───────┬───────┘
        │ Analog
        ▼
┌───────────────┐
│    AN0 / RA0  │
└───────┬───────┘
        │
        ▼
┌────────────────────┐
│    ADC 10-bit      │
│      0 → 1023      │
└─────────┬──────────┘
          │
          ▼
┌────────────────────┐
│ ADC → PWM Mapping  │
└─────────┬──────────┘
          │
          ▼
┌────────────────────┐
│     CCP1 / RC2     │
│      PWM 5 kHz     │
└─────────┬──────────┘
          │
          ▼
┌────────────────────┐
│      Motor DC      │
└────────────────────┘
```

---

## ✨ Funcționalități

- 🎚️ Citirea unei valori analogice de pe **AN0/RA0**
- 🔢 Conversie analog-digitală pe **10 biți**
- ⚙️ Calcularea duty cycle-ului PWM pe baza valorii ADC
- 📡 Generarea PWM folosind **CCP1**
- ⏱️ Utilizarea **Timer2** pentru temporizarea PWM
- 🔌 Ieșire PWM pe **RC2**
- 🔄 Actualizarea continuă a comenzii motorului
- 🧪 Validarea sistemului prin simulare în **Proteus**

---

## 🧠 Principiul de control

ADC-ul produce o valoare:

```text
0 ≤ ADC ≤ 1023
```

Această valoare este utilizată pentru calcularea duty cycle-ului PWM.

Conceptual:

```text
ADC value
    │
    │ 0
    ├──────────────────────────────► Minimum PWM
    │
    │
    │ 512
    │
    │
    │ 1023
    └──────────────────────────────► Maximum PWM
```

Prin urmare:

| Potențiometru | ADC | PWM |
|---:|---:|---:|
| Minim | ~0 | Minim |
| Mediu | ~512 | Mediu |
| Maxim | ~1023 | Maxim |

Actualizarea comenzii este realizată continuu, cu o perioadă de aproximativ **50 ms** între citiri consecutive.

---

## ⚙️ Configurație hardware

| Componentă / periferic | Configurație |
|---|---|
| Microcontroller | **PIC18F8722** |
| Oscillator | **4 MHz** |
| Analog input | **AN0 / RA0** |
| ADC resolution | **10-bit** |
| ADC range | **0–1023** |
| PWM module | **CCP1** |
| PWM output | **RC2** |
| PWM frequency | **5 kHz** |
| PWM timer | **Timer2** |
| Simulation | **Proteus** |
| Programming language | **C** |
| Compiler | **MPLAB XC8** |

---

## 🔬 Implementare firmware

### 1. Inițializare ADC

Funcția `init_ADC()` configurează:

- AN0 ca intrare analogică;
- tensiunea de referință la alimentare;
- rezultatul ADC cu aliniere la dreapta;
- timpul de achiziție;
- frecvența de conversie;
- activarea modulului ADC.

Rezultatul conversiei este reconstruit din registrele `ADRESH` și `ADRESL`:

```c
ch = ((ADRESH << 8) | ADRESL);
```

Rezultatul este astfel disponibil ca o valoare de **10 biți**.

---

### 2. conversie analog-digitală

Funcția `ADC_start()` pornește conversia și așteaptă finalizarea acesteia:

```c
GO = 1;

while(DONE);

ch = ((ADRESH << 8) | ADRESL);
```

Valoarea rezultată este transmisă mai departe către rutina de control PWM.

---

### 3. Inițializare PWM

Funcția `PWM_init()` configurează modulul **CCP1** în modul PWM.

Frecvența este calculată pe baza:

```text
Fosc = 4 MHz
FPWM = 5 kHz
Timer2 prescaler = 1:4
```

Registrul `PR2` este calculat în firmware pentru a obține frecvența PWM dorită.

Ieșirea CCP1 este configurată pe:

```text
RC2
```

---

### 4. Mapare ADC → PWM

Funcția `PWM_duty()` primește valoarea ADC și o transformă într-o valoare PWM.

Maparea este realizată proporțional cu domeniul ADC:

```c
duty = ((float)duty / 1023) *
       (_XTAL_FREQ / (PWM_freq * TMR2_pre));
```

Valoarea rezultată este apoi împărțită între:

- `CCPR1L` — partea principală a duty cycle-ului;
- biții corespunzători din `CCP1CON` — rezoluția suplimentară.

Această implementare permite controlul duty cycle-ului PWM utilizând întreaga valoare furnizată de ADC.

---

## 🔁 Bucla principală de control

Firmware-ul execută în mod continuu următorul ciclu:

```c
while(1)
{
    y = ADC_start();
    PWM_duty(y);

    __delay_ms(50);
}
```

Astfel, sistemul:

```text
Read ADC
   ↓
Calculate PWM
   ↓
Update CCP1
   ↓
Wait 50 ms
   ↓
Read ADC again
   ↺
```

Nu este utilizată o valoare PWM fixă; aceasta este recalculată în funcție de poziția curentă a potențiometrului.

---

## 🧪 Simulare în Proteus

Proiectul include o simulare **Proteus** pentru verificarea funcționării firmware-ului și a traseului:

```text
Analog Input
     ↓
    ADC
     ↓
PWM Duty Cycle
     ↓
CCP1 / RC2
     ↓
DC Motor
```

Simularea permite observarea comportamentului sistemului fără utilizarea imediată a hardware-ului fizic.

---

## 📁 Structura proiectului

```text
Controlul-turatiei-unui-motor-DC-prin-PWM/
│
├── README.md
│
├── proteus/
│   └── project.pdsprj
│
└── src/
    └── main.c
```

### `src/main.c`

Firmware-ul microcontrolerului, incluzând:

- configurarea ADC;
- citirea AN0;
- configurarea CCP1;
- configurarea Timer2;
- calculul duty cycle-ului;
- bucla principală de control.

### `proteus/project.pdsprj`

Proiectul de simulare Proteus utilizat pentru testarea sistemului.

---

# 🇬🇧 English

## 📌 Project Overview

This project implements a **DC motor speed control system using a PIC18F8722 microcontroller**.

A potentiometer connected to **AN0/RA0** provides an analog input that is converted into a **10-bit ADC value** ranging from `0` to `1023`.

The ADC reading is then mapped to the duty cycle of a **5 kHz PWM signal** generated by the **CCP1 peripheral on RC2**.

As the potentiometer position changes, the PWM duty cycle is updated accordingly, providing proportional control of the DC motor speed.

---

## ✨ Features

- 🎚️ Analog input acquisition through **AN0/RA0**
- 🔢 **10-bit ADC** conversion
- ⚙️ ADC-to-PWM value mapping
- 📡 Hardware PWM using **CCP1**
- ⏱️ **Timer2** used as the PWM time base
- 🔌 PWM output through **RC2**
- 🔄 Continuous control loop
- 🧪 **Proteus** simulation for system validation

---

## 🔄 System Architecture

```text
┌────────────────┐
│  Potentiometer │
└───────┬────────┘
        │ Analog Voltage
        ▼
┌────────────────┐
│   AN0 / RA0    │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│    10-bit ADC  │
│    0 ... 1023  │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│ ADC → PWM      │
│     Mapping    │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│   CCP1 / RC2   │
│    PWM 5 kHz   │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│    DC Motor    │
└────────────────┘
```

---

## ⚙️ Technical Specifications

| Parameter | Configuration |
|---|---|
| MCU | **PIC18F8722** |
| Clock frequency | **4 MHz** |
| Analog input | **AN0 / RA0** |
| ADC resolution | **10-bit** |
| ADC range | **0–1023** |
| PWM peripheral | **CCP1** |
| PWM output | **RC2** |
| PWM frequency | **5 kHz** |
| Timer | **Timer2** |
| Simulation | **Proteus** |
| Language | **C** |
| Compiler | **MPLAB XC8** |

---

## 🧩 Firmware Implementation

### ADC

The `init_ADC()` function configures AN0 as an analog input and initializes the ADC peripheral.

The conversion result is reconstructed from the high and low ADC registers:

```c
ch = ((ADRESH << 8) | ADRESL);
```

This provides the complete 10-bit ADC reading.

### PWM

The `PWM_init()` function configures **CCP1** in PWM mode and uses **Timer2** as its timing source.

The firmware calculates the Timer2 period register based on:

```text
Fosc = 4 MHz
PWM frequency = 5 kHz
Timer2 prescaler = 1:4
```

The PWM output is assigned to:

```text
RC2 / CCP1
```

### ADC-to-PWM Conversion

The ADC value is normalized against its maximum value of `1023` and converted into the corresponding PWM range.

Conceptually:

```text
ADC = 0       → minimum PWM
ADC ≈ 512     → intermediate PWM
ADC = 1023    → maximum PWM
```

This creates a direct relationship between potentiometer position and motor control signal.

---

## 🔁 Control Loop

The main firmware loop continuously reads the ADC and updates the PWM output:

```c
while(1)
{
    y = ADC_start();
    PWM_duty(y);

    __delay_ms(50);
}
```

The control cycle can therefore be summarized as:

```text
ADC Read
   ↓
PWM Calculation
   ↓
CCP1 Update
   ↓
50 ms delay
   ↓
Repeat
```

---

## 🧪 Simulation

The project includes a **Proteus simulation** used to validate the embedded system before deployment to physical hardware.

The simulation focuses on the complete signal path:

```text
Potentiometer
      ↓
   AN0 / ADC
      ↓
 ADC value
      ↓
 PWM calculation
      ↓
  CCP1 / RC2
      ↓
   DC Motor
```

---

## 📁 Project Structure

```text
Controlul-turatiei-unui-motor-DC-prin-PWM/
│
├── README.md
│
├── proteus/
│   └── project.pdsprj
│
└── src/
    └── main.c
```

| File | Description |
|---|---|
| `src/main.c` | PIC18F8722 firmware |
| `proteus/project.pdsprj` | Proteus simulation project |

---

## 🎯 What This Project Demonstrates

This project demonstrates practical experience with:

- **PIC18F8722 microcontroller programming**
- **ADC peripheral configuration**
- **10-bit analog signal acquisition**
- **CCP1 PWM generation**
- **Timer2 configuration**
- **Register-level embedded programming**
- **ADC-to-PWM scaling**
- **DC motor control**
- **Embedded system simulation in Proteus**
- **C firmware development using MPLAB XC8**

---

## 👨‍💻 Project Focus

The main goal of the project is to demonstrate how a **real-world analog input can be acquired, processed and converted into a hardware-generated control signal** using the peripherals of a microcontroller.

In other words:

```text
Analog Input → Digital Processing → PWM Output → Motor Control
```

This makes the project a compact example of **embedded signal acquisition and actuator control** using a PIC microcontroller.

---

## 👤 Autor / Author

**IonutD**
