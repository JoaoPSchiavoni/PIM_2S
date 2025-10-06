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

void view_students(const char* filename) {
    char* json_data = read_file(filename);
    if (!json_data) return;

    cJSON* root = cJSON_Parse(json_data);
    if (!root) {
        printf("Error parsing JSON.\n");
        free(json_data);
        return;
    }

    cJSON* student = NULL;
    cJSON_ArrayForEach(student, root) {
        printf("--------------------------------------------------\n");

        cJSON* name = cJSON_GetObjectItem(student, "name");
        cJSON* ra = cJSON_GetObjectItem(student, "RA");
        printf("Name: %s\n", name->valuestring);
        printf("RA: %s\n", ra->valuestring);

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
                    printf("%.1f ", g->valuedouble);
                    sum += g->valuedouble;
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

int main() {
    const char* filename = "students.json";
    view_students(filename);
    return 0;
}
