# 🕹️ ARM-based Tetris
Fully functional Tetris from scratch for the LandTiger NXP LPC1768 development board, where both low-level hardware constraints and high-level game logic needed to be satisfied.

![foto](https://github.com/user-attachments/assets/70cb26b7-cce3-41a5-b85c-369365e6edfa)

Developed by: Fernando Velilla Hurtado

Computer Architectures Extra-Point Project | Politecnico di Torino 

A fully functional Tetris engine built from the ground up for the LandTiger NXP LPC1768 development board, where both low-level hardware constraints and high-level game logic needed to be satisfied.

# 📺 Watch the Gameplay

[![Watch the video](https://img.youtube.com/vi/yfAPtPvEgNg/0.jpg)](https://youtu.be/yfAPtPvEgNg)

# 🏗️ Implementation details
## ⏱️ Precise Hardware Timing
The game engine relies on dedicated hardware timers to maintain perfect synchronization:


Gravity effect: Powered by Timer 0 at a frequency of 1Hz, ensuring the Tetromino drops exactly once per second.


Button debouncing: Timer 1 is utilized specifically to eliminate mechanical button bouncing for KEY1 and KEY2.


Real-Time Responsiveness: The Repetitive Interrupt Timer (RIT) handles the Joystick inputs, allowing for fluid player movement.

## 🤖 Collaborative AI Development
This project utilized Gemini (Google AI) to optimize complex logic:


Mathematical Rotation: Assisted in calculating Tetromino rotation centered precisely about a pivot point.


Collision Detection: AI-assisted functions in lib_tetris.c handle complex boundary checks and line-clearing algorithms.


UI Design: Influenced the visual layout for the splash screen and the Game Over sequence.

# 🚀 Deployment
The system is designed for both physical hardware and simulation:


Hardware: Fully tested on the LandTiger development board.


Emulator: Compatible with the LandTiger emulator by switching the timer initialization constants in sample.c.

## How to build

1. Open the project in Keil uVision.
2. If using the emulator, ensure the timer initialization lines in sample.c are set for the emulator constants.
3. Compile and flash to the LPC1768.
