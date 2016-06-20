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

/**
 * @file   src/include/arch/armv7m/scs.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7-M System Control Space Registers
 */
#ifndef _ARMV7M_SCS_H_
#define _ARMV7M_SCS_H_

#include <arch/armv7m/types.h>

/* System Reset */

/** Vector Reset Bit */
#define NVIC_VECTRESET        0
/** System Reset Request */
#define NVIC_SYSRESETREQ      2
/** AIRCR Key for write access */
#define NVIC_AIRCR_VECTKEY    (0x5FA << 16)
/** Endianess */
#define NVIC_AIRCR_ENDIANESS  15

/* Bit definition for SCB_SHCSR register */
#define SCB_SHCSR_MEMFAULTACT       ((uint32_t)0x00000001)
#define SCB_SHCSR_BUSFAULTACT       ((uint32_t)0x00000002)
#define SCB_SHCSR_USGFAULTACT       ((uint32_t)0x00000008)
#define SCB_SHCSR_SVCALLACT         ((uint32_t)0x00000080)
#define SCB_SHCSR_MONITORACT        ((uint32_t)0x00000100)
#define SCB_SHCSR_PENDSVACT         ((uint32_t)0x00000400)
#define SCB_SHCSR_SYSTICKACT        ((uint32_t)0x00000800)
#define SCB_SHCSR_USGFAULTPENDED    ((uint32_t)0x00001000)
#define SCB_SHCSR_MEMFAULTPENDED    ((uint32_t)0x00002000)
#define SCB_SHCSR_BUSFAULTPENDED    ((uint32_t)0x00004000)
#define SCB_SHCSR_SVCALLPENDED      ((uint32_t)0x00008000)
#define SCB_SHCSR_MEMFAULTENA       ((uint32_t)0x00010000)
#define SCB_SHCSR_BUSFAULTENA       ((uint32_t)0x00020000)
#define SCB_SHCSR_USGFAULTENA       ((uint32_t)0x00040000)

/* Bit definition for SCB_ICSR register */
#define SCB_ICSR_VECTACTIVE         ((uint32_t)0x000001FF)
#define SCB_ICSR_RETTOBASE          ((uint32_t)0x00000800)
#define SCB_ICSR_VECTPENDING        ((uint32_t)0x003FF000)
#define SCB_ICSR_ISRPENDING         ((uint32_t)0x00400000)
#define SCB_ICSR_ISRPREEMPT         ((uint32_t)0x00800000)
#define SCB_ICSR_PENDSTCLR          ((uint32_t)0x02000000)
#define SCB_ICSR_PENDSTSET          ((uint32_t)0x04000000)
#define SCB_ICSR_PENDSVCLR          ((uint32_t)0x08000000)
#define SCB_ICSR_PENDSVSET          ((uint32_t)0x10000000)
#define SCB_ICSR_NMIPENDSET         ((uint32_t)0x80000000)

/* Bit definition for SCB_CCR register */
#define SCB_CCR_BP                  ((uint32_t)(1UL << 18))
#define SCB_CCR_IC                  ((uint32_t)(1UL << 17))
#define SCB_CCR_DC                  ((uint32_t)(1UL << 16))
#define SCB_CCR_STKALIGN            ((uint32_t)(1UL << 9))
#define SCB_CCR_BFHFNMIGN           ((uint32_t)(1UL << 8))
#define SCB_CCR_DIV_0_TRP           ((uint32_t)(1UL << 4))
#define SCB_CCR_UNALIGN_TRP         ((uint32_t)(1UL << 3))
#define SCB_CCR_USERSETMPEND        ((uint32_t)(1UL << 1))
#define SCB_CCR_NONBASETHRDENA      ((uint32_t)(1UL))

/* Bit definition for SysTick_CTRL register */
#define SysTick_CTRL_ENABLE         ((uint32_t)0x00000001)
#define SysTick_CTRL_TICKINT        ((uint32_t)0x00000002)
#define SysTick_CTRL_CLKSOURCE      ((uint32_t)0x00000004)
#define SysTick_CTRL_COUNTFLAG      ((uint32_t)0x00010000)

/* Bit definition for SysTick_LOAD register */
#define SysTick_LOAD_RELOAD         ((uint32_t)0x00FFFFFF)

/* Bit definition for SysTick_VAL register */
#define SysTick_VAL_CURRENT         ((uint32_t)0x00FFFFFF)

/* Bit definition for SysTick_CALIB register */
#define SysTick_CALIB_TENMS         ((uint32_t)0x00FFFFFF)
#define SysTick_CALIB_SKEW          ((uint32_t)0x40000000)
#define SysTick_CALIB_NOREF         ((uint32_t)0x80000000)

