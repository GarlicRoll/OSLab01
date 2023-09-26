#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

struct OpenFileArgs {
    std::string file_path;
};

void* open_and_create_file(void* args) {
    OpenFileArgs* file_args = static_cast<OpenFileArgs*>(args);
    int fd = open(file_args->file_path.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
        perror(("Failed to create/open file: " + file_args->file_path).c_str());
        return nullptr;
    } else {
        std::cout << "File created/opened: " << file_args->file_path << std::endl;
        close(fd);
    }
    return nullptr;
}

void open_and_create_files_simultaneously(const std::vector<std::string>& file_paths) {
    std::vector<pthread_t> thread_ids(file_paths.size());
    std::vector<OpenFileArgs> thread_args(file_paths.size());

    // Launch a separate thread for each file
    for (size_t i = 0; i < file_paths.size(); ++i) {
        thread_args[i].file_path = file_paths[i];
        if (pthread_create(&thread_ids[i], nullptr, open_and_create_file, &thread_args[i]) != 0) {
            perror("Failed to create thread");
            exit(1);
        }
    }

    // Wait for all threads to complete
    for (size_t i = 0; i < file_paths.size(); ++i) {
        pthread_join(thread_ids[i], nullptr);
    }
}

void printError() {
    const char *error_string = strerror(errno);
    std::cerr << "Error: " << error_string << std::endl;
}

int main() {


    int input = -1;
    std::string input_string;
    std::string input_string_2;
    while (input != 0) {
        std::cout << "1 - Open\\creating files\n"
                     "0 - Exit" << std::endl;
        std::cin >> input;
        switch (input) {
            case 1:
                std::vector<std::string> file_paths = {
                        "file1.txt", "file2.txt", "file3.txt"
                };
                // Call the function on the list of files
                open_and_create_files_simultaneously(file_paths);
                printError();
                break;
        }

        return 0;
    }
}