#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int roll;
    char name[50];
    int age;
    float gpa;
} Student;

// Helper to clear the input buffer before taking string inputs or after bad inputs
void clear_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Returns the index of a student by roll number, or -1 if not found
int find_index(Student *db, int count, int roll) {
    for (int i = 0; i < count; i++) {
        if (db[i].roll == roll) {
            return i;
        }
    }
    return -1;
}

void add_student(Student **db, int *count, int *cap) {
    // Check if we need to resize the array
    if (*count >= *cap) {
        int new_cap = (*cap) * 2;
        // Avoid overflow just in case
        if (new_cap <= 0) new_cap = *cap + 2; 

        Student *temp = realloc(*db, new_cap * sizeof(Student));
        if (temp == NULL) {
            printf("Error: Memory reallocation failed. Cannot add more records.\n");
            return; // Original array is kept untouched
        }
        *db = temp;
        *cap = new_cap;
    }

    Student s;
    printf("Enter Roll Number (positive integer): ");
    if (scanf("%d", &s.roll) != 1 || s.roll <= 0) {
        clear_buffer();
        printf("Invalid roll number. Must be a positive integer.\n");
        return;
    }

    if (find_index(*db, *count, s.roll) != -1) {
        printf("Error: A student with this roll number already exists.\n");
        return;
    }

    clear_buffer();
    printf("Enter Name: ");
    fgets(s.name, sizeof(s.name), stdin);
    
    // Check if the input was too long (no newline found)
    if (s.name[strlen(s.name) - 1] != '\n') {
        clear_buffer(); // Flush the extra characters left in the buffer
    } else {
        // Strip the trailing newline left by fgets
        s.name[strcspn(s.name, "\n")] = '\0';
    }

    printf("Enter Age (16-100): ");
    if (scanf("%d", &s.age) != 1 || s.age < 16 || s.age > 100) {
        clear_buffer();
        printf("Error: Age must be a number between 16 and 100.\n");
        return;
    }

    printf("Enter GPA (0.00 - 4.00): ");
    if (scanf("%f", &s.gpa) != 1 || s.gpa < 0.0 || s.gpa > 4.0) {
        clear_buffer();
        printf("Error: GPA must be a number between 0.00 and 4.00.\n");
        return;
    }

    (*db)[*count] = s;
    (*count)++;
    printf("Student record added successfully!\n");
}

void display_all(Student *db, int count) {
    if (count == 0) {
        printf("No records found.\n");
        return;
    }
    printf("\n--- All Student Records ---\n");
    printf("%-8s | %-20s | %-5s | %-5s\n", "Roll No", "Name", "Age", "GPA");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-8d | %-20s | %-5d | %-4.2f\n", db[i].roll, db[i].name, db[i].age, db[i].gpa);
    }
}

void search_student(Student *db, int count) {
    if (count == 0) {
        printf("Database is empty.\n");
        return;
    }
    int roll;
    printf("Enter Roll Number to search: ");
    if (scanf("%d", &roll) != 1) {
        clear_buffer();
        printf("Invalid input.\n");
        return;
    }
    
    int idx = find_index(db, count, roll);
    if (idx != -1) {
        printf("Record found: Name = %s, Age = %d, GPA = %.2f\n", 
               db[idx].name, db[idx].age, db[idx].gpa);
    } else {
        printf("Student with Roll Number %d not found.\n", roll);
    }
}

void update_student(Student *db, int count) {
    if (count == 0) {
        printf("Database is empty.\n");
        return;
    }
    int roll;
    printf("Enter Roll Number to update: ");
    if (scanf("%d", &roll) != 1) {
        clear_buffer();
        printf("Invalid input.\n");
        return;
    }
    
    int idx = find_index(db, count, roll);
    if (idx == -1) {
        printf("Student with Roll Number %d not found.\n", roll);
        return;
    }

    float new_gpa;
    printf("Enter new GPA (0.00 - 4.00): ");
    if (scanf("%f", &new_gpa) == 1 && new_gpa >= 0.0 && new_gpa <= 4.0) {
        db[idx].gpa = new_gpa;
        printf("GPA updated successfully.\n");
    } else {
        clear_buffer();
        printf("Invalid GPA. Update aborted.\n");
    }
}

