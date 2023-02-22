# ToggleKeyboardBacklight

Automatically toggles your keyboard backlight.

It is turned off after a certain amount of seconds and turned on again if an event on a given device happend.

Devices that can be used are listed under */dev/input*.
<br> I prefer using the links under */dev/input/by-path* as they are more human readable:
<br> Keyboards do end with *-kbd* and mouses/touchpads with *-mouse*.

## Usage

``` Bash
toggleKeyboardBacklight <Timeout in seconds> <List of Devices seperated by white spaces> 
```

E.g.:

``` Bash
toggleKeyboardBacklight 10 /dev/input/by-path/platform-i8042-serio-1-event-mouse /dev/input/by-path/platform-i8042-serio-0-event-kbd
```

## Dependencies
- gcc
- make
- brightnessctl

## Build & Install

```Bash
make build
sudo make install
```

## Uninstall
```Bash
sudo make uninstall
```