#ifdef __USE_FPU__
/* Floating-Point Context Control Register */
#define FPU_FPCCR_ASPEN             (1UL << 31)
#define FPU_FPCCR_LSPEN             (1UL << 30)
#define FPU_FPCCR_MONRDY            (1UL << 8)
#define FPU_FPCCR_BFRDY             (1UL << 6)
#define FPU_FPCCR_MMRDY             (1UL << 5)
#define FPU_FPCCR_HFRDY             (1UL << 4)
#define FPU_FPCCR_THREAD            (1UL << 3)
#define FPU_FPCCR_USER              (1UL << 1)
#define FPU_FPCCR_LSPACT            (1UL << 0)

/* Floating-Point Context Address Register */
#define FPU_FPCAR_ADDRESS           (0x1FFFFFFFUL << 3)

/* Floating-Point Default StaRegister */
#define FPU_FPDSCR_AHP              (1UL << 26)
#define FPU_FPDSCR_DN               (1UL << 25)
#define FPU_FPDSCR_FZ               (1UL << 24)
#define FPU_FPDSCR_RMode            (3UL << 22)

/* Media and FP Feature Register 0 */
#define FPU_MVFR0_FP_rounding_modes (0xFUL << 28)
#define FPU_MVFR0_Short_vectors     (0xFUL << 24)
#define FPU_MVFR0_Square_root       (0xFUL << 20)
#define FPU_MVFR0_Divide            (0xFUL << 16)
#define FPU_MVFR0_FP_excep_trapping (0xFUL << 12)
#define FPU_MVFR0_Double_precision  (0xFUL << 8)
#define FPU_MVFR0_Single_precision  (0xFUL << 4)
#define FPU_MVFR0_A_SIMD_registers  (0xFUL << 0)

/* Media and FP Feature Register 1 */
#define FPU_MVFR1_FP_fused_MAC      (0xFUL << 28)
#define FPU_MVFR1_FP_HPFP           (0xFUL << 24)
#define FPU_MVFR1_D_NaN_mode        (0xFUL << 4)
#define FPU_MVFR1_FtZ_mode          (0xFUL << 0)
#endif

#ifdef __USE_CACHE__
/* SCB Cache Size ID Register */
#define SCB_CCSIDR_WT               (1UL << 31U)
#define SCB_CCSIDR_WB               (1UL << 30U)
#define SCB_CCSIDR_RA               (1UL << 29U)
#define SCB_CCSIDR_WA               (1UL << 28U)
#define SCB_CCSIDR_NUMSETS          (0x7FFFUL << 13U)
#define SCB_CCSIDR_ASSOCIATIVITY    (0x3FFUL << 3U)
#define SCB_CCSIDR_LINESIZE         (7UL)
#endif

/**
 * Memory mapping struct for Nested Vectored Interrupt
 * Controller (NVIC)
 */
typedef struct
{
  /** Interrupt Set Enable Register */
  volatile uint32_t ISER[8];
  uint32_t RESERVED0[24];
  /** Interrupt Clear Enable Register */
  volatile uint32_t ICER[8];
  uint32_t RSERVED1[24];
  /** Interrupt Set Pending Register */
  volatile uint32_t ISPR[8];
  uint32_t RESERVED2[24];
  /** Interrupt Clear Pending Register */
  volatile uint32_t ICPR[8];
  uint32_t RESERVED3[24];
  /** Interrupt Active bit Register */
  volatile uint32_t IABR[8];
  uint32_t RESERVED4[56];
  /** Interrupt Priority Register, 8Bit wide */
  volatile uint8_t  IP[240];
  uint32_t RESERVED5[644];
  /** Software Trigger Interrupt Register */
  volatile uint32_t STIR;
} NVIC_Type;

/**
 * Memory mapping struct for System Control Block
 */
