#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
using namespace std;

struct Task {
    int id;
    string description;
    string category;
    string priority;
    string dueDate;
    string status;
    Task* next;
};

class TodoList {
private:
    Task* head;
    int taskCount;

public:
    TodoList() : head(nullptr), taskCount(0) {}

    ~TodoList() {
        while (head) {
            Task* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void addTask(const string& description, const string& category, const string& priority, const string& dueDate) {
        Task* newTask = new Task;
        newTask->id = ++taskCount;
        newTask->description = description;
        newTask->category = category;
        newTask->priority = priority;
        newTask->dueDate = dueDate;
        newTask->status = "Pending";
        newTask->next = nullptr;

        if (!head) {
            head = newTask;
        } else {
            Task* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newTask;
        }
    }

    void displayTasks() {
        if (!head) {
            cout << "No tasks available.\n";
            return;
        }

        Task* current = head;
        while (current) {
            cout << "\nID: " << current->id << "\n"
                 << "Description: " << current->description << "\n"
                 << "Category: " << current->category << "\n"
                 << "Priority: " << current->priority << "\n"
                 << "Due Date: " << current->dueDate << "\n"
                 << "Status: " << current->status << "\n";
            current = current->next;
        }
    }

    void markTaskDone(int taskId) {
        Task* task = findTask(taskId);
        if (task) {
            task->status = "Done";
        } else {
            cout << "Task not found!\n";
        }
    }

    void sortTasks() {
        if (!head || !head->next) return;

        bool swapped;
        do {
            swapped = false;
            Task* current = head;
            Task* prev = nullptr;

            while (current && current->next) {
                Task* nextTask = current->next;

                if (current->dueDate > nextTask->dueDate ||
                   (current->dueDate == nextTask->dueDate && current->priority > nextTask->priority) ||
                   (current->dueDate == nextTask->dueDate && current->priority == nextTask->priority && current->description > nextTask->description)) {

                    swapped = true;
                    current->next = nextTask->next;
                    nextTask->next = current;

                    if (prev) {
                        prev->next = nextTask;
                    } else {
                        head = nextTask;
                    }

                    prev = nextTask;
                } else {
                    prev = current;
                    current = current->next;
                }
            }
        } while (swapped);
    }

    void generateSummary() {
        int totalTasks = 0, completedTasks = 0, pendingTasks = 0;
        Task* current = head;

        while (current) {
            ++totalTasks;
            if (current->status == "Done") {
                ++completedTasks;
            } else {
                ++pendingTasks;
            }
            current = current->next;
        }

        cout << "\nTask Summary:\n";
        cout << "Total Tasks: " << totalTasks << "\n";
        cout << "Completed Tasks: " << completedTasks << "\n";
        cout << "Pending Tasks: " << pendingTasks << "\n";
    }

    void saveTasks() {
        ofstream file("tasks.txt");
        Task* current = head;

        while (current) {
            file << current->id << "," << current->description << "," << current->category << "," << current->priority << "," << current->dueDate << "," << current->status << "\n";
            current = current->next;
        }

        file.close();
    }

    void loadTasks() {
        ifstream file("tasks.txt");
        string line;

        while (getline(file, line)) {
            size_t pos = 0;
            string fields[6];
            int index = 0;

            while ((pos = line.find(",")) != string::npos) {
                fields[index++] = line.substr(0, pos);
                line.erase(0, pos + 1);
            }
            fields[index] = line;

            addTask(fields[1], fields[2], fields[3], fields[4]);
            head->status = fields[5];
        }

        file.close();
    }

private:
    Task* findTask(int taskId) {
        Task* current = head;
        while (current) {
            if (current->id == taskId) return current;
            current = current->next;
        }
        return nullptr;
    }
};

int main() {
    TodoList todoList;
    todoList.loadTasks();

    todoList.addTask("Finish DSA project", "Work", "High", "2025-01-25");
    todoList.addTask("Buy groceries", "Personal", "Medium", "2025-01-23");

    todoList.sortTasks();
    todoList.displayTasks();

    todoList.markTaskDone(2);

    todoList.generateSummary();
    todoList.saveTasks();

    return 0;
}
