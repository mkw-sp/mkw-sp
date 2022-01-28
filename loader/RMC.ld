SECTIONS {
    .text base : { *(first) *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) *(.bss*) *(.sbss*) }
}
