#ifdef WIN32
#include <windows.h>
#include <bcrypt.h>
#include <stdint.h>
#include <stdio.h>
#else
//Supported by Linux, MacOS, BSD, Android, iOS, Unix-Like OS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#ifdef WIN32
///
/// @brief PRNG mini - device based - random bytes generation for Windows
/// @details Fill the provided buffer with cryptographically secure random bytes.
/// @param buffer Pointer to memory where random bytes will be written.
/// @param length Number of bytes to generate.
/// @return 0 on success, non-zero error code on failure.
///
int pm_random_device_bytes_windows(void* buffer, int length)
{
    if (!buffer || length == 0)
        return -1; // invalid arguments

    NTSTATUS status = BCryptGenRandom(
        NULL,                           // Use system-preferred RNG
        (PUCHAR)buffer,
        (ULONG)length,
        BCRYPT_USE_SYSTEM_PREFERRED_RNG
    );

    return (status == 0) ? 0 : (int)status;
}
#else
///
/// @brief PRNG mini - device based - random bytes generation for Unix
/// @details Fills the given buffer with cryptographically secure random bytes using /dev/urandom.
/// @param buffer Pointer to the memory where random bytes will be written.
/// @param length Number of bytes to generate.
/// @return 0 on success,
///         -1 if buffer is NULL or length is zero,
///         -2 if /dev/urandom could not be opened,
///         -3 if reading from /dev/urandom failed or was incomplete.
///
int random_device_bytes_unix(void* buffer, size_t length) {
    if (!buffer || length == 0)
        return -1;

    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0)
        return -2;

    ssize_t result = read(fd, buffer, length);
    close(fd);
    return (result == (ssize_t)length) ? 0 : -3;
}
#endif

///
/// @brief PRNG mini - device based - random bytes generation
/// @details Fill the provided buffer with cryptographically secure random bytes.
/// Allocate: uint8_t* buffer = NULL; 
/// Usage: ...bytes(&buffer,...);
/// @param buffer Pointer to memory of Pointer to memory where random bytes will be written.
/// @param length Number of bytes to generate.
/// @return 0 on success,
///         non-zero error code on failure.
///         -1 - invalid arguments.
///         status < -1 - Use RtlNtStatusToDosError() to convert to a Win32 error code on Windows.
///         status < -1 - Check /dev/urandom error codes on Linux.
/// 
int pm_get_random_bytes(void** buffer, int length)
{
    int result = -1;
    if (*buffer == NULL)
    {
        *buffer = malloc(length);
        return -2;
    }
#ifdef WIN32
    result = pm_random_device_bytes_windows(*buffer, length);
#else
    result = pm_random_device_bytes_unix(*buffer, length);
#endif

    return result;
}

///
/// @brief PRNG mini - device based - random integers generation
/// @details Fill the provided buffer with cryptographically secure random bytes.
/// Allocate: int* buffer = NULL; 
/// Usage: ...integers(&buffer,...);
/// @param integers Pointer to memory of Pointer to memory where random integers will be written.
/// @param size Number of integers to generate.
/// @return 0 on success,
///         non-zero error code on failure.
///         -1 - invalid arguments.
///         -2 - memory allocation failed.
///         -3 - random bytes generation failed
/// 
int pm_get_random_integers(int** integers, int size, int min, int max)
{
    if (size <= 0 || min > max)
        return -1; // invalid arguments

    if (*integers == NULL)
    {
        *integers = malloc(sizeof(int) * size);
        if (*integers == NULL)
            return -2; // memory allocation failed
    }

    // Allocate temporary random bytes buffer
    int byte_len = size * sizeof(int);
    void* byte_buffer = NULL;
    int result = pm_get_random_bytes(&byte_buffer, byte_len);
    if (result != 0 || byte_buffer == NULL)
        return -3; // random byte generation failed

    // Convert bytes to integers in [min, max]
    int range = max - min + 1;
    int* raw = (int*)byte_buffer;
    for (int i = 0; i < size; ++i)
    {
        uint32_t val = ((uint32_t)raw[i]) & 0x7FFFFFFF; // ensure non-negative bytes to prevent overflows
        (*integers)[i] = min + (val % range);
    }

    free(byte_buffer);
    return 0; // success
}