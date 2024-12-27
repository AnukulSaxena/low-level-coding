// Forward declarations for JsonArray and JsonObject
struct JsonArray; // Forward declaration of JsonArray
struct JsonObject; // Forward declaration of JsonObject

// 1. Enum for JSON data types
typedef enum {
    JSON_NULL,    // Represents null
    JSON_STRING,  // Represents a string (e.g., "hello")
    JSON_NUMBER,  // Represents a number (e.g., 42, 3.14)
    JSON_BOOLEAN, // Represents true or false
    JSON_ARRAY,   // Represents an array (e.g., [1, 2, "hello"])
    JSON_OBJECT   // Represents an object (e.g., {"key": "value"})
} JsonType;

// 2. Union for JSON value
typedef union {
    char *stringValue;                // String data (e.g., "hello")
    double numberValue;               // Numeric data (e.g., 3.14)
    int booleanValue;                 // Boolean data (0 for false, 1 for true)
    struct JsonArray *arrayValue;     // Pointer to an array structure
    struct JsonObject *objectValue;   // Pointer to an object structure
} JsonValue;

// 3. Struct for a JSON element
typedef struct {
    JsonType type;   // Type of the JSON element
    JsonValue value; // Value of the JSON element
} JsonElement;

// 4. Struct for a JSON array
typedef struct JsonArray {
    JsonElement *elements; // Dynamic array of JSON elements
    int length;            // Number of elements in the array
} JsonArray;

// 5. Struct for a JSON object
typedef struct JsonObject {
    char **keys;           // Array of keys (strings)
    JsonElement *values;   // Array of corresponding values
    int length;            // Number of key-value pairs
} JsonObject;
