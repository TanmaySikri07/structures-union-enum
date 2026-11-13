#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_INITIAL_CAPACITY 1000
#define EPSILON 1e-5 // Tolerance for floating-point comparisons

typedef struct {
    int id;
    float width;
    float height;
    float area;
    float perimeter;
} Rectangle;

// Helper to clear the input buffer and prevent infinite loops
void clear_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Recomputes area and perimeter for a given rectangle
void compute_geometry(Rectangle *r) {
    r->area = r->width * r->height;
    r->perimeter = 2.0f * (r->width + r->height);
}

// Returns 1 if square, 0 if not, using floating-point tolerance
int is_square(Rectangle r) {
    return fabs(r.width - r.height) < EPSILON;
}

// Returns the index of a rectangle by ID, or -1 if not found
int find_index(Rectangle *db, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (db[i].id == id) {
            return i;
        }
    }
    return -1;
}

void add_rectangle(Rectangle **db, int *count, int *cap) {
    if (*count >= *cap) {
        int new_cap = (*cap) * 2;
        if (new_cap <= 0) new_cap = *cap + 2; 

        Rectangle *temp = realloc(*db, new_cap * sizeof(Rectangle));
        if (temp == NULL) {
            printf("Error: Memory reallocation failed. Cannot add more records.\n");
            return; 
        }
        *db = temp;
        *cap = new_cap;
    }

    Rectangle r;
    printf("Enter Rectangle ID (positive integer): ");
    if (scanf("%d", &r.id) != 1 || r.id <= 0) {
        clear_buffer();
        printf("Invalid ID. Must be a positive integer.\n");
        return;
    }

    if (find_index(*db, *count, r.id) != -1) {
        printf("Error: A rectangle with this ID already exists.\n");
        return;
    }

    printf("Enter Width (finite positive number): ");
    if (scanf("%f", &r.width) != 1 || !isfinite(r.width) || r.width <= 0.0f) {
        clear_buffer();
        printf("Error: Width must be a valid positive number.\n");
        return;
    }

    printf("Enter Height (finite positive number): ");
    if (scanf("%f", &r.height) != 1 || !isfinite(r.height) || r.height <= 0.0f) {
        clear_buffer();
        printf("Error: Height must be a valid positive number.\n");
        return;
    }

    compute_geometry(&r);
    (*db)[*count] = r;
    (*count)++;
    printf("Rectangle added successfully!\n");
}

void display_all(Rectangle *db, int count) {
    if (count == 0) {
        printf("Inventory is empty.\n");
        return;
    }
    printf("\n--- Rectangle Inventory ---\n");
    printf("%-5s | %-8s | %-8s | %-8s | %-10s | %-8s\n", "ID", "Width", "Height", "Area", "Perimeter", "Is Square");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-5d | %-8.2f | %-8.2f | %-8.2f | %-10.2f | %-8s\n", 
               db[i].id, db[i].width, db[i].height, db[i].area, db[i].perimeter, 
               is_square(db[i]) ? "Yes" : "No");
    }
}

void search_rectangle(Rectangle *db, int count) {
    if (count == 0) {
        printf("Inventory is empty.\n");
        return;
    }
    int id;
    printf("Enter Rectangle ID to search: ");
    if (scanf("%d", &id) != 1) {
        clear_buffer();
        printf("Invalid input.\n");
        return;
    }
    
    int idx = find_index(db, count, id);
    if (idx != -1) {
        printf("Found: Width = %.2f, Height = %.2f, Area = %.2f, Perimeter = %.2f, Square = %s\n", 
               db[idx].width, db[idx].height, db[idx].area, db[idx].perimeter,
               is_square(db[idx]) ? "Yes" : "No");
    } else {
        printf("Rectangle with ID %d not found.\n", id);
    }
}

void update_rectangle(Rectangle *db, int count) {
    if (count == 0) {
        printf("Inventory is empty.\n");
        return;
    }
    int id;
    printf("Enter Rectangle ID to update: ");
    if (scanf("%d", &id) != 1) {
        clear_buffer();
        printf("Invalid input.\n");
        return;
    }
    
    int idx = find_index(db, count, id);
    if (idx == -1) {
        printf("Rectangle with ID %d not found.\n", id);
        return;
    }

    float new_w, new_h;
    printf("Enter new Width: ");
    if (scanf("%f", &new_w) != 1 || !isfinite(new_w) || new_w <= 0.0f) {
        clear_buffer();
        printf("Invalid width. Update aborted.\n");
        return;
    }
    
    printf("Enter new Height: ");
    if (scanf("%f", &new_h) != 1 || !isfinite(new_h) || new_h <= 0.0f) {
        clear_buffer();
        printf("Invalid height. Update aborted.\n");
        return;
    }

    db[idx].width = new_w;
    db[idx].height = new_h;
    compute_geometry(&db[idx]);
    printf("Rectangle updated successfully.\n");
}

