#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <jansson.h>

void createDatabase(char *databaseName)
{
    mkdir(databaseName, 0700);
}

void createTable(char *databaseName, char *tableName, char *columns[], char *types[])
{
    char filename[100];
    sprintf(filename, "%s/%s.json", databaseName, tableName);
    json_t *json_arr = json_array();
    json_t *column_info = json_object();
    for (int i = 0; columns[i] != NULL; i++)
    {
        json_object_set_new(column_info, columns[i], json_string(types[i]));
    }
    json_array_append_new(json_arr, column_info);
    json_dump_file(json_arr, filename, 0);
    json_decref(json_arr);
}

void dropDatabase(char *databaseName)
{
    char command[100];
    sprintf(command, "rm -r %s", databaseName);
    system(command);
}

void dropTable(char *databaseName, char *tableName)
{
    char command[100];
    sprintf(command, "rm %s/%s.json", databaseName, tableName);
    system(command);
}

void dropColumn(char *databaseName, char *tableName, char *columnName)
{
    char filename[100];
    sprintf(filename, "%s/%s.json", databaseName, tableName);

    json_error_t error;
    json_t *root = json_load_file(filename, 0, &error);

    if (!root)
    {
        printf("Error reading table: %s\n", error.text);
        return;
    }

    size_t index;
    json_t *value;

    json_array_foreach(root, index, value)
    {
        json_object_del(value, columnName);
    }

    json_dump_file(root, filename, 0);
    json_decref(root);
}

void insertIntoTable(char *databaseName, char *tableName, char *columns[], char *values[])
{
    char filename[100];
    sprintf(filename, "%s/%s.json", databaseName, tableName);
    json_error_t error;
    json_t *root = json_load_file(filename, 0, &error);
    json_t *new_entry = json_object();
    for (int i = 0; values[i] != NULL; i++)
    {
        json_object_set_new(new_entry, columns[i], json_string(values[i]));
    }
    json_array_append_new(root, new_entry);
    json_dump_file(root, filename, 0);
    json_decref(root);
}
void updateTable(char *databaseName, char *tableName, char *columnName, char *value)
{
    char filename[100];
    sprintf(filename, "%s/%s.json", databaseName, tableName);

    json_error_t error;
    json_t *root = json_load_file(filename, 0, &error);

    if (!root)
    {
        printf("Error reading table: %s\n", error.text);
        return;
    }

    size_t index;
    json_t *entry;

    // Mulai dari indeks 1 untuk melewati header
    for (index = 1; index < json_array_size(root); index++)
    {
        entry = json_array_get(root, index);

        json_t *field = json_object_get(entry, columnName);

        if (field)
        {
            json_object_set(entry, columnName, json_string(value));
        }
    }

    json_dump_file(root, filename, 0);
    json_decref(root);
}

void deleteFromTable(char *databaseName, char *tableName)
{
    char filename[100];
    sprintf(filename, "%s/%s.json", databaseName, tableName);
    FILE *file = fopen(filename, "w");
    fprintf(file, "[]");
    fclose(file);
}

void selectFromTable(char *databaseName, char *tableName, char *keys[])
{
    char filename[100];
    sprintf(filename, "%s/%s.json", databaseName, tableName);

    json_error_t error;
    json_t *table = json_load_file(filename, 0, &error);

    if (!table)
    {
        printf("Error reading table: %s\n", error.text);
        return;
    }

    size_t index;
    json_t *value;

    json_array_foreach(table, index, value)
    {
        for (int i = 0; keys[i] != NULL; i++)
        {
            const char *key = keys[i];
            json_t *field = json_object_get(value, key);

            if (json_is_string(field))
            {
                printf("%s: %s\n", key, json_string_value(field));
            }
        }
        printf("\n");
    }

    json_decref(table);
}

void selectFromTableWhere(char *databaseName, char *tableName, char *columnName, char *value)
{
    char filename[100];
    sprintf(filename, "%s/%s.json", databaseName, tableName);

    json_error_t error;
    json_t *root = json_load_file(filename, 0, &error);

    if (!root)
    {
        printf("Error reading table: %s\n", error.text);
        return;
    }

    size_t index;
    json_t *entry;

    json_array_foreach(root, index, entry)
    {
        json_t *field = json_object_get(entry, columnName);

        if (field && json_is_string(field) && strcmp(json_string_value(field), value) == 0)
        {
            json_t *key;
            json_t *value;

            json_object_foreach(entry, key, value)
            {
                if (json_is_string(value))
                {
                    printf("%s: %s\n", json_string_value(key), json_string_value(value));
                }
            }

            printf("\n");
        }
    }

    json_decref(root);
}

int main()
{
    char *columnsForTable1[] = {"kolom1", "kolom2", "kolom3", "kolom4", NULL};
    char *typesForTable1[] = {"string", "int", "string", "int", NULL};
    char *valuesForTable1[] = {"value1", "2", "value3", "4", NULL};

    // Membuat database
    createDatabase("database1");

    // Membuat tabel
    createTable("database1", "table1", columnsForTable1, typesForTable1);

    // Menambahkan data ke tabel
    insertIntoTable("database1", "table1", columnsForTable1, valuesForTable1);

    // // Menghapus kolom dari tabel
    dropColumn("database1", "table1", "kolom3");

    // // Memperbarui nilai dalam tabel
    updateTable("database1", "table1", "kolom2", "5");

    // Menampilkan isi tabel
    char* keys[] = {"kolom1", "kolom2", "kolom4", NULL};
    selectFromTable("database1", "table1", keys);

    // Menampilkan isi tabel dengan kondisi WHERE
    selectFromTableWhere("database1", "table1", "kolom2", "5");

    // // Menghapus isi tabel
    deleteFromTable("database1", "table1");

    // // Menghapus tabel
    dropTable("database1", "table1");

    // // Menghapus database
    dropDatabase("database1");

    return 0;
}
