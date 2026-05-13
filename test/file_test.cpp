#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem> // Chuẩn C++17: Thao tác với file/folder vật lý
#include <fstream>    // Đọc/ghi nội dung file

namespace fs = std::filesystem;

// ================= LỚP CHA CHUNG =================
class FileSystemEntity {
protected:
    fs::path entityPath; // Đường dẫn vật lý trên ổ cứng

public:
    FileSystemEntity(const fs::path& path) : entityPath(path) {}
    virtual ~FileSystemEntity() {}
    
    // Hàm thuần ảo yêu cầu các lớp con phải tự định nghĩa cách hiển thị
    virtual void display(int indent = 0) const = 0;
    
    // Các Getter / Setter cơ bản
    fs::path getPath() const { return entityPath; }
    std::string getName() const { return entityPath.filename().string(); }
    void setPath(const fs::path& newPath) { entityPath = newPath; }
};

// ================= LỚP FILE =================
class File : public FileSystemEntity {
public:
    // Khởi tạo File đồng thời tạo file vật lý trên ổ cứng
    File(const fs::path& path, const std::string& initialData = "") 
        : FileSystemEntity(path) {
        // Tự động tạo file trên đĩa (dù chuỗi rỗng thì vẫn đẻ ra file trống)
        write(initialData); 
    }

    // Ghi đè nội dung mới vào file
    void write(const std::string& newData) {
        std::ofstream ofs(entityPath, std::ios::out);
        if (ofs.is_open()) {
            ofs << newData;
            ofs.close();
        } else {
            std::cout << "[Lỗi OS] Không thể ghi file: " << getName() << "\n";
        }
    }

    // Ghi nối tiếp nội dung vào cuối file
    void append(const std::string& extraData) {
        std::ofstream ofs(entityPath, std::ios::app);
        if (ofs.is_open()) {
            ofs << extraData;
            ofs.close();
        }
    }

    // Đọc và in nội dung file ra console
    void read() const {
        if (!fs::exists(entityPath)) {
            std::cout << "[Lỗi] File không tồn tại trên đĩa!\n";
            return;
        }
        std::ifstream ifs(entityPath);
        std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        std::cout << "\n📖 [Đọc file: " << getName() << "]\n" << content << "\n";
    }

    // Hiển thị tên file kèm dung lượng thực tế lấy từ OS
    void display(int indent = 0) const override {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        uintmax_t size = fs::exists(entityPath) ? fs::file_size(entityPath) : 0;
        std::cout << "📄 " << getName() << " (" << size << " bytes)\n";
    }
};

// ================= LỚP FOLDER =================
class Folder : public FileSystemEntity {
private:
    // Chứa danh sách các đối tượng con (Đa hình: đựng được cả File lẫn Folder)
    std::vector<std::shared_ptr<FileSystemEntity>> children;

public:
    // Khởi tạo Folder đồng thời tạo thư mục thật trên OS nếu chưa có
    Folder(const fs::path& path) : FileSystemEntity(path) {
        if (!fs::exists(entityPath)) {
            fs::create_directories(entityPath);
        }
    }

    // 1. TẠO FOLDER CON TRONG FOLDER HIỆN TẠI
    std::shared_ptr<Folder> createSubFolder(const std::string& subName) {
        fs::path subPath = entityPath / subName; // Nối đường dẫn chuẩn OS
        auto subFolder = std::make_shared<Folder>(subPath);
        children.push_back(subFolder);
        return subFolder;
    }

    // 2. TẠO FILE TRONG FOLDER HIỆN TẠI
    std::shared_ptr<File> createFile(const std::string& fileName, const std::string& data = "") {
        fs::path filePath = entityPath / fileName;
        auto newFile = std::make_shared<File>(filePath, data);
        children.push_back(newFile);
        return newFile;
    }

    // 3. THÊM FILE ĐÃ CÓ SẴN VÀO FOLDER HIỆN TẠI (Copy vật lý + nạp OOP)
    void addExistingFile(std::shared_ptr<File> fileObj) {
        if (!fileObj) return;

        fs::path targetPath = this->entityPath / fileObj->getName();

        try {
            // Copy file vật lý trên đĩa cứng sang vị trí folder mới
            if (fs::exists(fileObj->getPath()) && fileObj->getPath() != targetPath) {
                fs::copy(fileObj->getPath(), targetPath, fs::copy_options::overwrite_existing);
            }
            // Cập nhật lại đường dẫn mới cho Object
            fileObj->setPath(targetPath);
            // Đưa vào mảng quản lý
            children.push_back(fileObj);
            std::cout << "[Hệ thống] Đã copy/thêm file '" << fileObj->getName() << "' vào folder [" << getName() << "].\n";

        } catch (const fs::filesystem_error& e) {
            std::cout << "[Lỗi OS] Không thể copy file: " << e.what() << "\n";
        }
    }

    // Hiển thị đệ quy toàn bộ cấu trúc cây thư mục
    void display(int indent = 0) const override {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        std::cout << "📁 [" << getName() << "]\n";
        for (const auto& child : children) {
            child->display(indent + 2);
        }
    }
};

// ================= HÀM MAIN (DEMO TOÀN BỘ CHỨC NĂNG) =================
int main() {
    // Tối ưu hiển thị tiếng Việt trên Console
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    // Lấy chính xác đường dẫn thư mục đang chạy code
    fs::path currentPath = fs::current_path();
    std::cout << "🚀 Thư mục gốc đang làm việc: " << currentPath.string() << "\n\n";

    // Khởi tạo đối tượng Folder gốc đại diện cho thư mục hiện tại
    auto rootFolder = std::make_shared<Folder>(currentPath);

    // --- TEST 1: TẠO FOLDER CON & TẠO FILE TRONG FOLDER ---
    std::cout << "1. Đang khởi tạo các folder và file nội bộ...\n";
    auto projectFolder = rootFolder->createSubFolder("Du_An_OOP");
    projectFolder->createFile("yeu_cau.txt", "Mô phỏng File System bằng C++17");
    
    auto srcFolder = projectFolder->createSubFolder("src");
    auto codeFile = srcFolder->createFile("main.cpp", "#include <iostream>\n// Code C++ ở đây");

    // --- TEST 2: GHI THÊM DATA VÀO FILE ---
    codeFile->append("\nint main() { return 0; }");

    // --- TEST 3: TẠO FILE ĐỘC LẬP BÊN NGOÀI VÀ THÊM VÀO FOLDER ---
    std::cout << "2. Đang tạo file độc lập và di chuyển vào folder...\n";
    // Tạo một file nằm ngay ở root ban đầu
    auto myDoc = std::make_shared<File>(currentPath / "Tai_Lieu_Ngoai.txt", "Dữ liệu mật cần đưa vào dự án.");
    // Gắp file đó bỏ vào trong thư mục "src"
    srcFolder->addExistingFile(myDoc);

    // --- XUẤT KẾT QUẢ RA MÀN HÌNH ---
    std::cout << "\n========================================\n";
    std::cout << "       CẤU TRÚC THƯ MỤC THỰC TẾ         \n";
    std::cout << "========================================\n";
    rootFolder->display();
    std::cout << "========================================\n";

    // Đọc thử nội dung của file vừa bị di chuyển để xác nhận data vẫn nguyên vẹn
    myDoc->read();

    return 0;
}