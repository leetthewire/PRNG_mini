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
int random_device_bytes_unix(void* buffer, size_t length) 
{
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
    if (buffer == NULL)
    {
        return -1;
    }

    if (*buffer == NULL)
    {
        *buffer = malloc(length);
        if (*buffer == NULL)
            return -2; // memory allocation failed
    }
    int result = -1;

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
    if (integers == NULL || size <= 0 || min > max)
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

/// 
/// @brief Generates a GUID string into a provided buffer using random hex digits.
/// @details Standard-based GUID generated using PRNG_mini. 36 length, 32 symbols.
///          Internal memory allocation adds a null terminator.
/// @param Pointer to memory of Pointer to memory where GUID will be written.
/// @return 0 on success, negative error code on failure:
///         -1 if the buffer pointer itself is NULL,
///         -2 if memory allocation fails.
/// 
int pm_get_guid_std(char** buffer) 
{

    if (buffer == NULL)
        return -1;

    if (*buffer == NULL)
    {
        *buffer = malloc(sizeof(char) * 37); //std_length for a guid + /null terminator
        if (*buffer == NULL)
            return -2; // memory allocation failed
        memset(*buffer, 0, 37);
    }

    char* output_buffer = *buffer;

    int std_length = 32;

    int* integers_buffer = NULL;
    pm_get_random_integers(&integers_buffer, 32, 0, 15);
    int int_buff_iter = 0;

    for (int i = 0; i < std_length; i++) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            output_buffer[std_length] = "-";
            std_length = std_length + 1;
        }
        else {
            int value = 0;
            value = integers_buffer[int_buff_iter];
            if (value <= 9)
                output_buffer[std_length] = (value + 48);
            else
                output_buffer[std_length] = (value + 97 - 10);

            int_buff_iter++;
        }
    }

    free(integers_buffer);

    return 0;
}

///
/// @brief Generates a 16-digit hexadecimal license key with dashes, matching a given checksum signature.
/// @param output_key Pointer to a char* that will be allocated and filled with the generated key (format: XXXX-XXXX-XXXX-XXXX).
/// @param signature Target checksum value; the sum of all 16 hex digits will be adjusted to match this value.
/// @return 0 on success, -1 if output pointer is NULL, -2 if random generation fails, -3 if memory allocation fails.
///
int pm_get_id_hex(char** buffer, int size)
{
    if (buffer == NULL)
        return -1;

    if (*buffer == NULL)
    {
        *buffer = malloc(sizeof(char) * size + 1); //std_length for a guid + /null terminator
        if (*buffer == NULL)
            return -2; // memory allocation failed
        memset(*buffer, 0, size + 1);
    }

    char* output_buffer = *buffer;

    int* integers_buffer = NULL;
    pm_get_random_integers(&integers_buffer, size, 0, 15);
    int int_buff_iter = 0;

    for (int i = 0; i < size; i++) {
         int value = 0;
         value = integers_buffer[size];
         if (value <= 9)
             output_buffer[size] = (value + 48);
         else
             output_buffer[size] = (value + 97 - 10);
    }

    free(integers_buffer);

    return 0;
}

///
/// @brief Validates a 16-digit hex license key.
/// @param signature Target checksum value to verify against (use any number for a key list).
/// @param key Null-terminated string containing the license key (e.g., "8A1F-B9C0-D4E0-3D5A").
/// @return 1 if the key is valid (checksum matches), 0 if invalid or input is NULL.
///
int pm_generate_license_key(char** output_key, int signature) 
{
    if (output_key == NULL)
        return -1;

    const int size = 16;              // number of hex digits
    const int formatted_size = 19;    // 16 digits + 3 dashes + null terminator

    // Allocate output string
    *output_key = malloc(formatted_size);
    if (*output_key == NULL) {
        return -3;
    }

    int* buffer = NULL;
    if (pm_get_random_integers(&buffer, size, 0, 15) != 0 || buffer == NULL)
        return -2;

    // Compute initial checksum
    int current_sum = 0;
    for (int i = 0; i < size; i++)
        current_sum += buffer[i];

    // Adjust to match the requested signature
    int diff = signature - current_sum;
    while (diff != 0) {
        for (int i = 0; i < size && diff != 0; i++) {
            if (diff > 0 && buffer[i] < 15) {
                buffer[i]++;
                diff--;
            }
            else if (diff < 0 && buffer[i] > 0) {
                buffer[i]--;
                diff++;
            }
        }
    }

    // Format the key as "xxxx-xxxx-xxxx-xxxx"
    int out_idx = 0;
    for (int i = 0; i < size; i++) {
        int val = buffer[i];
        (*output_key)[out_idx++] = (val < 10) ? ('0' + val) : ('A' + val - 10);
        if ((i + 1) % 4 == 0 && i + 1 != size)
            (*output_key)[out_idx++] = '-';
    }

    (*output_key)[out_idx] = '\0';
    free(buffer);
    return 0;
}

///
/// @brief Validates a 16-digit hex license key.
/// @param signature Target checksum value to verify against (use any number for a key list).
/// @param key Null-terminated string containing the license key (e.g., "8A1F-B9C0-D4E0-3D5A").
/// @return 1 if the key is valid (checksum matches), 0 if invalid or input is NULL.
///
int pm_validate_license_key(const char* key, int signature)
{
    if (key == NULL)
        return 0;

    int validate_signature = 0;

    for (size_t i = 0; i < strlen(key); ++i)
    {
        char c = key[i];

        if (c >= '0' && c <= '9')
            validate_signature += c - '0';
        else if (c >= 'A' && c <= 'F')
            validate_signature += c - 'A' + 10;
        else if (c >= 'a' && c <= 'f')
            validate_signature += c - 'a' + 10;
    }

    return (validate_signature == signature);
}