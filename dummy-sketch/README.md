## Dependencies (specifically, for Debian Bullseye)

* Arduino IDE (preferably the latest stable version);
* `arduino-builder`;
* `arduino-mk`.

The last two can be installed with `sudo apt install arduino-builder arduino-mk`.

Additionally, see `main.cpp`.

Note:

For LSP (`clangd`) integration (e.g.: for vim or Visual Studio Code) you can
obtain the `compile_commands.json` file with `bear`:

```bash
$ sudo apt install bear
$ cd /path/to/dummy-sketch
$ make clean
$ bear -- make
```
