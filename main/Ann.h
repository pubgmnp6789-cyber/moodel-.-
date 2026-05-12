#ifndef Ann_h
#define Ann_h

#include "Matrix.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <functional>
#include <iomanip>
#include <cmath>



namespace ann{
//actvation_fuc
    //ReLU
    inline double ReLU(double x){
        return x>=0.0 ? x:0.0; 
    }
    inline double ReLU_derivativa(double x){
        return x>=0.0 ? 1.0:0.0; 
    }
    //Sigmoid
    inline double Sigmoid(double x){
        return 1.0/(1.0+exp(-x));
    }
    inline double Sigmoid_derivativa(double x){
        double sig=Sigmoid(x);
        return sig*(1.0-sig);
    }
    //linear
    inline double Linear(double x){
        return x;
    }
    inline double Linear_derivativa(double x){
        return 1;
    }
    //softmax
    Matrix::matrix Softmax(const Matrix::matrix& x){
        size_t row = x.get_row(),cols = x.get_cols();
        Matrix::matrix result(row,cols);
        result = x;
        for(size_t j=0;j<cols;j++){
            double max=result(0,j);
            for(size_t i=0;i<row;i++){
                if(result(i,j)>max) max=result(i,j);
            }
            double sum_exp=0.0;
            for(size_t i=0;i<row;i++){
                double val=exp(result(i,j)-max);
                result(i,j)=val;
                sum_exp+=val;
            }
            for(size_t i=0;i<row;i++){
                result(i,j)/=sum_exp;
            }
        }
        return result;
    }

    enum class activation_type{
        None,
        ReLU,
        Sigmoid,
        Linear,
        Softmax,
    };

    class layer{
    private:
        size_t n_out;
        size_t n_in;
        activation_type act_type;
        // weight
        Matrix::matrix w;
        Matrix::matrix b;
        //cache to save input and z
        Matrix::matrix input_cache;
        Matrix::matrix z_cache;
        Matrix::matrix A_cache;
        //gradient        
        Matrix::matrix dw;
        Matrix::matrix db;
    public:
        layer(size_t n_input,size_t n_output,const std::string& active_fuction)
        :
            n_out(n_output),n_in(n_input),
            w(n_out,n_in),
            b(n_out,1),
            dw(n_out,n_in),
            db(n_out,1)

        {
            if(active_fuction == "ReLU"){
                act_type=activation_type::ReLU;
                w.he_initialization();
            }
            else if(active_fuction == "Sigmoid"){
                act_type=activation_type::Sigmoid;
                w.xavier_initialization();
            }
            else if(active_fuction == "linear"){
                act_type=activation_type::Linear;
                w.xavier_initialization();
            }            
            else if(active_fuction == "Softmax"){
                act_type=activation_type::Softmax;
                w.xavier_initialization();
            }
            else{
                act_type=activation_type::None;
                w.xavier_initialization();
            }
        }
        Matrix::matrix forward(Matrix::matrix& x){
            Matrix::matrix A;
            Matrix::matrix Z=(w*x);
            Z=Z.add_columns(b);
            switch (act_type)
            {
            case activation_type::ReLU:
                A=Z.map(ReLU);
                break;
            case activation_type::Sigmoid:
                A=Z.map(Sigmoid);
                break;
            case activation_type::Softmax:
                A=Softmax(Z);
                break;
            case activation_type::Linear:
                A=Z.map(Linear);
                break;
            default:
                throw std::runtime_error("act_func!");
            }
            input_cache = x;
            z_cache=Z;
            A_cache=A;
            return A;
        }
        Matrix::matrix backward(Matrix::matrix& da){
            double m=input_cache.get_cols();
            Matrix::matrix dz;
            if(act_type==activation_type::ReLU){
                Matrix::matrix d_ReLU=z_cache.map(ReLU_derivativa);
                dz=da^d_ReLU;
            }
            else if(act_type==activation_type::Sigmoid){
                Matrix::matrix d_Sigmoid=z_cache.map(Sigmoid_derivativa);
                dz=da^d_Sigmoid;
            }
            else if(act_type==activation_type::Linear){
                dz=da;
            }
            else if(act_type==activation_type::Softmax){
                dz=da;
            }
            else {
                throw std::runtime_error("act_func!");
            }
            Matrix::matrix in=input_cache;
            Matrix::matrix dW=(1.0/m)*(dz*in.transpose());
            Matrix::matrix dB=(1.0/m)*dz.row_sum();
            Matrix::matrix da_prev=w.transpose()*dz;
            dw=dW;
            db=dB;
            return da_prev;
        }
        void update_weight(double alpha){
            w=w-(alpha*dw);
            b=b-(alpha*db);
        }
        //getter
        Matrix::matrix get_w(){
            return w;
        }
        Matrix::matrix get_b(){
            return b;
        }
        std::string get_activation_function() const {
            switch (act_type) {
                case activation_type::ReLU:
                    return "ReLU";
                case activation_type::Sigmoid:
                    return "Sigmoid";
                case activation_type::Linear:
                    return "linear";  
                case activation_type::Softmax:
                    return "Softmax"; 
                default:
                    return "None";
            }
        }
        
    };
    class inference_layer{
    private:
        Matrix::matrix w;
        Matrix::matrix b;   
        activation_type act_type;
        bool weight_load;
    public:
        inference_layer(const std::string& active_fuction) : weight_load(false)
        {
            if(active_fuction == "ReLU"){
                act_type=activation_type::ReLU;
            }
            else if(active_fuction == "Sigmoid"){
                act_type=activation_type::Sigmoid;
            }
            else if(active_fuction == "linear"){
                act_type=activation_type::Linear;
            }            
            else if(active_fuction == "Softmax"){
                act_type=activation_type::Softmax;
            }
            else{
                throw std::invalid_argument("active funtion!");
            }
        }

