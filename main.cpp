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
    int numServers = 1;
    std::vector<int> eventTime = {5, 100};
    
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


int main(){
    Config config;

    for (int i = 0; i < config.numEvents; i++){
        std::mt19937 rng(std::random_device{}());

        std::uniform_int_distribution<uint32_t> server_dist(1, config.numServers);
        std::uniform_int_distribution<uint32_t> duration_dist(config.eventTime[0], config.eventTime[1]);
        std::bernoulli_distribution success_dist(config.successChance);

        std::cout << "    server: " << server_dist(rng) << "\n";
        std::cout << "  duration: " << duration_dist(rng) << "\n";
        std::cout << "   success: " << success_dist(rng) << "\n\n";
    }

    return 0;
}
