#ifndef Model_h
#define Model_h
#include"Matrix.h"
#include "Ann.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <functional>
#include <iomanip>
#include <random>
#include <cmath>
#include <thread>
#include <chrono>
#include <algorithm>

namespace Model{
    inline double Threshold_Filter(double x) {
        return x > 0.5 ? 1.0 : 0.0;
    }
    Matrix::matrix get_arg_max(const Matrix::matrix &y){
        size_t cols=y.get_cols();
        size_t row=y.get_row();
        Matrix::matrix result(1,cols);
        for(size_t j=0;j<cols;j++){
            double max_val=y(0,j);
            int max_index=0;
            for (size_t i=1;i<row;i++){
                if(y(i,j)>max_val){
                    max_val=y(i,j);
                    max_index = i;
                }
            }
            result(0,j) = static_cast<double> (max_index);                   

        }
        return result;
    }
    void PrintProgressBar(int current_step, int total_steps, int bar_width = 50) {
        int completed_width = static_cast<int>((static_cast<double>(current_step) / total_steps) * bar_width);
        std::string bar;
        for (int i = 0; i < bar_width; ++i) {
            if (i < completed_width) {
                bar += "="; 
            } else if (i == completed_width && current_step < total_steps) {
                bar += ">"; 
            } else {
                bar += " "; 
            }
        }        
        std::cout << "\r[" << bar << "] " << current_step << "/" << total_steps << std::flush;
    }
    class moodel{
    private:
        std::vector<ann::layer> layers;
        std::vector<ann::inference_layer> layers_inference;
        std::string Model_name;

    public:
        moodel(std::string name): Model_name(name){} 
        void add(const ann::layer& L){
            layers.push_back(L);
            layers_inference.push_back(ann::inference_layer(L.get_activation_function()));
        }
        Matrix::matrix forward(const Matrix::matrix& x){
            Matrix::matrix input = x;
            for(size_t i=0;i<layers.size();i++){
                input = layers[i].forward(input);
            }
            return input;
        }   
        void backward(const Matrix::matrix& da_last){
            Matrix::matrix da = da_last;
            for(int i=static_cast<int>(layers.size())-1;i>=0;i--){
                da= layers[i].backward(da);
            }
        }
        Matrix::matrix Inference(const Matrix::matrix& x){
            for(size_t i=0;i<layers_inference.size();i++){
                layers_inference[i].input_weight(layers[i].get_w(),layers[i].get_b());
            }
            Matrix::matrix input=x;
            for(size_t i=0;i<layers_inference.size();i++){
                input=layers_inference[i].inference(input);
            }
            return input;
        }
        void update_weight(double alpha){
            for(size_t i=0;i<layers.size();i++){
                layers[i].update_weight(alpha);
            }
        }
        //getters
        size_t size() const{
            return layers.size();
        }
        ann::layer& get_layer(size_t index){
            return layers[index];
        }
        void save_weight(std::string folder_name){
            if (!std::filesystem::exists(folder_name)) {
                std::filesystem::create_directories(folder_name);
            }
            size_t size_of_model = layers.size();
            for(int i=0;i<size_of_model;i++){
                std::string b_name="b"+std::to_string(i)+"weight.txt";
                std::string w_name="w"+std::to_string(i)+"weight.txt";
                std::filesystem::path b_path = std::filesystem::path(folder_name)/b_name;
                std::filesystem::path w_path = std::filesystem::path(folder_name)/w_name;
                
                layers[i].get_b().output(b_path.string());
                layers[i].get_w().output(w_path.string());
            }
        }

        void load_weight(std::string folder_name){
            if (!std::filesystem::exists(folder_name)) {
                throw std::invalid_argument("folder!");
            }
            size_t size_of_model = layers.size();
            for(int i=0;i<size_of_model;i++){
                std::string b_name="b"+std::to_string(i)+"weight.txt";
                std::string w_name="w"+std::to_string(i)+"weight.txt";
                std::filesystem::path b_path = std::filesystem::path(folder_name)/b_name;
                std::filesystem::path w_path = std::filesystem::path(folder_name)/w_name;
                
                Matrix::matrix temp_b=layers[i].get_b();
                Matrix::matrix temp_w=layers[i].get_w();

                temp_b.input(b_path.string());
                temp_w.input(w_path.string());
                layers[i].input_weight(temp_w,temp_b);
            }
        }
        

