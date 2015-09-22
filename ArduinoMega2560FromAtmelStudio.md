Programming Arduino Mega 2560 from Atmel Studio
===============================================

This is for those that want to program the Arduino Mega 2560 from Atmel Studio, without the use of licenced software such as Visual Micro

TODO: Check if it is possible to use Arduino standard headers/library if creating a c++ project
        
Programming the Arduino with avrdude over USB
---------------------------------------------

This method provides no debugging capabilities. But since it does not use the Atmel-ICE, you don't need to connect it interchangably between two devices (the other being the ATmega162).


1. **Install Arduino software from https://www.arduino.cc/en/Main/Software**  
    This is needed for USB driver and avrdude  
    This is already done on the machines at the lab
    
2. **Find the COM port of the Arduino**  
    Plug in the Arduino with its USB cable  
    Open device manager (type "devmgmt.msc" in the start menu)  
    Look under "Ports (Com & LPT)"  

3. **Set up avrdude as external tool in Atmel Studio**  
    In Atmel Studio, open "Tools" -> "External Tools..."  
    Add a new tool with settings:  
      - Title:      ArduinoMega2560 Programmer
      - Command:    `{Arduino install dir}\hardware\tools\avr\bin\avrdude.exe`
      - Arguments:  `-D -C "{Arduino install dir}\hardware\tools\avr\etc\avrdude.conf" -p atmega2560 -c wiring -P COM## -b 115200 -U flash:w:$(ProjectDir)Debug\$(TargetName).hex:i`  
        Remember to set COM## to the COM port number found from the device manager
      - Enable checkbox "Use Output window"
    
    TODO: find the arduino install dir on the lab computers
    
4. **Create a simple test program**  
    See the [bottom of this document](#simple-test-program)

5. **Compile**  
    Open "Build" -> "Build Solution" (F7) (or rebuild, Ctrl+Alt+F7)

6. **Run avrdude from Atmel Studio**  
    Open "Tools" -> "ArduinoMega2560 Programmer" (no shortcut available)
    
7. **Verify**  
    The orange "L" LED on the board should be blinking


Programming the Arduino with JTAG
---------------------------------

This method does not program over USB, but the device still requires power, so the USB connection still has some value. 
With JTAG, you get full debugging capabilities, but beware that the JTAG uses 4 pins from port F. 
This means that you may want to use port C instead of port F for communicating with the motor box. 

TODO: verify the above claim

If you configure the Arduino to use JTAG, you must re-load the bootloader to be able to program it over USB again. 
This procedure is explained in the [next chapter](#resetting-the-bootloader). 

1. **Connect Atmel-ICE with ISP interface**  
    Remove the expansion card on the Arduino (carefully...)  
    Connect the 2x3 header from the Atmel-ICE kit to the Arduino  
        The plastic "nibble" should point toward the center of the card  
    
2. **Set the JTAG Enable fuse bit**  
    In Atmel Studio, open "Tools" -> Device Programming" (Ctrl+Shift+P)  
    Select  
     - Tool:       Atmel-ICE
     - Device:     ATmega2560
     - Interface:  ISP
    Click "Apply", and verify that you can read Target Voltage and Device signature  
    Select "Fuses" from the left menu  
    Enable checkbox "JTAGEN", and click "Program"  

3. **Connect the Atmel-ICE with JTAG**  
    Disconnect the ISP header  
    Connect the JTAG header  
     - 5V, GND, and reset are found on the lower left header
     - TCK, TMS, TDO, TDI are found on PF4..PF7, aka A4..A7
       (These are in the same order as on the ATmega162)
    Open "Tools" -> Device Programming"  
    Select tool and device as before, but set interface to JTAG  
    Click "Apply", and verify that you can read Target Voltage and Device signature  
    
4. **Create a simple test program**  
    See the [bottom of this document](#simple-test-program)

5. **Program and debug as normal**  
    Program with "Debug" -> "Start Without Debugging" (Ctrl+Alt+F5)  
    or use Debug mode ("Debug" -> "Continue", F5) and I/O View ("Debug" -> "Windows" -> "I/O View"), etc
        
Resetting the bootloader
========================

1. **Connect Atmel-ICE with ISP interface**  
    As [described above](#programming-the-arduino-with-jtag)

2. **Disable the JTAG Enable fuse bit**  
    As [described above](#programming-the-arduino-with-jtag)

3. **Program the ATmega2560 with the default bootloader:**  
    `{Arduino install dir}\hardware\arduino\avr\bootloaders\stk500v2\stk500boot_v2_mega2560.hex`  
    This may take some time, allow Atmel Studio to "Wait 1 more minute" if necessary.
    
4. **Verify that you can program the Arduino over USB**  
    The simplest way is to run the blink sketch from the Arduino editor  
    From the Arduino editor, select "File" -> "Examples -> "01.Basics" -> "Blink" (or "AnalogReadSerial", as this can also test serial communication over USB)  
    Choose "Tools" -> "Board: [...]" -> "Arduino/Genuino Mega or Mega 2560"  
    Choose "Tools" -> "Port" -> whatever port it is connected to  
    Choose "Tools" -> "Programmer" -> "Arduino as ISP"  
    Run "Sketch" -> "Upload" (Ctrl+U)  
    If running AnalogReadSerial example, open "Tools" -> "Serial Monitor" (Ctrl+Shift+M)  
    

Simple Test Program
===================

"File" ->  "New" -> "Project..."  
Choose "GCC C Executable Project"  
In Device Selection, choose "ATmega2560"  

```C
#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>

int main(void){
    DDRB = 0xff;
    while(1)
    {
        PORTB = 0xff;
        _delay_ms(500);
        PORTB = 0;
        _delay_ms(500);
    }
}
``` 
