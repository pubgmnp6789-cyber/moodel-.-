#include <bits/stdc++.h>
#include "../include/Matrix.h"
#include "../include/Ann.h"
#include "../include/Model.h"

using namespace std;

int main() {
    Matrix::matrix data(10000, 785);
    data.input("../data/fashion-mnist_test/fashion-mnist_test.txt");

    Matrix::matrix y_raw(10000, 1);
    Matrix::matrix x_raw(10000, 784);
    data.separation_cols(y_raw, x_raw, 1, 784); 


    Matrix::matrix y_train_raw(9000, 1), y_test_raw(1000, 1);
    Matrix::matrix x_train_raw(9000, 784), x_test_raw(1000, 784);
    
    y_raw.separation_row(y_train_raw, y_test_raw, 9000, 1000);
    x_raw.separation_row(x_train_raw, x_test_raw, 9000, 1000);


    Matrix::matrix x_train = x_train_raw.transpose(); 
    Matrix::matrix x_test = x_test_raw.transpose();   
    Matrix::matrix y_train_T = y_train_raw.transpose(); 
    Matrix::matrix y_test_T = y_test_raw.transpose();   

    x_train = x_train * (1.0 / 255.0);
    x_test = x_test * (1.0 / 255.0);

    Matrix::matrix y_train = y_train_T.one_hot(10);
    Matrix::matrix y_test = y_test_T.one_hot(10);   

    cout << "load data is done.\n";

    Model::moodel my_model("fashion-mnist_test_train");
    
    my_model.add(ann::layer(784, 512, "ReLU"));
    my_model.add(ann::layer(512, 256, "ReLU"));
    my_model.add(ann::layer(256, 10, "Softmax"));

    double alpha = 0.05;       
    size_t batch_size = 256;    
    int epochs = 50;          
    int step = 1;            

    my_model.fit(x_train, y_train, x_test, y_test, alpha, batch_size, epochs, step);

    return 0;
}