#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>

#include <thread>

struct Config{
    int num_events = 1'000;
    int num_servers = 2;
    std::vector<int> eventTime = {50'000, 1'000'000};
    
    double success_chance = 0.99;
};

struct Event{
    int complexity;
    bool success; // packet loss
    std::chrono::duration<double> execution_time;
    std::chrono::duration<double> timestamp;
};

// thread safe queue for events
class SafeQueue {
    std::queue<Event> event_queue;
    std::mutex mutex;
    std::condition_variable cv;

    public:
        void push(Event e) {
            std::lock_guard<std::mutex> lock(mutex);
            event_queue.push(std::move(e));
            cv.notify_one();  // wake up handler
        }

        Event pop() {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this]{ return !event_queue.empty(); }); // sleep until data arrives
            Event e = std::move(event_queue.front());
            event_queue.pop();
            return e;
        }
};

class EventGenerator {
    Config config;
    int id;
    SafeQueue& event_queue;

    public:
        //constructor with member initializer list
        EventGenerator(const Config& conf, int generator_id, SafeQueue& eq):
            config(conf), id(generator_id), event_queue(eq)
            {
                std::cout << "constructor with member initializer list" << "\n";
                std::cout << "   Event Generator: " << id << "\n";
            }
        
        void run() {
            // create random generators here
            std::mt19937 rng(std::random_device{}());
            
            std::bernoulli_distribution success_dist; 
            std::uniform_int_distribution<uint32_t> complexity_dist;
            std::uniform_int_distribution<uint32_t> server_dist;
            
            std::uniform_int_distribution<> delay(100, 1000);
            
            while (true){
                // Creating Event
                Event e;
                e.complexity = complexity_dist(rng);
                e.success = success_dist(rng);
                
                // lag
                std::this_thread::sleep_for(std::chrono::milliseconds(delay(rng)));
                
                // send event to queue
                event_queue.push(e);
            }
        }
};

void simulate_work(int complexity){
    int i = 0;
    while (i < complexity){
        i++;
    }
}

int main(){
    Config config;
    SafeQueue event_queue;
    EventGenerator generator (config, 1, event_queue);
    generator.run();

    const auto run_start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < config.num_events; i++){
        //Event e = generator.next();

        //simulate_work(e.complexity);


        //const auto start = std::chrono::high_resolution_clock::now();

        
        //const auto end = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double> elapsed = end - start;
        
        /*
        std::cout << "      server: " << server_dist(rng) << "\n";
        std::cout << "     success: " << success_dist(rng) << "\n";
        std::cout << "    duration: " << duration_dist(rng) << "\n";
        std::cout << "        time: " << elapsed.count() << "\n\n";
        */
    }
    
    const auto run_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> run_elapsed = run_end - run_start;

    std::cout << "elapsed time: " << run_elapsed.count() << "\n";
    std::cout << "    # events: " << config.num_events << "\n";

    unsigned int threads = std::thread::hardware_concurrency();
    std::cout << "threads: " << threads << "\n";
    return 0;
}
