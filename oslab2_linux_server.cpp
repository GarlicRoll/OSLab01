#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FILENAME "shared_file.txt"
#define FILESIZE 4096

int main() {
    int fd = open(FILENAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    // Увеличиваем размер файла до нужной величины
    if (ftruncate(fd, FILESIZE) == -1) {
        std::cerr << "Failed to truncate file" << std::endl;
        close(fd);
        return 1;
    }

    // Отображаем файл в память
    char* ptr = nullptr;


    // Меню сервера
    char choice;
    do {
        std::cout << "Server Menu:" << std::endl;
        std::cout << "1. Perform projection" << std::endl;
        std::cout << "2. Write data" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case '1': {
                // Выполняем проецирование
                std::cout << "Performing projection" << std::endl;
                // Отображаем файл в память
                ptr = static_cast<char*>(mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
                if (ptr == MAP_FAILED) {
                    std::cerr << "Failed to mmap" << std::endl;
                    close(fd);
                    return 1;
                }
                break;
            }
            case '2': {
                // Записываем данные
                std::cout << "Enter data to write: ";
                std::cin.ignore(); // Игнорируем символ новой строки после предыдущего ввода
                std::string data;
                std::getline(std::cin, data);

                std::copy(data.begin(), data.end(), ptr); // Копируем данные в память

                std::cout << "Data written successfully" << std::endl;

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

    // Отменяем проецирование и удаляем файл
    munmap(ptr, FILESIZE);
    close(fd);
    unlink(FILENAME);

    return 0;
}
