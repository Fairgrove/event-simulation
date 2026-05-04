#include <iostream>
#include <vector>
#include <unordered_map>
#include <deque>
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>

struct Config{
    int numEvents = 10;
    int numServers = 2;
    std::vector<int> eventTime = {50'000, 1'000'000};
    
    double successChance = 0.5;
};

struct Event{
    int timestamp;

    bool success;
};

class EventGenerator {
    private:
    public:
        EventGenerator(const Config& config) {
            std::cout << "constructor";
        }
};

void simulate_work(int duration){
    int i = 0;
    while (i < duration){
        i++;
    }
}

int main(){
    Config config;

    const auto run_start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < config.numEvents; i++){
        const auto start = std::chrono::high_resolution_clock::now();
        std::mt19937 rng(std::random_device{}());

        std::uniform_int_distribution<uint32_t> server_dist(1, config.numServers);
        std::uniform_int_distribution<uint32_t> duration_dist(config.eventTime[0], config.eventTime[1]);
        std::bernoulli_distribution success_dist(config.successChance);

        simulate_work(duration_dist(rng));
        
        const auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "      server: " << server_dist(rng) << "\n";
        std::cout << "     success: " << success_dist(rng) << "\n";
        std::cout << "    duration: " << duration_dist(rng) << "\n";
        std::cout << "        time: " << elapsed.count() << "\n\n";
        
    }
    
    const auto run_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> run_elapsed = run_end - run_start;

    std::cout << "elapsed time: " << run_elapsed.count();

    return 0;
}
