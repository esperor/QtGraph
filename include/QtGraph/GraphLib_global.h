
#pragma once

#include <QtCore/qglobal.h>

#if defined(GRAPHLIB_LIBRARY)
#  define GRAPHLIB_EXPORT Q_DECL_EXPORT
#elif defined(IMPORT_GRAPHLIB)
#  define GRAPHLIB_EXPORT Q_DECL_IMPORT
#else
#  define GRAPHLIB_EXPORT
#endif
