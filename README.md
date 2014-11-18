TTK4155 "Byggern" - Project work
================================

####Authors
Adelaide Mellem  
Anders Rønning Petersen  
Eirik Wold Solnør

Extras
------
 - Fully customizable controls for Pong
   - Select (almost) any input for any control! Choose sliders, joystick, or even:
   - Bluetooth controller! The game can be played with a virtual controller on your phone.
 - 2048
   - The game that has wasted countless hours across the globe.
 - The heap is moved to the SRAM
   - The SRAM proved so reliable, we're willing to put system-critical things on it.
 - Basic font creator
   - Create a folder of .png files, and automatically generate c- and h-files.

UnExtras
--------
 - If suspend/resume is the poor man's concurrency control, then `cli()` and `sei()` is the broke man's concurrency control. We opt to actively avoid interrupts wherever possible, as this introduces a bunch of global state and uncontrollable execution order. This does, however, mean that the entire system may fail if a hardware component fails. It also means we avoid 99% of all common byggern-bugs.
 - Turns out there are GCC extensions for module constructors, scope exit functions, and nested functions. There will be lambdas!

 

