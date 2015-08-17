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
 * @file   src/include/arch/armv7m/interrupt.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7-M Interrupt Management
 */
#ifndef _ARMV7M_INTERRUPT_H_
#define _ARMV7M_INTERRUPT_H_

#include <arch/armv7m/types.h>
#include <arch/armv7m/scs.h>
#include <arch/armv7m/utils.h>
#include <arch/armv7m/atomic.h>
#include <config/config.h>
#include <cc.h>

/*
 * A minimum of 3 bits will be implemented by all Cortex M3
 * microcontrollers. To be portable, we assume only 3 bit
 * are implemented and sub-priority is not used.
 *
 * In order to enable SVCall at all times except fault
 * handling, we set the priority of SVCall to highest (0).
 * All Category 1 ISRs have a higher priority then Category
 * 2 ISRs. Category 2 ISRs have the same priority as system
 * tick handler since systick handler and ISR2 handler can
 * both access kernel data structures. PendSV also has the
 * same priority because context switch cannot happen
 * with interrupts nested and we do not want ISR2 to occur
 * in the process since system services can be called.
 */
/** Priority of SVCall (highest) */
#define SVC_PRI     0x00
/** Priority of Category 1 ISR */
#define ISR1_PRI    0x20
/** Priority of Category 2 ISR */
#define ISR2_PRI    0xE0
/** Priority of SYSTICK */
#define SYSTK_PRI   0xE0
/** Priority of PendSV (lowest) */
#define PSV_PRI     0xE0

/** Exception return value for handler mode */
#define EXC_RETURN_HANDLER    0xFFFFFFF1U
/** Exception return value for user mode with MSP */
#define EXC_RETURN_MSP        0xFFFFFFF9U
/** Exception return value for user mode with PSP */
#define EXC_RETURN_PSP        0xFFFFFFFDU

#ifdef __USE_FPU__
/** Exception return value for handler mode with FP frame */
#define EXC_RETURN_HANDLER_FP 0xFFFFFFE1U
/** Exception return value for user mode with MSP with FP frame */
#define EXC_RETURN_MSP_FP     0xFFFFFFE9U
/** Exception return value for user mode with PSP with FP frame */
#define EXC_RETURN_PSP_FP     0xFFFFFFEDU
#endif

/**
 * @def __set_pendsv
 * @brief Pend PendSV
 */
#define __set_pendsv() do {                         \
  SCB->ICSR |= SCB_ICSR_PENDSVSET;                  \
while (0)

/**
 * @def __pendsv_pended
 * @brief Tests whether PendSV is pended
 *
 * Evaluates to 1 if PendSV is pended. 0 otherwise.
 */
#define __pendsv_pended()                           \
  ((SCB->ICSR & SCB_ICSR_PENDSVSET) >> 28)

/**
 * @def IRQSaveContext()
 * @brief Save processor context for Category 2 ISR.
 *
 * System services can be called in Category 2 ISR
 * and at the end of Category 2 ISR, rescheduling
 * might occur. We need to pessimistically save the
 * whole processor context. This macro will be placed
 * at the beginning of each category 2 ISR automatically.
 */
#define IRQSaveContext()                          \
  __asm__ volatile ("push {lr}\n\t"               \
                    "ldr r3, =cur_task\n\t"       \
                    "ldr r3, [r3]\n\t"            \
                    "cmp r3, #0\n\t"              \
                    "beq 1f\n\t"                  \
                    "adds r3, r3, #8\n\t"         \
                    "stmia r3!, {r4-r11, lr}\n\t" \
                    "1:\n\t"                      \
                    :                             \
                    :                             \
                    :/* No need for clobbers */)

/**
 * @def IRQRestoreContext()
 * @brief Restore processor context for Category 2 ISR.
 *
 * Counterpart to IRQSaveContext macro. It will be placed
 * at the bottom of each category 2 ISR automatically. It
 * is only used if preemption did not occur.
 *
 * No need to restore r4-r11 from TCB since we are using
 * a single kernel stack. If we reached IRQRestoreContext
 * in ISR2, it means no preemption occured. And all the
 * callee saved registers are safe.
 */
#define IRQRestoreContext()                       \
  __asm__ volatile ("pop {pc}\n\t"                \
                    :                             \
                    :                             \
                    :/* No need for clobbers */)

/**
 * @def ISR_CAT1
 * @brief Definition macro for Category 1 ISR
 *
 * Macro used for the definition of Category 1 ISR. System
 * services allowed in Category 1 ISR are:
 *
 * EnableAllInterrupts, DisableAllInterrupts
 * ResumeAllInterrupts, SuspendAllInterrupts
 * ResumeOSInterrupts, SuspendOSInterrupts
 *
 * @param[in] vector
 *   Interrupt vector
 */
