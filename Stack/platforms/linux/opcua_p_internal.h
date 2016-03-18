/******************************************************************************************************/
/* (c) Copyright 2005-2008 The OPC Foundation                                                         */
/* ALL RIGHTS RESERVED.                                                                               */
/*                                                                                                    */
/* DISCLAIMER:                                                                                        */
/*  This code is provided by the OPC Foundation solely to assist in                                   */
/*  understanding and use of the appropriate OPC Specification(s) and may be                          */
/*  used as set forth in the License Grant section of the OPC Specification.                          */
/*  This code is provided as-is and without warranty or support of any sort                           */
/*  and is subject to the Warranty and Liability Disclaimers which appear                             */
/*  in the printed OPC Specification.                                                                 */
/******************************************************************************************************/
/* Platform Portability Layer                                                                         */
/* Modify the content of this file according to the event implementation on your system.              */
/* This is the win32 implementation                                                                   */
/******************************************************************************************************/

/* UA platform definitions, base types and configuration */
#include "opcua_platformdefs.h"

/* proxy stub errorhandling */
#include <opcua_statuscodes.h>
#include <opcua_stackstatuscodes.h>
#include <opcua_errorhandling.h>
#include <opcua_trace.h>

#include "opcua_p_trace.h"

/* import types for crypto and pki */
#include <opcua_types.h>
#include <opcua_crypto.h>
#include <opcua_pki.h>

/* own */
#include "opcua_p_interface.h"

/**********************************************************************************/
/*/  Configuration section.                                                      /*/
/**********************************************************************************/

/** @brief Maximum wait time for socket module (in Milli sec) at the blocking point. */
#define OPCUA_SOCKET_MAXLOOPTIME (OpcUa_UInt32)1000 /* reloop after 1 second to be secure against hangs */

/**********************************************************************************/
/*/  Trace Modules.                                                              /*/
/**********************************************************************************/
#define OpcUa_Module_P_OpenSSL 0
#define OpcUa_Module_P_CryptoFactory 1
#define OpcUa_Module_P_PKIFactory 2

/**********************************************************************************/
/*/  Evaluate Security Config.                                                   /*/
/**********************************************************************************/
/* determine wether OpenSSL is required and set the compiler switch appropriately */
/* DON'T CHANGE THIS MANUALLY, just add new supported policies! */

#if OPCUA_SUPPORT_SECURITYPOLICY_BASIC128RSA15 && !OPCUA_REQUIRE_OPENSSL
#  define OPCUA_REQUIRE_OPENSSL OPCUA_CONFIG_YES
#endif /* OPCUA_SUPPORT_SECURITYPOLICY_BASIC128RSA15 */

#if OPCUA_SUPPORT_SECURITYPOLICY_BASIC256 && !OPCUA_REQUIRE_OPENSSL
#  define OPCUA_REQUIRE_OPENSSL OPCUA_CONFIG_YES
#endif /* OPCUA_SUPPORT_SECURITYPOLICY_BASIC256 */

#if OPCUA_SUPPORT_SECURITYPOLICY_BASIC256SHA256 && !OPCUA_REQUIRE_OPENSSL
#  define OPCUA_REQUIRE_OPENSSL OPCUA_CONFIG_YES
#endif /* OPCUA_SUPPORT_SECURITYPOLICY_BASIC256 */

#if OPCUA_SUPPORT_PKI && !OPCUA_REQUIRE_OPENSSL
#  define OPCUA_REQUIRE_OPENSSL OPCUA_CONFIG_YES
#endif /* OPCUA_SUPPORT_PKI */

/* if at all, OPCUA_REQUIRE_OPENSSL is set to OPCUA_CONFIG_YES before this point. */
#ifndef OPCUA_REQUIRE_OPENSSL
#define OPCUA_REQUIRE_OPENSSL OPCUA_CONFIG_NO
#endif /* OPCUA_REQUIRE_OPENSSL */

#if OPCUA_REQUIRE_OPENSSL && !OPCUA_HAVE_OPENSSL
# error OpenSSL required; globally define OPCUA_HAVE_OPENSSL if OpenSSL is available or disable security!
#endif

#if defined(OPCUA_HAVE_HTTPS) && !OPCUA_SUPPORT_PKI
# error PKI support is required for HTTPS; globally define OPCUA_SUPPORT_PKI 
#endif

/**********************************************************************************/
/*/  Internally used function prototypes.                                        /*/
/**********************************************************************************/

/**
 * @brief Subtract Value 2 from Value 1 and store the result in the given location.
 *
 * @param a_Value1  [ in] Operand 1
 * @param a_Value2  [ in] Operand 2
 * @param a_pResult [out] Pointer to designated result.
 * @return Statuscode; OpcUa_Good; OpcUa_BadInvalidArgument; OpcUa_BadOutOfRange;
 */
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_GetDateTimeDiff(
    OpcUa_DateTime  a_Value1,
    OpcUa_DateTime  a_Value2,
    OpcUa_DateTime* a_pResult);

/**
 * @brief Subtract Value 2 from Value 1 and store the result as rounded number
 *        of seconds in Result.
 *
 * @param a_Value1   [ in] Operand 1
 * @param a_Value2   [ in] Operand 2
 * @param a_puResult [out] Pointer to designated result.
 * @return Statuscode; OpcUa_Good; OpcUa_BadInvalidArgument; OpcUa_BadOutOfRange;
 */
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_GetDateTimeDiffInSeconds32(
    OpcUa_DateTime  a_Value1,
    OpcUa_DateTime  a_Value2,
    OpcUa_UInt32*   a_puResult);

/**********************************************************************************/
/*/                                 End Of File.                                 /*/
/**********************************************************************************/

