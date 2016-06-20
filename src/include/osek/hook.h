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
 * @file   src/include/osek/hook.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  OSEK Hook Routines
 */
#ifndef _OSEK_HOOK_H_
#define _OSEK_HOOK_H_

#include <osek/types.h>
#include <config/config.h>
#include <sdvos.h>

#if defined(USEGETSERVICEID) && (USEGETSERVICEID == 0x1)
/**
 * @brief OS Service Info Structure
 *
 * OS service info structure that records the service ID
 * and parameters of the last OS service called.
 */
struct OSErrorService_t {
  OSServiceIdType service;  /**< OS service ID */
#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
  ParamType param1;         /**< OS service parameter 1 */
  ParamType param2;         /**< OS service parameter 2 */
  ParamType param3;         /**< OS service parameter 3 */
#endif
};

/** Global OS service info structure for debug */
extern struct OSErrorService_t OSErrorService;

/**
 * @def OSErrorGetServiceId
 * @brief
 * OSErrorGetServiceId provides the service id where the
 * error has been risen.
 *
 * The service id is of type OSServiceIdType. Possible
 * values are OSServiceId_xx, where xx is the name of the
 * system service.
 */
#define OSErrorGetServiceId() OSErrorService.service

/**
 * @def SaveOSErrorService
 * @brief Populate OS service info structure
 *
 * SaveOSErrorService populates the global OS service info
 * structure with the parameters given.
 *
 * @param[in] i
 *   Service ID
 * @param[in] p1
 *   Parameter 1
 * @param[in] p2
 *   Parameter 2
 * @param[in] p3
 *   Parameter 3
 */
#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
#define SaveOSErrorService(i, p1, p2, p3)  do {    \
  OSErrorService.service = (OSServiceIdType) i;    \
  OSErrorService.param1 = (ParamType) p1;          \
  OSErrorService.param2 = (ParamType) p2;          \
  OSErrorService.param3 = (ParamType) p3;          \
} while (0)
#else
#define SaveOSErrorService(i, p1, p2, p3)  do {    \
  OSErrorService.service = (OSServiceIdType) i;    \
} while (0)
#endif

#else

#define OSErrorGetServiceId() ;
#define SaveOSErrorService(i, p1, p2, p3) ;

#endif

/*
 * Macros of IDs for OS services.
 */
/** Service ID of ActivateTask */
#define OSServiceId_ActivateTask                0x00u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service ActivateTask */
#define OSError_ActivateTask_TaskID()    \
  ((TaskType) OSErrorService.param1)
#endif

/** Service ID of TerminateTask */
#define OSServiceId_TerminateTask               0x01u

/** Service ID of ChainTask */
#define OSServiceId_ChainTask                   0x02u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service ChainTask */
#define OSError_ChainTask_TaskID()    \
  ((TaskType) OSErrorService.param1)
#endif

/** Service ID of Schedule */
#define OSServiceId_Schedule                    0x03u

/** Service ID of GetTaskID */
#define OSServiceId_GetTaskID                   0x04u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service GetTaskID */
#define OSError_GetTaskID_TaskID()    \
  ((TaskRefType) OSErrorService.param1)
#endif

/** Service ID of GetTaskState */
#define OSServiceId_GetTaskState                0x05u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service GetTaskState */
#define OSError_GetTaskState_TaskID()    \
  ((TaskType) OSErrorService.param1)
/** Macro to access parameter of service GetTaskState */
#define OSError_GetTaskState_State()    \
  ((TaskStateRefType) OSErrorService.param2)
#endif

/** Service ID of GetResource */
#define OSServiceId_GetResource                 0x06u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service GetResource */
#define OSError_GetResource_ResID()    \
  ((ResourceType) OSErrorService.param1)
#endif

/** Service ID of ReleaseResource */
#define OSServiceId_ReleaseResource             0x07u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service ReleaseResource */
#define OSError_ReleaseResource_ResID()    \
  ((ResourceType) OSErrorService.param1)
#endif

/** Service ID of SetEvent */
#define OSServiceId_SetEvent                    0x08u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service SetEvent */
#define OSError_SetEvent_TaskID()    \
  ((TaskType) OSErrorService.param1)
