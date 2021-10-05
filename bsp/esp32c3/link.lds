#define SRAM_IRAM_START     0x4037C000
#define SRAM_DRAM_START     0x3FC7C000
#define ICACHE_SIZE         0x4000 /* ICache size is fixed to 16KB on ESP32-C3 */
#define I_D_SRAM_OFFSET     (SRAM_IRAM_START - SRAM_DRAM_START)
#define SRAM_DRAM_END       0x403D0000 - I_D_SRAM_OFFSET  /* 2nd stage bootloader iram_loader_seg start address */
#define SRAM_IRAM_ORG       (SRAM_IRAM_START + ICACHE_SIZE)
#define SRAM_DRAM_ORG       (SRAM_DRAM_START + ICACHE_SIZE)
#define I_D_SRAM_SIZE       SRAM_DRAM_END - SRAM_DRAM_ORG

/* Global symbols required for espressif hal build */
MEMORY
{
  iram0_0_seg(RX): org = SRAM_IRAM_ORG, len = I_D_SRAM_SIZE
  irom0_0_seg(RX): org = 0x42000020, len = 0x8000000-0x20

  drom0_0_seg (R) : org = 0x3C000020, len = 0x8000000-0x20
  dram0_0_seg(RW): org = SRAM_DRAM_ORG, len = I_D_SRAM_SIZE

  rtc_iram_seg(RWX): org = 0x50000000, len = 0x2000
}

/*  Default entry point:  */
ENTRY(__start)

_rom_store_table = 0;

PROVIDE ( esp_rom_uart_tx_one_char = uart_tx_one_char );
PROVIDE ( esp_rom_uart_rx_one_char = uart_rx_one_char );
PROVIDE ( esp_rom_uart_attach = uart_attach );
PROVIDE ( esp_rom_uart_tx_wait_idle = uart_tx_wait_idle );
PROVIDE ( esp_rom_intr_matrix_set = intr_matrix_set );
PROVIDE ( esp_rom_ets_set_user_start = ets_set_user_start );
PROVIDE ( esp_rom_ets_printf = ets_printf );
PROVIDE ( esp_rom_ets_delay_us = ets_delay_us );
PROVIDE ( esp_rom_cache_set_idrom_mmu_size = Cache_Set_IDROM_MMU_Size );