        double train(const Matrix::matrix& x,const Matrix::matrix& y,double alpha,int epochs,size_t batches_size){
            size_t total_samples=x.get_cols();
            size_t num_features=x.get_row();
            size_t num_classes=y.get_row();
            std::string activat_type=layers[layers.size()-1].get_activation_function();
            
            double final_epoch_loss = 0.0;

            for (int e = 0; e < epochs; e++) {
                double current_epoch_loss = 0.0;
                
                std::vector<size_t> indices(total_samples);
                for(size_t i=0;i<total_samples;i++) indices[i]=i;
                static std::random_device rd;
                static std::mt19937 g(rd());
                std::shuffle(indices.begin(),indices.end(),g);

                size_t num_batches = ((total_samples)+batches_size-1) / batches_size;

                for(size_t b = 0; b < num_batches; b++){
                    size_t start_idx = b * batches_size;
                    size_t current_batch_size = std::min(batches_size, total_samples - start_idx);
                    Matrix::matrix x_batch(num_features, current_batch_size);
                    Matrix::matrix y_batch(num_classes, current_batch_size);

                    for(size_t j = 0; j < current_batch_size; j++){
                        size_t original_sample_idx = indices[start_idx + j];
                        for(size_t i = 0; i < num_features; i++){
                            x_batch(i, j) = x(i, original_sample_idx);
                        }
                        for(size_t i = 0; i < num_classes; i++){
                            y_batch(i, j) = y(i, original_sample_idx);
                        }
                    }
                    Matrix::matrix out;
                    out = this->forward(x_batch);
                    Matrix::matrix da_last;

                    if(activat_type == "Sigmoid"){
                        da_last = Loss::binary_cross_entropy_derivative(out, y_batch);
                        current_epoch_loss += Loss::binary_cross_entropy(out, y_batch) * current_batch_size;
                    }
                    else if(activat_type == "linear"){
                        da_last = Loss::Mean_Squared_Error_derivative(out, y_batch);
                        current_epoch_loss += Loss::Mean_Squared_Error(out, y_batch) * current_batch_size;
                    }
                    else if(activat_type == "Softmax"){
                        da_last = out - y_batch;
                        current_epoch_loss += Loss::Categorical_Cross_Entropy(out, y_batch) * current_batch_size;
                    }

                    this->backward(da_last);
                    this->update_weight(alpha);

                    PrintProgressBar(static_cast<int>(b + 1), static_cast<int>(num_batches));
                }
                
                final_epoch_loss = current_epoch_loss / total_samples;
            }

            return final_epoch_loss;
        }

        void fit(const Matrix::matrix& x,const Matrix::matrix& y,const Matrix::matrix& x_test,const Matrix::matrix& y_test,double alpha,size_t batches_size,int epochss,int step=10){
            std::string model_log = Model_name + "_log.txt";  
            std::string folder_name = Model_name;

            if (!std::filesystem::exists(folder_name)) {
                std::filesystem::create_directories(folder_name);
            }

            std::filesystem::path b_path = std::filesystem::path(folder_name) / model_log;
            std::ofstream file(b_path);

            if (!file.is_open()) {
                std::cerr << "Can not open log file!\n";
            }
            std::string activat_type=layers[layers.size()-1].get_activation_function();
                    
            size_t total_loops = epochss / step;
                    
            for(size_t i = 0; i < total_loops; i++){
                std::cout << (i + 1) * step << "/" << epochss << "\n";
                    
                double loss = train(x, y, alpha, step, batches_size);
                alpha = alpha * 0.95;
                    
                double accuracy = 0.0;
                double accuracy_x_test = 0.0;
                        
                Matrix::matrix predict = this->Inference(x);
                Matrix::matrix predict_x_test = this->Inference(x_test);
                    
                if(activat_type == "Sigmoid"){
                    predict = predict.map(Threshold_Filter);
                    accuracy = static_cast<double>(predict.Similarity(y)) / predict.get_cols();
                        
                    predict_x_test = predict_x_test.map(Threshold_Filter);
                    accuracy_x_test = static_cast<double>(predict_x_test.Similarity(y_test)) / predict_x_test.get_cols();
                }
                else if(activat_type == "Softmax"){
                    Matrix::matrix pred_labels = get_arg_max(predict);
                    Matrix::matrix true_labels = get_arg_max(y);
                    accuracy = static_cast<double>(pred_labels.Similarity(true_labels)) / predict.get_cols();

                    Matrix::matrix pred_labels_x_test = get_arg_max(predict_x_test);
                    Matrix::matrix true_labels_x_test = get_arg_max(y_test);                    
                    accuracy_x_test = static_cast<double>(pred_labels_x_test.Similarity(true_labels_x_test)) / predict_x_test.get_cols();
                }
                    
                std::cout   << "\nepochs: "    << std::left << std::setw(6)  << (i + 1) * step << " | "
                            << "loss: "      << std::left << std::setw(10) << loss           << " | "
                            << "acc_train: " << std::fixed << std::setprecision(5) << accuracy
                            << " | acc_test: " << std::fixed << std::setprecision(5) << accuracy_x_test << "\n"
                            << "--------------------------------------------------\n";
                file        <<"\nepochs: "    << std::left << std::setw(6)  << (i + 1) * step << " | "
                            << "loss: "      << std::left << std::setw(10) << loss           << " | "
                            << "acc_train: " << std::fixed << std::setprecision(5) << accuracy
                            << " | acc_test: " << std::fixed << std::setprecision(5) << accuracy_x_test << "\n"
                            << "--------------------------------------------------\n";
                std::string checkpoint_folder = folder_name + "/log_" + std::to_string(i + 1);
                save_weight(checkpoint_folder);

            }   
            file.close();
                
        }
    };
} 


#endif