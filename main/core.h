#ifndef core_h
#define core_h

#include"Matrix.h"
#include "Ann.h"
#include "Model.h"
#include <iostream>
#include <vector>
#include <iostream>
#include <filesystem>

namespace fs=std::filesystem;

namespace core{
    void create_folder(std::string name_folder){
        if(!fs::exists(name_folder)){
            if(fs::create_directories(name_folder)){
                std::cout<<"folder was create\n";
            }
            else{
                std::cout<<"folder!\n";
            }
        }
        else{
            std::cout<<"folder!\n";
        }
    }
    void create_file(std::string name_file){
        
    }
}
    

#endif