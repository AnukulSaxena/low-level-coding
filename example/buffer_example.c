#include <stdio.h>
#include <stdlib.h>
#include <time.h> // Include the time.h header for clock() function

#define BUFFER_SIZE 8 // Define the size of the buffer

int main() {
    FILE *file = fopen("D:\\c\\example\\example.txt", "r"); // Open a file in read mode
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char buffer[BUFFER_SIZE]; // Define a buffer to store chunks of data
    size_t bytesRead;         // Variable to store the number of bytes read

    // Start measuring time for fread
    clock_t start_time_fread = clock();

    // Step 1: Read the file in chunks using the buffer
    printf("Reading file in chunks of %d bytes:\n", BUFFER_SIZE);
    while ((bytesRead = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
        printf("Read %zu bytes: ", bytesRead);
        buffer[bytesRead] = '\0'; // Null-terminate the buffer to make it a valid string
        printf("%s -\n", buffer); // Print the chunk (process it in your application)
    }

    // Measure the time after fread
    clock_t end_time_fread = clock();
    double time_taken_fread = ((double)end_time_fread - start_time_fread) / CLOCKS_PER_SEC;
    printf("CLOCKS_PER_SEC: %ld\n", CLOCKS_PER_SEC);
    printf("\nTime taken to read in chunks: %f seconds\n", time_taken_fread);

    // Step 2: Reset the file pointer to the beginning to read character-by-character
    fseek(file, 0, SEEK_SET);  // Move file pointer back to the start of the file

    // Start measuring time for fgetc
    clock_t start_time_fgetc = clock();

    printf("\nReading the file character by character:\n");
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch); // Print each character
    }

    // Measure the time after fgetc
    clock_t end_time_fgetc = clock();
    double time_taken_fgetc = ((double)end_time_fgetc - start_time_fgetc) / CLOCKS_PER_SEC;
    printf("\nTime taken to read character by character: %f seconds\n", time_taken_fgetc);

    printf("\nDone reading the file.\n");

    fclose(file); // Close the file
    return 0;
}
