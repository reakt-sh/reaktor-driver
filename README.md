# REAKTOR Driver

The driver software for the REAKTOR test vehicle developed as part of the [REAKT innovation community](https://reakt.sh).

<img width="400" alt="Image of the REAKTOR vehicle from a low angle exposing the plate with the REAKT logo." src="https://github.com/user-attachments/assets/1e5339dd-a5fe-49ee-86d7-2a3da29ce01f" />\
© REAKT / Tatjana Kay

## The Vehicle

The REAKTOR vehicle is a test platform for the development of autonomous driving software and safe [remote control](https://github.com/reakt-sh/remote-control) of rail vehicles. It consists of a aluminum chassis, an electric motor, a battery pack, and a custom-built control box. See [schematics](./schematics) for the components of the control box and the connected devices. 

<img width="800" alt="Image of the entire REAKTOR vehicle" src="https://github.com/user-attachments/assets/85eb56dd-6f61-4c78-8a7a-b465dd6a0e6b" />\
© REAKT / Alexander SR

## Main Control Components

The motor is controlled by a Kelly KLS-RM 7225 motor controller. This MCU is connected to an Arduino Mega that handles all hardware interaction. It is additionally connected to the manual throttle level, switches, and a display. The onboard computer is currently a Rasberry Pi 5 that is connected to the Arduino board, a power meter, GPS, and via Network to further devices, such as cameras.

## Repository Structure

This is a monorepo containing both the driver for the Arduino controller board and the Python connector for onboard computer.

This repository is structured into two main components and auxiliary data:
- **[driver](./driver)** The Arduino software that controls the motor controller and handles manual inputs (throttle lever, switches, etc.).
- **[connector](./connector)** The Python module that provides an API for the control of all REAKTOR systems, including the driver board and power sensors.
- **[definitions](./definitions)** Central definitions and configurations for both main components.
- **[doc](./doc)** Documentation for the vehicle setup, including schematics for the REAKTOR control box with all peripheral devices.