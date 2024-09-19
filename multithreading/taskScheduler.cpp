#include <iostream>
#include <queue>
#include <thread>
#include <functional>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>

using namespace std;

// Task class that holds the priority and the function to execute.
class Task {
public:
    int priority;  
    function<void()> func;   

    Task(int p, function<void()> f) : priority(p), func(f) {}
};

 
struct CompareTask {
    bool operator()(const Task& t1, const Task& t2) {
        return t1.priority < t2.priority;   
    }
};

// Task Scheduler that manages task execution across multiple threads.
class TaskScheduler {
private:
    priority_queue<Task, vector<Task>, CompareTask> taskQueue;  
    mutex queueMutex;   
    condition_variable cv;  
    atomic<bool> stop;   
    vector<thread> workers;  

    void dispatcherThread() {
        while (!stop) {
            Task task(0, []() {});
            {
                unique_lock<mutex> lock(queueMutex);
                cv.wait(lock, [this]() { return !taskQueue.empty() || stop; });

                if (stop && taskQueue.empty()) {
                    return;
                }
                task = taskQueue.top();
                taskQueue.pop();
            }
            async(launch::async, task.func);
        }
    }

public:
    TaskScheduler() : stop(false) {
        workers.emplace_back(&TaskScheduler::dispatcherThread, this);
    }
    ~TaskScheduler() {
        stopScheduler();
    }

    void addTask(int priority, function<void()> func) {
        {
            lock_guard<mutex> lock(queueMutex);
            taskQueue.emplace(priority, func);
        }
        cv.notify_one();
    }

    void stopScheduler() {
        {
            lock_guard<mutex> lock(queueMutex);
            stop = true;
        }
        cv.notify_all();  
        for (thread &worker : workers) {
            if (worker.joinable()) {
                worker.join();   
            }
        }
    }
};

int main() {
    TaskScheduler scheduler;

    scheduler.addTask(1, []() { cout << "Low priority task executed\n"; });
    scheduler.addTask(3, []() { cout << "High priority task executed\n"; });
    scheduler.addTask(2, []() { cout << "Medium priority task executed\n"; });
    scheduler.addTask(5, []() { cout << "Very high priority task executed\n"; });

    this_thread::sleep_for(chrono::seconds(2));
    scheduler.stopScheduler();

    return 0;
}
//g++ -std=c++11 -pthread -o taskScheduler taskScheduler.cpp
//./taskScheduler