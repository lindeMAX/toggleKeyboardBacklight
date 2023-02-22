# ToggleKeyboardBacklight

Automatically toggles your keyboard backlight after a certain amount of seconds.

## Usage

``` Bash
toggleKeyboardBacklight <Timeout in seconds> <List of Devices seperated by white spaces> 
```

E.g.:

``` Bash
toggleKeyboardBacklight 10 /dev/input/by-path/platform-i8042-serio-1-event-mouse /dev/input/by-path/platform-i8042-serio-0-event-kbd
```

## Build & Install

```Bash
make build
sudo make install
```

### Uninstall
```Bash
sudo make uninstall
```
