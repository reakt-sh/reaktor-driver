# REAKTOR Driver

The driver software for the REAKTOR test vehicle, part of the [REAKT innovation community](https://reakt.sh).

<img width="800" alt="Image of the REAKTOR vehicle from a low angle exposing the plate with the REAKT logo." src="https://github.com/user-attachments/assets/4316c723-d8cc-4965-a245-3949372ff433" />
© REAKT / Tatjana Kay

## The Vehicle

The REAKTOR vehicle is a test platform for the development of autonomous driving software and safe [remote control](https://github.com/reakt-sh/remote-control) of rail vehicles. It consists of a aluminum chassis, an electric motor, a battery pack, and a custom-built control box. See [schematics](./schematics) for the components of the control box and the connected devices. 

<img width="800" alt="Image of the entire REAKTOR vehicle" src="https://github.com/user-attachments/assets/8dcb3e83-a1b8-4fee-a675-b0ac60c8b263" />
© REAKT / Alexander SR

## Main Control Components

The motor is controlled by a Kelly KLS-RM 7225 motor controller. This MCU is connected to an Arduino Mega that handles all hardware interaction. It is additionally connected to the manual throttle level, switches, and a display. The onboard computer is currently a Rasberry Pi 5 that is connected to the Arduino board, a power meter, GPS, and via Network to further devices, such as cameras.

## Repository Structure

This is a monorepo containing both the driver for the Arduino controller board and the Python connector for onboard computer.

This repository is structured into two main components and auxiliary data:
- **[driver](./driver)** The Arduino software that controls the motor controller and handles manual inputs (throttle lever, switches, etc.).
- **[connector](./connector)** The Python module that provides an API for the control of all REAKTOR systems, including the driver board and power sensors.
- **[definitions](./definitions)** Central definitions and configurations for both main components.
- **[schematics](./schematics)** The schematics for the REAKTOR control box with all peripheral devices.
