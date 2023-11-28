#include <Windows.h>
#include <iostream>

using namespace std;
void printSystemInfo(SYSTEM_INFO siSysInfo) {

    // Считываем информацию о системе в структуру siSysInfo

    GetSystemInfo(&siSysInfo);

    // Отображаем информацию о системе из структуры

    printf("Hardware information: \n");
    printf(" Processor Architecture: ");
    switch (siSysInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            std::cout << "x64" << std::endl;
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
            std::cout << "ARM" << std::endl;
            break;
        case PROCESSOR_ARCHITECTURE_IA64:
            std::cout << "Itanium-based" << std::endl;
            break;
        case PROCESSOR_ARCHITECTURE_INTEL:
            std::cout << "x86" << std::endl;
            break;
        case PROCESSOR_ARCHITECTURE_UNKNOWN:
        default:
            std::cout << "Unknown" << std::endl;
            break;
    }
    cout << "The page size: " << siSysInfo.dwPageSize << endl;
    cout << "The lowest memory address accessible: " << siSysInfo.lpMinimumApplicationAddress << endl;
    cout << "The highest memory address accessible: " << siSysInfo.lpMaximumApplicationAddress << endl;
    cout << "A mask representing the set of processors\n"
            "configured into the system: "  <<(siSysInfo.dwActiveProcessorMask) << endl;
    cout << "The number of logical processors in the current group: " << siSysInfo.dwNumberOfProcessors << endl;
    cout << "The granularity for the starting address \nat which virtual memory can be allocated: "
         << siSysInfo.dwAllocationGranularity << endl;
    cout << "The architecture-dependent processor level: " << siSysInfo.wProcessorLevel << endl;
    cout << "The architecture-dependent processor revision: " << siSysInfo.wProcessorRevision << "\n\n";
}

// Определение статуса виртуальной памяти

void getMemoryStatus()
{
    MEMORYSTATUS memStatus;
    GlobalMemoryStatus(&memStatus);
    std::cout << "Total physical memory: " << memStatus.dwTotalPhys << " bytes" << std::endl;
    std::cout << "Available physical memory: " << memStatus.dwAvailPhys << " bytes" << std::endl;
    std::cout << "Total virtual memory: " << memStatus.dwTotalVirtual << " bytes" << std::endl;
    std::cout << "Available virtual memory: " << memStatus.dwAvailVirtual << " bytes" << std::endl;
}

// Определение состояния конкретного участка памяти по
// заданному с клавиатуры адресу

void getMemoryState()
{
    LPVOID address;
    MEMORY_BASIC_INFORMATION info;
    std::cout << "Enter the memory address: 0x";
    cin >>hex >> address;
    if (VirtualQuery(address, &info, sizeof(info)) == 0) {
        cout << "Error 0x" << GetLastError() << "\n";
    }
    cout << "A pointer to the base address of the region of pages to be queried: " << hex<<address << endl;
    cout << "A pointer to the base address of the region of pages: " << hex << info.BaseAddress << endl;
    cout << "A pointer to the base address of a range of pages allocated by the VirtualAlloc function: " << info.AllocationBase << endl;
    cout << "The memory protection option when the region was initially allocated: " <<
         info.AllocationProtect << ((info.AllocationProtect != 0) ? "" : " (caller does not have access)") << endl;
    cout << "The size of the region beginning at the base address \n"
            "in which all pages have identical attributes, in bytes: " << info.RegionSize << endl;
    cout << "The state of the pages in the region:";
    switch (info.State)
    {
        case MEM_COMMIT:
            cout << "Indicates committed pages for which physical storage has been allocated, either in memory or in the paging file on disk." << endl;
            break;
        case MEM_FREE:
            cout << "Indicates free pages not accessible to the calling process and available to be allocated\n";
            break;
        case MEM_RESERVE:
            cout << "Indicates reserved pages where a range of the process's virtual address space is reserved without any physical storage being allocated." << endl;
            break;
    }
    cout << "The access protection of the pages in the region: ";
    if (info.Protect & PAGE_EXECUTE) cout << "PAGE_EXECUTE\n";
    if (info.Protect & PAGE_EXECUTE_READ) cout << "PAGE_EXECUTE_READ\n";
    if (info.Protect & PAGE_EXECUTE_READWRITE) cout << "PAGE_EXECUTE_READWRITE\n";
    if (info.Protect & PAGE_EXECUTE_WRITECOPY) cout << "PAGE_EXECUTE_WRITECOPY\n";
    if (info.Protect & PAGE_NOACCESS) cout << "PAGE_NOACCESS\n";
    if (info.Protect & PAGE_READONLY) cout << "PAGE_READONLY\n";
    if (info.Protect & PAGE_READWRITE) cout << "PAGE_READWRITE\n";
    if (info.Protect & PAGE_WRITECOPY) cout << "PAGE_WRITECOPY\n";
    if (info.Protect & PAGE_TARGETS_INVALID) cout << "PAGE_TARGETS_INVALID\n";
    if (info.Protect & PAGE_TARGETS_NO_UPDATE) cout << "PAGE_TARGETS_NO_UPDATE\n";
    cout << "\nThe type of pages in the region:";
    switch (info.Type)
    {
        case MEM_IMAGE:
            cout << "Indicates that the memory pages within the region are mapped into the view of an image section." << endl;
            break;
        case MEM_MAPPED:
            cout << "Indicates that the memory pages within the region are mapped into the view of a section.\n";
            break;
        case MEM_PRIVATE:
            cout << "Indicates that the memory pages within the region are private (that is, not shared by other processes)." << endl;
            break;
        default:
            cout << "Unknown type\n";
    }
}

