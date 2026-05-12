#include <bits/stdc++.h>
#include "Matrix.h"
#include "Ann.h"
#include "Model.h"
using namespace std;
int main(){
    Matrix::matrix data(12,3);
    data.input("dataset_test.txt");
    //cout<<data;
    Matrix::matrix x(12,2),y(12,1);
    data.separation_cols(x,y,2,1);
    x=x.transpose();
    y=y.transpose();
    //cout<<x<<y;
    Model::moodel model;
    model.add(ann::layer(x.get_row(),4,"ReLU"));
    model.add(ann::layer(4,2,"ReLU"));
    model.add(ann::layer(2,y.get_row(),"Sigmoid"));
    model.fit(x,y,0.1,100,10);
    Matrix::matrix y_test=model.Inference(x);
    cout<<y<<y_test;

    return 0;
}