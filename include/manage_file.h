#ifndef manage_file_h
#define manage_file_h

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>
#include <cstdint> // Bổ sung cho uintmax_t

namespace fs = std::filesystem;

namespace File {

    class FileSystemEntity {
    protected:
        fs::path entityPath;
    
    public:
        FileSystemEntity(const fs::path& path) : entityPath(path) {}
        virtual ~FileSystemEntity() {}
        
        virtual void display(int indent = 0) const = 0;
        
        fs::path getPath() const { return entityPath; }
        std::string getName() const { return entityPath.filename().string(); }
        void setPath(const fs::path& newPath) { entityPath = newPath; }
    };
    
    class File : public FileSystemEntity {
    public:
        File(const fs::path& path, const std::string& initialData = "") 
            : FileSystemEntity(path) {
            write(initialData); 
        }
    
        void write(const std::string& newData) {
            std::ofstream ofs(entityPath, std::ios::out);
            if (ofs.is_open()) {
                ofs << newData;
                ofs.close();
            } else {
                std::cout << "[OS Error] Cannot write to file: " << getName() << "\n";
            }
        }
    
        void append(const std::string& extraData) {
            std::ofstream ofs(entityPath, std::ios::app);
            if (ofs.is_open()) {
                ofs << extraData;
                ofs.close();
            } else {
                std::cout << "[OS Error] Cannot append to file: " << getName() << "\n";
            }
        }
    
        void read() const {
            if (!fs::exists(entityPath)) {
                std::cout << "[Error] File does not exist on disk!\n";
                return;
            }
            std::ifstream ifs(entityPath);
            std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
            std::cout << "\n📖 [Reading file: " << getName() << "]\n" << content << "\n";
        }
    
        void display(int indent = 0) const override {
            for (int i = 0; i < indent; ++i) std::cout << "  ";
            uintmax_t size = fs::exists(entityPath) ? fs::file_size(entityPath) : 0;
            std::cout << "📄 " << getName() << " (" << size << " bytes)\n";
        }
    };
    
    class Folder : public FileSystemEntity {
    private:
        std::vector<std::shared_ptr<FileSystemEntity>> children;
    
    public:
        Folder(const fs::path& path) : FileSystemEntity(path) {
            if (!fs::exists(entityPath)) {
                fs::create_directories(entityPath);
            }
        }
    
        std::shared_ptr<Folder> createSubFolder(const std::string& subName) {
            fs::path subPath = entityPath / subName;
            auto subFolder = std::make_shared<Folder>(subPath);
            children.push_back(subFolder);
            return subFolder;
        }
    
        std::shared_ptr<File> createFile(const std::string& fileName, const std::string& data = "") {
            fs::path filePath = entityPath / fileName;
            auto newFile = std::make_shared<File>(filePath, data);
            children.push_back(newFile);
            return newFile;
        }
    
        void addExistingFile(std::shared_ptr<File> fileObj) {
            if (!fileObj) return;
    
            fs::path targetPath = this->entityPath / fileObj->getName();
    
            try {
                if (fs::exists(fileObj->getPath()) && fileObj->getPath() != targetPath) {
                    fs::copy(fileObj->getPath(), targetPath, fs::copy_options::overwrite_existing);
                }
                fileObj->setPath(targetPath);
                children.push_back(fileObj);
                std::cout << "[System] Copied/added file '" << fileObj->getName() << "' to folder [" << getName() << "].\n";
    
            } catch (const fs::filesystem_error& e) {
                std::cout << "[OS Error] Cannot copy file: " << e.what() << "\n";
            }
        }
    
        void display(int indent = 0) const override {
            for (int i = 0; i < indent; ++i) std::cout << "  ";
            std::cout << "📁 [" << getName() << "]\n";
            for (const auto& child : children) {
                child->display(indent + 2);
            }
        }
    };
}

#endif