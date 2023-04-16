# Groove Coaster controller (WIP)
Welcome to the Groove Coaster Arcade Controller project!
This project aims to create a custom Groove Coaster arcade controller using real arcade parts, a Raspberry Pi Pico, and custom PCBs with Neopixels for enhanced lighting control. 
With this controller, you can enjoy the Groove Coaster experience at home!

**This project is very much work in progress and is not ready for use yet!**

## Table of Contents
- [Features](#features)
- [Hardware](#hardware)
    - [Components](#components)
    - [Wiring Diagram and PCB](#wiring-diagram-and-pcb)
    - [Boosters](#boosters)
- [Firmware](#firmware)
- [Installation](#installation)
- [Usage](#usage)
- [License](#license)

## Features
- Authentic arcade-like experience with real Groove Coaster arcade parts
- Precise input handling using a Raspberry Pi Pico microcontroller
- Customizable lighting effects using Neopixels
- Open-source firmware and hardware for easy modification and customization
- KiCad designed PCBs for the lighting system

## Hardware

### Components
- Real pair of Groove Coaster arcade boosters
- Raspberry Pi Pico
- Custom PCBs for lighting (designed in KiCad)
    - Side lighting PCBs (4 Neopixels each, 8 per booster)
    - Top lighting PCBs (8 Neopixels each, 1 per booster)
    - Main board with Raspberry Pi Pico and booster connectors
- Enclosure
- Various cables and connectors

### Wiring Diagram and PCB
The project includes three different PCB designs:
1. Side lighting PCB: Each booster has eight of these PCBs on the side, and each PCB includes 4 Neopixels.
2. Top lighting PCB: Each booster has one of these PCBs, and each PCB has eight Neopixels.
3. Main board: This board includes the Raspberry Pi Pico and connectors for the boosters.

### Boosters
This project uses boosters from a real Groove Coaster arcade cabinet. 
With some luck these can be found on ebay or Yahoo Auctions in Japan.
Shipping can be arranged by using a proxy service like [Japan Rabbit](https://japanrabbit.com/).

## Firmware
The firmware for this project is written in C and built using CMake. 
The Pico SDK is included as a submodule in the `lib/pico-sdk` folder of this repository.

## Installation
TODO

## Usage
TODO

## License
TODO
