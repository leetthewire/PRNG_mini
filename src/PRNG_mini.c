#include <string.h>
#include <stdint.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>
#else
//Supported by Linux, MacOS, BSD, Android, iOS, Unix-Like OS
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#ifdef _WIN32
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
int pm_random_device_bytes_unix(void* buffer, size_t length) 
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
/// @brief Safe zeroization and free of a memory
/// @details Use this function to prevent a heap error.
/// @param Pointer to memory to be released.
/// @param Size to zeroize. 0 to parameters disables zeroization.
/// 
void pm_free(void* buffer, int size)
{
    if (buffer == NULL)
        return;

    if (size != 0)
    {
        memset(buffer, 0, size);
    }
    free(buffer);
}

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

    if (result < 0)
    {
        pm_free(*buffer, length);
    }

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

    int byte_len = sizeof(uint32_t) * size; // 4 bytes per integer (uint32_t)
    unsigned char* byte_buffer = NULL;

    int result = pm_get_random_bytes(&byte_buffer, byte_len);
    if (result != 0 || byte_buffer == NULL)
        return -3; // random byte generation failed

    int range = max - min + 1;

    for (int i = 0; i < size; ++i)
    {
        // Assemble 4 bytes into a uint32_t (big endian)
        uint32_t val = 0;
        val |= (uint32_t)byte_buffer[i * 4 + 0] << 24;
        val |= (uint32_t)byte_buffer[i * 4 + 1] << 16;
        val |= (uint32_t)byte_buffer[i * 4 + 2] << 8;
        val |= (uint32_t)byte_buffer[i * 4 + 3];

        val &= 0x7FFFFFFF; // ensure non-negative
        (*integers)[i] = min + (val % range);
    }

    pm_free(byte_buffer, byte_len);
    return 0; // success
}

///
/// @brief PRNG mini – device-based random integer generation
/// @details Returns a randomly generated integer using cryptographically secure random bytes.
/// Usage: int secure_integer = pm_get_random_int(...);
/// @param min Minimum value of the range (inclusive).
/// @param max Maximum value of the range (inclusive).
/// @return A random integer on success.
///
int pm_get_random_int(int min, int max)
{
    // Allocate temporary random bytes buffer
    int byte_len = sizeof(int); 
    unsigned char* byte_buffer = NULL;
    int result = pm_get_random_bytes(&byte_buffer, byte_len);
    if (result != 0 || byte_buffer == NULL)
        return -3; // random byte generation failed

    // Convert bytes to integers in [min, max]
    uint32_t val = 0;
    for (int i = 0; i < byte_len; ++i)
    {
        val = (val << 8) | byte_buffer[i];
    }

    pm_free(byte_buffer, byte_len);

    val &= 0x7FFFFFFF; // ensure non-negative

    int range = max - min + 1;
    return min + (val % range);
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
        *buffer = malloc(37); // 36 chars + null terminator
        if (*buffer == NULL)
            return -2;
        memset(*buffer, 0, 37);
    }

    char* output_buffer = *buffer;

    // Generate 16 random bytes (128 bits)
    unsigned char uuid[16] = { 0 };
    int* random_ints = NULL;
    if (pm_get_random_integers(&random_ints, 16, 0, 255) != 0)
        return -3;

    for (int i = 0; i < 16; ++i)
        uuid[i] = (unsigned char)(random_ints[i] & 0xFF);

    pm_free(random_ints, 16 * sizeof(int));

    // Set UUID version and variant bits
    uuid[6] = (uuid[6] & 0x0F) | 0x40; // Version 4
    uuid[8] = (uuid[8] & 0x3F) | 0x80; // Variant 1 (RFC 4122)

    // Format to 8-4-4-4-12
    snprintf(output_buffer, 37,
        "%02x%02x%02x%02x-"
        "%02x%02x-"
        "%02x%02x-"
        "%02x%02x-"
        "%02x%02x%02x%02x%02x%02x",
        uuid[0], uuid[1], uuid[2], uuid[3],
        uuid[4], uuid[5],
        uuid[6], uuid[7],
        uuid[8], uuid[9],
        uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);

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
    if (buffer == NULL || size <= 0)
        return -1;

    if (*buffer == NULL)
    {
        *buffer = malloc(size + 1); // size hex chars + null terminator
        if (*buffer == NULL)
            return -2; // memory allocation failed
        memset(*buffer, 0, size + 1);
    }

    char* output_buffer = *buffer;

    int* integers_buffer = NULL;
    if (pm_get_random_integers(&integers_buffer, size, 0, 15) != 0 || !integers_buffer)
        return -3;

    for (int i = 0; i < size; i++) {
        int value = integers_buffer[i];
        if (value <= 9)
            output_buffer[i] = value + '0';
        else
            output_buffer[i] = value - 10 + 'a';
    }

    output_buffer[size] = '\0'; // ensure null-terminated

    pm_free(integers_buffer, size * sizeof(int));

    return 0;
}

