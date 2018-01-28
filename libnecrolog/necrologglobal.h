#pragma once

#if defined _WIN32
#define _NECROLOG_DECL_EXPORT     __declspec(dllexport)
#define _NECROLOG_DECL_IMPORT     __declspec(dllimport)
#else
#define _NECROLOG_DECL_EXPORT     __attribute__((visibility("default")))
#define _NECROLOG_DECL_IMPORT     __attribute__((visibility("default")))
#define _NECROLOG_DECL_HIDDEN     __attribute__((visibility("hidden")))
#endif

/// Declaration of macros required for exporting symbols
/// into shared libraries
#if defined(NECROLOG_BUILD_DLL)
#define NECROLOG_DECL_EXPORT _NECROLOG_DECL_EXPORT
#else
#define NECROLOG_DECL_EXPORT _NECROLOG_DECL_IMPORT
#endif

