#include <bits/stdc++.h>
#include "../include/Matrix.h"
#include "../include/Ann.h"
#include "../include/Model.h"
using namespace std;
int main(){
    // dataset
    size_t label=1797,size_label=65;
    Matrix::matrix data(label,size_label);
    //load data
    string name_of_dataset="../data/data_real_full.txt";
    data.input(name_of_dataset);
    //x y
    Matrix::matrix x(label,size_label-1),y(label,1);
    data.separation_cols(x,y,size_label-1,1);

    x=x.transpose();
    y=y.transpose();
    y=y.one_hot(10);
    //test dataset
    size_t label_test=100,size_label_test=65;
    Matrix::matrix data_test(label_test,size_label_test);

    string name_of_dataset_test="../data/digits_100.txt";
    data_test.input(name_of_dataset_test);

    Matrix::matrix x_test(label_test,size_label_test-1),y_test(label_test,1);
    data_test.separation_cols(x_test,y_test,size_label_test-1,1);
    
    x_test=x_test.transpose();
    y_test=y_test.transpose();
    y_test=y_test.one_hot(10);
    //cout<<x_test;
    //cout<<y_test;
    //craete model 
    Model::moodel model;
    //neuron off layer
    size_t n1=64;
    size_t n2=32;
    //layer of model
    model.add(ann::layer(x.get_row(),n1,"ReLU"));
    model.add(ann::layer(n1,n2,"ReLU"));
    model.add(ann::layer(n2,y.get_row(),"Softmax"));
    //train
    double alphal=0.05;
    int epchoss=100;
    int step=20;
    model.fit(x,y,x_test,y_test,alphal,epchoss,step);


    return 0;
}