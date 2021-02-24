# Logitech G600 on Linux

Utility program for binding actions to keys on the Logitech G600 gaming mouse. Supports 16 keys and the G-shift button for a total of 32 fast shortcuts. `g600.py` can be configured to set program specific bindings.

Before running this program open the Logitech Gaming Software on a Windows or Mac OS machine. Assign the three basic mouse buttons to their standard functions. The G-shift button should be assigned to the G-shift function. All the remaining buttons (scroll left, scroll right, G7, ... G20) should be set to emulate (unique) keyboard keys (but not modifier keys).

## Usage

1. Clone this repository.
2. Open `g600.py` and fill in the commands for the keys.
    * Set default bindings in `DEFAULT_BINDINGS`
    * Set program specific bindings in `CUSTOM_BINDINGS`
3. Compile with `gcc g600.c -o g600`.
4. Run with `sudo ./g600`.

For command ideas you can look at programs like `xdotool`, `xdo`, `pulseaudio-ctl`, `xclip`, `i3-msg`. You can also run your own scripts.

The program needs privileges to communicate with G600 so typically it'll be started with `sudo`. Alternatively (works on Ubuntu 18.04) you can force the program to run as the `input` group with:

```bash
sudo chown .input g600
sudo chmod g+s g600
```

## Notes

When doing initial configuration through Logitech Gaming Software, the G-shift + G9 ... G20 keys should be changed from their default bindings Ctrl+1, Ctrl+2, etc, to something like A, B, etc. Not doing so caused all keys with a Ctrl+num binding to emit the same signature thus unable to assign them different bindings.

To find the name of a program to add to `CUSTOM_BINDINGS` run 1) `xprop WM_CLASS`, 2) click program, 3) use the first value from output as key in `CUSTOM_BINDINGS`.

Depending on when your G600 mouse was purchased, your mouse scancodes may be different from my config. Run `./g600` and press around your mouse buttons to see its scancodes. Change the scancode in `downCommands`, `upCommands` if needed, then recompile `g600.c`.
