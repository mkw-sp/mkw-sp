SECTIONS {
    .text base : { *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) *(.bss*) *(.sbss*) }

    stackTop = base - 0x8;

    ppcmsg = 0xcd000000;
    ppcctrl = 0xcd000004;
    armmsg = 0xcd000008;
}
