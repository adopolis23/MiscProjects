
#include <iostream>
#include <fstream>
#include <random>
#include <string>

#define EXIT_FAIL 1
#define EXIT_SUCCESS 0



auto main(int argc, char** argv) -> int
{
    std::random_device rd;                        
    std::mt19937 gen(rd());                      
    std::uniform_int_distribution<int> dist1(0, 25);
    std::uniform_int_distribution<int> dist2(0, 100000);
    
    std::ofstream file("sensor_readings.raw", std::ios::out);

    constexpr int iterations = 1e6;

    for (int i = 0; i < iterations; i++)
    {
        char sensor = 'A' + dist1(gen);
        int reading = dist2(gen);

        file << sensor << ":" << std::to_string(reading) << "\n";
    }

    file.close();
}
