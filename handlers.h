/**
 * Author: Mark Gottscho
 * Email: mgottscho@ucla.edu
 */

#ifndef HANDLERS_H
#define HANDLERS_H

#include "memory_due.h"

//Declare relevant global data structures that are needed for DUE handlers at runtime (but extern -- they should be defined in handlers.c)
DECL_DUE_INFO_EXTERN(main, bar)
DECL_DUE_INFO_EXTERN(main, 2)
DECL_DUE_INFO_EXTERN(main, 3)
DECL_RECOVERY_EXTERN(main,m)
DECL_RECOVERY_EXTERN(main,b)
DECL_RECOVERY_EXTERN(main,i)
DECL_RECOVERY_EXTERN(main,x)
DECL_RECOVERY_EXTERN(main,y)

//Declare handler functions
int DUE_RECOVERY_HANDLER(main, bar, dueinfo_t *recovery_context);
int DUE_RECOVERY_HANDLER(main, 2, dueinfo_t *recovery_context);
int DUE_RECOVERY_HANDLER(main, 3, dueinfo_t *recovery_context); 

#endif
