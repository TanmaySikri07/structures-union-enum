#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Enums for task status and command types
typedef enum {
    PENDING,
    DONE
} TaskStatus;

typedef enum {
    CMD_ADD,
    CMD_DELETE,
    CMD_VIEW,
    CMD_UPDATE,
    CMD_COMPLETE,
    CMD_FIND,
    CMD_HELP,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

typedef struct {
    int id;
    char description[150];
    TaskStatus status;
} Task;

// Helper to flush the input buffer
void clear_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Helper to safely read a string and handle overlong lines
int read_string(char *buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        return 0; // EOF encountered
    }
    
    // Check if the input was too long (no newline found)
    if (buffer[strlen(buffer) - 1] != '\n') {
        clear_buffer();
    } else {
        // Strip trailing newline
        buffer[strcspn(buffer, "\n")] = '\0';
    }
    return 1;
}

// Converts a string to uppercase so commands are case-insensitive
void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

// Maps the string to the correct enum
CommandType get_command_type(const char* commandStr) {
    char cmd[50];
    strncpy(cmd, commandStr, sizeof(cmd) - 1);
    cmd[sizeof(cmd) - 1] = '\0';
    to_uppercase(cmd);

    if (strcmp(cmd, "ADD") == 0) return CMD_ADD;
    if (strcmp(cmd, "DELETE") == 0) return CMD_DELETE;
    if (strcmp(cmd, "VIEW") == 0) return CMD_VIEW;
    if (strcmp(cmd, "UPDATE") == 0) return CMD_UPDATE;
    if (strcmp(cmd, "COMPLETE") == 0) return CMD_COMPLETE;
    if (strcmp(cmd, "FIND") == 0) return CMD_FIND;
    if (strcmp(cmd, "HELP") == 0) return CMD_HELP;
    if (strcmp(cmd, "EXIT") == 0) return CMD_EXIT;
    return CMD_UNKNOWN;
}

// Returns the index of a task by ID, or -1 if not found
int find_task_index(Task *tasks, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (tasks[i].id == id) {
            return i;
        }
    }
    return -1;
}

void add_task(Task **tasks, int *count, int *cap) {
    if (*count >= *cap) {
        int new_cap = (*cap == 0) ? 2 : (*cap) * 2;
        Task *temp = realloc(*tasks, new_cap * sizeof(Task));
        if (temp == NULL) {
            printf("Error: Memory allocation failed.\n");
            return;
        }
        *tasks = temp;
        *cap = new_cap;
    }

    Task t;
    printf("Enter Task ID (positive integer): ");
    if (scanf("%d", &t.id) != 1 || t.id <= 0) {
        clear_buffer();
        printf("Invalid ID. Action aborted.\n");
        return;
    }

    if (find_task_index(*tasks, *count, t.id) != -1) {
        printf("Error: Task ID %d already exists.\n", t.id);
        return;
    }

    clear_buffer();
    printf("Enter Task Description: ");
    if (!read_string(t.description, sizeof(t.description)) || strlen(t.description) == 0) {
        printf("Description cannot be empty. Action aborted.\n");
        return;
    }

    t.status = PENDING;
    (*tasks)[*count] = t;
    (*count)++;
    printf("Task %d added successfully.\n", t.id);
}

