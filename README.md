markdown_content = """# Mạng Nơ-ron Nhân Tạo (ANN) C++ Từ Con Số 0 (From Scratch)

Dự án này là một framework Deep Learning hạng nhẹ được xây dựng hoàn toàn bằng C++ nguyên bản  . Không sử dụng bất kỳ thư viện toán học hay học máy của bên thứ ba nào (như Eigen, TensorFlow, hay PyTorch), dự án tập trung vào việc tự triển khai các cấu trúc ma trận, thuật toán lan truyền xuôi (Forward Propagation), lan truyền ngược (Backward Propagation) và tối ưu hóa từ những công thức toán học cốt lõi nhất.

Được thiết kế để tối ưu hóa hiệu suất trên CPU, framework này hoàn toàn có khả năng huấn luyện các tập dữ liệu thực tế như Digits hay Fashion-MNIST.

---

## ✨ Tính năng nổi bật

* **Thư viện Ma trận Tùy chỉnh (`Matrix.h`):** Tự triển khai các phép toán đại số tuyến tính cơ bản (Cộng, trừ, nhân ma trận, Dot product, Transpose) và xử lý mảng 1D/2D tối ưu.
* **Hỗ trợ Mini-batch Gradient Descent:** Xáo trộn dữ liệu (shuffle) và chia lô (batch) tự động giúp hội tụ nhanh và tránh nhiễu.
* **Đa dạng Hàm kích hoạt (Activation Functions):** * `ReLU`
    * `Sigmoid`
    * `Softmax`
    * `Linear`
* **Khởi tạo Trọng số Chuẩn (Weight Initialization):**
    * `He Initialization` (Tối ưu cho ReLU)
    * `Xavier/Glorot Initialization` (Tối ưu cho Sigmoid/Softmax)
* **Các Hàm mất mát (Loss Functions):**
    * `Mean Squared Error (MSE)`
    * `Binary Cross Entropy (BCE)`
    * `Categorical Cross Entropy (CCE)`
* **Quản lý Vòng đời Mô hình (Model Checkpointing):** Tự động ghi log độ chính xác (Training/Test Accuracy) và lưu lại tệp trọng số (`.txt`) sau mỗi tiến độ cấu hình sẵn để chống mất dữ liệu.

---

##  Cấu trúc Core Modules

Dự án bao gồm 3 tệp header (`.h`) đóng vai trò là xương sống của toàn bộ kiến trúc:

1.  **`Matrix.h`**: Trái tim toán học của dự án. Chứa class `matrix` với các overload operators và các hàm tiện ích như `one_hot` encoding, `map` function.
2.  **`Ann.h`**: Định nghĩa cấu trúc nơ-ron cục bộ. Chứa namespace `ann` quản lý các hàm kích hoạt, đạo hàm, hàm mất mát và định nghĩa class `layer` (quản lý node, weight, bias).
3.  **`Model.h`**: Bộ điều khiển trung tâm (Controller). Chứa class `model` chịu trách nhiệm lắp ghép các layer, chạy lan truyền xuôi/ngược, thực thi hàm `fit` (huấn luyện) và đọc/ghi file log, file weights.

---

## 🚀 Hướng dẫn sử dụng (Quick Start)

Dưới đây là đoạn mã mẫu minh họa cách khởi tạo và huấn luyện một mô hình mạng nơ-ron đa lớp phân loại 10 nhãn:
