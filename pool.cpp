#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

// Перечислимый тип для статуса задачи
enum class TaskStatus {
    NEW,          // новая
    IN_PROGRESS,  // в разработке
    TESTING,      // на тестировании
    DONE          // завершена
};

// Объявляем тип-синоним для map<TaskStatus, int>,
// позволяющего хранить количество задач каждого статуса
using TasksInfo = map<TaskStatus, int>;

class TeamTasks {
public:
    // Получить статистику по статусам задач конкретного разработчика
    const TasksInfo& GetPersonTasksInfo(const string& person) const {
        return tracker_.at(person);
    }

    // Добавить новую задачу (в статусе NEW) для конкретного разработчика
    void AddNewTask(const string& person) {
        ++tracker_[person][TaskStatus::NEW];
    }

    // Обновить статусы по данному количеству задач конкретного разработчика,
    // подробности см. ниже
    tuple<TasksInfo, TasksInfo> PerformPersonTasks(const string& person, int task_count) {
        
        TasksInfo updated_tasks;
        TasksInfo untouched_tasks;
        tuple<TasksInfo, TasksInfo> person_tasks_updated_untouched = tuple(updated_tasks, untouched_tasks);

        if (tracker_.count(person)) {
            int tasks_could_update = 0;
            for (auto& [status, count] : tracker_.at(person)) {
                if (static_cast<int>(status) == 3) {
                    break;
                }
                tasks_could_update += count;
            }
            if (task_count > tasks_could_update) {
                task_count = tasks_could_update;
            }

            int new_tasks = tracker_.at(person)[TaskStatus::NEW];
            int new_to_in_progress = 0;
            int in_progress_tasks = tracker_.at(person)[TaskStatus::IN_PROGRESS];
            int in_progress_to_testing = 0;
            int testing_tasks = tracker_.at(person)[TaskStatus::TESTING];
            int testing_to_done = 0;
            int done_tasks = tracker_.at(person)[TaskStatus::DONE];

            for (int i = 0; i < new_tasks; ++i) {
                if (task_count == 0) {
                    break;
                }
                ++new_to_in_progress;
                --task_count;  
            }
            int new_new_tasks = new_tasks - new_to_in_progress;
            updated_tasks[TaskStatus::NEW] = 0;
            untouched_tasks[TaskStatus::NEW] = new_new_tasks;
            tracker_[person][TaskStatus::NEW] = new_new_tasks;

            for (int i = 0; i < in_progress_tasks; ++i) {
                if (task_count == 0) {
                    break;
                }
                ++in_progress_to_testing;
                --task_count;
            }
            int new_in_progress_tasks = in_progress_tasks - in_progress_to_testing;
            updated_tasks[TaskStatus::IN_PROGRESS] = new_to_in_progress;
            untouched_tasks[TaskStatus::IN_PROGRESS] = new_in_progress_tasks;
            tracker_[person][TaskStatus::IN_PROGRESS] = new_in_progress_tasks + new_to_in_progress;

            for (int i = 0; i < testing_tasks; ++i) {
                if (task_count == 0) {
                    break;
                }
                ++testing_to_done;
                --task_count;
            }
            int new_testing_tasks = testing_tasks - testing_to_done;
            updated_tasks[TaskStatus::TESTING] = in_progress_to_testing;
            untouched_tasks[TaskStatus::TESTING] = new_testing_tasks;
            tracker_[person][TaskStatus::TESTING] = new_testing_tasks + in_progress_to_testing;

            updated_tasks[TaskStatus::DONE] = testing_to_done;
            untouched_tasks[TaskStatus::DONE] = 0;
            tracker_[person][TaskStatus::DONE] = done_tasks + testing_to_done;

        person_tasks_updated_untouched = tuple(updated_tasks, untouched_tasks);
        }
        return person_tasks_updated_untouched;
    }

/*     static TaskStatus Next(TaskStatus task_status) {
        return static_cast<TaskStatus>(static_cast<int>(task_status) + 1);
    } */

private:

map<string, TasksInfo> tracker_;

};

// Принимаем словарь по значению, чтобы иметь возможность
// обращаться к отсутствующим ключам с помощью [] и получать 0,
// не меняя при этом исходный словарь.
void PrintTasksInfo(TasksInfo tasks_info) {
    cout << tasks_info[TaskStatus::NEW] << " new tasks"s
         << ", "s << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress"s
         << ", "s << tasks_info[TaskStatus::TESTING] << " tasks are being tested"s
         << ", "s << tasks_info[TaskStatus::DONE] << " tasks are done"s << endl;
}

int main() {
    TeamTasks tasks;
    tasks.AddNewTask("Ilia"s);
    for (int i = 0; i < 3; ++i) {
        tasks.AddNewTask("Ivan"s);
    }
    cout << "Ilia's tasks: "s;
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"s));
    cout << "Ivan's tasks: "s;
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"s));

    TasksInfo updated_tasks, untouched_tasks;

    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan"s, 2);
    cout << "Updated Ivan's tasks: "s;
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: "s;
    PrintTasksInfo(untouched_tasks);

    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan"s, 2);
    cout << "Updated Ivan's tasks: "s;
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: "s;
    PrintTasksInfo(untouched_tasks);
}