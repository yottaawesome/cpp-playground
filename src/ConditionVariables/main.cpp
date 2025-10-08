import std;

// See https://en.cppreference.com/w/cpp/thread/latch.html
namespace LatchExample
{
    struct Job
    {
        const std::string name;
        std::string product{ "not worked" };
        std::thread action{};
    };

    void Run()
    {
        Job jobs[]{ {"Annika"}, {"Buru"}, {"Chuck"} };

        std::latch work_done{ std::size(jobs) };
        std::latch start_clean_up{ 1 };

        auto work = 
            [&](Job& my_job)
            {
                my_job.product = my_job.name + " worked";
                work_done.count_down();
                start_clean_up.wait();
                my_job.product = my_job.name + " cleaned";
            };

        std::cout << "Work is starting... ";
        for (auto& job : jobs)
            job.action = std::thread{ work, std::ref(job) };

        work_done.wait();
        std::cout << "done:\n";
        for (auto const& job : jobs)
            std::cout << "  " << job.product << '\n';

        std::cout << "Workers are cleaning up... ";
        start_clean_up.count_down();
        for (auto& job : jobs)
            job.action.join();

        std::cout << "done:\n";
        for (auto const& job : jobs)
            std::cout << "  " << job.product << '\n';
    }
}

// See https://en.cppreference.com/w/cpp/thread/condition_variable
namespace ConditionVariableExample
{
    std::mutex m;
    std::condition_variable cv;
    std::string data;
    bool ready = false;
    bool processed = false;

    void worker_thread()
    {
        // Wait until main() sends data
        std::unique_lock lk(m);
        cv.wait(lk, [] {return ready; });

        // after the wait, we own the lock.
        std::cout << "Worker thread is processing data\n";
        data += " after processing";

        // Send data back to main()
        processed = true;
        std::cout << "Worker thread signals data processing completed\n";

        // Manual unlocking is done before notifying, to avoid waking up
        // the waiting thread only to block again (see notify_one for details)
        lk.unlock();
        cv.notify_one();
    }

    void Run()
    {
        std::thread worker(worker_thread);

        data = "Example data";
        // send data to the worker thread
        {
            std::lock_guard lk(m);
            ready = true;
            std::cout << "main() signals data ready for processing\n";
        }
        cv.notify_one();

        // wait for the worker
        {
            std::unique_lock lk(m);
            cv.wait(lk, [] {return processed; });
        }
        std::cout << "Back in main(), data = " << data << '\n';

        worker.join();
    }
}

int main()
{
    return 0;
}
