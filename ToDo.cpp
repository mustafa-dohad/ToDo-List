#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>

using namespace std;

// Task structure
struct Task {
    int id;
    string description;
    string priority; // High, Medium, Low
    string dueDate;  // Format: YYYY-MM-DD
    string status;   // Pending, Done
    Task* next;
};

class TodoList {
private:
    Task* head;
    int taskCount;

public:
    TodoList() : head(nullptr), taskCount(0) {}

    // Add a new task
    void addTask(const string& description, const string& priority, const string& dueDate) {
        Task* newTask = new Task;
        newTask->id = ++taskCount;
        newTask->description = description;
        newTask->priority = priority;
        newTask->dueDate = dueDate;
        newTask->status = "Pending";
        newTask->next = head;
        head = newTask;

        saveTasksToFile();
        cout << "Task added successfully.\n";
    }

    // Mark a task as done
    void markTaskDone(int id) {
        Task* current = head;
        while (current) {
            if (current->id == id) {
                current->status = "Done";
                saveTasksToFile();
                cout << "Task marked as done.\n";
                return;
            }
            current = current->next;
        }
        cout << "Task not found.\n";
    }

    // Display tasks
    void displayTasks() {
        if (!head) {
            cout << "No tasks available.\n";
            return;
        }
        Task* current = head;
        cout << "ID\tDescription\tPriority\tDue Date\tStatus\n";
        while (current) {
            cout << current->id << "\t" << current->description << "\t" << current->priority
                 << "\t" << current->dueDate << "\t" << current->status << "\n";
            current = current->next;
        }
    }

    // Sort tasks by due date, priority, and alphabetical order
    void sortTasks() {
        if (!head || !head->next) return;

        bool swapped;
        do {
            swapped = false;
            Task* current = head;

            while (current && current->next) {
                bool shouldSwap = false;

                // Compare due dates (earlier dates rank higher)
                if (current->dueDate > current->next->dueDate) {
                    shouldSwap = true;
                }
                // If due dates are the same, compare priorities
                else if (current->dueDate == current->next->dueDate) {
                    if (getPriorityValue(current->priority) < getPriorityValue(current->next->priority)) {
                        shouldSwap = true;
                    }
                    // If priorities are also the same, sort alphabetically by description
                    else if (getPriorityValue(current->priority) == getPriorityValue(current->next->priority) &&
                             current->description > current->next->description) {
                        shouldSwap = true;
                    }
                }

                if (shouldSwap) {
                    swapTasks(current, current->next);
                    swapped = true;
                }
                current = current->next;
            }
        } while (swapped);

        saveTasksToFile();
        cout << "Tasks sorted by due date, priority, and description.\n";
    }

    // Delete a task
    void deleteTask(int id) {
        Task* current = head;
        Task* previous = nullptr;

        while (current) {
            if (current->id == id) {
                if (previous) {
                    previous->next = current->next;
                } else {
                    head = current->next;
                }
                delete current;
                saveTasksToFile();
                cout << "Task deleted successfully.\n";
                return;
            }
            previous = current;
            current = current->next;
        }
        cout << "Task not found.\n";
    }

    // Save tasks to file
    void saveTasksToFile() {
        ofstream file("tasks.txt");
        Task* current = head;
        while (current) {
            file << current->id << "," << current->description << "," << current->priority
                 << "," << current->dueDate << "," << current->status << "\n";
            current = current->next;
        }
        file.close();
    }

    // Load tasks from file
    void loadTasksFromFile() {
        ifstream file("tasks.txt");
        string line;

        while (getline(file, line)) {
            stringstream ss(line);
            string idStr, description, priority, dueDate, status;

            getline(ss, idStr, ',');
            getline(ss, description, ',');
            getline(ss, priority, ',');
            getline(ss, dueDate, ',');
            getline(ss, status, ',');

            Task* newTask = new Task;
            newTask->id = stoi(idStr);
            newTask->description = description;
            newTask->priority = priority;
            newTask->dueDate = dueDate;
            newTask->status = status;
            newTask->next = head;
            head = newTask;

            taskCount = max(taskCount, newTask->id);
        }
        file.close();
    }

private:
    // Helper function to assign numerical values to priorities for comparison
    int getPriorityValue(const string& priority) {
        if (priority == "High") return 3;
        if (priority == "Medium") return 2;
        if (priority == "Low") return 1;
        return 0;
    }

    // Swap two tasks
    void swapTasks(Task* a, Task* b) {
        swap(a->id, b->id);
        swap(a->description, b->description);
        swap(a->priority, b->priority);
        swap(a->dueDate, b->dueDate);
        swap(a->status, b->status);
    }
};

int main() {
    TodoList todo;
    todo.loadTasksFromFile();

    int choice;
    do {
        cout << "\n--- To-Do List Menu ---\n";
        cout << "1. Add Task\n";
        cout << "2. Mark Task as Done\n";
        cout << "3. Display Tasks\n";
        cout << "4. Delete Task\n";
        cout << "5. Sort Tasks\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            string description, priority, dueDate;
            cout << "Enter task description: ";
            cin.ignore();
            getline(cin, description);
            cout << "Enter task priority (High/Medium/Low): ";
            getline(cin, priority);
            cout << "Enter task due date (YYYY-MM-DD): ";
            getline(cin, dueDate);
            todo.addTask(description, priority, dueDate);
            break;
        }
        case 2: {
            int id;
            cout << "Enter task ID to mark as done: ";
            cin >> id;
            todo.markTaskDone(id);
            break;
        }
        case 3:
            todo.displayTasks();
            break;
        case 4: {
            int id;
            cout << "Enter task ID to delete: ";
            cin >> id;
            todo.deleteTask(id);
            break;
        }
        case 5:
            todo.sortTasks();
            break;
        case 6:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 6);

    return 0;
}
