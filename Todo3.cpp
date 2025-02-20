#include<iostream>
#include <fstream>   // File operations 
#include <string>    // String operations 
#include <iomanip>   // For setting output formatting like width
using namespace std;

struct Task {
    int id;                
    string description;    
    string category;       
    string priority;       
    string dueDate;        
    string status;        
    Task* next;            // Pointer to the next Task in the list

    // Constructor to initialize a Task
    Task(int id, const string& desc, const string& cat, const string& pri, const string& due, const string& stat, Task* nxt)
        : id(id), description(desc), category(cat), priority(pri), dueDate(due), status(stat), next(nxt) {}
};

struct Action {
    string type;           // Action type (like 'add', 'mark_done', etc.)
    Task* task;            // Task in which actions are to be performed
    Action* next;          // Pointer to the next action (for undo/redo stack)

    // Constructor to initialize an Action
    Action(string t, Task* taskPtr) : type(t), task(taskPtr), next(nullptr) {}
};

Task* head = nullptr;      // Head of the Task linked list
int taskCount = 0;         // Counter to keep track of tasks' IDs
const string filename = "tasks.txt";  
Action* undoStack = nullptr; // Stack for undo actions
Action* redoStack = nullptr; // Stack for redo actions

// Function declarations
void saveTasks();  
void loadTasks();  
void addTask();  
void displayTasks();  
void sortTasks();  
void markTaskAsDone();  
void undoAction();  
void redoAction();  
void pushUndo(const string& type, Task* task);  
void pushRedo(const string& type, Task* task);  
void popUndo();  
void popRedo();


void saveTasks() {
    ofstream file(filename);  // File open
    if (!file) {             
        cout << "Error saving tasks.\n"; 
        return;
    }

    Task* temp = head; 
    while (temp) {  
        file << temp->id << "," << temp->description << "," << temp->category << ","
             << temp->priority << "," << temp->dueDate << "," << temp->status << "\n"; // Task details to save in file
        temp = temp->next; 
    }
    file.close(); 
    cout << "Tasks saved successfully!\n"; 
}

// Function to load tasks from a file
void loadTasks() {
    ifstream file(filename); 
    if (!file) { 
        cout << "No saved tasks found.\n";  
        return;
    }

    string line;
    while (getline(file, line)) {  // Read every line
        int id;
        string description, category, priority, dueDate, status;

        // Data extract from CSV format
        id = stoi(line.substr(0, line.find(',')));  
        line.erase(0, line.find(',') + 1);  
        description = line.substr(0, line.find(','));
        line.erase(0, line.find(',') + 1);
        category = line.substr(0, line.find(','));
        line.erase(0, line.find(',') + 1);
        priority = line.substr(0, line.find(','));
        line.erase(0, line.find(',') + 1);
        dueDate = line.substr(0, line.find(','));
        line.erase(0, line.find(',') + 1);
        status = line;

        // To create new tasks and add them to the head
        Task* newTask = new Task(id, description, category, priority, dueDate, status, head);
        head = newTask;

        if (id > taskCount) {  // Task ID ko update karna agar wo maximum ho
            taskCount = id;
        }
    }

    file.close();  
    cout << "Tasks loaded successfully!\n";  
}

// Function to add a new task
void addTask() {
    string description, category, priority, dueDate;
    cout << "Description: ";
    cin.ignore();  // Ignore any leftover input in the buffer
    getline(cin, description); 
    cout << "Category: ";
    getline(cin, category);  
    cout << "Priority (High/Medium/Low): ";
    cin >> priority;  
    cout << "Due Date (YYYY-MM-DD): ";
    cin >> dueDate;  

    // Create new task and add it to the linked list
    Task* newTask = new Task(++taskCount, description, category, priority, dueDate, "Pending", head);
    head = newTask;

    pushUndo("add", newTask);  // add in undo stack
    cout << "Task added successfully!\n"; 
}

// Function to display all tasks
void displayTasks() {
    if (!head) {  
        cout << "No tasks available.\n"; 
        return;
    }

    cout << left << setw(5) << "ID" << setw(25) << "Description" << setw(15) 
         << "Category" << setw(10) << "Priority" << setw(12) << "Due Date" 
         << setw(10) << "Status" << "\n";
    cout << string(77, '-') << "\n";  // Formatting line

    Task* temp = head;  
    while (temp) {  
        cout << left << setw(5) << temp->id << setw(25) << temp->description 
             << setw(15) << temp->category << setw(10) << temp->priority 
             << setw(12) << temp->dueDate << setw(10) << temp->status << "\n";
        temp = temp->next;  
    }
    cout << string(77, '-') << "\n";
}

// Function to sort tasks
void sortTasks() {
    if (!head || !head->next) return;  

    bool swapped;  // Sorting flag
    Task *ptr1;
    Task *lptr = nullptr;  // Last pointer

    do {
        swapped = false;
        ptr1 = head;

        while (ptr1->next != lptr) {  // Loop for sorting
            Task* nextTask = ptr1->next;

            // Sorting first by Due Date, then by Priority, then alphabetically by Description
            if (ptr1->dueDate > nextTask->dueDate ||
                (ptr1->dueDate == nextTask->dueDate && ptr1->priority < nextTask->priority) ||
                (ptr1->dueDate == nextTask->dueDate && ptr1->priority == nextTask->priority && ptr1->description > nextTask->description)) {

                swap(ptr1->id, nextTask->id);  // Swapping task details
                swap(ptr1->description, nextTask->description);
                swap(ptr1->category, nextTask->category);
                swap(ptr1->priority, nextTask->priority);
                swap(ptr1->dueDate, nextTask->dueDate);
                swap(ptr1->status, nextTask->status);
                swapped = true;  // Set swapped to true if any swap happens
            }
            ptr1 = ptr1->next; 
        }
        lptr = ptr1;  // Last task marking
    } while (swapped);  
}

// Function to mark a task as done
void markTaskAsDone() {
    int id;
    cout << "Enter task ID to mark as done: ";
    cin >> id;  // Task ID lena

    Task* temp = head;
    while (temp) {  
        if (temp->id == id) {
            temp->status = "Done"; 
            pushUndo("mark_done", temp);  
            cout << "Task marked as done.\n";  
            return;
        }
        temp = temp->next;  
    }
    cout << "Task with ID " << id << " not found.\n";
}

// Function to push an action into the undo stack
void pushUndo(const string& type, Task* task) {
    Action* newAction = new Action(type, task);  
    newAction->next = undoStack;  
    undoStack = newAction;  
}

// Function to push an action into the redo stack
void pushRedo(const string& type, Task* task) {
    Action* newAction = new Action(type, task);  
    newAction->next = redoStack;  
    redoStack = newAction;  
}

// Function to pop an action from the undo stack
void popUndo() {
    if (!undoStack) {
        cout << "No actions to undo.\n"; 
        return;
    }

    Action* action = undoStack;  
    undoStack = undoStack->next;  

    if (action->type == "add") {  // if action type is "add" 
        Task* temp = head;
        if (temp == action->task) { 
            head = temp->next;  
            delete temp;  
        } else {
            while (temp->next != action->task) {
                temp = temp->next;  // Find task
            }
            temp->next = temp->next->next;  // Next task update
            delete action->task;  // Task deleting
        }
        pushRedo("add", action->task); 
    } else if (action->type == "mark_done") { 
        action->task->status = "Pending"; 
        pushRedo("mark_done", action->task);  
    }

    delete action; 
    cout << "Undo successful.\n";  
}

// Function to pop an action from the redo stack
void popRedo() {
    if (!redoStack) {  
        cout << "No actions to redo.\n"; 
        return;
    }

    Action* action = redoStack;  
    redoStack = redoStack->next; 

    if (action->type == "add") {  
        Task* temp = new Task(action->task->id, action->task->description, action->task->category,
                              action->task->priority, action->task->dueDate, action->task->status, head);
        head = temp;  
    } else if (action->type == "mark_done") {  
        action->task->status = "Done";  
    }

    delete action;  
    cout << "Redo successful.\n";  
}

int main() {
    loadTasks(); 

    int choice;
    do {
        cout << "[1] Add Task [2] Display [3] Mark Done [4] Sort Tasks [5] Save Tasks [6] Load Tasks [7] Undo [8] Redo [0] Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) { 
            case 1: addTask(); break;
            case 2: displayTasks(); break;
            case 3: markTaskAsDone(); break;
            case 4: sortTasks(); break;
            case 5: saveTasks(); break;
            case 6: loadTasks(); break;
            case 7: popUndo(); break;
            case 8: popRedo(); break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice.\n"; break;
        }
    } while (choice != 0);  

    return 0; 
}