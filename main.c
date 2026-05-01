#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_COLUMNS 20
#define MAX_FIELD_LEN 100

char header[MAX_COLUMNS][MAX_FIELD_LEN];

typedef struct
{
    char fields[MAX_COLUMNS][MAX_FIELD_LEN];
} Row;

int main(const int argc, char* argv[])
{
    // --help
    if (argc == 2 && strcmp(argv[1], "--help") == 0)
    {
        printf(
            "Usage: %s <file.csv> [--column <column>] [--search <value>]\n\n"
            "Description:\n"
            "  Reads a CSV file and lets you query rows by column value.\n\n"
            "Arguments:\n"
            "  <file.csv>             Path to the CSV file to read\n"
            "  --column <column>      Column name to filter by\n"
            "  --search <value>       Value to search for in the specified column\n\n"
            "Examples:\n"
            "  %s data.csv                                  Print all rows\n"
            "  %s data.csv --column name                    Print all values in name column\n"
            "  %s data.csv --column name --search David     Search for David in name column\n",
            argv[0], argv[0], argv[0], argv[0]
        );
        return 0;
    }

    // only 2, 4, 6 are valid arg counts
    if (argc != 2 && argc != 4 && argc != 6)
    {
        printf("Usage: %s <file.csv> [--column <column>] [--search <value>]\n", argv[0]);
        printf("Run '%s --help' for more information.\n", argv[0]);
        return 1;
    }

    // validate flags upfront, extract values
    char* columnArg = NULL;
    char* searchArg = NULL;

    if (argc >= 4)
    {
        if (strcmp(argv[2], "--column") != 0)
        {
            printf("Error: expected '--column', got '%s'\n", argv[2]);
            printf("Run '%s --help' for more information.\n", argv[0]);
            return 1;
        }
        columnArg = argv[3];
    }

    if (argc == 6)
    {
        if (strcmp(argv[4], "--search") != 0)
        {
            printf("Error: expected '--search', got '%s'\n", argv[4]);
            printf("Run '%s --help' for more information.\n", argv[0]);
            return 1;
        }
        searchArg = argv[5];
    }

    // open file
    FILE* pF = fopen(argv[1], "r");
    if (pF == NULL)
    {
        printf("Error: could not open file '%s'\n", argv[1]);
        return 1;
    }

    // dynamic storage — start with 10, grow as needed
    int capacity = 10;
    int rowCount = 0;
    int columnCount = 0;
    bool isFirstLine = true;
    char buffer[255];

    Row* rows = calloc(capacity, sizeof(Row));
    if (rows == NULL)
    {
        printf("Error: out of memory\n");
        fclose(pF);
        return 1;
    }

    // read and parse CSV
    while (fgets(buffer, sizeof(buffer), pF) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = 0;

        int colIndex = 0;
        const char* token = strtok(buffer, ",");

        while (token != NULL)
        {
            if (isFirstLine)
            {
                strcpy(header[colIndex], token);
                columnCount++;
            }
            else
            {
                if (colIndex < MAX_COLUMNS)
                {
                    strcpy(rows[rowCount].fields[colIndex], token);
                }
            }

            colIndex++;
            token = strtok(NULL, ",");
        }

        if (isFirstLine)
        {
            isFirstLine = false;
        }
        else
        {
            rowCount++;

            // grow if we're about to overflow
            if (rowCount >= capacity)
            {
                capacity *= 2;
                Row* temp = realloc(rows, capacity * sizeof(Row));
                if (temp == NULL)
                {
                    printf("Error: out of memory\n");
                    free(rows);
                    fclose(pF);
                    return 1;
                }
                rows = temp;
            }
        }
    }

    fclose(pF);

    // query
    if (columnArg == NULL)
    {
        // no column specified — print everything
        printf("Headers: ");
        for (int i = 0; i < columnCount; i++)
            printf("[%s] ", header[i]);
        printf("\n\n");

        for (int i = 0; i < rowCount; i++)
        {
            for (int j = 0; j < columnCount; j++)
                printf("%s: %s  ", header[j], rows[i].fields[j]);
            printf("\n");
        }
    }
    else
    {
        // find column index
        int colIndex = -1;
        for (int i = 0; i < columnCount; i++)
        {
            if (strcmp(header[i], columnArg) == 0)
            {
                colIndex = i;
                break;
            }
        }

        if (colIndex == -1)
        {
            printf("Error: column '%s' not found in '%s'\n", columnArg, argv[1]);
            free(rows);
            return 1;
        }

        // print matching rows
        int matchCount = 0;
        for (int i = 0; i < rowCount; i++)
        {
            if (searchArg == NULL || strstr(rows[i].fields[colIndex], searchArg) != NULL)
            {
                for (int j = 0; j < columnCount; j++)
                    printf("%s: %s  ", header[j], rows[i].fields[j]);
                printf("\n");
                matchCount++;
            }
        }

        if (matchCount == 0)
            printf("No results found for '%s' in column '%s'\n", searchArg, columnArg);
    }

    free(rows);
    return 0;
}