#if defined(_WIN32)
#ifndef PRNG_MINI_EXPORTS
#define PRNG_MINI_API __declspec(dllexport)
#else
#define PRNG_MINI_API __declspec(dllimport)
#endif
#endif

#if defined(_WIN32)
PRNG_MINI_API
#endif
int pm_get_random_bytes(void** buffer, int length);

#if defined(_WIN32)
PRNG_MINI_API
#endif
int pm_get_random_integers(int** integers, int size, int min, int max);