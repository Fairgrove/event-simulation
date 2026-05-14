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
    int num_servers = 10;
    int num_generators = 1000;

    std::vector<int> event_complexity_extremes = {50'000, 1'000'000};
    
    double success_chance = 0.99;
};

struct Event{
    int device_id;
    int event_id;
    
    int complexity;

    bool success; // packet loss
    
    std::chrono::duration<double> execution_time;
    std::chrono::duration<double> timestamp;
};

// had some help with this one from AI.
// Cannot create instances if the server class when they have an instance of a class with
// non-movable attributes `std::mutex` is non-movable
class SafeQueue {
    std::queue<Event> event_queue;
    std::unique_ptr<std::mutex> mutex = std::make_unique<std::mutex>();
    std::unique_ptr<std::condition_variable> cv = std::make_unique<std::condition_variable>();

    public:
        void push(Event e) {
            std::lock_guard<std::mutex> lock(*mutex);
            event_queue.push(std::move(e));
            cv->notify_one();
        }

        Event pop() {
            std::unique_lock<std::mutex> lock(*mutex);
            cv->wait(lock, [this]{ return !event_queue.empty(); });
            Event e = std::move(event_queue.front());
            event_queue.pop();
            return e;
        }

        int size() {
            std::lock_guard<std::mutex> lock(*mutex); // also fix: size() should lock!
            return event_queue.size();
        }
};

class Server {
    private:
        // initial members
        Config config;
        int id;
        
        // defined in constructor body
        std::mt19937 rng;

        // metrics
        bool running = false;

        // methods
        void simulate_work(int complexity){
            int i = 0;
            
            while (i < complexity) {
                i++;
            }
            
            // std::this_thread::sleep_for(std::chrono::milliseconds(complexity));
        }

    public:
        SafeQueue event_queue;
        int total_events_processed = 0;
        
        //constructor with member initializer list
        Server(const Config& conf, int server_id):
            config(conf), id(server_id), 
            rng(std::random_device{}())
            {}
        
        void run(){
            running = true;

            while (running) {
                Event e = event_queue.pop();
                simulate_work(e.complexity);
                total_events_processed++;
            }
        }
};

class EventGenerator {
    private:
        // initial memebers
        Config config;
        int id;
        std::vector<Server>& servers;

        // defined in constructor body
        std::mt19937 rng;
        Server& server;
        SafeQueue& event_queue;

        // metrics
        int event_counter = 0;
        bool running = false;

        // methods
        Server& selectServer(std::vector<Server>& serv_lst) {
            std::uniform_int_distribution<> server_id(0, config.num_servers-1);
            return serv_lst[server_id(rng)];
        }

    public:
        //constructor with member initializer list
        EventGenerator(const Config& conf, int generator_id, std::vector<Server>& serv_lst):
            config(conf), id(generator_id), servers(serv_lst),
            rng(std::random_device{}()),
            server(selectServer(serv_lst)),
            event_queue(server.event_queue)
            {}
        
        void run() {
            running = true;

            // create random generators here
            
            std::bernoulli_distribution success_dist; 
            std::uniform_int_distribution<uint32_t> complexity_dist(config.event_complexity_extremes[0], config.event_complexity_extremes[1]);
            std::uniform_int_distribution<uint32_t> server_dist;
            
            std::uniform_int_distribution<> delay(100, 1000);
            
            while (running){
                // Creating Event
                Event e;
                e.complexity = complexity_dist(rng);
                e.success = success_dist(rng);
                e.device_id = id;
                e.event_id = event_counter;
                
                // lag
                std::this_thread::sleep_for(std::chrono::milliseconds(delay(rng)));
                
                // send event to queue
                event_queue.push(e);
                event_counter++;
                //std::cout << "hello from generator - " << id << " - queue size : " << event_queue.size()<< "\n";
            }
        }
};

int main(){
    Config config;

    std::mt19937 rng(std::random_device{}());
    
    std::vector<Server> servers;
    std::vector<EventGenerator> generators;
    
    std::vector<std::thread> serverThreads;
    std::vector<std::thread> generatorThreads;

    /*
     *  Starting Servers
     */
    
    std::cout << "Firing up " << config.num_servers << " Servers" << "\n";
    for (int i = 0; i < config.num_servers; i++){
        // emplace constructs the generator directly into the vector, which is apparently faster???
        servers.emplace_back(config, i); 
    }

    // generator threads
    for (auto& serv : servers){
        // reference to the i'th instance of generator and 'tells' the thread to run run() from that instance
        serverThreads.emplace_back(&Server::run, &serv); 
    }
    std::cout << "   Done" << "\n\n";
    
    /*
     *  Starting Devics
     */
    std::cout << "Firing up " << config.num_generators << " generators" << "\n";
    for (int i = 0; i < config.num_generators; i++){
        // emplace constructs the generator directly into the vector, which is apparently faster???
        generators.emplace_back(config, i, servers); 
    }

    // generator threads
    for (auto& gen : generators){
        // reference to the i'th instance of generator and 'tells' the thread to run run() from that instance
        generatorThreads.emplace_back(&EventGenerator::run, &gen); 
    }
    std::cout << "   Done" << "\n";
    
    while (true) {
        // Print your lines
        for (int i = 0; i < config.num_servers; i++){
            std::cout << "Server" << i << "queue size: " << servers[i].event_queue.size() << "  (" << servers[i].total_events_processed << ")\n";
        }
        std::cout << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // Move cursor back up N lines
        for (int i = 0; i < config.num_servers+1 ; i++) {
            std::cout << "\033[A"  // move up one line
                      << "\033[2K"; // clear the line
        }
    }

    for (auto& t : generatorThreads) t.join();

    return 0;
}

/*
    const auto run_start = std::chrono::high_resolution_clock::now();
    const auto run_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> run_elapsed = run_end - run_start;

    std::cout << "elapsed time: " << run_elapsed.count() << "\n";
    std::cout << "    # events: " << config.num_events << "\n";
    
    unsigned int threads = std::thread::hardware_concurrency();
    std::cout << "threads: " << threads << "\n";
*/