// Резервирование региона в автоматическом режиме и в режиме
// ввода адреса начала региона

void reserveMemoryRegion(SYSTEM_INFO sys) {
    LPVOID address = nullptr;
    int option = 0;
    cout << "What do you wanna choose?\n";
    cout << "1 - reserve the region in automatic mode\n";
    cout << "2 - reserve the region in the mode enter the address of the beginning of the region\n";

    while (option != 1 && option != 2) {
        cin >> option;
    }
    if (option == 2) {
        std::cout << "Enter the memory address: 0x";
        cin >> std::hex >> address;
    }
    if (address = VirtualAlloc(address, sys.dwPageSize, MEM_RESERVE, PAGE_READWRITE)) cout << "Reserved memory region starting at address: " << address << endl;
    else  cout << "Memory hasnt allocated, Erorr: " << GetLastError() << endl;
}

// Резервирование региона и передача ему физической памяти в
// автоматическом режиме и в режиме ввода адреса начала региона

void reserveMemoryRegionWithMemory(SYSTEM_INFO sys) {
    LPVOID address = nullptr;
    int option = 0;
    cout << "What do you wanna choose?\n";
    cout << "1 - reserve the region in automatic mode\n";
    cout << "2 - reserve the region in the mode enter the address of the beginning of the region\n";

    while (option != 1 && option != 2) {
        cin >> option;
    }
    if (option == 2) {
        std::cout << "Enter the memory address: 0x";
        cin >> std::hex >> address;
    }
    if( address = VirtualAlloc(address, sys.dwPageSize, MEM_RESERVE| MEM_COMMIT, PAGE_READWRITE)) cout << "Reserved memory region starting at address: " << address << endl;
    else  cout << "No memory has been allocated. Error: " << GetLastError()<<  endl;
}

// Ввод данных

void writeData() {
    LPVOID address = nullptr;
    string data;
    std::cout << "Enter the memory address: 0x";
    cin >> std::hex >> address;
    std::cout << "Enter data:";
    cin >> data;
    if(CopyMemory(address,data.c_str(),data.length()*sizeof(char)))  cout << "Data recorded " << endl;
    else  cout << "No memory has been allocated. Error: " << GetLastError() << endl;
}

// Установка защиты доступа для заданного (с клавиатуры) региона
// памяти и ее проверку

