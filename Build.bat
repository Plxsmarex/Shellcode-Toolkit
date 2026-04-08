@gcc -o Program.exe Main.c -e EntryPoint -T Linker.ld -Oz -s -ffunction-sections -Wl,--gc-sections -fno-asynchronous-unwind-tables -nostartfiles
@objcopy -j .text -O binary Program.exe Program.bin
@pause
