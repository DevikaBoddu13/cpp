#include <iostream>
#include <queue>
#include <thread>
#include <functional>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>

// Task class that holds the priority and the function to execute.
class Task {
public:
    int priority;  
    std::function<void()> func;   

    Task(int p, std::function<void()> f) : priority(p), func(f) {}

    // Overload the comparison operator for the priority queue
    bool operator<(const Task& other) const {
        return priority < other.priority;
    }
};

// Task Scheduler that manages task execution across multiple threads.
class TaskScheduler {
private:
    std::priority_queue<Task> taskQueue;   
    std::mutex queueMutex;                 
    std::condition_variable cv;            
    std::atomic<bool> stop;                
    std::vector<std::thread> workers;      

    
    void workerThread() {
        while (!stop) {
            Task task(0, []() {});
            {
                std::unique_lock<std::mutex> lock(queueMutex);

                 
                cv.wait(lock, [this]() { return !taskQueue.empty() || stop; });

                if (stop && taskQueue.empty()) {
                    return;
                }

                 
                task = taskQueue.top();
                taskQueue.pop();
            }

             
            task.func();
        }
    }

public:
    
    TaskScheduler(int numThreads) : stop(false) {
        for (int i = 0; i < numThreads; ++i) {
            workers.emplace_back(&TaskScheduler::workerThread, this);
        }
    }

     
    ~TaskScheduler() {
        stopScheduler();
    }

     
    void addTask(int priority, std::function<void()> func) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            taskQueue.emplace(priority, func);
        }
        cv.notify_one();
    }

    void stopScheduler() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stop = true;
        }
        cv.notify_all();   
        for (std::thread &worker : workers) {
            if (worker.joinable()) {
                worker.join();   
            }
        }
    }
};

int main() {
    // Create a task scheduler with 4 worker threads
    TaskScheduler scheduler(4);

    // Add tasks with different priorities
    scheduler.addTask(1, []() { std::cout << "Low priority task executed\n"; });
    scheduler.addTask(3, []() { std::cout << "High priority task executed\n"; });
    scheduler.addTask(2, []() { std::cout << "Medium priority task executed\n"; });
    scheduler.addTask(5, []() { std::cout << "Very high priority task executed\n"; });

    // Let the tasks finish (this is a simple delay to simulate work being done)
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Stop the scheduler
    scheduler.stopScheduler();

    return 0;
}
