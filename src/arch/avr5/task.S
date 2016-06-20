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

#include <arch/avr5/types.h>
#include <task.h>

#define SREG 0x3F
#define SPH  0x3E
#define SPL  0x3D

.globl SwitchTask

.section .text

; Call-used registers  (r18-r27, r30-r31)
; Call-saved registers (r2-r17, r28-r29)

; Arguments - allocated left to right, r25 to r8. All
; arguments are aligned to start in even-numbered
; registers (odd-sized arguments, including char, have
; one free register above them). This allows making
; better use of the movw instruction on the enhanced
; core.

; See doc/avr5_registers.txt for detail

; Stack after call to SwitchTask
; ++++++++
; |  PCL |
; ++++++++
; |  PCH |
; ++++++++
; |      | <---- sp

; r25:r24 -- Source task struct (src)
; r23:r22 -- Destination task struct (dst)
SwitchTask:
  ; Preserve sreg
  in r0, SREG
  movw r30, r22 ; move dst to Z
  sbiw r30, 0x0
  brne 1f
  ; dst is NULL
  call panic
1:
  cp r24, r22
  cpc r25, r23
  brne 1f
  ; scr == dst, should not happen!
  call panic
1:
  sbiw r24, 0x0
  ; src is NULL
  ; Go to dst context restore
  breq 1f

  ; src is not NULL
  movw r30, r24 ; move src to Z

  ; Check preemption flag
  ldd r18, z+PREEMPT_FLAG_OFFSET
  andi r18, TASK_PREEMPT_CTX
  ; Skip context save if preemption flag is set
  brne 1f

  ; Save src context
  ; Save sregs
  std z+CONTEXT_SREG_OFFSET, r0
  ; Save PC in source context
  pop r0 ; PCH
  std z+CONTEXT_PCH_OFFSET, r0
  pop r0 ; PCL
  std z+CONTEXT_PCL_OFFSET, r0
  ; Save SP in source context
  in r0, SPL
  st z, r0
  in r0, SPH
  std z+1, r0
  ; Save call saved registers (r2-r17, r28-r29)
  std z+CONTEXT_REGS_OFFSET+2, r2
  std z+CONTEXT_REGS_OFFSET+3, r3
  std z+CONTEXT_REGS_OFFSET+4, r4
  std z+CONTEXT_REGS_OFFSET+5, r5
  std z+CONTEXT_REGS_OFFSET+6, r6
  std z+CONTEXT_REGS_OFFSET+7, r7
  std z+CONTEXT_REGS_OFFSET+8, r8
  std z+CONTEXT_REGS_OFFSET+9, r9
  std z+CONTEXT_REGS_OFFSET+10, r10
  std z+CONTEXT_REGS_OFFSET+11, r11
  std z+CONTEXT_REGS_OFFSET+12, r12
  std z+CONTEXT_REGS_OFFSET+13, r13
  std z+CONTEXT_REGS_OFFSET+14, r14
  std z+CONTEXT_REGS_OFFSET+15, r15
  std z+CONTEXT_REGS_OFFSET+16, r16
  std z+CONTEXT_REGS_OFFSET+17, r17
  std z+CONTEXT_REGS_OFFSET+28, r28
  std z+CONTEXT_REGS_OFFSET+29, r29
  ; Set the ctx_flag to 1, which means reduced context
  ldi r18, 0x1
  std z+CONTEXT_FLAG_OFFSET, r18
1:
  movw r30, r22 ; move dst to Z
  ; Restore call saved registers (r2-r17, r28-r29)
  ldd r2, z+CONTEXT_REGS_OFFSET+2
  ldd r3, z+CONTEXT_REGS_OFFSET+3
  ldd r4, z+CONTEXT_REGS_OFFSET+4
  ldd r5, z+CONTEXT_REGS_OFFSET+5
  ldd r6, z+CONTEXT_REGS_OFFSET+6
  ldd r7, z+CONTEXT_REGS_OFFSET+7
  ldd r8, z+CONTEXT_REGS_OFFSET+8
  ldd r9, z+CONTEXT_REGS_OFFSET+9
  ldd r10, z+CONTEXT_REGS_OFFSET+10
  ldd r11, z+CONTEXT_REGS_OFFSET+11
  ldd r12, z+CONTEXT_REGS_OFFSET+12
  ldd r13, z+CONTEXT_REGS_OFFSET+13
  ldd r14, z+CONTEXT_REGS_OFFSET+14
  ldd r15, z+CONTEXT_REGS_OFFSET+15
  ldd r16, z+CONTEXT_REGS_OFFSET+16
  ldd r17, z+CONTEXT_REGS_OFFSET+17
  ldd r28, z+CONTEXT_REGS_OFFSET+28
  ldd r29, z+CONTEXT_REGS_OFFSET+29
  ; Switch Stack
  ld r0, z
  out SPL, r0
  ldd r0, z+1
  out SPH, r0
  ; Patch stack with dst return address
  ldd r0, z+CONTEXT_PCL_OFFSET
  push r0
  ldd r0, z+CONTEXT_PCH_OFFSET
  push r0
  ; r1 needs to be 0 if context is reduced context
  clr r1

  ; Do we need to restore full context?
  ldd r0, z+CONTEXT_FLAG_OFFSET
  tst r0
  brne 1f
  ; Restore full context
  ; This is for tasks pre-empted by Category 2 IRQ
  ; Restore sreg, I bit should be cleared
  ldd r0, z+CONTEXT_SREG_OFFSET
  out SREG, r0
  ldd r0, z+CONTEXT_REGS_OFFSET
  ldd r1, z+CONTEXT_REGS_OFFSET+1
  ldd r18, z+CONTEXT_REGS_OFFSET+18
  ldd r19, z+CONTEXT_REGS_OFFSET+19
  ldd r20, z+CONTEXT_REGS_OFFSET+20
  ldd r21, z+CONTEXT_REGS_OFFSET+21
  ldd r22, z+CONTEXT_REGS_OFFSET+22
  ldd r23, z+CONTEXT_REGS_OFFSET+23
  ldd r24, z+CONTEXT_REGS_OFFSET+24
  ldd r25, z+CONTEXT_REGS_OFFSET+25
  ldd r26, z+CONTEXT_REGS_OFFSET+26
  ldd r27, z+CONTEXT_REGS_OFFSET+27
  push r26
  push r27
  ldd r26, z+CONTEXT_REGS_OFFSET+30
  ldd r27, z+CONTEXT_REGS_OFFSET+31
  movw r30, r26
  pop r27
  pop r26
  ; Should re-enable interrupt
  reti

1:
  ; Reduced context, restore sreg and return to system level
  ldd r0, z+CONTEXT_SREG_OFFSET
  out SREG, r0
  ret

/* vi: set et ai sw=2 sts=2: */
