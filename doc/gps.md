# GNNS Receiver

The Reaktor uses an [L76X GPS HAT](https://www.waveshare.com/wiki/L76X_GPS_HAT) for the Raspberry Pi as GNNS Receiver.

# Installation

1. Enable the serial port on the Raspberry Pi by running `sudo raspi-config`, navigating to `Interface Options` > `Serial Port`, and selecting `No` for the login shell and `Yes` for enabling the serial port hardware. Reboot the Raspberry Pi after making these changes.
1. Acivate UART mapping by switching to `dtoverlay=uart0-pi5` in `/boot/firmware/config.txt`. Reboot the Raspberry Pi after making these changes.
1. Install GPSD via `sudo apt-get install gpsd gpsd-clients`
1. Configure L76X ad default device in GPSD in `/etc/default/gpsd`:\
    ```
    # Devices gpsd should collect to at boot time.
    # They need to be read/writeable, either by user gpsd or the group dialout.
    DEVICES="/dev/ttyAMA0"
    # Other options you want to pass to gpsd
    GPSD_OPTIONS="-n -s 9600"
    ```
1. Apply configuration changes with `sudo systemctl enable gpsd` and `sudo systemctl restart gpsd`.
1. Check if the GPS is working with `cgps -s` or `gpsmon`. You should see the GPS coordinates and other information. For trouble shooting, check `sudo systemctl status gpsd` or consult https://raspberry.tips/en/raspberrypi-tutorials/raspberry-pi-gps-module-positioning-navigation or https://www.waveshare.com/wiki/L76X_GPS_HAT.

