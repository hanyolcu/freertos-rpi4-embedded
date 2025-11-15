// main.cpp - Simple C++ test program
// This verifies your C++ toolchain works

#include <iostream>
#include <string>
#include <vector>

class EmbeddedProject {
private:
    std::string projectName;
    std::vector<std::string> features;

public:
    EmbeddedProject(const std::string& name) : projectName(name) {
        std::cout << "Initializing project: " << projectName << std::endl;
    }

    void addFeature(const std::string& feature) {
        features.push_back(feature);
        std::cout << "Added feature: " << feature << std::endl;
    }

    void showFeatures() const {
        std::cout << "\nProject: " << projectName << std::endl;
        std::cout << "Features implemented:" << std::endl;
        for (size_t i = 0; i < features.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << features[i] << std::endl;
        }
    }
};

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Embedded Systems Project - Raspberry Pi 4" << std::endl;
    std::cout << "========================================\n" << std::endl;

    EmbeddedProject project("FreeRTOS on RPi4");
    
    project.addFeature("UART Communication");
    project.addFeature("GPIO Control");
    project.addFeature("Task Scheduling");
    project.addFeature("Inter-task Communication");
    
    project.showFeatures();
    
    std::cout << "\nC++ toolchain working successfully!" << std::endl;
    
    return 0;
}