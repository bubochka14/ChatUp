#pragma once 
#include <QtGlobal>
#define CC_NETWORK_EXPORT
#ifdef CC_NETWORK_SHARED
#define CC_NETWORK_EXPORT Q_DECL_EXPORT
#else
#define CC_NETWORK_EXPORT Q_DECL_IMPORT
#endif
