#pragma once 
#include <QtGlobal>

#ifdef CC_CORE_SHARED
#define CC_CORE_EXPORT Q_DECL_EXPORT
#else
#define CC_CORE_EXPORT Q_DECL_IMPORT
#endif
