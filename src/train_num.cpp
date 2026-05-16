#include <bits/stdc++.h>
#include "../include/Matrix.h"
#include "../include/Ann.h"
#include "../include/Model.h"

using namespace std;

int main() {
    Matrix::matrix data(1000, 65);
    data.input("../data/data_real_full.txt");
    
    Matrix::matrix x_raw(1000, 64);
    Matrix::matrix y_raw(1000, 1);
    
    data.separation_cols(x_raw, y_raw, 64, 1); 
    
    Matrix::matrix x_train_raw(900, 64), x_test_raw(100, 64);
    Matrix::matrix y_train_raw(900, 1), y_test_raw(100, 1);
    
    x_raw.separation_row(x_train_raw, x_test_raw, 900, 100);
    y_raw.separation_row(y_train_raw, y_test_raw, 900, 100);

    Matrix::matrix x_train = x_train_raw.transpose(); 
    Matrix::matrix x_test = x_test_raw.transpose();   
    Matrix::matrix y_train_T = y_train_raw.transpose(); 
    Matrix::matrix y_test_T = y_test_raw.transpose();   
    
    x_train = x_train * (1.0 / 255.0);
    x_test = x_test * (1.0 / 255.0);
    
    Matrix::matrix y_train = y_train_T.one_hot(10);
    Matrix::matrix y_test = y_test_T.one_hot(10);   
    
    cout << "Load data is done.\n";
    
    Model::moodel my_model("num_train_test");
    
    my_model.add(ann::layer(64, 32, "ReLU"));
    my_model.add(ann::layer(32, 16, "ReLU"));
    my_model.add(ann::layer(16, 10, "Softmax"));
    
    double alpha = 0.05;       
    size_t batch_size = 256;    
    int epochs = 10;          
    int step = 1;            
    
    my_model.fit(x_train, y_train, x_test, y_test, alpha, batch_size, epochs, step);

    return 0;
}