#ifndef Model_h
#define Model_h
#include"Matrix.h"
#include "Ann.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <functional>
#include <iomanip>
#include <random>
#include <cmath>

namespace Model{
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
    };
} 


#endif