void delete_task(Task *tasks, int *count) {
    if (*count == 0) {
        printf("Task list is empty.\n");
        return;
    }
    int id;
    printf("Enter Task ID to delete: ");
    if (scanf("%d", &id) != 1) {
        clear_buffer();
        printf("Invalid input.\n");
        return;
    }
    
    int idx = find_task_index(tasks, *count, id);
    if (idx == -1) {
        printf("Task %d not found.\n", id);
        return;
    }

    for (int i = idx; i < (*count) - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    (*count)--;
    printf("Task %d deleted successfully.\n", id);
}

void update_task(Task *tasks, int count) {
    if (count == 0) {
        printf("Task list is empty.\n");
        return;
    }
    int id;
    printf("Enter Task ID to update: ");
    if (scanf("%d", &id) != 1) {
        clear_buffer();
        printf("Invalid input.\n");
        return;
    }
    
    int idx = find_task_index(tasks, count, id);
    if (idx == -1) {
        printf("Task %d not found.\n", id);
        return;
    }

    clear_buffer();
    printf("Enter New Description: ");
    char new_desc[150];
    if (read_string(new_desc, sizeof(new_desc)) && strlen(new_desc) > 0) {
        strcpy(tasks[idx].description, new_desc);
        printf("Task %d updated successfully.\n", id);
    } else {
        printf("Invalid description. Update aborted.\n");
    }
}

void complete_task(Task *tasks, int count) {
    if (count == 0) {
        printf("Task list is empty.\n");
        return;
    }
    int id;
    printf("Enter Task ID to mark as COMPLETE: ");
    if (scanf("%d", &id) != 1) {
        clear_buffer();
        printf("Invalid input.\n");
        return;
    }
    
    int idx = find_task_index(tasks, count, id);
    if (idx == -1) {
        printf("Task %d not found.\n", id);
        return;
    }

    tasks[idx].status = DONE;
    printf("Task %d marked as DONE.\n", id);
}

void view_tasks(Task *tasks, int count) {
    if (count == 0) {
        printf("No tasks to display.\n");
        return;
    }
    printf("\n--- Task List ---\n");
    printf("%-5s | %-12s | %s\n", "ID", "Status", "Description");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        const char *status_str = (tasks[i].status == DONE) ? "[DONE]" : "[PENDING]";
        printf("%-5d | %-12s | %s\n", tasks[i].id, status_str, tasks[i].description);
    }
    printf("--------------------------------------------------\n");
}

void find_task(Task *tasks, int count) {
    if (count == 0) {
        printf("Task list is empty.\n");
        return;
    }
    int id;
    printf("Enter Task ID to find: ");
    if (scanf("%d", &id) != 1) {
        clear_buffer();
        printf("Invalid input.\n");
        return;
    }
    
    int idx = find_task_index(tasks, count, id);
    if (idx != -1) {
        const char *status_str = (tasks[idx].status == DONE) ? "DONE" : "PENDING";
        printf("Found Task %d: [%s] %s\n", tasks[idx].id, status_str, tasks[idx].description);
    } else {
        printf("Task %d not found.\n", id);
    }
}

void print_help() {
    printf("\nAvailable Commands:\n");
    printf("  ADD      - Add a new task\n");
    printf("  DELETE   - Remove a task by ID\n");
    printf("  UPDATE   - Edit a task's description\n");
    printf("  COMPLETE - Mark a task as done\n");
    printf("  VIEW     - List all tasks\n");
    printf("  FIND     - View a specific task by ID\n");
    printf("  HELP     - Show this menu\n");
    printf("  EXIT     - Terminate program\n");
}

int main() {
    char input[50];
    CommandType cmd;
    
    Task *task_array = NULL;
    int capacity = 0;
    int count = 0;

    printf("Welcome to the Task Manager. Type HELP for commands.\n");

    while (1) {
        printf("\n> Enter command: ");
        if (!read_string(input, sizeof(input))) {
            printf("\nEOF detected. Exiting...\n");
            break;
        }

        // Handle empty enter presses
        if (strlen(input) == 0) continue; 

        cmd = get_command_type(input);

        switch (cmd) {
            case CMD_ADD:
                add_task(&task_array, &count, &capacity);
                break;
            case CMD_DELETE:
                delete_task(task_array, &count);
                break;
            case CMD_VIEW:
                view_tasks(task_array, count);
                break;
            case CMD_UPDATE:
                update_task(task_array, count);
                break;
            case CMD_COMPLETE:
                complete_task(task_array, count);
                break;
            case CMD_FIND:
                find_task(task_array, count);
                break;
            case CMD_HELP:
                print_help();
                break;
            case CMD_EXIT:
                printf("Exiting program. Freeing memory...\n");
                free(task_array);
                return 0;
            case CMD_UNKNOWN:
            default:
                printf("Unknown command. Type HELP for a list of valid commands.\n");
        }
    }

    free(task_array);
    return 0;
}