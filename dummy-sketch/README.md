## Dependencies (specifically, for Debian Bullseye)

* Arduino IDE (the [latest stable version](https://downloads.arduino.cc/arduino-ide/arduino-ide_2.0.0_Linux_64bit.AppImage));
* `arduino-builder`;
* `arduino-mk`.

The last two can be installed with `sudo apt install arduino-builder arduino-mk`.

Various commands (assuming `IntroductionToRobotics/dummy-sketch` is the current directory and the Arduino is plugged in):

```bash
$ make # compile
$ make upload # upload to the board
$ make monitor # monitor the serial output
```

Additionally, see the comment at the top of [`main.cpp`](main.cpp).

Note:

For LSP (`clangd`) integration (e.g.: for vim or Visual Studio Code) you can
obtain the `compile_commands.json` file with `bear`:

```bash
$ sudo apt install bear
$ cd /path/to/dummy-sketch
$ make clean
$ bear -- make
```
