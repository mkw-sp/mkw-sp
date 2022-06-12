SECTIONS {
    bootstrap 0x80003400 : { *(bootstrap) }
    .text base : { *(first) *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) *(.bss*) *(.sbss*) }

    stackTop = base - 0x8;

    diskID = 0x80000000;

    versionInfo = 0x80003f00;

    vtr = 0xcc002000;
    dcr = 0xcc002002;
    vto = 0xcc00200c;
    vte = 0xcc002010;
    tfbl = 0xcc00201c;
    bfbl = 0xcc002024;
    visel = 0xcc00206e;

    ppcmsg = 0xcd000000;
    ppcctrl = 0xcd000004;
    armmsg = 0xcd000008;

    irqmask = 0xcd000034;

    aicr = 0xcd006c00;
}
