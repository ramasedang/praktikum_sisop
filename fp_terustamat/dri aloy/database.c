#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <jansson.h>
#include <unistd.h>
#include <sys/stat.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void createDatabase(const char *databaseName)
{
    char sanitized_name[100];
    size_t len = strlen(databaseName);
    if (len > 0 && databaseName[len-1] == '\n') {
        strncpy(sanitized_name, databaseName, len-1);
        sanitized_name[len-1] = '\0';
    } else {
        strcpy(sanitized_name, databaseName);
    }
    mkdir(sanitized_name, 0700);
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
    sprintf(command, "rm -rf %s", databaseName);
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
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char *hello = "Hello from server";
    char databaseName[100] = "";
    char tableName[100] = "";
    char command[100];

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address and port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to localhost port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Accept incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    printf("New connection accepted\n");

    // Communication loop
    // Communication loop
while (1)
{
    memset(buffer, 0, BUFFER_SIZE); // Clear buffer before receiving data from client
    valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
    if (valread <= 0)
    {
        break;
    }

    printf("Received command: %s\n", buffer);

    // Process the command here and send response
    char *response = "Command received";
    send(new_socket, response, strlen(response), 0);

    // Check for trailing semicolon and remove if present
    int len = strlen(buffer);
    if (buffer[len-1] == ';') {
        buffer[len-1] = '\0';
    }
    char *token = strtok(buffer, " ");

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
                        *commaPos ='\0';
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
            if (token == NULL)
            {
                printf("Please specify a database name.\n");
            }
            else if (strcmp(databaseName, "") != 0 && strcmp(databaseName, token) != 0)
            {
                printf("Error: Database '%s' does not exist or is not active.\n", token);
            }
            else if (access(token, F_OK) != 0)
            {
                printf("Error: Database '%s' does not exist.\n", token);
            }
            else
            {
                strcpy(databaseName, token); // Set active database name
                printf("Database selected: %s\n", databaseName);
            }
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
        memset(buffer, 0, BUFFER_SIZE); // Clear buffer after processing command
    }
    printf("Client disconnected\n");

    close(new_socket);
    close(server_fd);

    return 0;
}