/**
 ****************************************************************************************
 *
 * @file ldscript_peripheral_examples.lds.S
 *
 * @brief GNU LD linker script file.
 *
 * Copyright (c) 2018-2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

MEMORY
{
    /* 48K SRAM */
    RAM_IROM1         (rwx) : ORIGIN = 0x07FC0000, LENGTH = 48K
}

ENTRY(Reset_Handler)

SECTIONS
{
    .text :
    {
        /* IV table */
        KEEP(*(.vectors))
        . = 0x0c0;

        /* patch section - not used by peripheral examples */
        KEEP(*(.patch_table))
        . = 0x0c0 + 0x050;

        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        *(.rodata*)

        KEEP(*(.eh_frame*))
    } > RAM_IROM1

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > RAM_IROM1

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > RAM_IROM1
    __exidx_end = .;

    __etext = .;

    /* The initialised data section is stored immediately
     * at the end of the text section
     */
    .data : AT (__etext)
    {
        __data_start__ = .;
        *(vtable)
        *(.data*)

        . = ALIGN(4);
        /* init_array/fini_array moved to flash, align preserved */

        KEEP(*(.jcr*))
        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;
    } > RAM_IROM1

    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM_IROM1

    .heap (COPY):
    {
        __end__ = .;
        PROVIDE(end = .);
        *(.heap*)
        __HeapLimit = .;
    } > RAM_IROM1

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later.
     */
    .stack_dummy (COPY):
    {
            *(.stack*)
    } > RAM_IROM1

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section.
     */
    __StackTop = ORIGIN(RAM_IROM1) + LENGTH(RAM_IROM1);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}