#define ISR_CAT1(vector)                             \
  static void vector##_impl (void);                  \
  static void vector##_user_impl (void);             \
  code_addr_t * vector##_var                         \
  __attribute__ ((section("\"." #vector "\""))) =    \
  (code_addr_t *) vector##_impl;                     \
  static void vector##_impl (void) {                 \
    uatomic_inc (&NestedISRs);                       \
    vector##_user_impl ();                           \
    uatomic_dec (&NestedISRs);                       \
    return;                                          \
  }                                                  \
  static void vector##_user_impl (void)

/**
 * @def ISR
 * @brief Definition macro for Category 2 ISR
 *
 * Macro used for the definition of Category 2 ISR. System
 * services allowed in Category 2 ISR are:
 *
 * ActivateTask, GetTaskID, GetTaskState, GetResource
 * ReleaseResource, SetEvent, GetEvent, GetAlarmBase
 * GetAlarm, SetRelAlarm, SetAbsAlarm, CancelAlarm
 * GetActiveApplicationMode, ShutdownOS
 *
 * @param[in] vector
 *   Interrupt vector
 */
/* vector has to be ext_vec_[n] */
#define ISR(vector)                                  \
  static void vector##_impl (void) NAKED;            \
  static void vector##_user_impl (void);             \
  code_addr_t * vector##_var                         \
  __attribute__ ((section("\"." #vector "\""))) =    \
  (code_addr_t *) vector##_impl;                     \
  static void NOINLINE _##vector##_impl (void) {     \
    uatomic_inc (&NestedISRs);                       \
    vector##_user_impl ();                           \
    uatomic_dec (&NestedISRs);                       \
    CheckPreemption (PREEMPT_ISR);                   \
  }                                                  \
  static void vector##_impl (void) {                 \
    IRQSaveContext ();                               \
    _##vector##_impl ();                             \
    IRQRestoreContext ();                            \
    return;                                          \
  }                                                  \
  static void vector##_user_impl (void)

/**
 * @def __disable_irq
 * @brief Disable all interrupt using PRIMASK
 *
 * This macro is not used by system services. It is used
 * only internally during system initialization.
 */
#define __disable_irq()                              \
  __asm__ volatile ("cpsid i\n\t")

/**
 * @def __enable_irq
 * @brief Enable all interrupt using PRIMASK
 *
 * This macro is not used by system services. It is used
 * only internally during system initialization.
 */
#define __enable_irq()                               \
  __asm__ volatile ("cpsie i\n\t")

/**
 * @def __set_basepri
 * @brief Change the value of BASEPRI register
 *
 * @param[in] p
 *   8-bit unsigned integer to set BASEPRI to
 */
#define __set_basepri(p)                            \
  __asm__ volatile ("movs r0, %0\n\t"               \
                    "msr basepri, r0\n\t"           \
                    :                               \
                    :"I" (p)                        \
                    :"r0")

/**
 * @def __get_basepri
 * @brief Get current value of BASEPRI register
 *
 * @param[out] p
 *   8-bit unsigned integer to store the BASEPRI value
 */
#define __get_basepri(p)                            \
  __asm__ volatile ("mrs %0, basepri\n\t":"=l" (p)::)

/**
 * @def __soft_int
 * @brief Trigger a software interrupt
 *
 * @param[in] n
 *   External interrupt vector number
 */
#define __soft_int(n)  (NVIC->STIR = (n))

/**
 * @brief Enable Interrupt in NVIC Interrupt Controller
 *
 * Enable a device specific interupt in the NVIC interrupt
 * controller. The interrupt number cannot be a negative
 * value.
 *
 * @param[in] n
 *   Specifies the external interrupt number starting from
 *   0 to 239 max.
 */
extern void NVIC_EnableIRQ (IRQType n);

/**
 * @brief Disable Interrupt in NVIC Interrupt Controller
 * 
 * Disable a device specific interupt in the NVIC interrupt
 * controller. The interrupt number cannot be a negative
 * value.
 *
 * @param[in] n
 *   Specifies the external interrupt number starting from
 *   0 to 239 max.
 */
extern void NVIC_DisableIRQ (IRQType n);

/**
 * @brief  Set priority for an external interrupt
 *
 * We assume only 3 bits are implemented. This is the
 * minimum required by ARM specification.
 *
 * @param[in] n
 *   Specifies the external interrupt number starting from
 *   0 to 239 max.
 * @param[in] piro
 *   Priority for the interrupt.
 */
extern void NVIC_SetPriority (IRQType n, IRQPrioType prio);

/**
 * @def ArchEnableAllInterrupts
 * @brief Internal macro used by EnableAllInterrupts().
 *
 * To enable all interrupts, we set BASEPRI to 0, which
 * cancels masking.
 */
#define ArchEnableAllInterrupts()  __set_basepri(0)

/**
 * @def ArchDisableAllInterrupts
 * @brief Internal macro used by DisableAllInterrupts().
 *
 * To disable "all" interrupts, we set BASEPRI to the
 * priority level of Category 1 ISR. This masks all
 * interrupts except SVCall and Faults.
 */
#define ArchDisableAllInterrupts()  __set_basepri(ISR1_PRI)

/**
 * @def ArchResumeAllInterrupts
 * @brief Internal macro used by ResumeAllInterrupts().
 */
#define ArchResumeAllInterrupts  ArchEnableAllInterrupts

/**
 * @def ArchSuspendAllInterrupts
 * @brief Internal macro used by SuspendAllInterrupts().
 */
#define ArchSuspendAllInterrupts  ArchDisableAllInterrupts

/**
 * @def ArchResumeOSInterrupts
 * @brief Internal macro used by ResumeOSInterrupts().
 */
#define ArchResumeOSInterrupts  ArchEnableAllInterrupts

/**
 * @def ArchSuspendOSInterrupts
 * @brief Internal macro used by SuspendOSInterrupts().
 */
#define ArchSuspendOSInterrupts() __set_basepri(ISR2_PRI)

#endif

/* vi: set et ai sw=2 sts=2: */
