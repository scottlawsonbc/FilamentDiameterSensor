# FilamentDiameterSensor

ENPH 459 - Engineering Physics Project I
----------------------------------------
A Non-contact Diameter Measurement Device for 3D Printers and Filament Manufacturing

### File Organization ###

1.  *Arduino* - 
    Initial prototype was implemented for Arduino.

2.  *Code* - 
    Includes the rest of the project's (non-MATLAB) code. Several versions of the shadow-simulator, a fork of the printer firmware Marlin(modified to interface with the filament sensor), and the sensor's firmware to be run on a STM32F3 development board.
    
3.  *Eagle* - 
    PCB layout files for the photodiode array (TSL1402CL) breakout board.
    
4.  *Matlab* - 
    Matlab script used to measure the relative print quality based on the roughness of vertical printed edges.

5.  *Pictures* - 
    Pictures from the sensor development and print quality testing.

6.  *Reference Documents* - 
    Documentation for the STM32F3 development board, the linear photodiode array used in the sensor, and the prototype sensor.
    
7.  *Solidworks* - 
    All 3D design for the sensor, mostly Solidworks with some .STL-rasterized models for 3D printing.
