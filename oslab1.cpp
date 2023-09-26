#include <iostream>
#include <windows.h>
#include <fileapi.h>
#include <vector>
#include <iomanip>
#include <string>


void printLastErrorMessage() {
    DWORD error_code = GetLastError();

    if (error_code == 0) {
        std::wcout << L"No error." << std::endl;
        return;
    }

    LPWSTR message_buffer;
    DWORD buffer_length = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            error_code,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&message_buffer,
            0,
            nullptr
    );

    if (buffer_length == 0) {
        std::wcout << L"Failed to retrieve the error message." << std::endl;
        return;
    }

    HANDLE console_output = GetStdHandle(STD_OUTPUT_HANDLE);

    if (console_output == INVALID_HANDLE_VALUE) {
        std::wcout << L"Failed to get console output handle." << std::endl;
        LocalFree(message_buffer);
        return;
    }

    DWORD written;
    WriteConsoleW(console_output, L"Error message: ", 15, &written, nullptr);
    WriteConsoleW(console_output, message_buffer, buffer_length, &written, nullptr);

    LocalFree(message_buffer);
}

void printFileSysFlags(DWORD dwFileSysFlags) {
    struct FlagDefine {
        DWORD flag;
        const char* description;
    };

    static const FlagDefine file_sys_flags[] = {
            { FILE_CASE_SENSITIVE_SEARCH, "Case sensitive search" },
            { FILE_CASE_PRESERVED_NAMES, "Case preserved names" },
            { FILE_UNICODE_ON_DISK, "Unicode on disk" },
            { FILE_PERSISTENT_ACLS, "Persistent ACLs" },
            { FILE_FILE_COMPRESSION, "File compression" },
            { FILE_VOLUME_QUOTAS, "Volume quotas" },
            { FILE_SUPPORTS_SPARSE_FILES, "Supports sparse files" },
            { FILE_SUPPORTS_REPARSE_POINTS, "Supports reparse points" },
            { FILE_SUPPORTS_REMOTE_STORAGE, "Supports remote storage" },
            { FILE_VOLUME_IS_COMPRESSED, "Volume is compressed" },
            { FILE_SUPPORTS_OBJECT_IDS, "Supports object IDs" },
            { FILE_SUPPORTS_ENCRYPTION, "Supports encryption" },
            { FILE_NAMED_STREAMS, "Named streams" },
            { FILE_READ_ONLY_VOLUME, "Read-only volume" },
            { FILE_SEQUENTIAL_WRITE_ONCE, "Sequential write once" },
            { FILE_SUPPORTS_TRANSACTIONS, "Supports transactions" },
    };

    std::cout << "File system flags:" << std::endl;

    for (const auto& flag_def : file_sys_flags) {
        if (dwFileSysFlags & flag_def.flag) {
            std::cout << "  - " << flag_def.description << std::endl;
        }
    }
}

std::vector<std::string> getDrives() {
    std::string final = "Drives: \n";
    DWORD disks = GetLogicalDrives();

    DWORD disk;
    std::string drive;

    std::vector<std::string> drives;

    for (int i = 31; i >= 0; i--) {
        disk = disks;
        disk = disk >> (i);

        disk = disk & 1 ;

        if (disk == 1) {
            final += std::to_string(i) + ") ";
            final += (char) 65 + i;
            final += ":\\ \n";

            drive = (char) 65 + i;
            drive += ":\\";
            drives.push_back(drive);
        }
    }

    std::cout << final << std::endl;

    return drives;
}

void getDriveType(std::string drive) {
    UINT type = GetDriveTypeA(drive.c_str());
    switch (type) {
        case 0:
            std::cout << "Drive Unknown" << std::endl;
            break;
        case 1:
            std::cout << "Drive has no root directory" << std::endl;
            break;
        case 2:
            std::cout << "Removable Drive" << std::endl;
            break;
        case 3:
            std::cout << "Fixed Drive" << std::endl;
            break;
        case 4:
            std::cout << "Remote Drive" << std::endl;
            break;
        case 5:
            std::cout << "CD-ROM Drive" << std::endl;
            break;
        case 6:
            std::cout << "RAM-disk Drive" << std::endl;
            break;
    }
}

void getDriveFreeSpace(std::string drive) {
    DWORD sectorsPerCluster;
    DWORD bytesPerSector;
    DWORD freeClusters;
    DWORD totalClusters;
    GetDiskFreeSpaceA(
            drive.c_str(), //current drive
            &sectorsPerCluster, //sectors per cluster
            &bytesPerSector, //bytes per sector
            &freeClusters, //free clusters
            &totalClusters //total clusters
    );

    unsigned int kBPerCluster = bytesPerSector * sectorsPerCluster / 1024;

    std::cout << "Free Space: " << kBPerCluster * freeClusters / 1024 << " MB" << std::endl;

}

