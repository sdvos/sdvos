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
 * @file   src/arch/armv7m/interrupt.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7m Interrupt Management
 */
#include <arch/armv7m/interrupt.h>
#include <arch/armv7m/types.h>
#include <arch/armv7m/scs.h>
#include <config/config.h>
#include <sdvos.h>

/**
 * @brief SVCall Handler
 *
 * SVC handler that handles system calls.
 * See arch/armv7m/svc.S for implementation.
 */
extern void SvcHandler (void);

/**
 * @brief SYSTICK Exception Handler
 *
 * SYSTICK exception handler is responsible for OS tick
 * management.
 */
extern void SystickHandler (void);

/**
 * @brief PendSV Handler
 *
 * PendSV is not used in SDVOS. IRQ2 and SYSTICK are all of
 * the lowest priority. These interrupts can be preempted by
 * ISR1. However, ISR1 will never trigger context switch.
 * SVC is of highest priority (except faults) and it is
 * synchronous. As a result, SVC will not happen if there
 * are pending interrupts. Tail-chaining and late arrivals
 * are also not a problem here. Hence we do not really need
 * PendSV to handle context switche. It can be savely done
 * in SYSTICK handler, at the end of ISR2 and in SVC
 * handler.
 */
static void
PendSVHandler (void)
{
  return;
}

/**
 * @brief Default NMI Handler
 *
 * Default NMI handler in initial vector table.
 * This handler is required.
 */
static void
NmiHandler (void)
{
  DEBUG_PRINTF ("NMI\n");
  panic ();
}

/**
 * @brief Default Hard Fault Handler
 *
 * Default Hard Fault handler in initial vector table.
 * This handler is required.
 */
static void
HardFaultHandler (void)
{
  DEBUG_PRINTF ("Hard Fault\n");
  panic ();
}

#ifdef DEBUG_SDVOS_VERBOSE
/**
 * @brief Default Mem Fault Handler
 *
 * Default Mem Fault handler in initial vector table.
 * This handler is optional.
 */
static void
MemFaultHandler (void)
{
  DEBUG_PRINTF ("Mem Fault\n");
  panic ();
}

/**
 * @brief Default Bus Fault Handler
 *
 * Default Bus Fault handler in initial vector table.
 * This handler is optional.
 */
static void
BusFaultHandler (void)
{
  DEBUG_PRINTF ("Bus Fault\n");
  DEBUG_PRINTF ("BFSR=0x%X\n", (SCB->CFSR >> 8) & 0xFF);
  panic ();
}

/**
 * @brief Default Usage Fault Handler
 *
 * Default Usage Fault handler in initial vector table.
 * This handler is optional.
 */
static void
UsageFaultHandler (void)
{
  DEBUG_PRINTF ("Usage Fault\n");
  DEBUG_PRINTF ("UFSR=0x%X\n", (SCB->CFSR >> 16) & 0x3FF);
  panic ();
}
#endif

/**
 * @brief Initial Vector Table in Flash
 *
 * ARM Cortex M3 has 16 vector table entries (15 exceptions
 * and 1 MSP initial value). These are consistent across
 * all SoCs.
 */
code_addr_t * sys_vectors[16]
__attribute__ ((section(".vectors")))= {
  (code_addr_t *) KERN_STACK,        /* #0  MSP Inital Value */
  (code_addr_t *) _start,            /* #1  Reset */
  (code_addr_t *) NmiHandler,        /* #2  NMI */
  (code_addr_t *) HardFaultHandler,  /* #3  Hard Fault */
#ifdef DEBUG_SDVOS_VERBOSE
  (code_addr_t *) MemFaultHandler,   /* #4  MemManage Fault */
  (code_addr_t *) BusFaultHandler,   /* #5  Bus Fault */
  (code_addr_t *) UsageFaultHandler, /* #6  Usage Fault */
#else
  (code_addr_t *) 0,                 /* #4  MemManage Fault */
  (code_addr_t *) 0,                 /* #5  Bus Fault */
  (code_addr_t *) 0,                 /* #6  Usage Fault */
#endif
  (code_addr_t *) 0,                 /* #7  Reserved */
  (code_addr_t *) 0,                 /* #8  Reserved */
  (code_addr_t *) 0,                 /* #9  Reserved */
  (code_addr_t *) 0,                 /* #10 Reserved */
  (code_addr_t *) SvcHandler,        /* #11 SVC */
  (code_addr_t *) 0,                 /* #12 Debug Monitor */
  (code_addr_t *) 0,                 /* #13 Reserved */
  (code_addr_t *) PendSVHandler,     /* #14 PendSV */
  (code_addr_t *) SystickHandler     /* #15 SYSTICK */
};

void
NVIC_EnableIRQ (IRQType n)
{
  NVIC->ISER[((uint32_t)(n) >> 5)] =
    (1 << ((uint32_t)(n) & 0x1F)); 
}

void
NVIC_DisableIRQ (IRQType n)
{
  NVIC->ICER[((uint32_t)(n) >> 5)] =
    (1 << ((uint32_t)(n) & 0x1F));
}

void
NVIC_SetPriority (IRQType n, IRQPrioType prio)
{
  NVIC->IP[(uint32_t)(n)] = (prio & 0xE0);
}

