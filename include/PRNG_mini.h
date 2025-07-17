#if defined(_WIN32)
#ifndef PRNG_MINI_EXPORTS
#define PRNG_MINI_API __declspec(dllexport)
#else
#define PRNG_MINI_API __declspec(dllimport)
#endif
#endif

/// 
/// @brief Safe zeroization and free of a memory
/// @details Use this function to prevent a heap error.
/// @param Pointer to memory to be released.
/// @param Size to zeroize. 0 to parameters disables zeroization.
/// 
#if defined(_WIN32)
PRNG_MINI_API
#endif
void pm_free(void* buffer, int size);

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
#if defined(_WIN32)
PRNG_MINI_API
#endif
int pm_get_random_bytes(void** buffer, int length);

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
#if defined(_WIN32)
PRNG_MINI_API
#endif
int pm_get_random_integers(int** integers, int size, int min, int max);

/// 
/// @brief Generates a GUID string into a provided buffer using random hex digits.
/// @details Standard-based GUID generated using PRNG_mini. 36 length, 32 symbols.
///          Internal memory allocation adds a null terminator.
/// @param Pointer to memory of Pointer to memory where GUID will be written.
/// @return 0 on success, negative error code on failure:
///         -1 if the buffer pointer itself is NULL,
///         -2 if memory allocation fails.
/// 
#if defined(_WIN32)
PRNG_MINI_API
#endif
int pm_get_guid_std(char** buffer);

/// 
/// @brief Generates an ID string in HEX format. Saves into a provided buffer.
/// @details Unbounded ID generated using PRNG_mini.
///          Internal memory allocation adds a null terminator.
/// @param Pointer to memory of Pointer to memory where GUID will be written.
/// @return 0 on success, negative error code on failure:
///         -1 if the buffer pointer itself is NULL,
///         -2 if memory allocation fails.
/// 
#if defined(_WIN32)
PRNG_MINI_API
#endif
int pm_get_id_hex(char** buffer, int size);

///
/// @brief Validates a 16-digit hex license key.
/// @param signature Target checksum value to verify against (use any number for a key list).
/// @param key Null-terminated string containing the license key (e.g., "8A1F-B9C0-D4E0-3D5A").
/// @return 1 if the key is valid (checksum matches), 0 if invalid or input is NULL.
///
#if defined(_WIN32)
PRNG_MINI_API
#endif
int pm_get_license_key(char** output_key, int signature);

///
/// @brief Validates a 16-digit hex license key.
/// @param signature Target checksum value to verify against (use any number for a key list).
/// @param key Null-terminated string containing the license key (e.g., "8A1F-B9C0-D4E0-3D5A").
/// @return 1 if the key is valid (checksum matches), 0 if invalid or input is NULL.
///
#if defined(_WIN32)
PRNG_MINI_API
#endif
int pm_validate_license_key(const char* key, int signature);