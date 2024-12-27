#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// JSON value types
typedef enum {
    JSON_STRING,
    JSON_NUMBER,
    JSON_OBJECT
} JSONType;

// JSON value structure
typedef struct JSONValue {
    JSONType type;
    union {
        char *string;
        double number;
        struct JSONObject *object;
    };
} JSONValue;

// Represents a JSON object (key-value pair)
typedef struct JSONObject {
    char *key;
    JSONValue *value;
    struct JSONObject *next;
} JSONObject;

// Token types
typedef enum {
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_EOF
} TokenType;

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

// JSON tokenizer structure
typedef struct {
    const char *json;
    size_t position;
} JSONTokenizer;

Token next_token(JSONTokenizer *tokenizer);
JSONValue *parse_json(const char *json);
void free_json_value(JSONValue *value);
void free_json_object(JSONObject *object);

// Tokenizer function
Token next_token(JSONTokenizer *tokenizer) {
    const char *json = tokenizer->json;
    size_t pos = tokenizer->position;

    while (isspace(json[pos])) pos++;

    Token token = {TOKEN_EOF, &json[pos], 1};

    switch (json[pos]) {
        case '{': token.type = TOKEN_LBRACE; pos++; break;
        case '}': token.type = TOKEN_RBRACE; pos++; break;
        case ':': token.type = TOKEN_COLON; pos++; break;
        case ',': token.type = TOKEN_COMMA; pos++; break;
        case '"':
            token.type = TOKEN_STRING;
            token.start = &json[pos + 1];
            pos++;
            while (json[pos] != '"' && json[pos] != '\0') pos++;
            if (json[pos] == '\0') {
                json_error.message = "Unterminated string";
                json_error.position = pos;
                token.type = TOKEN_EOF;
            } else {
                token.length = &json[pos] - token.start;
                pos++;
            }
            break;
        default:
            if (isdigit(json[pos]) || json[pos] == '-') {
                token.type = TOKEN_NUMBER;
                token.start = &json[pos];
                while (isdigit(json[pos]) || json[pos] == '.' || json[pos] == 'e' || json[pos] == 'E' || json[pos] == '+' || json[pos] == '-') {
                    pos++;
                }
                token.length = &json[pos] - token.start;
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

// Parse a key-value pair
JSONObject *parse_object(JSONTokenizer *tokenizer) {
    Token token = next_token(tokenizer);
    if (token.type != TOKEN_LBRACE) return NULL;

    JSONObject *head = NULL;
    JSONObject **current = &head;

    while (1) {
        token = next_token(tokenizer);
        if (token.type == TOKEN_RBRACE) break;
        if (token.type != TOKEN_STRING) {
            json_error.message = "Expected string key";
            json_error.position = tokenizer->position;
            free_json_object(head);
            return NULL;
        }

        // Parse key
        char *key = strndup(token.start, token.length);

        token = next_token(tokenizer);
        if (token.type != TOKEN_COLON) {
            json_error.message = "Expected ':' after key";
            json_error.position = tokenizer->position;
            free(key);
            free_json_object(head);
            return NULL;
        }

        // Parse value
        token = next_token(tokenizer);
        JSONValue *value = malloc(sizeof(JSONValue));
        if (token.type == TOKEN_STRING) {
            value->type = JSON_STRING;
            value->string = strndup(token.start, token.length);
        } else if (token.type == TOKEN_NUMBER) {
            value->type = JSON_NUMBER;
            value->number = strtod(token.start, NULL);
        } else {
            json_error.message = "Expected string or number value";
            json_error.position = tokenizer->position;
            free(key);
            free(value);
            free_json_object(head);
            return NULL;
        }

        // Add key-value pair to the object
        *current = malloc(sizeof(JSONObject));
        (*current)->key = key;
        (*current)->value = value;
        (*current)->next = NULL;
        current = &(*current)->next;

        // Check for comma or end of object
        token = next_token(tokenizer);
        if (token.type == TOKEN_RBRACE) break;
        if (token.type != TOKEN_COMMA) {
            json_error.message = "Expected ',' or '}'";
            json_error.position = tokenizer->position;
            free_json_object(head);
            return NULL;
        }
    }

    return head;
}

// Parse JSON
JSONValue *parse_json(const char *json) {
    JSONTokenizer tokenizer = {json, 0};
    JSONObject *object = parse_object(&tokenizer);

    if (!object) return NULL;

    JSONValue *value = malloc(sizeof(JSONValue));
    value->type = JSON_OBJECT;
    value->object = object;
    return value;
}

// Free JSON value
void free_json_value(JSONValue *value) {
    if (!value) return;

    if (value->type == JSON_STRING) {
        free(value->string);
    } else if (value->type == JSON_OBJECT) {
        free_json_object(value->object);
    }

    free(value);
}

// Free JSON object
void free_json_object(JSONObject *object) {
    while (object) {
        JSONObject *next = object->next;
        free(object->key);
        free_json_value(object->value);
        free(object);
        object = next;
    }
}

// Main function
int main() {
    const char *json = "{\"name\": \"Alice\", \"age\": 25, \"city\": \"Wonderland\"}";

    JSONValue *value = parse_json(json);

    if (value) {
        printf("Parsed JSON successfully!\n");

        JSONObject *obj = value->object;
        while (obj) {
            printf("Key: %s, ", obj->key);
            if (obj->value->type == JSON_STRING) {
                printf("Value: %s (String)\n", obj->value->string);
            } else if (obj->value->type == JSON_NUMBER) {
                printf("Value: %.2f (Number)\n", obj->value->number);
            }
            obj = obj->next;
        }

        free_json_value(value);
    } else {
        printf("Error: %s at position %zu\n", json_error.message, json_error.position);
    }

    return 0;
}
