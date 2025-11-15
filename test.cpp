#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <string>
#include <thread>
using namespace std;
std::string input;
void print(const std::string& text) {
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);
    std::string date = std::asctime(localTime);
    date.pop_back(); 
    std::cout << date + " : " + text << std::endl;
}
//void Check_For_Updates(){
  //  std::this_thread::sleep_for(std::chrono::seconds(15));
    //int Aquium_Updator_ex = system("/home/gamerpc/Aquium_Updator_Project/AquiumUpdater -Wl");
    //if (Aquium_Updator_ex == 0){
     //   std::cout << "LOG: Checked for updates!" << std::endl;
    //}
//}
int InstallSystemdService() {
    const char* servicePath = "/etc/systemd/system/Gamer_Module.service";

    // Check if service already exists
    ifstream check(servicePath);
    if (check.good()) {
        print("Service already installed.");
        return 1;
    }
    check.close();

    print("Installing systemd service...");

    // Create service contents
    const char* serviceContent =
    "[Unit]\n"
    "Description=Gamer LED Module\n"
    "After=multi-user.target\n"
    "DefaultDependencies=yes\n\n"
    "[Service]\n"
    "Type=simple\n"
    "ExecStart=/home/gamer/LED_MODULE/LED_MODULE\n"
    "WorkingDirectory=/home/gamer/LED_MODULE\n"
    "User=root\n"
    "Group=root\n"
    "Restart=on-failure\n\n"
    "[Install]\n"
    "WantedBy=multi-user.target\n";

    // Write service file (requires root)
    ofstream out(servicePath);
    if (!out.is_open()) {
        print("ERROR: Cannot write to systemd directory! Run as sudo.");
        return 2;
    }
    out << serviceContent;
    out.close();

    print("Service file created.");

    // Reload systemd
    system("systemctl daemon-reload");

    // Enable service on boot
    system("systemctl enable Gamer_Module.service");

    // Start service immediately
    system("systemctl start Gamer_Module.service");

    print("Systemd service installed and started!");
    return 0;

}

int Check_If_BacklightsOff() {
    std::ifstream brightnessFile("/sys/class/leds/" + input + "::scrolllock/brightness");
    int status = -1;
    
    if (brightnessFile.is_open()) {
        brightnessFile >> status; 
        brightnessFile.close();
    }

    return status;
}


void set_backlight_on() {
    std::ofstream brightnessFile("/sys/class/leds/" + input + "::scrolllock/brightness");
    if (brightnessFile.is_open()) {
        brightnessFile << "1";  
        brightnessFile.close();
    }
}
void CheckForKeyboard() {
    const std::string target = "CASUE USB KB";
    std::ifstream input_devices("/proc/bus/input/devices");

    if (!input_devices.is_open()) {
        std::cerr << "Error: Unable to open /proc/bus/input/devices" << std::endl;
        return;
    }

    std::string line;
    bool found = false;

    std::string bus, vendor, product, version, phys, sysfs, handlers;

    while (std::getline(input_devices, line)) {
        if (line.find("I: Bus=") != std::string::npos && found) break;
        if (line.find(target) != std::string::npos) {
            found = true;
            std::cout << "Keyboard Found: " << target << std::endl;
        }

        if (found) {
            if (line.find("I: Bus=") != std::string::npos) {
                bus = line.substr(line.find("Bus=") + 4, 4);
                vendor = line.substr(line.find("Vendor=") + 7, 4);
                product = line.substr(line.find("Product=") + 8, 4);
                version = line.substr(line.find("Version=") + 8, 4);
            } else if (line.find("P: Phys=") != std::string::npos) {
                phys = line.substr(line.find("Phys=") + 5);
            } else if (line.find("S: Sysfs=") != std::string::npos) {
                sysfs = line.substr(line.find("Sysfs=") + 6);
            } else if (line.find("H: Handlers=") != std::string::npos) {
                handlers = line.substr(line.find("Handlers=") + 9);
            }
        }
    }

    if (found) {
        std::string input_part;
        if (!sysfs.empty()) {
            size_t pos = sysfs.find_last_of('/');
            if (pos != std::string::npos) {
                input_part = sysfs.substr(pos + 1);
            }
        }

        std::cout << std::left << std::setw(20) << "Field" 
                  << std::setw(50) << "Value" << std::endl;
        std::cout << std::string(70, '-') << std::endl;
        std::cout << std::setw(20) << "Bus" << std::setw(50) << bus << std::endl;
        std::cout << std::setw(20) << "Vendor" << std::setw(50) << vendor << std::endl;
        std::cout << std::setw(20) << "Product" << std::setw(50) << product << std::endl;
        std::cout << std::setw(20) << "Version" << std::setw(50) << version << std::endl;
        std::cout << std::setw(20) << "Physical Address" << std::setw(50) << phys << std::endl;
        std::cout << std::setw(20) << "Sysfs Path" << std::setw(50) << sysfs << std::endl;
        std::cout << std::setw(20) << "Handlers" << std::setw(50) << handlers << std::endl;
        std::cout << std::setw(20) << "Input Part" << std::setw(50) << input_part << std::endl;
        input = input_part;
        input_devices.close();
    } else {
        std::cout << "Keyboard not detected!" << std::endl;
    }

    input_devices.close();
}


int main() {
    print("Gamer_Module Daemon Started!");
    print("Programmer: Gamer100_Dev");
    int is_installed = InstallSystemdService();
    CheckForKeyboard();
  //  Check_For_Updates();
    if (is_installed == 1){
        return 1;
    }
    if (is_installed == 0){
        while (true) {
    
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        int status = Check_If_BacklightsOff();
        
        if (status == 0) {  
            print("Backlight is off, turning it on.");
            set_backlight_on();
        }
    }
    }
    return 0;
}
