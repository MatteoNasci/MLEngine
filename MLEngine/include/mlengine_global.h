#ifndef MLENGINE_GLOBAL_H
#define MLENGINE_GLOBAL_H


#if defined(MLENGINE_STATIC)
#define MLENGINE_SHARED_EXPORT
#else //!defined(MLENGINE_STATIC)

#if defined(MLENGINE_LIBRARY)
#define MLENGINE_SHARED_EXPORT __declspec(dllexport)
#else //!defined(MLENGINE_LIBRARY)
#define MLENGINE_SHARED_EXPORT __declspec(dllimport)
#endif //defined(MLENGINE_LIBRARY)

#endif //defined(MLENGINE_STATIC)

#endif //MLENGINE_GLOBAL_H