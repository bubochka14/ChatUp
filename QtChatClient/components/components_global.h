#pragma once
#if defined(SHARED)
#  define COMP_EXPORT Q_DECL_EXPORT
#else
#  define COMP_EXPORT Q_DECL_IMPORT
#endif