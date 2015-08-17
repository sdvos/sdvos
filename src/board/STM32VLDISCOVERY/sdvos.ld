MEMORY
{
  ram (rwx) : ORIGIN = 0x20000000, LENGTH = 8K
  rom (rx)  : ORIGIN = 0x08000000, LENGTH = 128K
}
SECTIONS
{
  . = 0x08000000;
  .text :
  {
    *(.vectors)   /* Vector table */
    /* List of external vector sections */
    INCLUDE board/STM32VLDISCOVERY/ext_vec.ld
    *(.text)      /* Program code */
    *(.rodata)    /* Read only data */
  } >rom

  . = 0x20000000;
  virt_start_of_data = .;

  .data :
  {
    *(.data)
  } >ram AT > rom

  start_of_data = LOADADDR (.data);
  length_of_data = SIZEOF (.data);

  virt_start_of_bss = .;

  .bss :
  {
    *(.bss)
  } >ram AT > rom

  start_of_bss = LOADADDR (.bss);
  length_of_bss = SIZEOF (.bss);
}