/** Macro to access parameter of service SetEvent */
#define OSError_SetEvent_Mask()    \
  ((EventMaskType) OSErrorService.param2)
#endif

/** Service ID of ClearEvent */
#define OSServiceId_ClearEvent                  0x09u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service ClearEvent */
#define OSError_ClearEvent_Mask()    \
  ((EventMaskType) OSErrorService.param1)
#endif

/** Service ID of GetEvent */
#define OSServiceId_GetEvent                    0x10u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service GetEvent */
#define OSError_GetEvent_TaskID()    \
  ((TaskType) OSErrorService.param1)
/** Macro to access parameter of service GetEvent */
#define OSError_GetEvent_Event()    \
  ((EventMaskRefType) OSErrorService.param2)
#endif

/** Service ID of WaitEvent */
#define OSServiceId_WaitEvent                   0x11u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service WaitEvent */
#define OSError_WaitEvent_Mask()    \
  ((EventMaskType) OSErrorService.param1)
#endif

/** Service ID of GetAlarmBase */
#define OSServiceId_GetAlarmBase                0x12u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service GetAlarmBase */
#define OSError_GetAlarmBase_AlarmID()    \
  ((AlarmType) OSErrorService.param1)
/** Macro to access parameter of service GetAlarmBase */
#define OSError_GetAlarmBase_Info()    \
  ((AlarmBaseRefType) OSErrorService.param2)
#endif

/** Service ID of GetAlarm */
#define OSServiceId_GetAlarm                    0x13u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service GetAlarm */
#define OSError_GetAlarm_AlarmID()    \
  ((AlarmType) OSErrorService.param1)
/** Macro to access parameter of service GetAlarm */
#define OSError_GetAlarm_Tick()    \
  ((TickRefType) OSErrorService.param2)
#endif

/** Service ID of SetRelAlarm */
#define OSServiceId_SetRelAlarm                 0x14u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service SetRelAlarm */
#define OSError_SetRelAlarm_AlarmID()    \
  ((AlarmType) OSErrorService.param1)
/** Macro to access parameter of service SetRelAlarm */
#define OSError_SetRelAlarm_increment()    \
  ((TickType) OSErrorService.param2)
/** Macro to access parameter of service SetRelAlarm */
#define OSError_SetRelAlarm_cycle()    \
  ((TickType) OSErrorService.param3)
#endif

/** Service ID of SetAbsAlarm */
#define OSServiceId_SetAbsAlarm                 0x15u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service SetAbsAlarm */
#define OSError_SetAbsAlarm_AlarmID()    \
  ((AlarmType) OSErrorService.param1)
/** Macro to access parameter of service SetAbsAlarm */
#define OSError_SetAbsAlarm_start()    \
  ((TickType) OSErrorService.param2)
/** Macro to access parameter of service SetAbsAlarm */
#define OSError_SetAbsAlarm_cycle()    \
  ((TickType) OSErrorService.param3)
#endif

/** Service ID of CancelAlarm */
#define OSServiceId_CancelAlarm                 0x16u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service CancelAlarm */
#define OSError_CancelAlarm_AlarmID()    \
  ((AlarmType) OSErrorService.param1)
#endif

/** Service ID of GetActiveApplicationMode */
#define OSServiceId_GetActiveApplicationMode    0x17u

/** Service ID of StartScheduleTableRel */
#define OSServiceId_StartScheduleTableRel       0x18u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service StartScheduleTableRel */
#define OSError_StartScheduleTableRel_ScheduleTableID()    \
  ((ScheduleTableType) OSErrorService.param1)
/** Macro to access parameter of service StartScheduleTableRel */
#define OSError_StartScheduleTableRel_Offset()    \
  ((TickType) OSErrorService.param2)
#endif

/** Service ID of StartScheduleTableAbs */
#define OSServiceId_StartScheduleTableAbs       0x19u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service StartScheduleTableAbs */
#define OSError_StartScheduleTableAbs_ScheduleTableID()    \
  ((ScheduleTableType) OSErrorService.param1)
