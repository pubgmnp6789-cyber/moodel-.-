#include <iostream>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <functional>
#include <iomanip>
#include <random>
#include <cmath>
#include <thread>
#include <chrono>

void PrintProgressBar(int percentage,int total_perpercentage,int bar_width = 50) {
    int completed_width = (percentage * bar_width) / 100;
    
    std::string bar;
    for (int i = 0; i < bar_width; ++i) {
        if (i < completed_width) {
            bar += "=";
        } else if (i == completed_width && percentage < 100) {
            bar += ">"; 
        } else {
            bar += " "; 
        }
    }
    
    std::cout << "\r[" << bar << "] " << percentage<<"/"<< total_perpercentage <<std::flush;
}

int main() {
    int epochss = 100;
    double loss = 0.12356;
    double Similarity = 0.888;


    std::cout<<"epochss:"
    <<std::left<<std::setw(6)<<epochss<<"|"
    <<"loss:"
    <<std::left<<std::setw(10)<<loss<<"|"
    <<Similarity<<"\n";
    

    return 0;
}