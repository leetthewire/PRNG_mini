#include <PRNG_mini.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Number of values to generate
#define SAMPLE_SIZE     100000
#define RANGE_MIN       0
#define RANGE_MAX       19

typedef struct
{
    int value;      // The number itself (0..19)
    int count;      // Number of occurrences
    int deviation;  // Absolute deviation from expected
} HistogramEntry;

// Compare function for sorting by deviation (descending)
int compare_deviation_desc(const void* a, const void* b)
{
    HistogramEntry* x = (HistogramEntry*)a;
    HistogramEntry* y = (HistogramEntry*)b;
    return y->deviation - x->deviation;
}

// Function to sort and print randomness quality
void sort_and_print_randomness_quality(const int* histogram, int bin_count, int total_samples, int min)
{
    int expected = total_samples / bin_count;
    HistogramEntry* entries = malloc(bin_count * sizeof(HistogramEntry));
    if (!entries)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    for (int i = 0; i < bin_count; ++i)
    {
        entries[i].value = i + min;
        entries[i].count = histogram[i];
        entries[i].deviation = abs(histogram[i] - expected);
    }

    qsort(entries, bin_count, sizeof(HistogramEntry), compare_deviation_desc);

    printf("\nSorted Randomness Deviation (most biased first):\n");
    for (int i = 0; i < bin_count; ++i)
    {
        printf("%2d: %6d (Deviation: %+d)\n",
            entries[i].value,
            entries[i].count,
            entries[i].count - expected);
    }

    memset(entries, 0, bin_count * sizeof(HistogramEntry));
    free(entries);
}

int main(void)
{
    int* samples = NULL;
    int range_size = RANGE_MAX - RANGE_MIN + 1;
    int histogram[RANGE_MAX - RANGE_MIN + 1] = { 0 };

    // Generate random integers
    if (pm_get_random_integers(&samples, SAMPLE_SIZE, RANGE_MIN, RANGE_MAX) != 0)
    {
        fprintf(stderr, "Error: failed to generate random integers\n");
        return 1;
    }

    // Fill histogram
    for (int i = 0; i < SAMPLE_SIZE; ++i)
    {
        if (samples[i] < RANGE_MIN || samples[i] > RANGE_MAX)
        {
            fprintf(stderr, "Error: value out of range: %d\n", samples[i]);
            continue;
        }
        histogram[samples[i] - RANGE_MIN]++;
    }

    // Print histogram
    printf("Randomness Histogram [%d samples in range %d-%d]:\n", SAMPLE_SIZE, RANGE_MIN, RANGE_MAX);
    for (int i = 0; i < range_size; ++i)
    {
        printf("%2d: %6d ", i + RANGE_MIN, histogram[i]);
        int stars = histogram[i] / (SAMPLE_SIZE / 100); // Scale stars
        for (int j = 0; j < stars; ++j)
            printf("*");
        printf("\n");
    }

    // Analyze quality
    sort_and_print_randomness_quality(histogram, range_size, SAMPLE_SIZE, RANGE_MIN);

    pm_free(samples, SAMPLE_SIZE * sizeof(int));
    return 0;
}