void
InterruptInit ()
{
  uint32_t reg_value = 0;
  uint32_t i = 0;

  /*
   * In EcuInit, we already have interrupts disabled. This
   * is done through PRIMASK register. We will initialize
   * everything and clear PRIMASK before going into first
   * user task.
   */

  /* Set up priority group */
  reg_value = SCB->AIRCR;
  reg_value &= ~((0xFFFFU << 16) | (0x7 << 8));
  /* Set priority group to 0 */
  reg_value  = ((reg_value | NVIC_AIRCR_VECTKEY |
                (0 /* PRIGROUP */ << 8)));
  SCB->AIRCR = reg_value;

  /* Set up individual interrupt priority */
  /* Reset System Handler Priority */
  for (i = 0; i < 12; i++) SCB->SHP[i] = 0;

  /* Set SVC, PendSV and SYSTICK Priority */
  SCB->SHP[11 - 4] = SVC_PRI;    /* SVC Priority */
  SCB->SHP[14 - 4] = PSV_PRI;    /* PendSV Priority */
  SCB->SHP[15 - 4] = SYSTK_PRI;  /* SYSTICK Priority */

  /* Set up external interrupt priorities */
  for (i = 0; i < NUM_ISR1; i++)
    NVIC_SetPriority (isr1_list[i], ISR1_PRI);
  for (i = 0; i < NUM_ISR2; i++)
    NVIC_SetPriority (isr2_list[i], ISR2_PRI);

  /* Clear enable for all external interrupts */
  for (i = 0; i < 8; i++) NVIC->ICER[i] = (uint32_t) (~0x0U);
  /* Clear pending for all external interrupts */
  for (i = 0; i < 8; i++) NVIC->ICPR[i] = (uint32_t) (~0x0U);

  /* Disable Systick Timer */
  reg_value = SysTick->CTRL;
  reg_value &= 0xFFFFFFF8;
  SysTick->CTRL = reg_value;

  /* Clear any possible pending of Systick and PendSV */
  reg_value = SCB->ICSR;
  reg_value &= 0xFF000000;
  SCB->ICSR = reg_value | SCB_ICSR_PENDSTCLR | SCB_ICSR_PENDSVCLR;

  /* Check system handler state */
  reg_value = SCB->SHCSR;
  if ((reg_value & SCB_SHCSR_MEMFAULTACT) |
      (reg_value & SCB_SHCSR_BUSFAULTACT) |
      (reg_value & SCB_SHCSR_USGFAULTACT) |
      (reg_value & SCB_SHCSR_SVCALLACT) |
      (reg_value & SCB_SHCSR_MONITORACT) |
      (reg_value & SCB_SHCSR_PENDSVACT) |
      (reg_value & SCB_SHCSR_SYSTICKACT) |
      (reg_value & SCB_SHCSR_USGFAULTPENDED) |
      (reg_value & SCB_SHCSR_MEMFAULTPENDED) |
      (reg_value & SCB_SHCSR_BUSFAULTPENDED) |
      (reg_value & SCB_SHCSR_SVCALLPENDED)) {
    panic ();
  }

  /*
   * Set STKALIGN and UNALIGN_TRP to 1.
   * For compatibility between CM3, CM4 and efficiency of memory
   * access.
   */
  SCB->CCR = SCB->CCR | SCB_CCR_STKALIGN | SCB_CCR_UNALIGN_TRP;

  /* About to enter "formal" kernel context */
  /* Clear PRIMASK. It is set by EcuInit. */
  __enable_irq ();

  /*
   * With all disabling and clearing above, we should not
   * have any interrupt before SVC. For SVC 0, we have to
   * restore the stack frame since the system stays in
   * handler mode. No exception return is performed.
   */
  __asm__ volatile ("mrs r0, msp\n\t"
                    "svc #0\n\t"
                    "msr msp, r0\n\t":::"r0");

  /*
   * Now, we should be in handler mode. Since SVC has
   * highest prio. We are safe. Set CONTROL[0] to 1. Thread
   * is unprivileged.
   */
  __asm__ volatile ("mrs r0, control\n\t"
                    "orrs r0, r0, #1\n\t"
                    "msr control, r0\n\t"
                    "isb\n\t":::"r0");

  /* Enable interrupts */
#ifdef DEBUG_SDVOS_VERBOSE
  /* Enalbe extra system fault handler in debug mode */
  reg_value = SCB->SHCSR | SCB_SHCSR_MEMFAULTENA |
              SCB_SHCSR_BUSFAULTENA | SCB_SHCSR_USGFAULTENA;
  SCB->SHCSR = reg_value;
#endif

  /* Enable all external interrupts in use */
  for (i = 0; i < NUM_ISR1; i++)
    NVIC_EnableIRQ (isr1_list[i]);
  for (i = 0; i < NUM_ISR2; i++)
    NVIC_EnableIRQ (isr2_list[i]);

  /*
   * Initialize system sleep mode 
   * No SEVONPEND, no SLEEPDEEP, no SLEEPONEXIT
   *
   * WFI Behavior                 Wake Up     IRQ Execution
   * ------------------------------------------------------
   * IRQ with BASEPRI
   * IRQ priority >  BASEPRI           Y                  Y
   * IRQ priority =< BASEPRI           N                  N
   * IRQ with BASEPRI and PRIMASK
   * IRQ priority > BASEPRI            Y                  N
   * IRQ priority =< BASEPRI           N                  N
   */
  SCB->SCR = 0x0;

  return;
}

/* vi: set et ai sw=2 sts=2: */
