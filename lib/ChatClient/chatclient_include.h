#pragma once 
#include <QtGlobal>

#ifdef CHAT_CLIENT_LIB
#define CHAT_CLIENT_EXPORT Q_DECL_EXPORT
#else
#define CHAT_CLIENT_EXPORT Q_DECL_IMPORT
#endif
