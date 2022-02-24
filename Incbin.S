#define STRINGIZE2(a) #a
#define STRINGIZE(a) STRINGIZE2(a)

#define CONCAT2(a, b) a ## b
#define CONCAT(a, b) CONCAT2(a, b)

    .section .rodata
    .align 0x4
    .global NAME
NAME:
    .incbin STRINGIZE(PATH)
CONCAT(NAME, End):
    .size NAME, . - NAME
    .align 0x4
    .global CONCAT(NAME, Size)
CONCAT(NAME, Size):
    .int CONCAT(NAME, End) - NAME
    .size CONCAT(NAME, Size), 0x4