SECTIONS
{
  .rtc.text :
  {
    . = ALIGN(4);
    *(.rtc.literal .rtc.text)
    *rtc_wake_stub*.o(.literal .text .literal.* .text.*)
  } > rtc_iram_seg

  /* This section is required to skip rtc.text area because the text and
   * data segments reflect the same address space on different buses.
   */
  .rtc.dummy (NOLOAD):
  {
    . = SIZEOF(.rtc.text);
  } > rtc_iram_seg

  .rtc.data :
  {
    _rtc_data_start = ABSOLUTE(.);
    *(.rtc.data)
    *(.rtc.rodata)
    *rtc_wake_stub*.o(.data .rodata .data.* .rodata.* .bss .bss.*)
    _rtc_data_end = ABSOLUTE(.);
  } > rtc_iram_seg

  .rtc.bss (NOLOAD) :
  {
    _rtc_bss_start = ABSOLUTE(.);
    *rtc_wake_stub*.o(.bss .bss.*)
    *rtc_wake_stub*.o(COMMON)
    _rtc_bss_end = ABSOLUTE(.);
  } > rtc_iram_seg

  .iram0.text : ALIGN(4)
  {
    /* Vectors go to IRAM */
    _iram_start = ABSOLUTE(.);
    _iram_text_start = ABSOLUTE(.);
    _init_start = ABSOLUTE(.);
    KEEP(*(.exception_vectors.text));
    . = ALIGN(256);

    KEEP(*(.exception.entry*));
    *(.exception.other*)
    . = ALIGN(4);

    . = ALIGN (4);
    *(.entry.text)
    *(.init.literal)
    *(.init)
    . = ALIGN(4);
    *(.iram1 .iram1.*)
    *(.iram0.literal .iram.literal .iram.text.literal .iram0.text .iram.text)
    *libgcc.a:lib2funcs.*(.literal .text .literal.* .text.*)
    *libgcov.a:(.literal .text .literal.* .text.*)

    *libnet80211.a:( .wifi0iram  .wifi0iram.* .wifislpiram .wifislpiram.*)
    *libpp.a:( .wifi0iram  .wifi0iram.* .wifislpiram .wifislpiram.*)
    *libnet80211.a:( .wifirxiram  .wifirxiram.* .wifislprxiram .wifislprxiram.*)
    *libpp.a:( .wifirxiram  .wifirxiram.* .wifislprxiram .wifislprxiram.*)

    /* section information for finsh shell */
    . = ALIGN(8);
    __fsymtab_start = .;
    KEEP(*(FSymTab))
    __fsymtab_end = .;
    . = ALIGN(8);
    __vsymtab_start = .;
    KEEP(*(VSymTab))
    __vsymtab_end = .;
    . = ALIGN(8);

    /* section information for initial. */
    . = ALIGN(8);
    __rt_init_start = .;
    KEEP(*(SORT(.rti_fn*)))
    __rt_init_end = .;
    . = ALIGN(8);

    . = ALIGN(4);
    _init_end = ABSOLUTE(.);
    _iram_text_end = ABSOLUTE(.);   *(.srodata)
    _iram_end = ABSOLUTE(.);
  }  > iram0_0_seg 

  /**
   * This section is required to skip .iram0.text area because iram0_0_seg and
   * dram0_0_seg reflect the same address space on different buses.
   */
  .dram0.dummy (NOLOAD):
  {
    . = ORIGIN(dram0_0_seg) + _iram_end - _iram_start;
  } > dram0_0_seg

  .dram0.data :
  {
    _data_start = ABSOLUTE(.);
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    *(.data1)
#ifdef CONFIG_RISCV_GP
    __global_pointer$ = . + 0x800;
#endif /* CONFIG_RISCV_GP */
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    KEEP(*(.jcr))
    *(.dram1 .dram1.*)
    _data_end = ABSOLUTE(.);
    _end = .;
    . = ALIGN(4);
  } > dram0_0_seg

  /* Shared RAM */
  dram0.bss (NOLOAD) :
  {
    . = ALIGN (8);
    __bss_start = ABSOLUTE(.);
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    *(.bss)
    *(.bss.*)
    *(.share.mem)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    __bss_end = ABSOLUTE(.);
  } > dram0_0_seg

  .noinit (NOLOAD):
  {
    . = ALIGN (8);
    *(.noinit)
    *(".noinit.*")
    . = ALIGN (8);
  } > dram0_0_seg

  .flash.text : ALIGN(4)
  {
    _stext = .;
    _text_start = ABSOLUTE(.);

    *(.literal .text .literal.* .text.*)
    _text_end = ABSOLUTE(.);
    _etext = .;
    _flash_cache_start = ABSOLUTE(0);
  } > irom0_0_seg

  .flash_rodata_dummy (NOLOAD):
  {
    . = SIZEOF(.flash.text);
    . = ALIGN(0x10000) + 0x20;
    _rodata_reserved_start = .;
  } > drom0_0_seg

  .flash.rodata : ALIGN(0x10)
  {
    _rodata_start = ABSOLUTE(.);
    __rodata_region_start = .;
    __esp_shell_root_cmds_start = .;
    KEEP(*(SORT(.shell_root_cmd_*)));
    __esp_shell_root_cmds_end = .;

    *(.rodata_desc .rodata_desc.*)               /* Should be the first.  App version info.        DO NOT PUT ANYTHING BEFORE IT! */
    *(.rodata_custom_desc .rodata_custom_desc.*) /* Should be the second. Custom app version info. DO NOT PUT ANYTHING BEFORE IT! */
    *(.irom1.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    __XT_EXCEPTION_TABLE_ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table .gcc_except_table.*)
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    . = (. + 3) & ~ 3;
    __eh_frame = ABSOLUTE(.);
    KEEP(*(.eh_frame))
    . = (. + 7) & ~ 3;
    /*
     * C++ constructor and destructor tables
     * Don't include anything from crtbegin.o or crtend.o, as IDF doesn't use toolchain crt.
     *
     * RISC-V gcc is configured with --enable-initfini-array so it emits an .init_array section instead.
     * But the init_priority sections will be sorted for iteration in ascending order during startup.
     * The rest of the init_array sections is sorted for iteration in descending order during startup, however.
     * Hence a different section is generated for the init_priority functions which is iterated in
     * ascending order during startup. The corresponding code can be found in startup.c.
     */
    __init_priority_array_start = ABSOLUTE(.);
    KEEP (*(EXCLUDE_FILE (*crtend.* *crtbegin.*) .init_array.*))
    __init_priority_array_end = ABSOLUTE(.);
    __init_array_start = ABSOLUTE(.);
    KEEP (*(EXCLUDE_FILE (*crtend.* *crtbegin.*) .init_array))
    __init_array_end = ABSOLUTE(.);
    KEEP (*crtbegin.*(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.*) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    /* C++ exception handlers table: */
    __XT_EXCEPTION_DESCS_ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    __rodata_region_end = .;
    _rodata_end = ABSOLUTE(.);
    /* Literals are also RO data. */
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
    . = ALIGN(4);
    _thread_local_start = ABSOLUTE(.);
    *(.tdata)
    *(.tdata.*)
    *(.tbss)
    *(.tbss.*)
    *(.srodata)
    *(".srodata.*")
    *(.rodata)
    *(.rodata.*)
    *(.rodata_wlog)
    *(.rodata_wlog*)
    _thread_local_end = ABSOLUTE(.);
    _rodata_reserved_end = ABSOLUTE(.);
    . = ALIGN(4);
  } > drom0_0_seg
}