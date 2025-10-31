OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(kernel_main)

SECTIONS
{
    . = 0x10000;

    .text : ALIGN(512)
    {
        *(.text)
    }

    .rodata : ALIGN(512)
    {
        *(.rodata)
        *(.rodata.*)
    }

    .data : ALIGN(512)
    {
        *(.data)
    }

    .bss : ALIGN(512)
    {
        *(COMMON)
        *(.bss)
    }
}