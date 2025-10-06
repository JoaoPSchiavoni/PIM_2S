#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libs/cjson/cJSON.h"

// Forward declarations for functions in Main.c
char* read_file(const char* filename);
char* process_student_data(const char* json_string);

// --- Test Helper Functions ---

void create_test_file(const char* filename, const char* content) {
    FILE* f = fopen(filename, "w");
    assert(f != NULL);
    fputs(content, f);
    fclose(f);
}

// --- Test Cases for read_file ---

void test_read_file_exists() {
    printf("Running test: test_read_file_exists\n");
    const char* filename = "test_file_1.txt";
    const char* content = "Hello, this is a test file.";
    create_test_file(filename, content);

    char* data = read_file(filename);
    assert(data != NULL);
    assert(strcmp(data, content) == 0);

    free(data);
    remove(filename);
    printf("PASS\n\n");
}

void test_read_file_not_exists() {
    printf("Running test: test_read_file_not_exists\n");
    const char* filename = "non_existent_file.txt";
    char* data = read_file(filename);
    assert(data == NULL);
    printf("PASS\n\n");
}

void test_read_file_empty() {
    printf("Running test: test_read_file_empty\n");
    const char* filename = "empty_file.txt";
    create_test_file(filename, "");

    char* data = read_file(filename);
    assert(data != NULL);
    assert(strcmp(data, "") == 0);

    free(data);
    remove(filename);
    printf("PASS\n\n");
}

// --- Test Cases for process_student_data ---

void test_process_valid_student_data() {
    printf("Running test: test_process_valid_student_data\n");
    const char* json_input =
        "["
        "  {"
        "    \"name\": \"John Doe\","
        "    \"RA\": \"123456\","
        "    \"grades\": {"
        "      \"Math\": [8.0, 7.5],"
        "      \"History\": [9.0, 9.5]"
        "    }"
        "  }"
        "]";

    const char* expected_output =
        "--------------------------------------------------\n"
        "Name: John Doe\n"
        "RA: 123456\n"
        "Math: 8.0 7.5 -> Average: 7.75 | Status: Approved\n"
        "History: 9.0 9.5 -> Average: 9.25 | Status: Approved\n"
        "\nOverall Average: 8.50 | Final Status: Approved\n"
        "--------------------------------------------------\n\n";

    char* result = process_student_data(json_input);
    assert(result != NULL);
    assert(strcmp(result, expected_output) == 0);
    free(result);
    printf("PASS\n\n");
}

void test_process_invalid_json() {
    printf("Running test: test_process_invalid_json\n");
    const char* json_input = "[{\"name\": \"John Doe\",}]"; // Invalid JSON
    char* result = process_student_data(json_input);
    assert(result != NULL);
    // Check for error message
    assert(strstr(result, "Error parsing JSON") != NULL);
    free(result);
    printf("PASS\n\n");
}

void test_process_null_input() {
    printf("Running test: test_process_null_input\n");
    char* result = process_student_data(NULL);
    assert(result == NULL);
    printf("PASS\n\n");
}

// --- Test Runner ---

int main() {
    printf("--- Starting Unit Tests ---\n\n");

    // Tests for read_file
    test_read_file_exists();
    test_read_file_not_exists();
    test_read_file_empty();

    // Tests for process_student_data
    test_process_valid_student_data();
    test_process_invalid_json();
    test_process_null_input();

    printf("--- All tests passed successfully! ---\n");
    return 0;
}