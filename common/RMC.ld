PHDRS {
    text PT_LOAD;
    rodata PT_LOAD;
    data PT_LOAD;
}

SECTIONS {
    .text base : { *(first) *(.text*) } :text
    .ctors : { *(.ctors*) } :rodata
    .rodata : { *(.rodata*) } :rodata
    .data : { *(.data*) *(.bss*) *(.sbss*) } :data

    stackTop = base - 0x8;

    ctors_start = ADDR(.ctors);
    ctors_end = ctors_start + SIZEOF(.ctors);

    diskID = 0x80000000;

    versionInfo = 0x80003f00;

    vtr = 0xcc002000;
    dcr = 0xcc002002;
    vto = 0xcc00200c;
    vte = 0xcc002010;
    tfbl = 0xcc00201c;
    bfbl = 0xcc002024;
    hsw = 0xcc002048;
    hsr = 0xcc00204a;
    visel = 0xcc00206e;

    ppcmsg = 0xcd000000;
    ppcctrl = 0xcd000004;
    armmsg = 0xcd000008;

    irqmask = 0xcd000034;

    aicr = 0xcd006c00;
}
