#pragma once 
#include <QtGlobal>

#ifdef CC_MEDIA_SHARED
#define CC_MEDIA_EXPORT Q_DECL_EXPORT
#else
#define CC_MEDIA_EXPORT Q_DECL_IMPORT
#endif
