#include <iostream>
#include <windows.h>
#include <fileapi.h>
#include <vector>
#include <iomanip>

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
    std::cout << "Serial Number: " << dwSerialNumber << "\nMax Component Len: " << dwMaxComponentLen << "\nFile System Flags: " << dwFileSysFlags << std::endl;
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
        std::cerr << "Error opening file: " << GetLastError() << std::endl;
    }

    // Call GetFileInformationByHandle
    BY_HANDLE_FILE_INFORMATION fileInfo;
    BOOL success = GetFileInformationByHandle(fileHandle, &fileInfo);

    if (!success) {
        std::cerr << "Error getting file information: " << GetLastError() << std::endl;
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
        std::cerr << "Error getting file attributes: " << GetLastError() << std::endl;
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
        std::cerr << "Error opening file: " << GetLastError() << std::endl;
    }

    // Set the new last write time to the current time
    FILETIME newLastWriteTime;
    SYSTEMTIME systemTime;
    GetSystemTime(&systemTime); // Get the current system time
    SystemTimeToFileTime(&systemTime, &newLastWriteTime); // Convert the SYSTEMTIME to FILETIME

    // Call SetFileTime to update the last write time
    BOOL success = SetFileTime(fileHandle, nullptr, nullptr, &newLastWriteTime);

    if (!success) {
        std::cerr << "Error setting file time: " << GetLastError() << std::endl;
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
            std::cout << "Error moving files" << std::endl;
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
            std::cout << "Error coping files" << std::endl;
        } else {
            std::cout << "Successfully copied files" << std::endl;
        }
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
                CreateDirectoryA(input_string.c_str(), nullptr);
                break;
            case 4:
                std::cout << "Enter directory (D:\\folder)" << std::endl;
                std::cin >> input_string;
                RemoveDirectoryA(input_string.c_str());
                break;
            case 5:
                std::cout << "Enter new file (D:\\folder\\file.txt)" << std::endl;
                std::cin >> input_string;
                CreateFileA(
                        input_string.c_str(),	// File name
                        GENERIC_READ | GENERIC_WRITE,	// Read-write
                        FILE_SHARE_READ
                        | FILE_SHARE_WRITE,		// Allow sharing-- we're only doing a quick scan
                        NULL,					// No security attributes
                        CREATE_ALWAYS,			// Create an existing file
                        0,						// Ignore file attributes
                        NULL);					// Ignore hTemplateFile

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
