#pragma once 
#include <QtGlobal>

#ifdef CC_CALLS_SHARED
#define CC_CALS_EXPORT Q_DECL_EXPORT
#else
#define CC_CALS_EXPORT Q_DECL_IMPORT
#endif
