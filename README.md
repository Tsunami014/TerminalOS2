# TerminalOS2
A terminal operating system... but in C!

# Dependancies
```bash
sudo apt-get install pkg-config # for building
sudo apt install libevdev-dev
```

# Building and running
- To build into an executable:
    - `make main`: Makes the main file into an executable `./main`
    - `make clean`: Removes all output from make and build and test
## Testing
- If you use VSCode, there's a launch configuration setup for this
- `make test`: Makes the main file into an executable and then runs it in kitty (ensure installed)

## Virtual terminal usage
I suggest a good way to get a real feel for it without making it it's own OS is to use a virtual terminal.

To use this in the virtual terminal, press <kbd>ctrl</kbd>+<kbd>alt</kbd>+<kbd>F1-12</kbd> and sign in. Then run the script as you would in a normal terminal; `./main` (after `make main`).

I suggest tty1 (<kbd>ctrl</kbd>+<kbd>alt</kbd>+<kbd>F1</kbd>) as it is the first one and is the most likely to be free (and easiest to remember). And with most systems, tty7 (<kbd>F7</kbd>) is the GUI environment (if not just try every one until one works).

You could even add the script somewhere in the path so you can just open the vt and instantly run the os! (I suggest renaming it to `OS` to be easier)

## To make into a bootable OS
- **BUILDING FOR THE FIRST TIME:** `./build`
- **BUILDING AFTER HAVING DONE IT BEFORE:** `./updateBuild`

### Manual use
Here are all the commands in `./build` and `./updateBuild` (only the ones with `*` are in `./updateBuild`)
1. Create the new filesystem's root folder
```bash
mkdir ./rootfs
```
2. `*`Compile the code into the rootfs
```bash
make init
```
3. `*`Pack it into an initrd and move it to the boot folder
```bash
(cd rootfs && find . | cpio -o -H newc) | gzip > initrd-tos.img
sudo mv initrd-tos.img /boot/
```
4. Add GRUB config files to have the new OS as an option
```bash
sudo tee /etc/grub.d/40_custom > /dev/null <<EOF
#!/bin/sh
exec tail -n +3 $0

menuentry "Terminal OS" {
            search --set=root --file /boot/vmlinuz
            linux /boot/vmlinuz root=/dev/ram0 rdinit=/init
            initrd /boot/initrd-tos.img
}
EOF
sudo chmod +x /etc/grub.d/40_custom
```
5. `*`Update grub and pray everything works
```bash
sudo update-grub
```
