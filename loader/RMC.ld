SECTIONS {
    .text base : { *(first) *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) *(.bss*) *(.sbss*) }

    stackTop = base;

    payloadSize = 0x80004000;
}
