

#include <algorithm>
#include <charconv>
#include <fcntl.h>
#include <fstream>
#include <limits>
#include <spanstream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>
#include <span>
#include <map>
#include <ranges>

#define EXIT_FAIL 1
#define EXIT_SUCCESS 0

struct Record
{
    int min{}, max{};
};

static std::map<int, Record> records;


auto PrintRecords(std::map<int, Record>& records) -> void
{
    for (const auto& key : std::views::keys(records))
    {
        const auto& record = records[key];

        char letter = static_cast<char>('A' + key);
        printf("Sensor %c - Min: %d Max: %d\n", letter, record.min, record.max);
    }
}


auto Method1() -> int
{
    std::ifstream file("sensor_readings.raw");

    if (!file.is_open())
        return EXIT_FAIL;

    std::string line;

    // Reads until EOF or an error occurs
    while (std::getline(file, line))
    {
        auto semi = line.find(':');
        auto sensor = line.substr(0, semi);


        auto value_begin = semi + 1;
        auto value = line.substr(value_begin, line.size());

        int val_int;

        [[maybe_unused]]
        auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), val_int);

        auto sensor_number = *sensor.data() - 'A';
        if (auto it = records.find(sensor_number); it != records.end())
        {
            // Sensor record already exists

            if (val_int < it->second.min)
                it->second.min = val_int;

            if (val_int > it->second.max)
                it->second.max = val_int;

        }
        else
        {
            // Insert new sensor into list

            records[sensor_number] = { val_int, val_int };
        }
    }

    PrintRecords(records);

    return EXIT_SUCCESS;
}


auto Method2() -> int
{
    int file_fd = ::open("sensor_readings.raw", O_RDONLY);
    if (file_fd <= 0)
        return EXIT_FAIL;
    
    struct stat sb;
    if (::fstat(file_fd, &sb) == -1) {
        perror("fstat");
        ::close(file_fd);
        return 1;
    }

    if (sb.st_size == 0) {
        std::cout << "(empty file)\n";
        ::close(file_fd);
        return 0;
    }

    void* addr = ::mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0);       
    if (addr == MAP_FAILED) {
        perror("mmap");
        ::close(file_fd);
        return 1;
    }

    std::ispanstream dataStream = std::ispanstream(std::span<char>((char*)addr, sb.st_size));

    auto view = dataStream.span();
    auto begin = view.begin();

    while (begin != view.end())
    {
        const auto semi = std::ranges::find(begin, std::unreachable_sentinel, ':'); 

        std::string_view sensor = { begin, semi };
        int sensor_val = *static_cast<const char*>(sensor.data()) - 'A';

        // Advance the beginning iterator to just after out semicolon
        begin = semi + 1;

        const auto endl = std::ranges::find(begin, std::unreachable_sentinel, '\n'); 

        std::string_view value = { begin, endl };
        int val_int;
        auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), val_int);

        begin = endl + 1;



        if (auto it = records.find(sensor_val); it != records.end())
        {
            // Sensor record already exists

            if (val_int < it->second.min)
                it->second.min = val_int;

            if (val_int > it->second.max)
                it->second.max = val_int;

        }
        else
        {
            // Insert new sensor into list

            records[sensor_val] = { val_int, val_int };
        }

    }


    PrintRecords(records);


    ::munmap(addr, sb.st_size);
    ::close(file_fd);

    return EXIT_SUCCESS;

}


auto main(int argc, char** argv) -> int
{

    if (argc-1 != 1)
    {
        printf("Error: Expected 1 input argument\n");
        return EXIT_FAIL;
    }

    auto ret_val = 0;


    if (*argv[1] == '1')
        ret_val = Method1();
    else if (*argv[1] == '2')
        ret_val = Method2();

    return ret_val;
}
