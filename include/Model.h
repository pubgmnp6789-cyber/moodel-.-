#ifndef Model_h
#define Model_h
#include"Matrix.h"
#include "Ann.h"
#include "manage_file.h"
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

    public:
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
            for(int i=0;i<layers_inference.size();i++){
                layers_inference[i].input_weight(layers[i].get_w(),layers[i].get_b());
            }
            Matrix::matrix input=x;
            for(int i=0;i<layers_inference.size();i++){
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

        double train(const Matrix::matrix& x,const Matrix::matrix& y,double alpha,int epochs){
            double loss=0.0;
            std::string activat_type=layers[layers.size()-1].get_activation_function();
            Matrix::matrix out ;
            for(int i=0;i<epochs;i++){
                PrintProgressBar(i+1,epochs);
                out = this->forward(x);
                Matrix::matrix da_last;

                if(activat_type=="Sigmoid"){
                    da_last = Loss::binary_cross_entropy_derivative(out,y);
                }
                else if(activat_type=="linear"){
                    da_last = Loss::Mean_Squared_Error_derivative(out,y);
                }
                else if(activat_type=="Softmax"){
                    da_last = out-y;
                }

                this->backward(da_last);
                this->update_weight(alpha);
                
            }
            std::cout<<"\n";
            
            if(activat_type=="Sigmoid"){
                loss = Loss::binary_cross_entropy(out,y);
            }
            else if(activat_type=="Softmax"){
                loss = Loss::Categorical_Cross_Entropy(out,y);
            }
            else if(activat_type=="linear"){
                loss = Loss::Mean_Squared_Error(out,y);
            }
            else{
                throw std::runtime_error("act is not define!");
            }
            return loss;
        }
        void fit(const Matrix::matrix& x,const Matrix::matrix& y,const Matrix::matrix& x_test,const Matrix::matrix& y_test,double alpha,int epochss,int step){
            std::string activat_type=layers[layers.size()-1].get_activation_function();
            int batch = epochss/step;
            for(size_t i=0;i<batch;i++){
                std::cout<<(i+1)*step<<"/"<<epochss<<"\n";
                double loss=train(x,y,alpha,step);
                double accuracy=0.0;
                double accuracy_x_test=0.0;
                Matrix::matrix predict = this->Inference(x);
                Matrix::matrix predict_x_test = this->Inference(x_test);
                if(activat_type=="Sigmoid"){
                    predict = predict.map(Threshold_Filter);
                    accuracy = static_cast<double>(predict.Similarity(y)) / predict.get_cols();
                    
                    predict_x_test = predict_x_test.map(Threshold_Filter);
                    accuracy_x_test = static_cast<double>(predict_x_test.Similarity(y_test)) / predict_x_test.get_cols();
                }
                else if(activat_type=="Softmax"){
                    Matrix::matrix pred_labels = get_arg_max(predict);
                    Matrix::matrix true_labels = get_arg_max(y);
                    accuracy = static_cast<double>(pred_labels.Similarity(true_labels)) / predict.get_cols();

                    Matrix::matrix pred_labels_x_test = get_arg_max(predict_x_test);
                    Matrix::matrix true_labels_x_test = get_arg_max(y_test);                    
                    accuracy_x_test = static_cast<double>(pred_labels_x_test.Similarity(true_labels_x_test)) / predict_x_test.get_cols();

                }
                std::cout << "epochs: "   << std::left << std::setw(6)  << (i + 1) * step << " | "
                          << "loss: "     << std::left << std::setw(10) << loss           << " | "
                          << "accuracy: " <<std::fixed<<std::setprecision(5)<<accuracy
                          <<     "|"      <<std::fixed<<std::setprecision(5)<<accuracy_x_test<<"\n"
                          << "--------------------------------------------------\n";
            }
        }
    };
} 


#endif