#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Represents a JSON object (key-value pair)
typedef struct JSONObject {
    char *key;               // Key (string)
    struct JSONValue *value; // Associated value
    struct JSONObject *next; // Linked list for multiple key-value pairs
} JSONObject;

// Represents a JSON array (list of values)
typedef struct JSONArray {
    struct JSONValue *value; // Array element
    struct JSONArray *next;  // Linked list for multiple elements
} JSONArray;

// // JSON value types
// typedef enum {
//     JSON_OBJECT,
//     JSON_ARRAY,
//     JSON_STRING,
//     JSON_NUMBER,
//     JSON_BOOLEAN,
//     JSON_NULL
// } JSONType;

// // JSON value structure
// typedef struct JSONValue {
//     JSONType type;
//     union {
//         struct JSONObject *object;
//         struct JSONArray *array;
//         char *string;
//         double number;
//         int boolean;
//     };
// } JSONValue;

// // Token types
// typedef enum {
//     TOKEN_LBRACE,    // {
//     TOKEN_RBRACE,    // }
//     TOKEN_LBRACKET,  // [
//     TOKEN_RBRACKET,  // ]
//     TOKEN_COLON,     // :
//     TOKEN_COMMA,     // ,
//     TOKEN_STRING,
//     TOKEN_NUMBER,
//     TOKEN_TRUE,
//     TOKEN_FALSE,
//     TOKEN_NULL,
//     TOKEN_EOF
// } TokenType;

// // Token structure
// typedef struct {
//     TokenType type;
//     const char *start;
//     size_t length;
// } Token;

// // Error structure
// typedef struct {
//     const char *message;
//     size_t position;
// } JSONError;

// // Global error variable
// JSONError json_error = {NULL, 0};

// // Function prototypes
// JSONValue *parse_json(const char *json);
// void free_json(JSONValue *value);

// // Tokenizer structure
// typedef struct {
//     const char *json;
//     size_t position;
// } JSONTokenizer;

// // Tokenizer: Fetch next token
// Token next_token(JSONTokenizer *tokenizer) {
//     const char *json = tokenizer->json;
//     size_t pos = tokenizer->position;

//     // Skip whitespace
//     while (isspace(json[pos])) pos++;

//     Token token = {TOKEN_EOF, &json[pos], 1};

//     switch (json[pos]) {
//         case '{': token.type = TOKEN_LBRACE; break;
//         case '}': token.type = TOKEN_RBRACE; break;
//         case '[': token.type = TOKEN_LBRACKET; break;
//         case ']': token.type = TOKEN_RBRACKET; break;
//         case ':': token.type = TOKEN_COLON; break;
//         case ',': token.type = TOKEN_COMMA; break;
//         case '"': // Handle string token
//             token.type = TOKEN_STRING;
//             token.start = &json[pos + 1]; // Skip opening quote
//             pos++;
//             while (json[pos] != '"' && json[pos] != '\0') pos++;
//             if (json[pos] == '\0') {
//                 json_error.message = "Unterminated string";
//                 json_error.position = pos;
//                 token.type = TOKEN_EOF;
//             }
//             token.length = &json[pos] - token.start;
//             pos++; // Skip closing quote
//             break;
//         case '\0': // End of input
//             token.type = TOKEN_EOF;
//             break;
//         default:
//             if (isdigit(json[pos]) || json[pos] == '-') {
//                 // Handle number token
//                 token.type = TOKEN_NUMBER;
//                 token.start = &json[pos];
//                 while (isdigit(json[pos]) || json[pos] == '.' || json[pos] == 'e' || json[pos] == 'E' || json[pos] == '+' || json[pos] == '-') {
//                     pos++;
//                 }
//                 token.length = &json[pos] - token.start;
//             } else if (strncmp(&json[pos], "true", 4) == 0) {
//                 token.type = TOKEN_TRUE;
//                 token.length = 4;
//                 pos += 4;
//             } else if (strncmp(&json[pos], "false", 5) == 0) {
//                 token.type = TOKEN_FALSE;
//                 token.length = 5;
//                 pos += 5;
//             } else if (strncmp(&json[pos], "null", 4) == 0) {
//                 token.type = TOKEN_NULL;
//                 token.length = 4;
//                 pos += 4;
//             } else {
//                 json_error.message = "Unexpected character";
//                 json_error.position = pos;
//                 token.type = TOKEN_EOF;
//             }
//             break;
//     }

//     tokenizer->position = pos;
//     return token;
// }

// // Parse a string value
// char *parse_string(const Token *token) {
//     char *string = malloc(token->length + 1);
//     if (string) {
//         strncpy(string, token->start, token->length);
//         string[token->length] = '\0';
//     }
//     return string;
// }

// // Parse a JSON object
// JSONValue *parse_object(JSONTokenizer *tokenizer) {
//     Token token = next_token(tokenizer);
//     if (token.type != TOKEN_LBRACE) return NULL;

//     JSONObject *head = NULL, *tail = NULL;

//     token = next_token(tokenizer);
//     while (token.type != TOKEN_RBRACE && token.type != TOKEN_EOF) {
//         if (token.type != TOKEN_STRING) {
//             json_error.message = "Expected string key";
//             json_error.position = tokenizer->position;
//             return NULL;
//         }

