# -LED-and-Button-Board

A 7x7 programmable LED and button board implementing an embedded system based on a NUCLEO-L476RG microcontroller. 

![7x7 LED Game Board](https://github.com/user-attachments/assets/e1920303-fd22-4ffe-aeeb-2260c6b6d389)

Features:
- WS2812B RGB LEDs with custom graphics
- Custom ADC matrix for precise position detection
- Game loops, animation timing, input response
- 7x7 main board + 16x16 secondary display
- Connect Four, Light Speed, and expandable game library
- STM32 HAL, DMA, PWM, ADC peripherals

Compontents:
- STM32L476RG (ARM Cortex-M4)
- 7x7 WS2812B RGB Matrix (49 LEDs)
- 16x16 WS2812B RGB Matrix (256 LEDs)

Team:
- Felix Coffelt: Embedded Software, Game Logic, System Architecture
- Kyle Magtoto: Embedded Software, Hardware Design, PCB Layout

References:
- Thomsen, Lars B. stm32-ws2812. GitHub, https://github.com/lbthomsen/stm32-ws2812. 
