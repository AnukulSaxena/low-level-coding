#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>




// JSON value types
typedef enum {
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOLEAN,
    JSON_NULL
} JSONType;

// JSON value structure
typedef struct JSONValue {
    JSONType type;
    union {
        struct JSONObject *object;
        struct JSONArray *array;
        char *string;
        double number;
        int boolean;
    };
} JSONValue;

// Token types
typedef enum {
    TOKEN_LBRACE,    // {
    TOKEN_RBRACE,    // }
    TOKEN_LBRACKET,  // [
    TOKEN_RBRACKET,  // ]
    TOKEN_COLON,     // :
    TOKEN_COMMA,     // ,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,
    TOKEN_EOF
} TokenType;

// Represents a JSON object (key-value pair)
typedef struct JSONObject {
    char *key;               // Key (string)
    JSONValue *value;        // Associated value
    struct JSONObject *next; // Linked list for multiple key-value pairs
} JSONObject;

// Represents a JSON array (list of values)
typedef struct JSONArray {
    JSONValue *value;        // Array element
    struct JSONArray *next;  // Linked list for multiple elements
} JSONArray;

// Token structure
typedef struct {
    TokenType type;
    const char *start;
    size_t length;
} Token;

// Error structure
typedef struct {
    const char *message;
    size_t position;
} JSONError;

// Global error variable
JSONError json_error = {NULL, 0};

// Function prototypes
JSONValue *parse_json(const char *json);
void free_json(JSONValue *value);


typedef struct {
    const char *json;
    size_t position;
} JSONTokenizer;

Token next_token(JSONTokenizer *tokenizer) {
    const char *json = tokenizer->json;
    size_t pos = tokenizer->position;

    // Skip whitespace
    while (isspace(json[pos])) pos++;

    Token token = {TOKEN_EOF, &json[pos], 1};

    switch (json[pos]) {
        case '{': token.type = TOKEN_LBRACE; break;
        case '}': token.type = TOKEN_RBRACE; break;
        case '[': token.type = TOKEN_LBRACKET; break;
        case ']': token.type = TOKEN_RBRACKET; break;
        case ':': token.type = TOKEN_COLON; break;
        case ',': token.type = TOKEN_COMMA; break;
        case '"': // Handle string token
            token.type = TOKEN_STRING;
            token.start = &json[pos + 1]; // Skip opening quote
            pos++;
            while (json[pos] != '"' && json[pos] != '\0') pos++;
            if (json[pos] == '\0') {
                json_error.message = "Unterminated string";
                json_error.position = pos;
                token.type = TOKEN_EOF;
            }
            token.length = &json[pos] - token.start;
            pos++; // Skip closing quote
            break;
        case '\0': // End of input
            token.type = TOKEN_EOF;
            break;
        default:
            if (isdigit(json[pos]) || json[pos] == '-') {
                // Handle number token
                token.type = TOKEN_NUMBER;
                token.start = &json[pos];
                while (isdigit(json[pos]) || json[pos] == '.' || json[pos] == 'e' || json[pos] == 'E' || json[pos] == '+' || json[pos] == '-') {
                    pos++;
                }
                token.length = &json[pos] - token.start;
            } else if (strncmp(&json[pos], "true", 4) == 0) {
                token.type = TOKEN_TRUE;
                token.length = 4;
                pos += 4;
            } else if (strncmp(&json[pos], "false", 5) == 0) {
                token.type = TOKEN_FALSE;
                token.length = 5;
                pos += 5;
            } else if (strncmp(&json[pos], "null", 4) == 0) {
                token.type = TOKEN_NULL;
                token.length = 4;
                pos += 4;
            } else {
                json_error.message = "Unexpected character";
                json_error.position = pos;
                token.type = TOKEN_EOF;
            }
            break;
    }

    tokenizer->position = pos;
    return token;
}


JSONValue *parse_value(JSONTokenizer *tokenizer);

JSONValue *parse_object(JSONTokenizer *tokenizer) {
    Token token = next_token(tokenizer);
    if (token.type != TOKEN_LBRACE) return NULL;

    // TODO: Parse key-value pairs

    token = next_token(tokenizer);
    if (token.type != TOKEN_RBRACE) {
        json_error.message = "Expected closing brace";
        json_error.position = tokenizer->position;
        return NULL;
    }

    JSONValue *object = malloc(sizeof(JSONValue));
    object->type = JSON_OBJECT;
    object->object = NULL; // Placeholder
    return object;
}

JSONValue *parse_array(JSONTokenizer *tokenizer) {
    Token token = next_token(tokenizer);
    if (token.type != TOKEN_LBRACKET) return NULL;

    // TODO: Parse array elements

    token = next_token(tokenizer);
    if (token.type != TOKEN_RBRACKET) {
        json_error.message = "Expected closing bracket";
        json_error.position = tokenizer->position;
        return NULL;
    }

    JSONValue *array = malloc(sizeof(JSONValue));
    array->type = JSON_ARRAY;
    array->array = NULL; // Placeholder
    return array;
}

JSONValue *parse_value(JSONTokenizer *tokenizer) {
    Token token = next_token(tokenizer);

    switch (token.type) {
        case TOKEN_LBRACE: return parse_object(tokenizer);
        case TOKEN_LBRACKET: return parse_array(tokenizer);
        case TOKEN_STRING:
            // TODO: Handle string
            break;
        case TOKEN_NUMBER:
            // TODO: Handle number
            break;
        case TOKEN_TRUE:
        case TOKEN_FALSE:
        case TOKEN_NULL:
            // TODO: Handle boolean/null
            break;
        default:
            json_error.message = "Unexpected token";
            json_error.position = tokenizer->position;
            return NULL;
    }

    return NULL;
}

JSONValue *parse_json(const char *json) {
    JSONTokenizer tokenizer = {json, 0};
    return parse_value(&tokenizer);
}

void free_json(JSONValue *value) {
    if (!value) return;

    switch (value->type) {
        case JSON_STRING:
            free(value->string);
            break;
        case JSON_OBJECT: {
            struct JSONObject *current = value->object;
            while (current) {
                struct JSONObject *next = current->next;
                free(current->key);
                free_json(current->value); // Free the value recursively
                free(current);
                current = next;
            }
            break;
        }
        case JSON_ARRAY: {
            struct JSONArray *current = value->array;
            while (current) {
                struct JSONArray *next = current->next;
                free_json(current->value); // Free the value recursively
                free(current);
                current = next;
            }
            break;
        }
        default:
            // For numbers, booleans, and null, no additional memory allocation is needed.
            break;
    }

    free(value);
}


int main() {
    const char *json = "{\"key\": [1, 2, 3]}";
    JSONValue *value = parse_json(json);

    if (value) {
        printf("Parsed JSON successfully!\n");
        free_json(value);
    } else {
        printf("Error: %s at position %zu\n", json_error.message, json_error.position);
    }

    return 0;
}

