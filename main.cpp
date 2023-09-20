#include <iostream>
#include <windows.h>
#include <fileapi.h>
#include <vector>

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
    GetVolumeInformation("C:\\", szVolName, MAX_PATH,
                         &dwSerialNumber, &dwMaxComponentLen,
                         &dwFileSysFlags, szFileSysName, sizeof(szFileSysName));
    std::cout << "Serial Number: " << dwSerialNumber << "\nMax Component Len: " << dwMaxComponentLen << "\nFile System Flags: " << dwFileSysFlags << std::endl;
}

void getDriveInfo(std::string drive) {
    getDriveType(drive);
    getDriveFreeSpace(drive);
    getDriveCommonInfo(drive);
}



int main() {
    int input = -1;
    std::string input_string;
    while (input != 0) {
        std::cout << "1 - Print list of drives\n"
                     "2 - Print disk info\n"
                     "3 - Create directories\n"
                     "4 - Remove directories\n"
                     "5 - Create files\n"
                     "6 - Copy and move files\n"
                     "7 - Analyse and change file attributes\n"
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
                break;
            case 7:
                break;
        }
    }
    return 0;
}