void delete_rectangle(Rectangle *db, int *count) {
    if (*count == 0) {
        printf("Inventory is empty.\n");
        return;
    }
    int id;
    printf("Enter Rectangle ID to delete: ");
    if (scanf("%d", &id) != 1) {
        clear_buffer();
        printf("Invalid input.\n");
        return;
    }
    
    int idx = find_index(db, *count, id);
    if (idx == -1) {
        printf("Rectangle with ID %d not found.\n", id);
        return;
    }

    for (int i = idx; i < (*count) - 1; i++) {
        db[i] = db[i + 1];
    }
    (*count)--;
    printf("Rectangle deleted successfully.\n");
}

void show_analytics(Rectangle *db, int count) {
    if (count == 0) {
        printf("Not enough data for analytics.\n");
        return;
    }

    int largest_idx = 0, smallest_idx = 0;
    float total_area = 0.0f;

    for (int i = 0; i < count; i++) {
        if (db[i].area > db[largest_idx].area) largest_idx = i;
        if (db[i].area < db[smallest_idx].area) smallest_idx = i;
        total_area += db[i].area;
    }

    printf("\n--- Geometry Analytics ---\n");
    printf("Largest Area: ID %d (%.2f)\n", db[largest_idx].id, db[largest_idx].area);
    printf("Smallest Area: ID %d (%.2f)\n", db[smallest_idx].id, db[smallest_idx].area);
    printf("Total Area: %.2f\n", total_area);
    printf("Average Area: %.2f\n", total_area / count);
}

void sort_and_filter(Rectangle *db, int count) {
    if (count == 0) {
        printf("Inventory is empty.\n");
        return;
    }

    // Sort original array directly (Bubble Sort)
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int swap_needed = 0;
            
            if (db[j].area > db[j+1].area) {
                swap_needed = 1;
            } else if (fabs(db[j].area - db[j+1].area) < EPSILON && db[j].id > db[j+1].id) {
                swap_needed = 1;
            }

            if (swap_needed) {
                Rectangle temp = db[j];
                db[j] = db[j+1];
                db[j+1] = temp;
            }
        }
    }
    printf("Records sorted by Area (Ascending).\n");

    float threshold;
    printf("Enter minimum area threshold to display: ");
    if (scanf("%f", &threshold) != 1) {
        clear_buffer();
        printf("Invalid input. Defaulting to 0.0.\n");
        threshold = 0.0f;
    }

    printf("\n--- Rectangles with Area > %.2f ---\n", threshold);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (db[i].area > threshold) {
            printf("ID: %-5d | Area: %.2f\n", db[i].id, db[i].area);
            found = 1;
        }
    }
    if (!found) {
        printf("No rectangles exceed the threshold.\n");
    }
}

int main() {
    int cap = 0;
    int count = 0;

    printf("Enter initial capacity (1 - %d): ", MAX_INITIAL_CAPACITY);
    if (scanf("%d", &cap) != 1) {
        printf("Invalid input. Exiting.\n");
        return 1;
    }

    while (cap <= 0 || cap > MAX_INITIAL_CAPACITY) {
        printf("Capacity must be between 1 and %d. Try again: ", MAX_INITIAL_CAPACITY);
        if (scanf("%d", &cap) != 1) {
            printf("Invalid input. Exiting.\n");
            return 1;
        }
    }

    Rectangle *arr = (Rectangle *)malloc(cap * sizeof(Rectangle));
    if (arr == NULL) {
        printf("Initial memory allocation failed. Exiting.\n");
        return 1;
    }

    int choice;
    do {
        printf("\n========== MENU ==========\n");
        printf("1. Add Rectangle\n");
        printf("2. Display All Rectangles\n");
        printf("3. Search by ID\n");
        printf("4. Update Dimensions\n");
        printf("5. Delete by ID\n");
        printf("6. View Analytics\n");
        printf("7. Sort & Filter by Area\n");
        printf("8. Exit\n");
        printf("==========================\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            clear_buffer();
            printf("Invalid input. Please enter a number.\n");
            choice = 0; 
            continue;   
        }

        switch (choice) {
            case 1: add_rectangle(&arr, &count, &cap); break;
            case 2: display_all(arr, count); break;
            case 3: search_rectangle(arr, count); break;
            case 4: update_rectangle(arr, count); break;
            case 5: delete_rectangle(arr, &count); break; 
            case 6: show_analytics(arr, count); break;
            case 7: sort_and_filter(arr, count); break;
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