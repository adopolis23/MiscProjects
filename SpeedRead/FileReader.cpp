

#include <fcntl.h>
#include <spanstream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>
#include <span>

#define EXIT_FAIL 1
#define EXIT_SUCCESS 0


struct Record
{
    char Sensor{}; 
    int min{}, max{};
};


auto main(int argc, char** argv) -> int
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


    }




    ::munmap(addr, sb.st_size);
    ::close(file_fd);

    return EXIT_SUCCESS;
}
