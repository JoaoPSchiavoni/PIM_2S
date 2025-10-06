#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libs/cjson/cJSON.h"



// Function to read entire file into memory
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error: could not open file %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    char* data = (char*)malloc(length + 1);
    if (!data) {
        printf("Memory allocation error.\n");
        fclose(file);
        return NULL;
    }

    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);
    return data;
}

// This function is for demonstration and not easily testable due to stdout.
void view_students_from_file(const char* filename) {
    char* json_data = read_file(filename);
    if (!json_data) return;

    cJSON* root = cJSON_Parse(json_data);
    if (!root) {
        printf("Error parsing JSON.\n");
        free(json_data);
        return;
    }

    cJSON* student = NULL;
    cJSON_ArrayForEach(student, root) { // This iterates through the values of the root object
        printf("--------------------------------------------------\n");

        cJSON* name = cJSON_GetObjectItem(student, "name");
        cJSON* ra = cJSON_GetObjectItem(student, "RA");
        if (cJSON_IsString(name)) {
            printf("Name: %s\n", name->valuestring);
        }
        if (cJSON_IsString(ra)) {
            printf("RA: %s\n", ra->valuestring);
        }

        // Get grades object
        cJSON* grades = cJSON_GetObjectItem(student, "grades");
        if (grades) {
            cJSON* subject = NULL;
            double total_avg = 0;
            int subjects_count = 0;

            cJSON_ArrayForEach(subject, grades) {
                const char* subject_name = subject->string;
                cJSON* grade_array = subject;

                int count = cJSON_GetArraySize(grade_array);
                double sum = 0;

                printf("%s: ", subject_name);

                for (int i = 0; i < count; i++) {
                    cJSON* g = cJSON_GetArrayItem(grade_array, i);
                    if(cJSON_IsNumber(g)) {
                        printf("%.1f ", g->valuedouble);
                        sum += g->valuedouble;
                    }
                }

                if (count > 0) {
                    double avg = sum / count;
                    total_avg += avg;
                    subjects_count++;
                    printf("-> Average: %.2f | Status: %s\n", avg, (avg >= 6.5 ? "Approved" : "Failed"));
                } else {
                    printf("No grades yet.\n");
                }
            }

            if (subjects_count > 0) {
                double overall_avg = total_avg / subjects_count;
                printf("\nOverall Average: %.2f | Final Status: %s\n",
                       overall_avg, (overall_avg >= 6.5 ? "Approved" : "Failed"));
            } else {
                printf("\nNo grades registered yet.\n");
            }
        }

        printf("--------------------------------------------------\n\n");
    }

    cJSON_Delete(root);
    free(json_data);
}

// Testable function for processing student data
char* process_student_data(const char* json_string) {
    if (!json_string) {
        return NULL;
    }

    cJSON* root = cJSON_Parse(json_string);
    if (!root) {
        const char* error_ptr = cJSON_GetErrorPtr();
        char* error_msg = (char*)malloc(100);
        if (error_msg) {
            snprintf(error_msg, 100, "Error parsing JSON near: %s", error_ptr);
        }
        return error_msg;
    }

    // Use a dynamic string to build the output
    char* output = (char*)malloc(4096); // Start with a reasonable buffer
    if (!output) {
        cJSON_Delete(root);
        return NULL;
    }
    output[0] = '\0';
    size_t buffer_size = 4096;
    size_t current_len = 0;

    cJSON* student = NULL;
    cJSON_ArrayForEach(student, root) {
        char student_buffer[1024];
        int written = snprintf(student_buffer, sizeof(student_buffer), "--------------------------------------------------\n");

        cJSON* name = cJSON_GetObjectItem(student, "name");
        cJSON* ra = cJSON_GetObjectItem(student, "RA");
        if (cJSON_IsString(name) && name->valuestring != NULL) {
            written += snprintf(student_buffer + written, sizeof(student_buffer) - written, "Name: %s\n", name->valuestring);
        }
        if (cJSON_IsString(ra) && ra->valuestring != NULL) {
            written += snprintf(student_buffer + written, sizeof(student_buffer) - written, "RA: %s\n", ra->valuestring);
        }

        // Get grades object
        cJSON* grades = cJSON_GetObjectItem(student, "grades");
        if (grades) {
            cJSON* subject = NULL;
            double total_avg = 0;
            int subjects_count = 0;

            cJSON_ArrayForEach(subject, grades) {
                const char* subject_name = subject->string;
                cJSON* grade_array = subject;

                if (cJSON_IsArray(grade_array)) {
                    int count = cJSON_GetArraySize(grade_array);
                    double sum = 0;

                    written += snprintf(student_buffer + written, sizeof(student_buffer) - written, "%s: ", subject_name);

                    for (int i = 0; i < count; i++) {
                        cJSON* g = cJSON_GetArrayItem(grade_array, i);
                        if (cJSON_IsNumber(g)) {
                            written += snprintf(student_buffer + written, sizeof(student_buffer) - written, "%.1f ", g->valuedouble);
                            sum += g->valuedouble;
                        }
                    }

                    if (count > 0) {
                        double avg = sum / count;
                        total_avg += avg;
                        subjects_count++;
                        written += snprintf(student_buffer + written, sizeof(student_buffer) - written, "-> Average: %.2f | Status: %s\n", avg, (avg >= 6.5 ? "Approved" : "Failed"));
                    } else {
                        written += snprintf(student_buffer + written, sizeof(student_buffer) - written, "No grades yet.\n");
                    }
                }
            }

            if (subjects_count > 0) {
                double overall_avg = total_avg / subjects_count;
                written += snprintf(student_buffer + written, sizeof(student_buffer) - written, "\nOverall Average: %.2f | Final Status: %s\n",
                                    overall_avg, (overall_avg >= 6.5 ? "Approved" : "Failed"));
            } else {
                written += snprintf(student_buffer + written, sizeof(student_buffer) - written, "\nNo grades registered yet.\n");
            }
        }

        written += snprintf(student_buffer + written, sizeof(student_buffer) - written, "--------------------------------------------------\n\n");

        if (current_len + written >= buffer_size) {
            buffer_size = (current_len + written) * 2;
            char* new_output = (char*)realloc(output, buffer_size);
            if (!new_output) {
                free(output);
                cJSON_Delete(root);
                return NULL;
            }
            output = new_output;
        }
        strcat(output, student_buffer);
        current_len += written;
    }

    cJSON_Delete(root);
    return output;
}

int main() {
    const char* filename = "students.json";
    view_students_from_file(filename);
    return 0;
}
