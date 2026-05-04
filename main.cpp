#include <iostream>
#include <vector>
#include <unordered_map>
#include <deque>
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>

struct Config{
    int num_events = 1'000;
    int numServers = 2;
    std::vector<int> eventTime = {50'000, 1'000'000};
    
    double success_chance = 0.99;
};

struct Event{
    int complexity;
    bool success; // packet loss
    std::chrono::duration<double> execution_time;
    std::chrono::duration<double> timestamp;
};

class EventGenerator {
    private:
        Config config;
        std::mt19937 rng;

        std::uniform_int_distribution<uint32_t> complexity_dist;
        std::bernoulli_distribution success_dist;
        
        //std::uniform_int_distribution<uint32_t> server_dist;
    
    public:
        //constructor with member initializer list
        EventGenerator(const Config& conf):
            config(conf),
            rng(std::random_device{}()),
            success_dist(config.success_chance),
            complexity_dist(config.eventTime[0], config.eventTime[1]){
                std::cout << "constructor with member initializer list" << "\n";
            }
        
        Event next() {
            Event e;
            //e.timestamp = std::chrono::high_resolution_clock::now();
            e.complexity = complexity_dist(rng);
            e.success = success_dist(rng);
            return e;
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
    EventGenerator generator (config);

    const auto run_start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < config.num_events; i++){
        Event e = generator.next();

        simulate_work(e.complexity);


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

    return 0;
}
