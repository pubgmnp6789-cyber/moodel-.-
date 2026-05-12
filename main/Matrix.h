#ifndef Matrix_h
#define Matrix_h

#include <iostream>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <functional>
#include <iomanip>
#include <random>
#include <cmath>

namespace Matrix{
    class matrix{
    private:
        std::vector<double> data;
        size_t row,cols;
    public:
        //fuctions initialization matrix
        matrix(size_t r,size_t c): row(r),cols(c),data(r*c,0.0){}
        //default constructor
        matrix() : row(0),cols(0),data(0){}
        //operator [i*c+j]
        double& operator()(size_t i,size_t j){
            return data[i*cols+j];
        }
        const double& operator()(size_t i,size_t j) const{
            return data[i*cols+j];
        }
        //getters
        size_t get_row()const{
            return row;
        }
        size_t get_cols()const{
            return cols;
        }
        //operator +
        matrix operator+(const matrix &other)const{
            if(row!=other.row||cols!=other.cols){
                throw std::invalid_argument("+!");
            }
            matrix result(row,cols);
            for(size_t i=0;i<row*cols;i++){
                result.data[i]=data[i]+other.data[i];
            }
            return result;
        }
        matrix operator+(double number)const{
            matrix result(row,cols);
            for(size_t i=0;i<row*cols;i++){
                result.data[i]=data[i]+number;
            }
            return result;
        }
        //operator -
        matrix operator-(const matrix &other)const{
            if(row!=other.row||cols!=other.cols){
                throw std::invalid_argument("-!");
            }
            matrix result(row,cols);
            for(size_t i=0;i<row*cols;i++){
                result.data[i]=data[i]-other.data[i];
            }
            return result;
        }
        matrix operator-(double number)const{
            matrix result(row,cols);
            for(size_t i=0;i<row*cols;i++){
                result.data[i]=data[i]-number;
            }
            return result;
        }
        //operator *
        matrix operator*(const matrix &other)const{
            if(cols!=other.row){
                throw std::invalid_argument("*!");
            }
            matrix result(row,other.cols);
            for (size_t i = 0; i < row; i++) {
                for (size_t k = 0; k < cols; k++) {
                    double r_ik = data[i * cols + k];
                    for (size_t j = 0; j < other.cols; j++) {
                        result.data[i * result.cols + j] += r_ik * other.data[k * other.cols + j];
                    }
                }
            }
            return result;
        }
        matrix operator*(double number)const{
            matrix result(row,cols);
            for(size_t i=0;i<row*cols;i++){
                result.data[i]=data[i]*number;
            }
            return result;
        }
        friend matrix operator*(double num,const matrix& mat){
            return mat*num;
        }
        //operator ^(Hadamard product) 
        matrix operator^(const matrix &other)const{
            if(row!=other.row||cols!=other.cols){
                throw std::invalid_argument("^!");
            }
            matrix result(row,cols);
            for(size_t i=0;i<row*cols;i++){
                result.data[i]=data[i]*other.data[i];
            }
            return result;
        }
        //to sum cols using for backward
        matrix row_sum()const{
            matrix result(row,1);
            for(size_t i=0;i<row;i++){
                for(size_t j=0;j<cols;j++){
                    result.data[i]+=data[i*cols+j];
                }
            }
            return result;
        } 
        //using for add bias to matrix
        matrix add_columns(const matrix &bias)const{
            matrix result(row,cols);
            if(row!=bias.row){
                throw std::invalid_argument("row!");
            }
            for(size_t i=0;i<row;i++){
                for(size_t j=0;j<cols;j++){
                    result.data[i*cols+j]=data[i*cols+j]+bias.data[i];
                }
            }
            return result;
        }
        // T matrix
        matrix transpose()const{
            matrix result(cols,row);
            for(size_t i=0;i<cols;i++){
                for(size_t j=0;j<row;j++){
                    result.data[i*row+j]=data[j*cols+i];
                }
            }
            return result;
        }
        // map(to add activation fuction)
        template<typename Func> 
        matrix map(Func func)const{
            matrix result(row,cols);
            for(size_t i=0;i<row*cols;i++){
                result.data[i]=func(data[i]);
            }
            return result;
        }
        //cin and cout in matrix
        friend std::istream& operator>>(std::istream& is,matrix &mat){
            for(size_t i=0;i<mat.row;i++){
                for(size_t j=0;j<mat.cols;j++){
                    is>>mat(i,j);
                }
            }
            return is;
        } 
        friend std::ostream& operator<<(std::ostream& os,const matrix &mat){
            for(size_t i=0;i<mat.row;i++){
                for(size_t j=0;j<mat.cols;j++){
                    os<<mat(i,j)<<" ";
                }
                os<<"\n";
            }
            return os;
        }
        // input from file
        void input(const std::string& file_name){
            std::ifstream file(file_name);
            if(!file.is_open()){
                throw std::invalid_argument("file!");
            }
            size_t index=0;
            size_t total_elements=row*cols;
            while (index<total_elements&&file>>data[index])
            {
                index++;
            }
            if(index<total_elements){
                throw std::runtime_error("total elements!");
            }
            file.close();
        }
        //output from file
        void output(const std::string& name_file) const{
            std::ofstream file(name_file);
            for(size_t i=0;i<row*cols;i++){
                file<<std::fixed<<std::setprecision(15)<<data[i];
                if((i+1)%cols==0) file<<"\n";
                else file <<" ";
            }
            file.close();
        }
        //separation matrix;
        void separation_cols(matrix& x,matrix& y,size_t size_of_x,size_t size_of_y=0){
            if(size_of_x > cols){
                throw std::invalid_argument("s_c!");
            }
            if(size_of_y==0) size_of_y=cols-size_of_x;
            x=matrix (row,size_of_x);
            y=matrix (row,size_of_y);
            for(size_t i=0;i<row;i++){
                for(size_t j=0;j<size_of_x;j++){
                    x(i,j)=(*this)(i,j);
                }
                for(size_t j=0;j<size_of_y;j++){
                    y(i,j)=(*this)(i,j+size_of_x);
                }
            }   
        }
        void separation_row(matrix& x,matrix& y,size_t size_of_x,size_t size_of_y=0){
            if(size_of_x > row){
                throw std::invalid_argument("s_r!");
            }
            if(size_of_y==0) size_of_y=row-size_of_x;
            x=matrix (size_of_x,cols);
            y=matrix (size_of_y,cols);
            for(size_t i=0;i<cols*size_of_x;i++){
                x.data[i]=data[i];
            }   
            size_t offset=cols*size_of_x;
            for(size_t i=0;i<cols*size_of_y;i++){
                y.data[i]=data[i+offset];
            }
        }
        //one hot for softmax label
        Matrix::matrix one_hot(Matrix::matrix& y_label,size_t num_classes){
            size_t sample=y_label.get_cols();
            Matrix::matrix result_label(num_classes,sample);
            for(size_t j=0;j<sample;j++){
                size_t class_id = static_cast<size_t>(y_label(0,j));
                if(class_id<num_classes){
                    result_label(class_id,j)=1;
                }
                else{
                    throw std::out_of_range("class id!");
                }
            }
            return result_label;
        }
        //He Initialization for relu layer
        void he_initialization(){
            //initialization random seed
            static std::random_device rd;
            static std::mt19937 gen(rd());
            //core of he initialization
            double std_dev=std::sqrt(2.0/cols);
            std::normal_distribution<double> dist(0.0,std_dev);

            for(size_t i=0;i<row*cols;i++){
                data[i]=dist(gen);
            }
        }// Xavier (Glorot) Initialization for Sigmoid / Tanh layers
        void xavier_initialization() {
            //initialization random seed
            static std::random_device rd;
            static std::mt19937 gen(rd());
            //core of xavier initialization
            double std_dev=std::sqrt(2.0/(row+cols));
            std::normal_distribution<double> dist(0.0,std_dev);

            for(size_t i=0;i<row*cols;i++){
                data[i]=dist(gen);
            } 
        }
    };
}

#endif