///
/// @brief generates a 16-digit hex license key.
/// @param Output pointer of a memory to store the license key (e.g., "8A1F-B9C0-D4E0-3D5A").
/// @param signature Target checksum value to perform the key.
/// @return key length on success, negative error code on failure:
///         -1 if the buffer pointer itself is NULL,
///         -2 if random integer generation fails.
///         -3 if memory allocation fails.
///
int pm_get_license_key(char** out_key, int signature)
{
    if (out_key == NULL)
        return -1;

    const int size = 16;
    const int formatted_size = 20; // 16 chars + 3 dashes + null terminator

    int key_raw_data[16];
    for (int i = 0; i < size; i++)
        key_raw_data[i] = 18;

    int change_amount = signature - (18 * size);
    int to_increment = 1;
    if (change_amount < 0)
    {
        to_increment = 0;
        change_amount = -change_amount;
    }

    // Adjust each symbol based on influence (1–4)
    while (change_amount > 0)
    {
        int influence = pm_get_random_int(1, 4);
        if (influence < 1 || influence > 4)
            return -2;
        if (influence > change_amount)
            influence = change_amount;
        change_amount -= influence;

        int index = pm_get_random_int(0, 15);
        if (index < 0 || index > 15)
            return -2;

        if (to_increment)
        {
            while (key_raw_data[index] + influence > 36)
            {
                index = pm_get_random_int(0, 15);
                if (index < 0 || index > 15)
                    return -2;
            }
            key_raw_data[index] += influence;
        }
        else
        {
            while (key_raw_data[index] - influence < 1)
            {
                index = pm_get_random_int(0, 15);
                if (index < 0 || index > 15)
                    return -2;
            }
            key_raw_data[index] -= influence;
        }
    }

    // Random redistribution of value
    int repeats = pm_get_random_int(500, 12000);
    for (int i = 0; i < repeats; i++)
    {
        int a = pm_get_random_int(0, 15);
        if (a < 0 || a > 15)
            return -2;

        int b = pm_get_random_int(0, 15);
        if (b < 0 || b > 15)
            return -2;

        while (a == b || key_raw_data[a] <= 1 || key_raw_data[b] >= 36)
        {
            a = pm_get_random_int(0, 15);
            if (a < 0 || a > 15)
                return -2;
            b = pm_get_random_int(0, 15);
            if (b < 0 || b > 15)
                return -2;
        }

        key_raw_data[a]--;
        key_raw_data[b]++;
    }

    // Allocate final formatted string
    *out_key = (char*)malloc(formatted_size);
    if (*out_key == NULL)
        return -2;

    int idx = 0;
    for (int i = 0; i < size; i++)
    {
        int val = key_raw_data[i];
        char ch;

        if (val >= 1 && val <= 10)
            ch = (char)('0' + val - 1);      // '1' to '9', '10' = ':'
        else if (val >= 11 && val <= 36)
            ch = (char)('A' + (val - 11));   // 'A' to 'Z'
        else
            ch = '?'; // out-of-bounds fallback

        (*out_key)[idx++] = ch;

        if ((i + 1) % 4 == 0 && i + 1 != size)
            (*out_key)[idx++] = '-';
    }

    (*out_key)[idx] = '\0';

    // Final checksum validation
    int final_sum = 0;
    for (int i = 0; i < size; i++)
        final_sum += key_raw_data[i];

    if (final_sum != signature)
    {
        pm_free(*out_key, formatted_size);
        *out_key = NULL;
        return -3; // Signature mismatch
    }

    return formatted_size; // Success
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
