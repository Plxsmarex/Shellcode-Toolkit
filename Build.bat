@color 2
@gcc -o Program.exe Main.c -e EntryPoint -T Linker.ld -Os -s -fPIC -Wl,--gc-sections -ffunction-sections -fno-asynchronous-unwind-tables -fno-ident -ffreestanding -nostartfiles -nostdlib -mno-stack-arg-probe
@objcopy -j .text -O binary Program.exe Program.bin
@pause
