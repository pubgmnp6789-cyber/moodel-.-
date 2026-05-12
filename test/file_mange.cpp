#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem> // C++17: Thao tác với hệ thống file thật của OS
#include <fstream>    // Ghi/đọc file thật

namespace fs = std::filesystem;

// ================= LỚP CHA CHUNG =================
class FileSystemEntity {
protected:
    fs::path entityPath; // Lưu đường dẫn vật lý thật trên ổ cứng

public:
    FileSystemEntity(const fs::path& path) : entityPath(path) {}
    virtual ~FileSystemEntity() {}
    virtual void display(int indent = 0) const = 0;
    
    fs::path getPath() const { return entityPath; }
    // Lấy tên file/folder từ đường dẫn
    std::string getName() const { return entityPath.filename().string(); }
};

// ================= LỚP FILE =================
class File : public FileSystemEntity {
public:
    // Khi khởi tạo Object File, ta tạo luôn file thật trên ổ cứng
    File(const fs::path& path, const std::string& initialData = "") 
        : FileSystemEntity(path) {
        write(initialData); // Ghi data (nếu chuỗi rỗng thì tạo file rỗng)
    }

    // Ghi đè nội dung vào file TRÊN Ổ CỨNG
    void write(const std::string& newData) {
        std::ofstream ofs(entityPath, std::ios::out);
        if (ofs.is_open()) {
            ofs << newData;
            ofs.close();
        } else {
            std::cout << "[Lỗi] Không thể ghi file: " << getName() << "\n";
        }
    }

    // Ghi nối tiếp nội dung
    void append(const std::string& extraData) {
        std::ofstream ofs(entityPath, std::ios::app);
        if (ofs.is_open()) {
            ofs << extraData;
            ofs.close();
        }
    }

    void display(int indent = 0) const override {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        // Hỏi trực tiếp OS xem file này nặng bao nhiêu byte
        uintmax_t size = fs::exists(entityPath) ? fs::file_size(entityPath) : 0;
        std::cout << "📄 " << getName() << " (" << size << " bytes)\n";
    }
};

// ================= LỚP FOLDER =================
class Folder : public FileSystemEntity {
private:
    std::vector<std::shared_ptr<FileSystemEntity>> children;

public:
    // Khi khởi tạo Folder, ra lệnh cho OS tạo thư mục thật nếu chưa có
    Folder(const fs::path& path) : FileSystemEntity(path) {
        if (!fs::exists(entityPath)) {
            fs::create_directories(entityPath); // Tạo folder thật
        }
    }

    // Tạo folder con thật trên ổ cứng
    std::shared_ptr<Folder> createSubFolder(const std::string& subName) {
        // Toán tử '/' của fs::path dùng để nối đường dẫn cực kỳ thông minh
        fs::path subPath = entityPath / subName; 
        auto sub = std::make_shared<Folder>(subPath);
        children.push_back(sub);
        return sub;
    }

    // Tạo file con thật trên ổ cứng
    std::shared_ptr<File> createFile(const std::string& fileName, const std::string& data = "") {
        fs::path filePath = entityPath / fileName;
        auto newFile = std::make_shared<File>(filePath, data);
        children.push_back(newFile);
        return newFile;
    }

    void display(int indent = 0) const override {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        std::cout << "📁 [" << getName() << "]\n";
        for (const auto& child : children) {
            child->display(indent + 2);
        }
    }
};

// ================= HÀM MAIN =================
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    // 1. LẤY ĐƯỜNG DẪN THỰC TẾ NƠI ĐANG CHẠY FILE MAIN
    fs::path currentRunningPath = fs::current_path();
    std::cout << "🚀 Nơi đang chạy file main: " << currentRunningPath.string() << "\n\n";

    // 2. GÁN FOLDER GỐC CHÍNH LÀ THƯ MỤC NÀY
    auto root = std::make_shared<Folder>(currentRunningPath);

    // 3. TẠO CÁC FOLDER VÀ FILE (SẼ XUẤT HIỆN THẬT TRONG EXPLORER CỦA BẠN)
    std::cout << "[Hệ thống] Đang tạo cấu trúc thư mục thực tế...\n";
    
    auto studyFolder = root->createSubFolder("Hoc_Tap");
    studyFolder->createFile("TKB.txt", "Thu 2: OOP\nThu 3: C++17 Filesystem");
    
    auto cppFolder = studyFolder->createSubFolder("Code_C++");
    cppFolder->createFile("test.cpp", "int main() { return 0; }");

    auto gameFolder = root->createSubFolder("Giai_Tri");
    gameFolder->createFile("Luu_y.txt", "Hoc xong moi duoc choi game!");

    // Cấu trúc xuất trực tiếp tại nơi chạy main
    root->createFile("Readme.md", "# Thư mục này được tạo tự động bằng C++ OOP");

    // 4. IN RA KẾT QUẢ CẤU TRÚC
    std::cout << "\n========================================\n";
    std::cout << "      CẤU TRÚC THƯ MỤC VỪA TẠO THẬT     \n";
    std::cout << "========================================\n";
    root->display();
    std::cout << "========================================\n";
    
    std::cout << "\n👉 Hãy mở thư mục chứa file code này ra để kiểm tra các file thật vừa xuất hiện nhé!\n";

    return 0;
}