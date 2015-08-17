/*
 *         Standard Dependable Vehicle Operating System
 *
 * Copyright (C) 2015 Ye Li (liye@sdvos.org)
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#define SPMEN     0x0
#define PGERS     0x1
#define PGWRT     0x2
#define RWWSRE    0x4
#define EEPE      0x1
#define EECR      0x1F
#define SPMCSR    0x37
#define SREG      0x3F

/* Page size 128 bytes. 256 pages in total */
#ifndef PGSIZE
#define PGSIZE       0x80
#endif

#ifdef WITH_FLASH_UTILITY

.global sdvos_pgm_read
.global sdvos_pgm_write

.section .text

; Call-used registers  (r18-r27, r30-r31)
; Call-saved registers (r2-r17, r28-r29)

; Arguments - allocated left to right, r25 to r8. All
; arguments are aligned to start in even-numbered
; registers (odd-sized arguments, including char, have
; one free register above them). This allows making
; better use of the movw instruction on the enhanced
; core.

; Return values: 8-bit in r24 (not r25!), 16-bit in
; r25:r24, up to 32 bits in r22-r25, up to 64 bits in
; r18-r25. 8-bit return values are zero/sign-extended
; to 16 bits by the called function (unsigned char is
; more efficient than signed char - just clr r25).
; Arguments to functions with variable argument lists
; (printf etc.) are all passed on stack, and char is
; extended to int.

; See doc/avr5_registers.txt for detail

; r25:r24 -- Flash address
sdvos_pgm_read:
  movw r30, r24
  lpm r24, Z
  clr r25
  ret

; r25:r24 -- Destination address
; r23:r22 -- Source address

; Write one page from SRAM to Flash
; Z is the Flash addr (destination)
; X is the SRAM addr (source) 

/* General register definition */
#define PARAM1    r18
#define TREG1     r19
#define COUNTL    r24
#define COUNTH    r25

sdvos_pgm_write:
  movw r26, r22     ; Move src to X
  movw r30, r24     ; Move dst to Z
  ldi PARAM1, ((1 << PGERS) | (1 << SPMEN))
  rcall do_spm      ; Erase page
  ldi PARAM1, ((1 << RWWSRE) | (1 << SPMEN))
  rcall do_spm      ; Re-enable RWW section
  ldi COUNTL, PGSIZE & 0xFF
  ldi COUNTH, (PGSIZE >> 8) & 0xFF
1:
  ld r0, X+
  ld r1, X+
  ldi PARAM1, 1 << SPMEN
  rcall do_spm
  adiw r30, 2       ; Add 2 to Z
  sbiw COUNTL, 2    ; Subtract 2 from Counter
  brne 1b

  subi r30, PGSIZE & 0xFF
  sbci r31, (PGSIZE >> 8) & 0xFF ; Restore Z
  ldi PARAM1, ((1 << PGWRT) | (1 << SPMEN))
  rcall do_spm      ; Write temp page to flash
  ldi PARAM1, ((1 << RWWSRE) | (1 << SPMEN))
  rcall do_spm      ; Re-enable RWW section
1:
  in TREG1, SPMCSR  ; Load SPMCSR
  sbrc TREG1, SPMEN ; Previous SPM finished?
                    ; Test bit 0 of SPMCSR (SPMEN)
  rjmp 1b

  clr r1
  ret

do_spm:             ; Perform SPM with SPMCSR value passed
                    ; in PARAM1.
1:
  in TREG1, SPMCSR  ; Load SPMCSR
  sbrc TREG1, SPMEN ; Previous SPM finished?
                    ; Test bit 0 of SPMCSR (SPMEN)
  rjmp 1b

  in TREG1, SREG
  cli               ; Disable interrupt

wait_eeprom:        ; EEPROM write prevents SPMCSR write
                    ; Refer to section 27.8.8 in spec
  sbic EECR, EEPE
  rjmp wait_eeprom
  out SPMCSR, PARAM1
  spm

1:
  in TREG1, SPMCSR  ; Load SPMCSR
  sbrc TREG1, SPMEN ; SPM finished?
                    ; Test bit 0 of SPMCSR (SPMEN)
  rjmp 1b

  out SREG, TREG1   ; Enable interrupt
  ret

#endif
