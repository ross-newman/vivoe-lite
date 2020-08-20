Prototype Windows build under MSYS2.

**Step 1: Install MSYS2**<br>
Download the MSYS2 installer that matches your platform and follow the installation instructions.

**Step 2: Install GTK3 and its dependencies**<br>
Open a MSYS2 shell, and run:
```
pacman -S mingw-w64-x86_64-gtk3
```
**Step 3 : Install build tools**
If you want to develop a GTK3 application in other languages like C, C++, Fortran, etc, you'll need a compiler like gcc and other development tools:
```
pacman -S mingw-w64-x86_64-toolchain base-devel
```
**Step 4 : Install project specific dependancies**
```
pacman -S protobuf mingw-w64-x86_64-ffmpeg
```

# Links
* [MSYS2](https://www.msys2.org/)
* [Cairo Graphics](https://www.cairographics.org/download/)
* [GTK for Windows](https://www.gtk.org/download/)