void delete_student(Student *db, int *count) {
    if (*count == 0) {
        printf("Database is empty.\n");
        return;
    }
    int roll;
    printf("Enter Roll Number to delete: ");
    if (scanf("%d", &roll) != 1) {
        clear_buffer();
        printf("Invalid input.\n");
        return;
    }
    
    int idx = find_index(db, *count, roll);
    if (idx == -1) {
        printf("Student with Roll Number %d not found.\n", roll);
        return;
    }

    // Shift elements left to overwrite the deleted record
    for (int i = idx; i < (*count) - 1; i++) {
        db[i] = db[i + 1];
    }
    (*count)--;
    printf("Student deleted successfully.\n");
}

void show_analytics(Student *db, int count) {
    if (count == 0) {
        printf("Not enough data for analytics.\n");
        return;
    }

    int best_idx = 0, worst_idx = 0;
    float total_gpa = 0.0;

    for (int i = 0; i < count; i++) {
        if (db[i].gpa > db[best_idx].gpa) best_idx = i;
        if (db[i].gpa < db[worst_idx].gpa) worst_idx = i;
        total_gpa += db[i].gpa;
    }

    printf("\n--- Academic Analytics ---\n");
    printf("Highest GPA: %s (%.2f)\n", db[best_idx].name, db[best_idx].gpa);
    printf("Lowest GPA: %s (%.2f)\n", db[worst_idx].name, db[worst_idx].gpa);
    printf("Average GPA: %.2f\n", total_gpa / count);
}

void sort_records(Student *db, int count) {
    if (count < 2) {
        printf("Not enough records to sort.\n");
        return;
    }

    // Standard bubble sort (sorting the original array directly)
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int swap_needed = 0;
            
            // Sort by GPA descending
            if (db[j].gpa < db[j+1].gpa) {
                swap_needed = 1;
            } 
            // Break ties with Roll Number ascending
            else if (db[j].gpa == db[j+1].gpa && db[j].roll > db[j+1].roll) {
                swap_needed = 1;
            }

            if (swap_needed) {
                Student temp = db[j];
                db[j] = db[j+1];
                db[j+1] = temp;
            }
        }
    }
    printf("Records sorted successfully!\n");
}

int main() {
    int cap = 0;
    int count = 0;

    printf("Enter initial capacity for student records: ");
    if (scanf("%d", &cap) != 1) {
        printf("Invalid input. Exiting.\n");
        return 1;
    }

    while (cap <= 0) {
        printf("Capacity must be positive. Try again: ");
        if (scanf("%d", &cap) != 1) {
            printf("Invalid input. Exiting.\n");
            return 1;
        }
    }

    Student *arr = (Student *)malloc(cap * sizeof(Student));
    if (arr == NULL) {
        printf("Initial memory allocation failed. Exiting.\n");
        return 1;
    }

    int choice;
    do {
        printf("\n========== MENU ==========\n");
        printf("1. Add New Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student by Roll\n");
        printf("4. Update Student GPA\n");
        printf("5. Delete Student\n");
        printf("6. View Academic Analytics\n");
        printf("7. Sort Records\n");
        printf("8. Exit\n");
        printf("==========================\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            clear_buffer(); // flush the bad input
            printf("Invalid input. Please enter a number.\n");
            choice = 0; // reset choice so the switch defaults gracefully
            continue;   // restart the loop
        }

        switch (choice) {
            case 1: add_student(&arr, &count, &cap); break;
            case 2: display_all(arr, count); break;
            case 3: search_student(arr, count); break;
            case 4: update_student(arr, count); break;
            case 5: delete_student(arr, &count); break; // FIXED: passed arr directly
            case 6: show_analytics(arr, count); break;
            case 7: 
                sort_records(arr, count); 
                display_all(arr, count); // show them right after sorting
                break;
            case 8: 
                printf("Exiting program and freeing memory. Goodbye!\n"); 
                break;
            default: 
                printf("Invalid choice. Please pick between 1 and 8.\n");
        }
    } while (choice != 8);

    free(arr);
    return 0;
}