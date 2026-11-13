#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

// Basic Union
typedef union {
    int i;
    float f;
    char str[20];
} UnionData;

// Basic Struct (same members)
typedef struct {
    int i;
    float f;
    char str[20];
} StructData;

// Tagged Union Setup
typedef enum {
    VALUE_NONE,
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_TEXT
} ValueType;

typedef struct {
    ValueType tag;
    UnionData data;
} TaggedRecord;

// Setters with validation
void set_int(TaggedRecord *rec, int val) {
    rec->data.i = val;
    rec->tag = VALUE_INT;
}

void set_float(TaggedRecord *rec, float val) {
    rec->data.f = val;
    rec->tag = VALUE_FLOAT;
}

void set_text(TaggedRecord *rec, const char *text) {
    // Leave 1 byte for null terminator (max 19 readable chars)
    if (strlen(text) >= sizeof(rec->data.str)) {
        printf("Warning: String too long. Truncating to 19 characters.\n");
    }
    strncpy(rec->data.str, text, sizeof(rec->data.str) - 1);
    rec->data.str[sizeof(rec->data.str) - 1] = '\0'; // ensure null termination
    rec->tag = VALUE_TEXT;
}

// Safe display function
void display_record(const TaggedRecord *rec, int index) {
    printf("Record %d: ", index);
    switch (rec->tag) {
        case VALUE_INT:
            printf("[INT] %d\n", rec->data.i);
            break;
        case VALUE_FLOAT:
            printf("[FLOAT] %.2f\n", rec->data.f);
            break;
        case VALUE_TEXT:
            printf("[TEXT] %s\n", rec->data.str);
            break;
        case VALUE_NONE:
            printf("[EMPTY] No valid data set.\n");
            break;
        default:
            printf("[ERROR] Corrupted tag!\n");
    }
}

// Requirement: Memory Analysis Utility
void run_memory_analysis() {
    UnionData udata;
    StructData sdata;
    TaggedRecord trec;

    printf("========== MEMORY & ALIGNMENT ANALYSIS ==========\n");
    
    // Size comparisons
    printf("Sizeof UnionData:    %zu bytes\n", sizeof(UnionData));
    printf("Sizeof StructData:   %zu bytes\n", sizeof(StructData));
    printf("Sizeof TaggedRecord: %zu bytes\n\n", sizeof(TaggedRecord));

    // Address and offset analysis for Struct
    printf("StructData Memory Layout (Offsets):\n");
    printf("  Offset of i:   %zu bytes  (Address: %p)\n", offsetof(StructData, i), (void*)&sdata.i);
    printf("  Offset of f:   %zu bytes  (Address: %p)\n", offsetof(StructData, f), (void*)&sdata.f);
    printf("  Offset of str: %zu bytes  (Address: %p)\n\n", offsetof(StructData, str), (void*)&sdata.str);

    // Address analysis for Union
    printf("UnionData Memory Layout:\n");
    printf("  Address of i:   %p\n", (void*)&udata.i);
    printf("  Address of f:   %p\n", (void*)&udata.f);
    printf("  Address of str: %p\n", (void*)&udata.str);
    printf("  (Notice how all union members share the exact same starting address)\n\n");

    // Scale calculation
    printf("Memory required for 100 standard Struct records: %zu bytes\n", 100 * sizeof(StructData));
    printf("Memory required for 100 tagged Union records:    %zu bytes\n", 100 * sizeof(TaggedRecord));
    printf("=================================================\n\n");
}

int main() {
    run_memory_analysis();

    printf("========== TAGGED RECORD DEMONSTRATION ==========\n");
    
    // Array of 5 records initialized to NONE
    TaggedRecord records[5] = {0};

    // 1. Set Integer
    set_int(&records[0], 10);
    
    // 2. Set Float
    set_float(&records[1], 220.5f);
    
    // 3. Set standard text
    set_text(&records[2], "Union Programming");
    
    // 4. Set another integer
    set_int(&records[3], 99);
    
    // 5. Test Edge Case: Overlength string (should truncate and warn)
    printf("Attempting to insert a very long string into Record 4...\n");
    set_text(&records[4], "This string is way too long for a twenty byte array");
    
    // Test Edge Case: Switching a record type safely
    printf("\nDisplaying Record 0 initially:\n");
    display_record(&records[0], 0);
    printf("Switching Record 0 to Text...\n");
    set_text(&records[0], "Now I am text");

    // Display all records
    printf("\n--- Final Safe Reads ---\n");
    for (int i = 0; i < 5; i++) {
        display_record(&records[i], i);
    }

    return 0;
}