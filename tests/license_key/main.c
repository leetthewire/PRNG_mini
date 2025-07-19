#include <PRNG_mini.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char* input_file;
    const char* output_file;
    int signature;
    int keys_number;
} Options;

void print_usage()
{
    printf(" \t \t======================\n");
    printf("\tLicense Key generation and validation\n\n");
    printf("-----------------------------------------------------------\n");
    printf("Generation usage: license_key.exe -out file -sg signature\n");
    printf("Validation usage: license_key.exe -in file -sg signature -n keys_number\n");
    printf("Signature: an integer value between 1 and 254 (inclusive)\n");
    printf("-----------------------------------------------------------\n\n");
}

void parse_arguments(int argc, char** argv, Options* opts)
{
    opts->input_file = "list.txt";
    opts->output_file = "list.txt";
    opts->signature = 210;
    opts->keys_number = 10;

    for (int i = 1; i < argc - 1; ++i)
    {
        if (strcmp(argv[i], "-out") == 0)
        {
            opts->output_file = argv[i + 1];
        }
        else if (strcmp(argv[i], "-in") == 0)
        {
            opts->input_file = argv[i + 1];
        }
        else if (strcmp(argv[i], "-sg") == 0)
        {
            opts->signature = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-n") == 0)
        {
            opts->keys_number = atoi(argv[i + 1]);
        }
    }
}

void generate_license_keys(const char* filename, int signature, int keys_number)
{
    FILE* f = fopen(filename, "w");
    if (!f)
    {
        perror("Failed to open output file");
        exit(1);
    }

    for (int i = 0; i < keys_number; i++)
    {
        char* license_key = NULL;
        int size = pm_get_license_key(&license_key, signature);

        if (license_key)
        {
            printf("%s\n", license_key);
            fprintf(f, "%s\n", license_key);
            pm_free(license_key, size);
        }
        else
        {
            fprintf(stderr, "Failed to generate license key.\n");
        }
    }

    fclose(f);
    printf("License keys saved to %s\n", filename);
}

void validate_license_keys(const char* filename, int signature)
{
    FILE* f = fopen(filename, "r");
    if (!f)
    {
        perror("Failed to open input file");
        exit(1);
    }

    char buffer[256];
    int total = 0;
    int valid = 0;
    int line = 0;

    while (fgets(buffer, sizeof(buffer), f))
    {
        line++;
        char* newline = strchr(buffer, '\n');
        if (newline) *newline = '\0';

        if (strlen(buffer) == 0)
            continue;

        total++;

        if (pm_validate_license_key(buffer, signature))
        {
            printf("Line %d: Valid - %s\n", line, buffer);
            valid++;
        }
        else
        {
            printf("Line %d: Invalid - %s\n", line, buffer);
        }
    }

    fclose(f);
    printf("Validation complete: %d valid out of %d\n", valid, total);
}

int main(int argc, char** argv)
{
    Options opts;
    parse_arguments(argc, argv, &opts);
    print_usage();

    if (opts.output_file)
    {
        generate_license_keys(opts.output_file, opts.signature, opts.keys_number);
    }
    else if (opts.input_file)
    {
        validate_license_keys(opts.input_file, opts.signature);
    }
    else
    {
        fprintf(stderr, "Error: use -out <file> to generate or -in <file> to validate.\n");
        return 1;
    }

    return 0;
}
