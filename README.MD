# csvtool

A command-line CSV parser written in C. Reads a CSV file, stores rows in an array of structs, and lets you query data by column name and search value.

## Build & Usage

```bash
gcc -o csvtool main.c
./csvtool <file.csv> [--column <column>] [--search <value>]
```

## Examples

```bash
./csvtool data.csv                                 # print all rows
./csvtool data.csv --column name                   # print all values in column
./csvtool data.csv --column name --search David    # search for value in column
```

## Arguments

| Argument | Description |
|---|---|
| `<file.csv>` | Path to the CSV file to read |
| `--column <column>` | Column name to filter by |
| `--search <value>` | Value to search for in the specified column |

## Concepts

- Structs and arrays of structs
- File I/O with `fgets`
- String parsing with `strtok`
- CLI argument handling with `argc` and `argv`
- Buffer safety and bounds checking

## Limitations

- Maximum 500 rows, 20 columns, 100 characters per field
- Does not support quoted fields containing commas