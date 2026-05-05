#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char** fields;
} Row;

void cleanup(
    Row* rows,
    char** header,
    const int columnCount,
    const int rowCount
)
{
    for (int i = 0; i < columnCount; i++)
    {
        free(header[i]);
    }

    for (int j = 0; j < rowCount; j++)
    {
        for (int i = 0; i < columnCount; i++)
        {
            free(rows[j].fields[i]);
        }

        free(rows[j].fields);
    }

    free(rows);
    free(header);
}

int main(void)
{
    FILE* pF = fopen("student_records.csv", "r");
    if (pF == NULL)
    {
        return 0;
    }

    char buffer[1024];
    int columnCapacity = 10;
    int rowCapacity = 10;
    int columnCount = 0;
    int rowCount = 0;
    bool isFirstLine = true;

    char** header = calloc(columnCapacity, sizeof(char*));

    if (header == NULL)
    {
        perror("Out of memory");
        fclose(pF);
        return 1;
    }

    Row* rows = calloc(rowCapacity, sizeof(Row));
    if (rows == NULL)
    {
        perror("Out of memory");
        fclose(pF);
        free(header);
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), pF) != NULL)
    {
        int colIndex = 0;
        const bool lineComplete = buffer[strcspn(buffer, "\n")] == '\n';
        buffer[strcspn(buffer, "\n")] = 0;
        const char* token = strtok(buffer, ",");

        if (rowCount >= rowCapacity && !isFirstLine)
        {
            rowCapacity *= 2;
            Row* temp = realloc(rows, rowCapacity * sizeof(Row));
            if (temp == NULL)
            {
                perror("Out of memory");
                cleanup(rows, header, columnCount, rowCount);
                fclose(pF);
                return 1;
            }

            rows = temp;
        }

        if (!isFirstLine)
        {
            char** fields = calloc(columnCount, sizeof(char*));
            if (fields == NULL)
            {
                perror("Out of memory");
                cleanup(rows, header, columnCount, rowCount);
                fclose(pF);
                return 1;
            }

            rows[rowCount].fields = fields;
        }

        while (token != NULL)
        {
            if (isFirstLine)
            {
                // check if space to allocate more
                if (columnCount >= columnCapacity)
                {
                    columnCapacity *= 2;
                    char** temp = realloc(header, columnCapacity * sizeof(char*));
                    if (temp == NULL)
                    {
                        perror("Out of memory");
                        cleanup(rows, header, columnCount, rowCount);
                        fclose(pF);
                        return 1;
                    }

                    header = temp;
                }

                header[colIndex] = strdup(token);
                columnCount++;
            }
            else
            {
                rows[rowCount].fields[colIndex] = strdup(token);
            }

            colIndex++;
            token = strtok(NULL, ",");
        }


        if (isFirstLine && lineComplete)
        {
            isFirstLine = false;
        }
        else if (lineComplete)
        {
            rowCount++;
        }
    }

    fclose(pF);

    for (int i = 0; i < columnCount; i++)
        printf("[%s] ", header[i]);
    printf("\n");

    for (int i = 0; i < rowCount; i++)
    {
        for (int j = 0; j < columnCount; j++)
            printf("%s: %s  ", header[j], rows[i].fields[j]);
        printf("\n");
    }

    cleanup(rows, header, columnCount, rowCount);

    return 0;
}
