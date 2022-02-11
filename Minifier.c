/*
 * Name: C Minifier
 * Description: A C program to remove comments and minimize the code (remove whitespace and summarize code)
 * Date: 2022-02-11
 * Author: Max Base
 * Repository: https://github.com/BaseMax/C-Minifier
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Read whole file into memory and return char*
char* file_read(char* filepath)
{
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Error: File not found\n");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(sizeof(char) * (size + 1));
    if (buffer == NULL) {
        printf("Error: Memory allocation failed\n");
        exit(1);
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';

    fclose(file);
    return(buffer);
}

// Remove inline and multi-line C-style comments
char* remove_comments(char* buffer)
{
    char* new_buffer = (char*)malloc(sizeof(char) * (strlen(buffer) + 1));
    if (new_buffer == NULL) {
        printf("Error: Memory allocation failed\n");
        exit(1);
    }

    int i = 0;
    int j = 0;
    while (buffer[i] != '\0') {
        if (buffer[i] == '/' && buffer[i + 1] == '/') {
            while (buffer[i] != '\n') {
                i++;
            }
        }
        else if (buffer[i] == '/' && buffer[i + 1] == '*') {
            i += 2;
            while (buffer[i] != '*' || buffer[i + 1] != '/') {
                i++;
            }
            i += 2;
        }
        else {
            new_buffer[j] = buffer[i];
            j++;
            i++;
        }
    }
    new_buffer[j] = '\0';
    return(new_buffer);
}

// Minify C code
// Note: Add newline to `new_code` for MACRO and #include statements
char* minify_c_code(char* code)
{
    char* new_code = (char*)malloc(sizeof(char) * (strlen(code) + 1));
    if (new_code == NULL) {
        printf("Error: Memory allocation failed\n");
        exit(1);
    }

    int i = 0;
    int j = 0;

    while (code[i] != '\0') {
        // Handle identifier
        if (isalpha(code[i]) || isdigit(code[i]) || code[i] == '_') {
            char* temp = (char*)malloc(sizeof(char) * (52)); // 52 is the max length of an identifier
            int temp_i = 0;
            while (isalpha(code[i]) || isdigit(code[i]) || code[i] == '_') {
                temp[temp_i++] = code[i];
                i++;
            }
            temp[temp_i] = '\0';

            // Append temp to `new_code`
            for (int k = 0; k < temp_i; k++) {
                new_code[j] = temp[k];
                j++;
            }

            if(strcmp(temp, "typedef") == 0 || strcmp(temp, "return") == 0 || strcmp(temp, "sizeof") == 0 || strcmp(temp, "int") == 0|| strcmp(temp, "long") == 0 || strcmp(temp, "if") == 0 || strcmp(temp, "else") == 0 || strcmp(temp, "float") == 0 || strcmp(temp, "double") == 0 || strcmp(temp, "char") == 0 || strcmp(temp, "bool") == 0) {
                while (code[i] == ' ' || code[i] == '\t' || code[i] == '\n' || code[i] == '\r')
                    i++;
                if(code[i] != '(')
                    new_code[j++] = ' ';
            } else { // Other identifier or maybe user-defined var, types, etc
                // Skip whitespaces after the identifier
                while (code[i] == ' ' || code[i] == '\t' || code[i] == '\n' || code[i] == '\r') {
                    i++;
                }
                if (isalpha(code[i]) || isdigit(code[i]) || code[i] == '_') {
                    new_code[j++] = ' ';
                    new_code[j++] = code[i];
                    i++;
                }
            }

            // Free temp
            free(temp);
        }
        else if (code[i] == '#') {
            while (code[i] != '\n') {
                new_code[j] = code[i];
                i++;
                j++;
            }
            new_code[j] = '\n';
            j++;
        }
        else if (code[i] == ' ' || code[i] == '\n' || code[i] == '\r' || code[i] == '\t') {
            i++;
        }
        else if (code[i] == '{' || code[i] == '}') {
            new_code[j] = code[i];
            j++;
            i++;
        }
        else if (code[i] == ';') {
            new_code[j] = code[i];
            j++;
            i++;
            while (code[i] == ' ' || code[i] == '\n' || code[i] == '\t') {
                i++;
            }
        }
        else if (code[i] == '(' || code[i] == ')') {
            new_code[j] = code[i];
            j++;
            i++;
        }
        else if (code[i] == ',') {
            new_code[j] = code[i];
            j++;
            i++;
            while (code[i] == ' ' || code[i] == '\n' || code[i] == '\t') {
                i++;
            }
        }
        else {
            if (code[i] == '\'') { // Single quote char
                new_code[j] = code[i];
                j++;
                i++;
                while (code[i] != '\'') { // I am aware C not allow to put more then one char inside single quote
                    new_code[j] = code[i];
                    j++;
                    i++;
                }
                new_code[j] = code[i];
                j++;
                i++;
            }
            if (code[i] == '"') { // Write string double quotes
                new_code[j] = code[i];
                i++;
                j++;

                while (code[i] != '"') {
                    new_code[j] = code[i];
                    j++;
                    i++;
                }
                new_code[j] = code[i];
                j++;
                i++;
            }
            else {
                new_code[j] = code[i];
                j++;
                i++;
            }
        }
    }

    new_code[j] = '\0';

    return(new_code);
}

// Start point
int main(int argc, char** argv)
{
    if(argc == 1) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    char* buffer = file_read(argv[1]);
    buffer = remove_comments(buffer);
    buffer = minify_c_code(buffer);

    if(argc == 3) {
        FILE* file = fopen(argv[2], "w");
        if (file == NULL) {
            printf("Error: Cannot create '%s' file!\n", argv[2]);
            exit(1);
        }
        fprintf(file, "%s", buffer);
        fclose(file);
    }
    else {
        printf("%s", buffer);
    }

    return 0;
}