        void input_weight(const Matrix::matrix& weight,const Matrix::matrix& bias){
            w=weight;
            b=bias;
            weight_load=true;
        }

        Matrix::matrix inference(const Matrix::matrix& x){
            if(!weight_load){
                throw std::invalid_argument("weight!");
            }
            Matrix::matrix Z=(w*x);
            Z=Z.add_columns(b);
            switch (act_type)
            {
            case activation_type::ReLU:
                return Z.map(ReLU);
            case activation_type::Sigmoid:
                return Z.map(Sigmoid);
            case activation_type::Softmax:
                return Softmax(Z);
            case activation_type::Linear:
                return Z.map(Linear);
            default:
                throw std::runtime_error("act_func!");
            }
        }
    };

}
    namespace Loss {
        //sigmoid
        double binary_cross_entropy(const Matrix::matrix& y_pred, const Matrix::matrix& y_true) {
            double total_loss = 0.0;
            size_t batch_size = y_pred.get_cols();
            const double epsilon = 1e-15; 
            for(size_t i = 0; i < batch_size; i++) {
                double y = y_true(0,i);       
                double y_hat = y_pred(0,i);   
                y_hat = std::clamp(y_hat,epsilon,1.0-epsilon);
                total_loss += -(y * std::log(y_hat) + (1.0 - y) * std::log(1.0 - y_hat));
            }
            return total_loss / batch_size; 
        }
        inline Matrix::matrix binary_cross_entropy_derivative(const Matrix::matrix& y_pred, const Matrix::matrix& y_true) {
            size_t row = y_pred.get_row();
            size_t cols = y_pred.get_cols();
            Matrix::matrix da(row, cols);
            const double epsilon = 1e-15;

            for(size_t i = 0; i < row; ++i) {
                for(size_t j = 0; j < cols; ++j) {
                    double y = y_true(i, j);
                    double y_hat = std::clamp(y_pred(i, j), epsilon, 1.0 - epsilon);
                    da(i, j) = - (y / y_hat) + ((1.0 - y) / (1.0 - y_hat));
                }
            }
            return da;
        }
        //linear
        double Mean_Squared_Error(const Matrix::matrix& y_pred, const Matrix::matrix& y_true){
            double loss=0.0;
            size_t row=y_pred.get_row();
            size_t cols=y_pred.get_cols();
            size_t total_elements=row*cols;
            for(size_t i=0;i<row;i++){
                for(size_t j=0;j<cols;j++){
                    double diff = y_true(i,j)-y_pred(i,j);
                    loss+=diff*diff;
                }
            }
            return loss/total_elements;   
        }
        inline Matrix::matrix Mean_Squared_Error_derivative(const Matrix::matrix& y_pred, const Matrix::matrix& y_true) {
            size_t total_elements = y_pred.get_row() * y_pred.get_cols();
            return (y_pred - y_true) * (2.0 / total_elements);
        }
        //softmax
        double Categorical_Cross_Entropy(const Matrix::matrix& y_pred, const Matrix::matrix& y_true){
            double loss=0.0;
            size_t row=y_pred.get_row();
            size_t cols=y_pred.get_cols();
            const double epsilon = 1e-15;
            size_t total_elements=cols;
            for(size_t j=0;j<cols;j++){
                double diff=0.0;
                for(size_t i=0;i<row;i++){
                    if(y_true(i,j)>0.0){
                    double y_hat = std::max(epsilon, y_pred(i, j));
                    diff += y_true(i, j) * std::log(y_hat);
                    }
                }
                loss += diff;
            }
            return -loss/total_elements;
        }
    }


#endif