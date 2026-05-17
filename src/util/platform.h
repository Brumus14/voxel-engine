#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN64
#define PLATFORM_WINDOWS 1

#elif __APPLE__
#define PLATFORM_MAC 1
#define PLATFORM_POSIX 1

#elif __linux__
#define PLATFORM_LINUX 1
#define PLATFORM_POSIX 1

#endif

#endif
