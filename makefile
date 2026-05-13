# 1. TÊN COMPILER VÀ CÁC FLAG
CXX = g++
# Sử dụng chuẩn C++17, bật cảnh báo lỗi (-Wall) và trỏ đường dẫn tìm tòi file .h vào thư mục include/
CXXFLAGS = -std=c++17 -Wall -Iinclude

# 2. ĐỊNH NGHĨA ĐƯỜNG DẪN THƯ MỤC
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# 3. FILE CHẠY ĐẦU RA (EXECUTABLE)
# Kết quả compile sẽ tạo ra file in_main.exe nằm trong thư mục build/
TARGET = $(BUILD_DIR)/in_main.exe

# 4. TỰ ĐỘNG QUÉTS CÁC FILE CODE
# Tìm tất cả các file .cpp trong thư mục src/
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# Tương ứng tạo ra danh sách các file trung gian .o trong build/
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# 5. CÁC QUY TẮC BIÊN DỊCH (RULES)
# Lệnh mặc định khi gõ 'make'
all: $(TARGET)

# Quy tắc nối (Link) các file .o thành file chạy cuối cùng
$(TARGET): $(OBJS)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CXX) $(OBJS) -o $(TARGET)
	@echo "---> Build thanh cong! File chay tai: $(TARGET)"

# Quy tắc dịch (Compile) từng file .cpp thành .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 6. DỌN DẸP FILE RÁC (CLEAN)
# Dùng lệnh 'make clean' để xóa thư mục build đi làm lại từ đầu
clean:
	@if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
	@echo "---> Da xoa thu muc build/ va cac file rac."

# Khai báo các lệnh không phải là tên file
.PHONY: all clean