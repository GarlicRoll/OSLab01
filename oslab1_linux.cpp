//
// Created by gleb on 03.10.23.
//

#include <aio.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <cassert>

using namespace std;


string getFileName() {
    string inputName;
    cout << "Enter file name (/home/username/folder/file.txt): ";
    cin >> inputName;
    return inputName;
}

void getLastError() {
    const char *errorString = strerror(errno);
    cerr << "Error: " << errorString << endl;
}

int openFile(string *fileName){
    int file;
    file = open(reinterpret_cast<const char *>(fileName), O_RDONLY|O_NONBLOCK, 0666);
    if (file == -1) {
        getLastError();
    }
    else {
        cout << "File open successfully!" << endl;
    }
    return file;
}

int createFile(string *file_name) {
    int file;
    file = open(reinterpret_cast<const char *>(file_name), O_CREAT | O_WRONLY | O_TRUNC | O_NONBLOCK, 0666);
    if (file == -1) {
        getLastError();
    }
    else {
        cout << "File create successfully!" << endl;
    }
    return file;
}


int main() {


    ///////////////
    ////////////// Reading
    //////////////



    int blockSize = 40960;
    cout << "Enter block size (in bytes): ";
    cin >> blockSize;


    int file = openFile((string *) getFileName().c_str());
    struct stat st;
    if (fstat(file, &st) == -1) {
        perror("fstat");
        return 1;
    }
    off_t fileSize = st.st_size;

    int nBlocks = fileSize / blockSize;
    if (fileSize % blockSize != 0) {
        ++nBlocks;
    }

    vector<aiocb> blocks(nBlocks);
    vector<char*> buffers(nBlocks);

    auto start1 = chrono::high_resolution_clock::now();

    for (int i = 0; i < nBlocks; i++) {
        aiocb readBlock{};
        char* readBuffer = new char[blockSize];  // dynamically allocated
        memset(readBuffer, ' ', blockSize);      // fill the buffer with spaces

        readBlock.aio_fildes = file;
        readBlock.aio_buf = readBuffer;          // assign the allocated buffer
        readBlock.aio_nbytes = blockSize;
        readBlock.aio_offset = i * blockSize;

        buffers[i] = readBuffer;                 // save the pointer to the buffer
        blocks[i] = readBlock;


    }

    for (int i = 0; i < nBlocks; i++) {
        if (aio_read(&blocks[i]) == -1) {
            perror("aio_read");
            exit(1);
        }
    }

    //waitForAll(blocks); // do i need it
    // Wait for all I/O operations to complete
    for (int i = 0; i < nBlocks; i++) {
        const struct aiocb *aiocb_list[] = { &blocks[i] };
        aio_suspend(aiocb_list, nBlocks, NULL);
    }

    auto end1 = chrono::high_resolution_clock::now();

    ///////////////////
    /////////////////// Writing
    //////////////////

    file = createFile((string *) getFileName().c_str());

    auto start2 = chrono::high_resolution_clock::now();

    for (int i = 0; i < nBlocks; i++) {
        aiocb readBlock{};
        //char* readBuffer = new char[blockSize];  // dynamically allocated
        //memset(readBuffer, ' ', blockSize);      // fill the buffer with spaces

        readBlock.aio_fildes = file;
        readBlock.aio_buf = blocks[i].aio_buf;          // assign the allocated buffer
        readBlock.aio_nbytes = blockSize;
        readBlock.aio_offset = i * blockSize;

        //buffers[i] = readBuffer;                 // save the pointer to the buffer
        blocks[i] = readBlock;

    }

    for (int i = 0; i < nBlocks; i++) {
        if (aio_write(&blocks[i]) == -1) {
            perror("aio_write");
            exit(1);
        }
    }

    //waitForAll(blocks); // do i need it
    // Wait for all I/O operations to complete
    for (int i = 0; i < nBlocks; i++) {
        const struct aiocb *aiocb_list[] = { &blocks[i] };
        aio_suspend(aiocb_list, nBlocks, NULL);
    }

    auto end2 = chrono::high_resolution_clock::now();

    printf("Block Size: %i\nCount Of Blocks: %i\n", blockSize, nBlocks);

    chrono::duration<double> elapsed1 = end1 - start1;
    chrono::duration<double> elapsed2 = end2 - start2;
    std::cout << "Coping operation took: " << elapsed1.count() + elapsed2.count() << " seconds.\n";

    return 0;
}
