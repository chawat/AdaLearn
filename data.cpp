#include <string.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include "data.hpp"

using namespace std;
using std::vector;

vector<float> DataIterator::strSplit(char * x, string delim) {
    vector<float> y;
    char * token = strtok(x,",");

    while (token != NULL) {
        y.push_back(stof(token));
        token = strtok(NULL, ",");
    }

    return y;
}

void DataIterator::readData(string x_data_filename, string y_data_filename) {
    ifstream x_file (x_data_filename);
    ifstream y_file (y_data_filename);

    string line;

    if (x_file.is_open()) {
        while (getline(x_file,line)) {
            this->rX.push_back(this->strSplit(const_cast<char*>(line.c_str()), ",")); 
        }
        x_file.close();
    }
    
    this->n_feat = this->rX[0].size();

    if (y_file.is_open()) {
        while (getline(y_file,line)) {
            this->rY.push_back(this->strSplit(const_cast<char*>(line.c_str()), ","));
        }
        y_file.close();
    }

}

void DataIterator::transformData() {
    if (this->type == "min_max_scaler") {
        this->minMaxScaler();
    } else if (this->type == "standard_scaler") {
        this->standardScaler();
    }
}

void DataIterator::minMaxScaler() {

    // For X_DATA
    // Calculating Min
    for (int i = 0; i < this->n_feat; i++) {
        float min = this->rX[0][i];
        for (int j = 0; j < this->rX.size(); j++) {
            if (min > this->rX[j][i]) {
                min =  this->rX[j][i];
            }
        }

        this->minX.push_back(min);
    }

    // Calculating Max
    for (int i = 0; i < this->n_feat; i++) {
        float max = this->rX[0][i];
        for (int j = 0; j < this->rX.size(); j++) {
            if (max < this->rX[j][i]) {
                max = this->rX[j][i];
            }
        }

        this->maxX.push_back(max);
    }

    // Transformation
    for (int i = 0; i < this->rX.size(); i++) {
        vector<float> tmp;
        for (int j = 0; j < this->n_feat; j++) {
            tmp.push_back((this->rX[i][j] - this->minX[j]) / (this->maxX[j] - this->minX[j]));
        }

        this->tX.push_back(tmp);
    }


    // For Y_DATA
    // Calculating Min
    float min = this->rY[0][0];
    for (int i = 0; i < this->rY.size(); i++) {
        if (min > this->rY[i][0]) {
            min = this->rY[i][0];
        }
    }
    this->minY = min;

    // Calculating Max
    float max = this->rY[0][0];
    for (int i = 0; i < this->rY.size(); i++) {
        if (max < this->rY[i][0]) {
            max = this->rY[i][0];
        }
    }

    this->maxY = max;

    // Transformation
    for (int i = 0; i < this->rY.size(); i++) {
        this->tY.push_back(vector<float>{(this->rY[i][0] - this->minY) / (this->maxY - this->minY)});
    }
}

void DataIterator::standardScaler() {

    // For X_DATA
    // Calculating Mean
    for (int i = 0; i < this->n_feat; i++) {
        float mean = 0;
        for (int j = 0; j < this->rX.size(); j++) {
            mean += this->rX[j][i];
        }

        this->meanX.push_back(mean / this->rX.size());
    }

    // Calculating Std Deviation
    for (int i = 0; i < this->n_feat; i++) {
        float std = 0;
        for (int j = 0; j < this->rX.size(); j++) {
            std += (this->rX[j][i] - this->meanX[i]) * (this->rX[j][i] - this->meanX[i]);
        }

        std /= this->rX.size();
        std = sqrt(std);

        this->stdX.push_back(std);
    }

    // Transformation
    for (int i = 0; i < this->rX.size(); i++) {
        vector<float> tmp;
        for (int j = 0; j < this->n_feat; j++) {
            tmp.push_back((this->rX[i][j] - this->meanX[j]) / this->stdX[j]);
        }

        this->tX.push_back(tmp);
    }


    // For Y_DATA
    // Calculating Mean
    float mean = 0;
    for (int i = 0; i < this->rY.size(); i++) {
        mean += this->rY[i][0];
    }
    this->meanY = mean / this->rY.size();

    // Calculating Std Deviation
    float std = 0;
    for (int i = 0; i < this->rY.size(); i++) {
        std += (this->rY[i][0] - this->meanY) * (this->rY[i][0] - this->meanY);
    }

    std /= this->rY.size();
    std = sqrt(std);

    this->stdY = std;

    // Transformation
    for (int i = 0; i < this->rY.size(); i++) {
        this->tY.push_back(vector<float>{(this->rY[i][0] - this->meanY) / this->stdY});
    }
}

vector<float> DataIterator::transformX(vector<float> x) {
    vector<float> y;
    for (int i = 0; i < x.size(); i++) {
        if (this->type == "standard_scaler") {
            y.push_back((x[i] - this->meanX[i]) / this->stdX[i]);
        } else if (this->type == "min_max_scaler") {
            y.push_back((x[i] - this->minX[i]) / (this->maxX[i] - this->minX[i]));
        }
    }

    return y;
}

vector<float> DataIterator::transformY(vector<float> x) {
    vector<float> y;
    for (int i = 0; i < x.size(); i++) {
        if (this->type == "standard_scaler") {
            y.push_back((x[i] - this->meanY) / this->stdY);
        } else if (this->type == "min_max_scaler") {
            y.push_back((x[i] - this->minY) / (this->maxY - this->minY));
        }
    }

    return y;
}


vector<float> DataIterator::inverseTransformX(vector<float> x) {
    vector<float> y;
    for (int i = 0; i < x.size(); i++) {
        if (this->type == "standard_scaler") {
            y.push_back(x[i] * this->stdX[i] + this->meanX[i]);
        } else if (this->type == "min_max_scaler") {
            y.push_back(x[i] * (this->maxX[i] - this->minX[i]) + this->minX[i]);
        }
    }

    return y;
}

vector<float> DataIterator::inverseTransformY(vector<float> x) {
    vector<float> y;
    for (int i = 0; i < x.size(); i++) {
        if (this->type == "standard_scaler") {
            y.push_back(x[i] * this->stdY + this->meanY);
        } else if (this->type == "min_max_scaler") {
            y.push_back((x[i] * (this->maxY - this->minY)) + this->minY);
        }
    }

    return y;
}