void getDriveCommonInfo(std::string drive) {
    char szVolName[MAX_PATH+1], szFileSysName[MAX_PATH+1];
    DWORD dwSerialNumber, dwMaxComponentLen, dwFileSysFlags;
    GetVolumeInformationA(drive.c_str(), szVolName, MAX_PATH,
                         &dwSerialNumber, &dwMaxComponentLen,
                         &dwFileSysFlags, szFileSysName, sizeof(szFileSysName));
    std::cout << "Serial Number: " << dwSerialNumber << "\nMax Component Len: " << dwMaxComponentLen << std::endl;
    printFileSysFlags(dwFileSysFlags);
}

void getDriveInfo(std::string drive) {
    getDriveType(drive);
    getDriveFreeSpace(drive);
    getDriveCommonInfo(drive);
}

void print_filetime_in_DDMMYYYY(const FILETIME &filetime) {
    SYSTEMTIME systemTime;

    // Convert the FILETIME to SYSTEMTIME
    FileTimeToSystemTime(&filetime, &systemTime);

    // Print the date in the DDMMYYYY format
    std::cout << std::setw(2) << std::setfill('0') << systemTime.wDay
              << "/" << std::setw(2) << std::setfill('0') << systemTime.wMonth
              << "/" << systemTime.wYear << std::endl;
}


void getHandleFileAttributes(std::string file) {
    HANDLE fileHandle = CreateFile(
            file.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
    );

    if(fileHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening file: " << std::endl;
        printLastErrorMessage();
    }

    // Call GetFileInformationByHandle
    BY_HANDLE_FILE_INFORMATION fileInfo;
    BOOL success = GetFileInformationByHandle(fileHandle, &fileInfo);

    if (!success) {
        std::cerr << "Error getting file information: " << std::endl;
        printLastErrorMessage();
        CloseHandle(fileHandle);
    }

    // Print the retrieved file information
    std::cout << "File information:" << std::endl;
    std::cout << "- File size: " << ((static_cast<__int64>(fileInfo.nFileSizeHigh) << 32)
                                     + fileInfo.nFileSizeLow) << " bytes" << std::endl;
    std::cout << "- Creation time: ";
    print_filetime_in_DDMMYYYY(fileInfo.ftCreationTime);


    std::cout << "- Last access time: ";
    print_filetime_in_DDMMYYYY(fileInfo.ftLastAccessTime);

    std::cout << "- Last write time: ";
    print_filetime_in_DDMMYYYY(fileInfo.ftLastWriteTime);


    std::cout << "- Number of links: " << fileInfo.nNumberOfLinks << std::endl;
    std::cout << "- File attributes: " << fileInfo.dwFileAttributes << std::endl;

    // Close the file handle after we're done
    CloseHandle(fileHandle);
}

void getFileAttributes(std::string file) {

    DWORD fileAttributes = GetFileAttributes(file.c_str());

    // Check if the call to GetFileAttributes was successful
    if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "Error getting file attributes: " << std::endl;
        printLastErrorMessage();
    }

    // Print the attributes of the file or directory
    std::cout << "File attributes:" << std::endl;

    if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        std::cout << "- Directory" << std::endl;
    } else {
        std::cout << "- File" << std::endl;
    }

    if (fileAttributes & FILE_ATTRIBUTE_READONLY) {
        std::cout << "- Read-only" << std::endl;
    }

    if (fileAttributes & FILE_ATTRIBUTE_HIDDEN) {
        std::cout << "- Hidden" << std::endl;
    }

    if (fileAttributes & FILE_ATTRIBUTE_SYSTEM) {
        std::cout << "- System" << std::endl;
    }

    if (fileAttributes & FILE_ATTRIBUTE_ARCHIVE) {
        std::cout << "- Archive" << std::endl;
    }
}

void getAllFileAttributes(std::string file) {
    getFileAttributes(file);
    getHandleFileAttributes(file);
}

void setCurrentTimeForFile(std::string file) {

    // Open the file with write access
    HANDLE fileHandle = CreateFile(
            file.c_str(),
            GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
    );

    if (fileHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening file: "  << std::endl;
        printLastErrorMessage();
    }

    // Set the new last write time to the current time
    FILETIME newLastWriteTime;
    SYSTEMTIME systemTime;
    GetSystemTime(&systemTime); // Get the current system time
    SystemTimeToFileTime(&systemTime, &newLastWriteTime); // Convert the SYSTEMTIME to FILETIME

    // Call SetFileTime to update the last write time
    BOOL success = SetFileTime(fileHandle, nullptr, nullptr, &newLastWriteTime);

    if (!success) {
        std::cerr << "Error setting file time: " << std::endl;
        printLastErrorMessage();
        CloseHandle(fileHandle);
    }

    // Close the file handle after we're done
    CloseHandle(fileHandle);
    std::cout << "File time updated successfully." << std::endl;

}

