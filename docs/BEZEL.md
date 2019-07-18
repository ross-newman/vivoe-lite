# Bezel Inputs
## Octal UARTS
For displays using the EXAR XR17V358IB176 8 port serial input device you will need to access the eighth serial port. To find this run:
```
~# dmesg | grep tty
[    0.000000] console [tty0] enabled
[    1.383931] 00:06: ttyS0 at I/O 0x3f8 (irq = 4, base_baud = 115200) is a 16550A
[    1.405176] 00:07: ttyS1 at I/O 0x2f8 (irq = 3, base_baud = 115200) is a 16550A
[    4.766887] 0000:01:00.0: ttyS4 at MMIO 0xf7d00000 (irq = 36, base_baud = 7812500) is a XR17V35X
[    4.769759] 0000:01:00.0: ttyS5 at MMIO 0xf7d00400 (irq = 36, base_baud = 7812500) is a XR17V35X
[    4.769977] 0000:01:00.0: ttyS6 at MMIO 0xf7d00800 (irq = 36, base_baud = 7812500) is a XR17V35X
[    4.772037] 0000:01:00.0: ttyS7 at MMIO 0xf7d00c00 (irq = 36, base_baud = 7812500) is a XR17V35X
[    4.775788] 0000:01:00.0: ttyS8 at MMIO 0xf7d01000 (irq = 36, base_baud = 7812500) is a XR17V35X
[    4.777859] 0000:01:00.0: ttyS9 at MMIO 0xf7d01400 (irq = 36, base_baud = 7812500) is a XR17V35X
[    4.778027] 0000:01:00.0: ttyS10 at MMIO 0xf7d01800 (irq = 36, base_baud = 7812500) is a XR17V35X
[    4.781749] 0000:01:00.0: ttyS11 at MMIO 0xf7d01c00 (irq = 36, base_baud = 7812500) is a XR17V35X

``` 
The MMIO is the octal device and the eighth port is /dev/ttyS11. Connect using 115200-N-1 and ? for help menu.