/** Macro to access parameter of service StartScheduleTableAbs */
#define OSError_StartScheduleTableAbs_Start()    \
  ((TickType) OSErrorService.param2)
#endif

/** Service ID of StopScheduleTable */
#define OSServiceId_StopScheduleTable           0x1Au

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service StopScheduleTable */
#define OSError_StopScheduleTable_ScheduleTableID()    \
  ((ScheduleTableType) OSErrorService.param1)
#endif

/** Service ID of NextScheduleTable */
#define OSServiceId_NextScheduleTable           0x1Bu

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service NextScheduleTable */
#define OSError_NextScheduleTable_ScheduleTableID_From()    \
  ((ScheduleTableType) OSErrorService.param1)
/** Macro to access parameter of service NextScheduleTable */
#define OSError_NextScheduleTable_ScheduleTableID_To()    \
  ((ScheduleTableType) OSErrorService.param2)
#endif

/** Service ID of StartScheduleTableSynchron */
#define OSServiceId_StartScheduleTableSynchron  0x1Cu

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service StartScheduleTableSynchron */
#define OSError_StartScheduleTableSynchron_ScheduleTableID()    \
  ((ScheduleTableType) OSErrorService.param1)
#endif

/** Service ID of SyncScheduleTable */
#define OSServiceId_SyncScheduleTable           0x1Du

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service SyncScheduleTable */
#define OSError_SyncScheduleTable_ScheduleTableID()    \
  ((ScheduleTableType) OSErrorService.param1)
/** Macro to access parameter of service SyncScheduleTable */
#define OSError_SyncScheduleTable_Value()    \
  ((TickType) OSErrorService.param2)
#endif

/** Service ID of SetScheduleTableAsync */
#define OSServiceId_SetScheduleTableAsync       0x1Eu

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service SetScheduletableAsync */
#define OSError_SetScheduletableAsync_ScheduleTableID()    \
  ((ScheduleTableType) OSErrorService.param1)
#endif

/** Service ID of GetScheduleTableStatus */
#define OSServiceId_GetScheduleTableStatus      0x1Fu

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service GetScheduleTableStatus */
#define OSError_GetScheduleTableStatus_ScheduleTableID()    \
  ((ScheduleTableType) OSErrorService.param1)
/** Macro to access parameter of service GetScheduleTableStatus */
#define OSError_GetScheduleTableStatus_ScheduleStatus()    \
  ((ScheduleTableStatusRefType) OSErrorService.param2)
#endif

/** Service ID of IncrementCounter */
#define OSServiceId_IncrementCounter            0x20u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service IncrementCounter */
#define OSError_IncrementCounter_CounterID()    \
  ((CounterType) OSErrorService.param1)
#endif

/** Service ID of GetCounterValue */
#define OSServiceId_GetCounterValue             0x21u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service GetCounterValue */
#define OSError_GetCounterValue_CounterID()    \
  ((CounterType) OSErrorService.param1)
/** Macro to access parameter of service GetCounterValue */
#define OSError_GetCounterValue_Value()    \
  ((TickRefType) OSErrorService.param2)
#endif

/** Service ID of GetElapsedValue */
#define OSServiceId_GetElapsedValue             0x22u

#if defined(USEPARAMETERACCESS) && (USEPARAMETERACCESS == 0x1)
/** Macro to access parameter of service GetElapsedValue */
#define OSError_GetElapsedValue_CounterID()    \
  ((CounterType) OSErrorService.param1)
/** Macro to access parameter of service GetElapsedValue */
#define OSError_GetElapsedValue_Value()    \
  ((TickRefType) OSErrorService.param2)
/** Macro to access parameter of service GetElapsedValue */
#define OSError_GetElapsedValue_ElapsedValue()    \
  ((TickRefType) OSErrorService.param2)
#endif

/*
 * Macros for hook routines. When not configured, they
 * become empty statements.
 */
#if defined(HAS_STARTUPHOOK) && (HAS_STARTUPHOOK == 0x1)
/**
 * @def STARTUPHOOK
 * @brief Wrapper for StartupHook
 *
 * STARTUPHOOK invokes StartupHook and takes care of system
 * processing level settings.
 */
