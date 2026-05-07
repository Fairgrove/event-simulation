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
    int num_generators = 1'000;

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

        int size() {
            return event_queue.size();
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
                //std::cout << "constructor with member initializer list" << "\n";
                //std::cout << "   Event Generator: " << id << "\n";
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
                //std::cout << "hello from generator - " << id << " - queue size : " << event_queue.size()<< "\n";
            }
        }
};

class Server {
    Config config;
    int id;
    SafeQueue& event_queue;

    int total_events_received = 0;

    public:
        //constructor with member initializer list
        Server(const Config& conf, int generator_id, SafeQueue& eq):
            config(conf), id(generator_id), event_queue(eq)
            {
                //std::cout << "constructor with member initializer list" << "\n";
                //std::cout << "   Event Generator: " << id << "\n";
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


    std::cout << "Firing up " << config.num_generators << " generators" << "\n";
    
    std::vector<EventGenerator> generators;
    for (int i = 0; i < config.num_generators; i++){
        // emplace constructs the generator directly into the vector, which is apparently faster???
        generators.emplace_back(config, i, event_queue); 
    }

    // generator threads
    std::vector<std::thread> generatorThreads;
    for (auto& gen : generators){
        // reference to the i'th instance of generator and 'tells' the thread to run run() from that instance
        generatorThreads.emplace_back(&EventGenerator::run, &gen); 
    }
    std::cout << "   Done" << "\n";
    
    while (true) {
        std::cout << "\r" << "Queue Size : " << event_queue.size() << std::flush;

    }

    //handlerThread.join();
    for (auto& t : generatorThreads) t.join();

    unsigned int threads = std::thread::hardware_concurrency();
    std::cout << "threads: " << threads << "\n";
    return 0;
}

/*
    const auto run_start = std::chrono::high_resolution_clock::now();
    const auto run_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> run_elapsed = run_end - run_start;

    std::cout << "elapsed time: " << run_elapsed.count() << "\n";
    std::cout << "    # events: " << config.num_events << "\n";
*/