//         // Parse key
//         char *key = parse_string(&token);

//         // Expect colon
//         token = next_token(tokenizer);
//         if (token.type != TOKEN_COLON) {
//             json_error.message = "Expected colon";
//             json_error.position = tokenizer->position;
//             free(key);
//             return NULL;
//         }

//         // Parse value
//         JSONValue *value = parse_value(tokenizer);
//         if (!value) {
//             free(key);
//             return NULL;
//         }

//         // Create object node
//         JSONObject *node = malloc(sizeof(JSONObject));
//         node->key = key;
//         node->value = value;
//         node->next = NULL;

//         if (!head) head = tail = node;
//         else {
//             tail->next = node;
//             tail = node;
//         }

//         // Check for comma or end
//         token = next_token(tokenizer);
//         if (token.type == TOKEN_COMMA) token = next_token(tokenizer);
//     }

//     if (token.type != TOKEN_RBRACE) {
//         json_error.message = "Expected closing brace";
//         json_error.position = tokenizer->position;
//         return NULL;
//     }

//     JSONValue *object = malloc(sizeof(JSONValue));
//     object->type = JSON_OBJECT;
//     object->object = head;
//     return object;
// }

// // Parse a JSON array
// JSONValue *parse_array(JSONTokenizer *tokenizer) {
//     Token token = next_token(tokenizer);
//     if (token.type != TOKEN_LBRACKET) return NULL;

//     JSONArray *head = NULL, *tail = NULL;

//     token = next_token(tokenizer);
//     while (token.type != TOKEN_RBRACKET && token.type != TOKEN_EOF) {
//         JSONValue *value = parse_value(tokenizer);
//         if (!value) return NULL;

//         // Create array node
//         JSONArray *node = malloc(sizeof(JSONArray));
//         node->value = value;
//         node->next = NULL;

//         if (!head) head = tail = node;
//         else {
//             tail->next = node;
//             tail = node;
//         }

//         // Check for comma or end
//         token = next_token(tokenizer);
//         if (token.type == TOKEN_COMMA) token = next_token(tokenizer);
//     }

//     if (token.type != TOKEN_RBRACKET) {
//         json_error.message = "Expected closing bracket";
//         json_error.position = tokenizer->position;
//         return NULL;
//     }

//     JSONValue *array = malloc(sizeof(JSONValue));
//     array->type = JSON_ARRAY;
//     array->array = head;
//     return array;
// }

// // Parse a generic value
// JSONValue *parse_value(JSONTokenizer *tokenizer) {
//     Token token = next_token(tokenizer);

//     switch (token.type) {
//         case TOKEN_LBRACE: return parse_object(tokenizer);
//         case TOKEN_LBRACKET: return parse_array(tokenizer);
//         case TOKEN_STRING: {
//             JSONValue *string = malloc(sizeof(JSONValue));
//             string->type = JSON_STRING;
//             string->string = parse_string(&token);
//             return string;
//         }
//         case TOKEN_NUMBER: {
//             JSONValue *number = malloc(sizeof(JSONValue));
//             number->type = JSON_NUMBER;
//             number->number = strtod(token.start, NULL);
//             return number;
//         }
//         case TOKEN_TRUE: {
//             JSONValue *boolean = malloc(sizeof(JSONValue));
//             boolean->type = JSON_BOOLEAN;
//             boolean->boolean = 1;
//             return boolean;
//         }
//         case TOKEN_FALSE: {
//             JSONValue *boolean = malloc(sizeof(JSONValue));
//             boolean->type = JSON_BOOLEAN;
//             boolean->boolean = 0;
//             return boolean;
//         }
//         case TOKEN_NULL: {
//             JSONValue *null_value = malloc(sizeof(JSONValue));
//             null_value->type = JSON_NULL;
//             return null_value;
//         }
//         default:
//             json_error.message = "Unexpected token";
//             json_error.position = tokenizer->position;
//             return NULL;
//     }
// }

// // Free JSON structure
// void free_json(JSONValue *value) {
//     if (!value) return;

//     switch (value->type) {
//         case JSON_STRING:
//             free(value->string);
//             break;
//         case JSON_OBJECT: {
//             JSONObject *current = value->object;
//             while (current) {
//                 JSONObject *next = current->next;
//                 free(current->key);
//                 free_json(current->value);
//                 free(current);
//                 current = next;
//             }
//             break;
//         }
//         case JSON_ARRAY: {
//             JSONArray *current = value->array;
//             while (current) {
//                 JSONArray *next = current->next;
//                 free_json(current->value);
//                 free(current);
//                 current = next;
//             }
//             break;
//         }
//         default:
//             break;
//     }

//     free(value);
// }

// // Main function to parse JSON
// JSONValue *parse_json(const char *json) {
//     JSONTokenizer tokenizer = {json, 0};
//     return parse_value(&tokenizer);
// }

// Example usage
int main() {
    const char *json = "{\"key\": \"value\", \"list\": [1, 2, 3]}";
printf("json: %s\n", json);
    // JSONValue *parsed = parse_json(json);

    // if (json_error.message) {
    //     printf("Error: %s at position %zu\n", json_error.message, json_error.position);
    // } else {
    //     printf("JSON parsed successfully!\n");
    // }

    // free_json(parsed);
    return 0;
}
