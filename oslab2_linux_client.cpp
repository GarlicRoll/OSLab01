#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

#define FILENAME "shared_file.txt"
#define FILESIZE 4096

int main() {
    int fd = open(FILENAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        std::cerr << "Failed to get file size" << std::endl;
        close(fd);
        return 1;
    }

    // Ожидаем доступности чтения из файла
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    struct timeval timeout;
    timeout.tv_sec = 5; // Ожидаем 5 секунд
    timeout.tv_usec = 0;

    std::cout << "Waiting for availability to read..." << std::endl;
    int result = select(fd + 1, &read_fds, NULL, NULL, &timeout);

    if (result == -1) {
        std::cerr << "Failed to wait for availability to read" << std::endl;
        close(fd);
        return 1;
    } else if (result == 0) {
        std::cout << "Timeout occurred. Exiting..." << std::endl;
        close(fd);
        return 0;
    }

    // Отображаем файл в память
    char* ptr = nullptr;


    // Меню клиента
    char choice;
    do {
        std::cout << "Client Menu:" << std::endl;
        std::cout << "1. Perform projection" << std::endl;
        std::cout << "2. Read data" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case '1': {
                // Выполняем проецирование
                std::cout << "Performing projection" << std::endl;
                // Отображаем файл в память
                ptr = static_cast<char*>(mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
                if (ptr == MAP_FAILED) {
                    std::cerr << "Failed to mmap" << std::endl;
                    close(fd);
                    return 1;
                }
                break;
            }
            case '2': {
                // Читаем данные
                char buffer[FILESIZE];
                std::copy(ptr, ptr + st.st_size, buffer);
                buffer[st.st_size] = '\0'; // Добавляем символ конца строки

                std::cout << "Data read: " << buffer << std::endl;

                break;
            }
            case '3': {
                // Завершаем работу
                std::cout << "Exiting..." << std::endl;
                break;
            }
            default: {
                std::cout << "Invalid choice" << std::endl;
            }
        }
    } while (choice != '3');

    // Отменяем проецирование
    munmap(ptr, st.st_size);
    close(fd);

    return 0;
}
