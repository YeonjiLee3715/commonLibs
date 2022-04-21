//
// Created by 이연지 on 2020-01-06.
//

#ifndef COMMONLIBDEF_H
#define COMMONLIBDEF_H

#ifndef __APPLE__
#if __cplusplus > 201402L //C++17
#define USE_CPP17   1
#elif __cplusplus > 201103L //C++14
#define USE_CPP14   1
#endif
#endif

#if defined(_MSC_VER)
#ifdef _WIN64
typedef __int64 ssize_t;
#else
typedef int     ssize_t;
#endif
#endif

#ifndef DEPRECATED
#if defined(__GNUC__) || defined(__clang__)
#define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED __declspec(deprecated)
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED
#endif
#endif

#endif //COMMONLIBDEF_H
