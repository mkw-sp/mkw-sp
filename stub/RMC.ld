SECTIONS {
    .text base : { *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) *(.bss*) *(.sbss*) }

    stackTop = base - 0x8;
}
