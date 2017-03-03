/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 * 
 * Handler template code for dealing with DUEs.
 * DO NOT compile or include this file directly!
 */

#ifndef SDECC_HANDLERS_H
#define SDECC_HANDLERS_H

#include <memory_due.h>
#include <minipk.h>

#define YOUR_FUNCTION_NAME foo
#define YOUR_IDENTIFIER bar
#define YOUR_CRITICAL_VARIABLE crit_var
#define YOUR_APPROXIMABLE_VARIABLE approx_var
#define YOUR_CUSTOM_VARIABLE custom_var
#define SOME_TYPE unsigned long 

//Declare relevant global data structures that are needed for DUE handlers at runtime (but extern -- they should be defined in handlers.c)
DECL_DUE_INFO_EXTERN(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER)
DECL_RECOVERY_EXTERN(YOUR_FUNCTION_NAME, YOUR_CRITICAL_VARIABLE, SOME_TYPE)
DECL_RECOVERY_EXTERN(YOUR_FUNCTION_NAME, YOUR_APPROXIMABLE_VARIABLE, SOME_TYPE)
DECL_RECOVERY_EXTERN(YOUR_FUNCTION_NAME, YOUR_CUSTOM_VARIABLE, SOME_TYPE)

//Declare handler functions
int DUE_RECOVERY_HANDLER(YOUR_FUNCTION_NAME, YOUR_IDENTIFIER, dueinfo_t *recovery_context);

#endif
