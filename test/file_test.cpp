#include <iostream>
#include <fstream>
#include <filesystem>

// Tạo bí danh cho ngắn gọn
namespace fs = std::filesystem;

int main() {
    // 1. Khai báo tên folder và đường dẫn file
    fs::path folderName = "ThuMucCuaToi";
    fs::path fileName = folderName / "tap_tin_moi.txt"; // Toán tử '/' tự động thêm dấu gạch chéo chuẩn cho từng HĐH

    // 2. Tạo Folder
    // create_directory trả về true nếu tạo mới thành công, false nếu folder đã tồn tại hoặc lỗi
    if (fs::create_directory(folderName)) {
        std::cout << "Da tao folder: " << folderName << "\n";
    } else {
        std::cout << "Folder da ton tai hoac khong the tao.\n";
    }

    // 3. Tạo File bên trong Folder
    // Sử dụng std::ofstream để tạo và ghi file
    std::ofstream outFile(fileName);

    if (outFile.is_open()) {
        // Ghi thử một dòng nội dung vào file
        outFile << "Day la noi dung duoc tao tu C++!\n";
        
        // Đóng file sau khi thao tác xong
        outFile.close();
        
        std::cout << "Da tao file thanh cong tai: " << fileName << "\n";
    } else {
        std::cout << "Loi: Khong the tao file!\n";
    }

    return 0;
}