void moveFiles(std::string input_string, std::string input_string_2) {
    if (input_string == input_string_2) {
        std::cout << "Attempt to move file in the same place" << std::endl;
    } else {
        BOOL success = MoveFileA(input_string.c_str(), input_string_2.c_str());
        if (!success) {
            std::cout << "Error moving files"  << std::endl;
            printLastErrorMessage();
        } else {
            std::cout << "Successfully moved files" << std::endl;
        }
    }
}

void copyFiles(std::string input_string, std::string input_string_2) {
    if ( input_string == input_string_2) {
        std::cout << "Attempt to copy file in the same place" << std::endl;
    } else {
        BOOL success = CopyFileA(input_string.c_str(), input_string_2.c_str(), true);
        if (!success) {
            std::cout << "Error coping files"  <<std::endl;
            printLastErrorMessage();
        } else {
            std::cout << "Successfully copied files" << std::endl;
        }
    }
}

void createFile(std::string file) {
    HANDLE fileHandle = CreateFileA(
            file.c_str(),             // File path
            GENERIC_READ | GENERIC_WRITE, // Read and write access
            0,                            // No sharing
            nullptr,                      // No security attributes
            CREATE_NEW,                   // Create a new file only if it doesn't exist
            FILE_ATTRIBUTE_NORMAL,        // Normal file attributes
            nullptr                       // No template file
    );

    if (fileHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Error creating file: " << std::endl;
        printLastErrorMessage();
    }

    std::cout << "File created successfully." << std::endl;
    CloseHandle(fileHandle); // Close the file handle
}

void createDirectory(std::string directory) {
    BOOL success = CreateDirectoryA(directory.c_str(), nullptr);
    if (!success) {
        std::cout << "Error creating directory" << std::endl;
        printLastErrorMessage();
    } else {
        std::cout << "Successfully created directory" << std::endl;
    }
}

void removeDirectory(std::string directory) {
    BOOL success = RemoveDirectoryA(directory.c_str());
    if (!success) {
        std::cout << "Error removing directory"  << std::endl;
        printLastErrorMessage();
    } else {
        std::cout << "Successfully removed directory" << std::endl;
    }
}

// Main function with menu
int main() {
    int input = -1;
    std::string input_string;
    std::string input_string_2;
    while (input != 0) {
        std::cout << "1 - Print list of drives\n"
                     "2 - Print disk info\n"
                     "3 - Create directories\n"
                     "4 - Remove directories\n"
                     "5 - Create files\n"
                     "6 - Copy files\n"
                     "7 - Move files\n"
                     "8 - Analyse file attributes\n"
                     "9 - Change file attributes (change file time to current)\n"
                     "0 - Exit" << std::endl;
        std::cin >> input;
        switch (input) {
            case 1:
                getDrives();
                break;
            case 2:
                std::cout << "Enter drive (D:\\)" << std::endl;
                std::cin >> input_string;
                getDriveInfo(input_string);
                break;
            case 3:
                std::cout << "Enter new directory (D:\\folder)" << std::endl;
                std::cin >> input_string;
                createDirectory(input_string);
                break;
            case 4:
                std::cout << "Enter directory (D:\\folder)" << std::endl;
                std::cin >> input_string;
                removeDirectory(input_string);
                break;
            case 5:
                std::cout << "Enter new file (D:\\folder\\file.txt)" << std::endl;
                std::cin >> input_string;
                createFile(input_string);
                break;
            case 6:
                std::cout << "Enter file to copy (D:\\folder\\file.txt)" << std::endl;
                std::cin >> input_string;
                std::cout << "Enter new file (D:\\folder\\file_2.txt)" << std::endl;
                std::cin >> input_string_2;
                copyFiles(input_string, input_string_2);
                break;
            case 7:
                std::cout << "Enter file to move (D:\\folder\\file.txt)" << std::endl;
                std::cin >> input_string;
                std::cout << "Enter new file directory (D:\\folder\\file_2.txt)" << std::endl;
                std::cin >> input_string_2;
                moveFiles(input_string, input_string_2);
                break;
            case 8:
                std::cout << "Enter file (D:\\folder\\file.txt)" << std::endl;
                std::cin >> input_string;
                getAllFileAttributes(input_string);
                break;
            case 9:
                std::cout << "Enter file (D:\\folder\\file.txt)" << std::endl;
                std::cin >> input_string;
                setCurrentTimeForFile(input_string);
                break;
            default:
                std::cout << "Enter correct number" << std::endl;
        }
    }
    return 0;
}