void setProtection() {
    LPVOID address = NULL;
    int protect=-1;
    DWORD newlevel=0, old;
    std::cout << "Enter the memory address: 0x";
    cin >> std::hex >> address;
    cout << "Chose protection:\n";
    cout << "1 - PAGE_EXECUTE\n";
    cout << "2 - PAGE_EXECUTE_READ\n";
    cout << "3 - PAGE_EXECUTE_READWRITE\n";
    cout << "4 - PAGE_EXECUTE_WRITECOPY\n";
    cout << "5 - PAGE_NOACCESS\n";
    cout << "6 - PAGE_READONLY\n";
    cout << "7 - PAGE_READWRITE\n";
    cout << "8 - PAGE_WRITECOPY\n";
    cout << "9 - PAGE_TARGETS_INVALID\n";
    cout << "10- PAGE_TARGETS_NO_UPDATE\n";
    cout << "Enter your choice: ";
    while (protect < 1 || protect > 10) {
        cin >> protect;
        if (protect < 1 || protect > 10) cout << "Wrong option\n";
    }
    switch (protect)
    {
        case 1:
            newlevel = PAGE_EXECUTE;
            break;
        case 2:
            newlevel = PAGE_EXECUTE_READ; break;
        case 3:
            newlevel = PAGE_EXECUTE_READWRITE; break;
        case 4:
            newlevel = PAGE_EXECUTE_WRITECOPY; break;
        case 5:
            newlevel = PAGE_NOACCESS; break;
        case 6:
            newlevel = PAGE_READONLY; break;
        case 7:
            newlevel = PAGE_READWRITE; break;
        case 8:
            newlevel = PAGE_WRITECOPY; break;
        case 9:
            newlevel = PAGE_TARGETS_INVALID; break;
        case 10:
            newlevel = PAGE_TARGETS_NO_UPDATE; break;
    }

    if (VirtualProtect(address, sizeof(DWORD), newlevel, &old)) cout << "Protection Changed\n";
    else cout << "Error: " << GetLastError() << endl;
}

// Возврат физической памяти и освобождение региона адресного
// пространства заданного (с клавиатуры) региона памяти

void memFree() {
    LPVOID address = nullptr;
    std::cout << "Enter the memory address: 0x";
    cin >> std::hex >> address;
    cout << "Physical memory:" << reinterpret_cast<char*>(address) << endl;
    if (VirtualFree(address, 0, MEM_RELEASE)) {
        cout << "Memory Free\n";
    } else cout << "Error: " << GetLastError() << endl;
    cout << "Error: " << GetLastError() << endl;
}

void menu(int& option) {
    option = -1;
    while (option < 0 || option > 8) {
        cout << "What command do you want to execute?\n";
        cout << "1 - Getting information about the computer system\n";
        cout << "2 - Determining the status of virtual memory\n";
        cout << "3 - Determining the state of a specific memory area by\n"
                "the address specified from the keyboard\n";
        cout << "4 - Reservation of the region in automatic mode and in the mode\n"
                "enter the address of the beginning of the region\n";
        cout << "5 - Reservation of the region and transfer of physical memory to it in automatic\n"
                "mode and in the mode of entering the address of the beginning of the region\n";
        cout << "6 - Writing data to memory cells at the addresses specified from the keyboard\n";
        cout << "7 - Installing access protection for a given (from the keyboard)\n"
                " memory region and checking it\n";
        cout << "8 - Returning physical memory and freeing the address\n"
                "space region of the specified (from the keyboard) memory region\n";
        cout << "0 - Exit\n";
        cout << "Enter your choice: ";
        cin >> option;
        system("cls");
        if (option < 0 || option > 8) cout << "Wrong option\n";
    }
}

int main()
{
    // Меню
    SYSTEM_INFO siSysInfo;
    GetSystemInfo(&siSysInfo);
    int option = -1;
    do {
        menu(option);
        switch (option) {
            case 0:
                break;
            case 1:
                printSystemInfo(siSysInfo);
                break;
            case 2:
                getMemoryStatus();
                break;
            case 3:
                getMemoryState();
                break;
            case 4:
                reserveMemoryRegion(siSysInfo);
                break;
            case 5:
                reserveMemoryRegionWithMemory(siSysInfo);
                break;
            case 6:
                writeData();
                break;
            case 7:
                setProtection();
                break;
            case 8:
                memFree();
                break;
        }
        cout << "\n\n\n";
    } while (option);

    return 0;
}
