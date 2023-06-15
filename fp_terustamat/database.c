    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/stat.h>
    #include <jansson.h>

    void createDatabase(const char *databaseName)
    {
        mkdir(databaseName, 0700);
    }

    void createTable(const char *databaseName, const char *tableName, char *columns[], char *types[])
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

    void dropDatabase(const char *databaseName)
    {
        char command[100];
        sprintf(command, "rm -r %s", databaseName);
        system(command);
    }

    void dropTable(const char *databaseName, const char *tableName)
    {
        char filename[100];
        sprintf(filename, "%s/%s.json", databaseName, tableName);
        remove(filename);
    }

    void dropColumn(const char *databaseName, const char *tableName, const char *columnName)
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

    void insertIntoTable(const char *databaseName, const char *tableName, char *columns[], char *values[])
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

        json_t *new_entry = json_object();
        for (int i = 0; columns[i] != NULL && values[i] != NULL; i++)
        {
            json_object_set_new(new_entry, columns[i], json_string(values[i]));
        }
        json_array_append_new(root, new_entry);
        json_dump_file(root, filename, 0);
        json_decref(root);
    }

    void updateTable(const char *databaseName, const char *tableName, const char *columnName, const char *value)
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

        // Start from index 1 to skip the header
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

    void deleteFromTable(const char *databaseName, const char *tableName)
    {
        char filename[100];
        sprintf(filename, "%s/%s.json", databaseName, tableName);
        FILE *file = fopen(filename, "w");
        fprintf(file, "[]");
        fclose(file);
    }

    void selectFromTable(const char *databaseName, const char *tableName, char *keys[])
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

    // void selectFromTableWhere(const char *databaseName, const char *tableName, const char *columnName, const char *value)
    // {
    //     char filename[100];
    //     sprintf(filename, "%s/%s.json", databaseName, tableName);

    //     json_error_t error;
    //     json_t *root = json_load_file(filename, 0, &error);

    //     if (!root)
    //     {
    //         printf("Error reading table: %s\n", error.text);
    //         return;
    //     }

    //     size_t index;
    //     json_t *entry;

    //     json_array_foreach(root, index, entry)
    //     {
    //         json_t *field = json_object_get(entry, columnName);

    //         if (field && json_is_string(field) && strcmp(json_string_value(field), value) == 0)
    //         {
    //             json_t *key;
    //             json_t *value;

    //             json_object_foreach(entry, key, value)
    //             {
    //                 if (json_is_string(value))
    //                 {
    //                     printf("%s: %s\n", json_string_value(key), json_string_value(value));
    //                 }
    //             }

    //             printf("\n");
    //         }
    //     }

    //     json_decref(root);
    // }

    void extractColumnNames(const char *databaseName, const char *tableName, char *columns[])
    {
        char filename[100];
        sprintf(filename, "%s/%s.json", databaseName, tableName);
        json_error_t error;

        // Open the JSON file and read its contents
        json_t *root = json_load_file(filename, 0, &error);
        if (!root)
        {
            printf("Error reading table: %s\n", error.text);
            return;
        }

        // Get the first object in the array (which contains column definitions)
        json_t *column_info = json_array_get(root, 0);
        if (!json_is_object(column_info))
        {
            printf("Error reading column info\n");
            json_decref(root);
            return;
        }

        // Copy column names to the output array
        const char *key;
        json_t *value;
        int i = 0;
        json_object_foreach(column_info, key, value)
        {
            columns[i] = strdup(key);
            i++;
        }
        columns[i] = NULL; // Mark the end of the array with NULL

        // Clean up
        json_decref(root);
    }

    int main()
    {
        char command[100];
        char databaseName[100] = "";
        char tableName[100] = "";
        while (1)
        {
            printf(">> ");
            fgets(command, sizeof(command), stdin);
            command[strcspn(command, "\n")] = '\0';
            char *token = strtok(command, " ");

            if (strcmp(token, "CREATE") == 0)
            {
                token = strtok(NULL, " ");

                if (strcmp(token, "DATABASE") == 0)
                {
                    token = strtok(NULL, " ");
                    token[strcspn(token, ";")] = '\0';
                    createDatabase(token);
                    printf("Database created: %s\n", token);
                    strcpy(databaseName, token);
                }
                else if (strcmp(token, "TABLE") == 0)
                {
                    if (strlen(databaseName) == 0)
                    {
                        printf("No active database. Please create or select a database first.\n");
                    }
                    else
                    {
                        token = strtok(NULL, " "); // Extract table name
                        strcpy(tableName, token);

                        char *columns[100];
                        char *types[100];
                        int i = 0;

                        token = strtok(NULL, " "); // Go to columns
                        while (token != NULL)
                        {
                            if (strcmp(token, ",") == 0) // skip ',' symbols
                            {
                                token = strtok(NULL, " ");
                                continue;
                            }

                            if (token[0] == '(') // removing '(' from column name
                            {
                                memmove(token, token + 1, strlen(token));
                            }

                            char *commaPos = strchr(token, ',');
                            if (commaPos != NULL) // removing ',' from type
                            {
                                *commaPos = '\0';
                            }

                            columns[i] = token; // Storing column name

                            token = strtok(NULL, " ");           // Extract column type
                            if (token[strlen(token) - 1] == ')') // removing ')' from type
                            {
                                token[strlen(token) - 1] = '\0';
                            }
                            types[i] = token;
                            i++;

                            token = strtok(NULL, " "); // Go to next column or NULL
                        }

                        columns[i] = NULL; // Null-terminate column and type arrays
                        types[i] = NULL;

                        createTable(databaseName, tableName, columns, types);
                        printf("Table '%s' created in database '%s'.\n", tableName, databaseName);
                    }
                }
            }
            else if (strcmp(token, "USE") == 0)
            {
                token = strtok(NULL, " ");
                strcpy(databaseName, token); // Set active database name
                printf("Database selected: %s\n", databaseName);
            }
            else if (strcmp(token, "DROP") == 0)
            {
                token = strtok(NULL, " ");

                if (strcmp(token, "DATABASE") == 0)
                {
                    token = strtok(NULL, " ");
                    token[strcspn(token, ";")] = '\0';
                    dropDatabase(token);
                    printf("Database dropped: %s\n", token);

                    // If the dropped database is the active one, clear the databaseName variable
                    if (strcmp(databaseName, token) == 0)
                    {
                        strcpy(databaseName, "");
                    }
                }
                else if (strcmp(token, "TABLE") == 0)
                {
                    if (strlen(databaseName) == 0)
                    {
                        printf("No active database. Please create or select a database first.\n");
                    }
                    else
                    {
                        token = strtok(NULL, " ");
                        token[strcspn(token, ";")] = '\0';
                        dropTable(databaseName, token);
                        printf("Table '%s' dropped from database '%s'.\n", token, databaseName);
                    }
                }
                else if (strcmp(token, "COLUMN") == 0)
                {
                    if (strlen(databaseName) == 0)
                    {
                        printf("No active database. Please create or select a database first.\n");
                    }
                    else
                    {
                        char columnName[100];
                        strcpy(columnName, strtok(NULL, " ")); // Extract column name

                        // Check the next token is "FROM"
                        token = strtok(NULL, " ");
                        if (strcmp(token, "FROM") != 0)
                        {
                            printf("Syntax error. Expected 'FROM', got '%s'.\n", token);
                            continue;
                        }

                        token = strtok(NULL, " ");
                        token[strcspn(token, ";")] = '\0';
                        dropColumn(databaseName, token, columnName);
                        printf("Column '%s' dropped from table '%s' in database '%s'.\n", columnName, token, databaseName);
                    }
                }
            }
            else if (strcmp(token, "INSERT") == 0)
            {
                token = strtok(NULL, " ");

                if (strcmp(token, "INTO") == 0)
                {
                    if (strlen(databaseName) == 0)
                    {
                        printf("No active database. Please create or select a database first.\n");
                    }
                    else
                    {
                        token = strtok(NULL, " "); // Extract table name
                        strcpy(tableName, token);

                        char *columns[100];
                        char *values[100];
                        int i = 0;

                        token = strtok(NULL, " "); // Go to values
                        while (token != NULL)
                        {
                            if (strcmp(token, ",") == 0) // skip ',' symbols
                            {
                                token = strtok(NULL, " ");
                                continue;
                            }

                            if (token[0] == '(') // removing '(' from value
                            {
                                memmove(token, token + 1, strlen(token));
                            }

                            char *commaPos = strchr(token, ',');
                            if (commaPos != NULL) // removing ',' from value
                            {
                                *commaPos = '\0';
                            }

                            char *endPos = strchr(token, ')');
                            if (endPos != NULL) // removing ')' from value
                            {
                                *endPos = '\0';
                            }

                            values[i] = token;
                            i++;

                            token = strtok(NULL, " "); // Go to next value or NULL
                        }

                        values[i] = NULL; // Null-terminate values array

                        // Extract column names from table definition
                        char *columnNames[100];
                        extractColumnNames(databaseName, tableName, columnNames);

                        insertIntoTable(databaseName, tableName, columnNames, values);
                        printf("Row inserted into table '%s' in database '%s'.\n", tableName, databaseName);
                    }
                }
            }
            else if (strcmp(token, "UPDATE") == 0)
            {
                token = strtok(NULL, " ");
                strcpy(tableName, token);
                token = strtok(NULL, " ");
                if (strcmp(token, "SET") == 0)
                {
                    token = strtok(NULL, "=");
                    char columnName[100];
                    strcpy(columnName, token);
                    token = strtok(NULL, " ");
                    char newValue[100];
                    strcpy(newValue, token);
                    updateTable(databaseName, tableName, columnName, newValue);
                    printf("Updated '%s' in table '%s' in database '%s'.\n", columnName, tableName, databaseName);
                }
            }
            else if (strcmp(token, "DELETE") == 0)
            {
                token = strtok(NULL, " ");
                if (strcmp(token, "FROM") == 0)
                {
                    token = strtok(NULL, " ");
                    strcpy(tableName, token);
                    deleteFromTable(databaseName, tableName);
                    printf("Data from table '%s' in database '%s' has been deleted.\n", tableName, databaseName);
                }
            }
            else if (strcmp(token, "SELECT") == 0)
            {
                token = strtok(NULL, " ");
                if (strcmp(token, "*") == 0)
                {
                    token = strtok(NULL, " ");
                    if (strcmp(token, "FROM") == 0)
                    {
                        token = strtok(NULL, " ");
                        strcpy(tableName, token);
                        char *keys[] = {NULL};
                        selectFromTable(databaseName, tableName, keys);
                    }
                }
                else
                {
                    char *columns[100];
                    int i = 0;
                    while (token != NULL && strcmp(token, "FROM") != 0)
                    {
                        columns[i] = token;
                        i++;
                        token = strtok(NULL, ", ");
                    }
                    columns[i] = NULL; // Null-terminate column array
                    if (strcmp(token, "FROM") == 0)
                    {
                        token = strtok(NULL, " ");
                        strcpy(tableName, token);
                        selectFromTable(databaseName, tableName, columns);
                    }
                }
            }
        }
        // char *columnsForTable1[] = {"kolom1", "kolom2", "kolom3", "kolom4", NULL};
        // char *typesForTable1[] = {"string", "int", "string", "int", NULL};
        // char *valuesForTable1[] = {"value1", "2", "value3", "4", NULL};

        // // Membuat database
        // createDatabase("database1");

        // // Membuat tabel
        // createTable("database1", "table1", columnsForTable1, typesForTable1);

        // // Menambahkan data ke tabel
        // insertIntoTable("database1", "table1", columnsForTable1, valuesForTable1);

        // // // Menghapus kolom dari tabel
        // dropColumn("database1", "table1", "kolom3");

        // // // Memperbarui nilai dalam tabel
        // updateTable("database1", "table1", "kolom2", "5");

        // // Menampilkan isi tabel
        // char* keys[] = {"kolom1", "kolom2", "kolom4", NULL};
        // selectFromTable("database1", "table1", keys);

        // // Menampilkan isi tabel dengan kondisi WHERE
        // selectFromTableWhere("database1", "table1", "kolom2", "5");

        // // // Menghapus isi tabel
        // deleteFromTable("database1", "table1");

        // // // Menghapus tabel
        // dropTable("database1", "table1");

        // // // Menghapus database
        // dropDatabase("database1");

        return 0;
    }
