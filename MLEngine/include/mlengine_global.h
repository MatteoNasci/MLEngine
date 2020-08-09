#ifndef MLENGINE_GLOBAL_H
#define MLENGINE_GLOBAL_H

#if defined(MLENGINE_LIBRARY)
#define MLENGINE_SHARED_EXPORT __declspec(dllexport)
#else
#define MLENGINE_SHARED_EXPORT __declspec(dllimport)
#endif

#endif //MLENGINE_GLOBAL_H