#define STARTUPHOOK() do {                                \
                        StartupHook ();                   \
                      } while (0)
#else
#define STARTUPHOOK() ;
#endif

#if defined(HAS_ERRORHOOK) && (HAS_ERRORHOOK == 0x1)
extern FlagType InErrorHook;

/**
 * @def ERRORHOOK
 * @brief Wrapper for ErrorHook
 *
 * ERRORHOOK invokes ErrorHook if e is not E_OK and takes
 * care of (avoiding) recursive error handling.
 */
#define ERRORHOOK(e) do {                                   \
                       if ((e != E_OK) && (!InErrorHook)) { \
                         InErrorHook = TRUE;                \
                         ErrorHook(e);                      \
                         InErrorHook = FALSE;               \
                       }                                    \
                     } while (0)
#else
#define ERRORHOOK(e) ;
#endif

#if defined(HAS_SHUTDOWNHOOK) && (HAS_SHUTDOWNHOOK == 0x1)
/**
 * @def SHUTDOWNHOOK
 * @brief Wrapper for ShutdownHook
 *
 * SHUTDOWNHOOK invokes ShutdownHook and takes care of system
 * processing level settings.
 */
#define SHUTDOWNHOOK(e) do {                               \
                          ShutdownHook (e);                \
                        } while (0)
#else
#define SHUTDOWNHOOK(e) ;
#endif

#if defined(HAS_PRETASKHOOK) && (HAS_PRETASKHOOK == 0x1)
/**
 * @def PRETASKHOOK
 * @brief Wrapper for PreTaskHook
 *
 * PRETASKHOOK invokes PreTaskHook and takes care of system
 * processing level settings.
 */
#define PRETASKHOOK() do {                              \
                        PreTaskHook ();                 \
                      } while (0)
#else
#define PRETASKHOOK() ;
#endif

#if defined(HAS_POSTTASKHOOK) && (HAS_POSTTASKHOOK == 0x1)
/**
 * @def POSTTASKHOOK
 * @brief Wrapper for PostTaskHook
 *
 * POSTTASKHOOK invokes PostTaskHook and takes care of system
 * processing level settings.
 */
#define POSTTASKHOOK() do {                               \
                         PostTaskHook ();                 \
                       } while (0)
#else
#define POSTTASKHOOK() ;
#endif

/**
 * @brief
 * This hook routine is called by the operating system at
 * the end of a system service which returns StatusType
 * not equal E_OK. It is called before returning to the
 * task level.
 *
 * This hook routine is called when an alarm expires and
 * an error is detected during task activation or event
 * setting.
 *
 * The ErrorHook is not called, if a system service called
 * from ErrorHook does not return E_OK as status value. Any
 * error by calling of system services from the ErrorHook
 * can only be detected by evaluating the status value.
 *
 * See chapter 11.1 for general description of hook
 * routines.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 *
 * @param[in] error
 *   Error occurred
 */
void ErrorHook (StatusType error);

/**
 * @brief
 * This hook routine is called by the operating system
 * before executing a new task, but after the transition of
 * the task to the running state (to allow evaluation of the
 * TaskID by GetTaskID).
 *
 * See chapter 11.1 for general description of hook routines.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 */
void PreTaskHook (void);

/**
 * @brief
 * This hook routine is called by the operating system after
 * executing the current task, but before leaving the task's
 * running state (to allow evaluation of the TaskID by
 * GetTaskID).
 *
 * See chapter 11.1 for general description of hook routines.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 */
void PostTaskHook (void);

/**
 * @brief
 * This hook routine is called by the operating system at
 * the end of the operating system initialisation and
 * before the scheduler is running. At this time the
 * application can initialise device drivers etc.
 *
 * See chapter 11.1 for general description of hook routines.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 */
void StartupHook (void);

/**
 * @brief
 * This hook routine is called by the operating system when
 * the OS service ShutdownOS has been called. This routine
 * is called during the operating system shut down.
 *
 * ShutdownHook is a hook routine for user defined shutdown
 * functionality, see chapter 11.4.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 */
void ShutdownHook (StatusType error);

#endif

/* vi: set et ai sw=2 sts=2: */