typedef struct
{
  /** CPU ID Base Register */
  volatile uint32_t CPUID;
  /** Interrupt Control State Register */
  volatile uint32_t ICSR;
  /** Vector Table Offset Register */
  volatile uint32_t VTOR;
  /** Application Interrupt / Reset Control Register */
  volatile uint32_t AIRCR;
  /** System Control Register */
  volatile uint32_t SCR;
  /** Configuration Control Register */
  volatile uint32_t CCR;
  /** System Handlers Priority Registers (4-7, 8-11, 12-15) */
  volatile uint8_t  SHP[12];
  /** System Handler Control and State Register */
  volatile uint32_t SHCSR;
  /** Configurable Fault Status Register */
  volatile uint32_t CFSR;
  /** Hard Fault Status Register */
  volatile uint32_t HFSR;
  /** Debug Fault Status Register */
  volatile uint32_t DFSR;
  /** Mem Manage Address Register */
  volatile uint32_t MMFAR;
  /** Bus Fault Address Register */
  volatile uint32_t BFAR;
  /** Auxiliary Fault Status Register */
  volatile uint32_t AFSR;
  /** Processor Feature Register */
  volatile uint32_t PFR[2];
  /** Debug Feature Register */
  volatile uint32_t DFR;
  /** Auxiliary Feature Register */
  volatile uint32_t ADR;
  /** Memory Model Feature Register */
  volatile uint32_t MMFR[4];
  /** ISA Feature Register */
  volatile uint32_t ISAR[5];
  uint32_t RESERVED0[1];
  /** Cache Level ID register */
  volatile uint32_t CLIDR;
  /** Cache Type register */
  volatile uint32_t CTR;
  /** Cache Size ID Register */
  volatile uint32_t CCSIDR;
  /** Cache Size Selection Register */
  volatile uint32_t CSSELR;
  /** Coprocessor Access Control Register */
  volatile uint32_t CPACR;
  uint32_t RESERVED3[93];
  /** Software Triggered Interrupt Register */
  volatile uint32_t STIR;
  uint32_t RESERVED4[15];
  /** Media and VFP Feature Register 0 */
  volatile uint32_t MVFR0;
  /** Media and VFP Feature Register 1 */
  volatile uint32_t MVFR1;
  /** Media and VFP Feature Register 1 */
  volatile uint32_t MVFR2;
  uint32_t RESERVED5[1];
  /** I-Cache Invalidate All to PoU */
  volatile uint32_t ICIALLU;
  uint32_t RESERVED6[1];
  /** I-Cache Invalidate by MVA to PoU */
  volatile uint32_t ICIMVAU;
  /** D-Cache Invalidate by MVA to PoC */
  volatile uint32_t DCIMVAC;
  /** D-Cache Invalidate by Set-way */
  volatile uint32_t DCISW;
  /** D-Cache Clean by MVA to PoU */
  volatile uint32_t DCCMVAU;
  /** D-Cache Clean by MVA to PoC */
  volatile uint32_t DCCMVAC;
  /** D-Cache Clean by Set-way */
  volatile uint32_t DCCSW;
  /** D-Cache Clean and Invalidate by MVA to PoC */
  volatile uint32_t DCCIMVAC;
  /** D-Cache Clean and Invalidate by Set-way */
  volatile uint32_t DCCISW;
  uint32_t RESERVED7[6];
  /** Instruction Tightly-Coupled Memory Control Register */
  volatile uint32_t ITCMCR;
  /** Data Tightly-Coupled Memory Control Registers */
  volatile uint32_t DTCMCR;
  /** AHBP Control Register */
  volatile uint32_t AHBPCR;
  /** L1 Cache Control Register */
  volatile uint32_t CACR;
  /** AHB Slave Control Register */
  volatile uint32_t AHBSCR;
  uint32_t RESERVED8[1U];
  /** Auxiliary Bus Fault Status Register */
  volatile uint32_t ABFSR;
} SCB_Type;

/**
 * Memory mapping struct for SysTick
 */
typedef struct
{
  /** SysTick Control and Status Register */
  volatile uint32_t CTRL;
  /** SysTick Reload Value Register */
  volatile uint32_t LOAD;
  /** SysTick Current Value Register */
  volatile uint32_t VAL;
  /** SysTick Calibration Register */
  volatile uint32_t CALIB;
} SysTick_Type;

/**
 * Memory mapped struct for Interrupt Type
 */
typedef struct
{
  uint32_t RESERVED0;
  /** Interrupt Control Type Register */
  volatile uint32_t ICTR;
#if ((defined __CM3_REV) && (__CM3_REV >= 0x200))
  /** Auxiliary Control Register */
  volatile uint32_t ACTLR;
#else
  uint32_t RESERVED1;
#endif
} InterruptType_Type;

#ifdef __USE_FPU__
/**
 * Structure type to access the Floating Point
 * Unit (FPU).
 */
typedef struct
{
  uint32_t RESERVED0[1];
  /** Floating-Point Context Control Register */
  volatile uint32_t FPCCR;
  /** Floating-Point Context Address Register */
  volatile uint32_t FPCAR;
  /** Floating-Point Default Status Control Register */
  volatile uint32_t FPDSCR;
  /** Media and FP Feature Register 0 */
  volatile uint32_t MVFR0;
  /** Media and FP Feature Register 1 */
  volatile uint32_t MVFR1;
} FPU_Type;
#endif

/* Memory mapping of Cortex-M3 Hardware */

/** System Control Space Base Address */
#define SCS_BASE      (0xE000E000)
/** SysTick Base Address */
#define SysTick_BASE  (SCS_BASE + 0x0010)
/** NVIC Base Address */
#define NVIC_BASE     (SCS_BASE + 0x0100)
/** System Control Block Base Address */
#define SCB_BASE      (SCS_BASE + 0x0D00)

/** Interrupt Type Register */
#define InterruptType  ((InterruptType_Type *) SCS_BASE)
/** SCB configuration struct */
#define SCB            ((SCB_Type *) SCB_BASE)
/** SysTick configuration struct */
#define SysTick        ((SysTick_Type *) SysTick_BASE)
/** NVIC configuration struct */
#define NVIC           ((NVIC_Type *) NVIC_BASE)

#ifdef __USE_FPU__
/** Floating Point Unit */
#define FPU_BASE       (SCS_BASE + 0x0F30UL)
/** Floating Point Unit */
#define FPU            ((FPU_Type *) FPU_BASE)
#endif

#endif

/* vi: set et ai sw=2 sts=2: */
