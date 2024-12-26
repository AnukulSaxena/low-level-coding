#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

// Function to trim whitespace
char *trim_whitespace(char *str)
{
    while (isspace((unsigned char)*str))
        str++;
    if (*str == 0)
        return str; // All spaces
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;
    end[1] = '\0';
    return str;
}

// Function to find a value by key in a simple JSON string
char *find_json_value(const char *json, const char *key, char *value, int value_size)
{
    char search_key[BUFFER_SIZE];
    snprintf(search_key, sizeof(search_key), "\"%s\"", key);

    // Locate the key in the JSON string
    char *key_pos = strstr(json, search_key);
    if (!key_pos)
    {
        printf("Key \"%s\" not found in JSON.\n", key);
        return NULL;
    }

    // Move past the key to find the colon
    char *colon_pos = strchr(key_pos, ':');
    if (!colon_pos)
    {
        printf("Malformed JSON (missing colon after key).\n");
        return NULL;
    }

    // Move past the colon and trim whitespace
    char *value_start = trim_whitespace(colon_pos + 1);

    // Determine the value type
    if (*value_start == '"')
    {
        // String value
        value_start++; // Skip the opening quote
        char *end_quote = strchr(value_start, '"');
        if (!end_quote)
        {
            printf("Malformed JSON (missing closing quote for string).\n");
            return NULL;
        }
        int length = end_quote - value_start;
        if (length >= value_size)
        {
            printf("Value buffer too small.\n");
            return NULL;
        }
        strncpy(value, value_start, length);
        value[length] = '\0';
    }
    else if (isdigit(*value_start) || *value_start == '-')
    {
        // Numeric value
        char *end_ptr;
        strncpy(value, value_start, value_size - 1);
        value[value_size - 1] = '\0';
        char *end = value;
        while (*end && (isdigit(*end) || *end == '.' || *end == 'e' || *end == 'E' || *end == '-'))
            end++;
        *end = '\0'; // Null-terminate
    }
    else if (strncmp(value_start, "true", 4) == 0 || strncmp(value_start, "false", 5) == 0)
    {
        // Boolean value
        if (strncmp(value_start, "true", 4) == 0)
            strncpy(value, "true", value_size);
        else
            strncpy(value, "false", value_size);
    }
    else
    {
        printf("Unsupported value type in JSON.\n");
        return NULL;
    }

    return value;
}

int main()
{
    // Example JSON string
    const char *json = "{\"name\": \"John\", \"age\": 30, \"isStudent\": false}";

    char value[BUFFER_SIZE];

    // Find "name"
    if (find_json_value(json, "name", value, sizeof(value)))
        printf("name: %s\n", value);

    // Find "age"
    if (find_json_value(json, "age", value, sizeof(value)))
        printf("age: %s\n", value);

    // Find "isStudent"
    if (find_json_value(json, "isStudent", value, sizeof(value)))
        printf("isStudent: %s\n", value);

    return 0;
}
