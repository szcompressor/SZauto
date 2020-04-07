#ifndef _utils_h
#define _utils_h

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <ctime>


template<typename Type>
Type *readfile(const char *file, size_t &num) {
    std::ifstream fin(file, std::ios::binary);
    if (!fin) {
        std::cout << " Error, Couldn't read file : " << file << "\n";
        return 0;
    }
    fin.seekg(0, std::ios::end);
    const size_t num_elements = fin.tellg() / sizeof(Type);
    fin.seekg(0, std::ios::beg);
    Type *data = (Type *) malloc(num_elements * sizeof(Type));
    fin.read(reinterpret_cast<char *>(&data[0]), num_elements * sizeof(Type));
    fin.close();
    num = num_elements;
    return data;
}

template<typename Type>
void readfile_to_buffer(char *file, size_t &num, Type *data) {
    std::ifstream fin(file, std::ios::binary);
    if (!fin) {
        std::cout << " Error, Couldn't find the file" << "\n";
        return;
    }
    fin.seekg(0, std::ios::end);
    const size_t num_elements = fin.tellg() / sizeof(Type);
    fin.seekg(0, std::ios::beg);
    fin.read(reinterpret_cast<char *>(&data[0]), num_elements * sizeof(Type));
    fin.close();
    num = num_elements;
}

template<typename Type>
void writefile(char *file, Type *data, size_t num_elements) {
    std::ofstream fout(file, std::ios::binary);
    fout.write(reinterpret_cast<const char *>(&data[0]), num_elements * sizeof(Type));
    fout.close();
}

template<typename Type>
double autocorrelation1D(const Type *data, size_t numOfElem, Type avg) {
    double cov = 0;
    for (size_t i = 0; i < numOfElem; i++) {
        cov += (data[i] - avg) * (data[i] - avg);
    }
    cov = cov / numOfElem;

    if (cov == 0) {
        return 0;
    } else {
        int delta = 1;
        double sum = 0;

        for (size_t i = 0; i < numOfElem - delta; i++) {
            sum += (data[i] - avg) * (data[i + delta] - avg);
        }
        return sum / (numOfElem - delta) / cov;
    }
}

template<typename Type>
void verify(Type *ori_data, Type *data, size_t num_elements, double &psnr, double &nrmse) {
    size_t i = 0;
    double Max = ori_data[0];
    double Min = ori_data[0];
    double diffMax = fabs(data[0] - ori_data[0]);
    double diff_sum = 0;
    double maxpw_relerr = fabs((data[0] - ori_data[0]) / ori_data[0]);
    double sum1 = 0, sum2 = 0;
    for (i = 0; i < num_elements; i++) {
        sum1 += ori_data[i];
        sum2 += data[i];
    }
    double mean1 = sum1 / num_elements;
    double mean2 = sum2 / num_elements;

    double sum3 = 0, sum4 = 0;
    double sum = 0, prodSum = 0, relerr = 0;

    double *diff = (double *) malloc(num_elements * sizeof(double));

    for (i = 0; i < num_elements; i++) {
        diff[i] = data[i] - ori_data[i];
        diff_sum += data[i] - ori_data[i];
        if (Max < ori_data[i]) Max = ori_data[i];
        if (Min > ori_data[i]) Min = ori_data[i];
        double err = fabs(data[i] - ori_data[i]);
        if (ori_data[i] != 0) {
            relerr = err / fabs(ori_data[i]);
            if (maxpw_relerr < relerr)
                maxpw_relerr = relerr;
        }

        if (diffMax < err)
            diffMax = err;
        prodSum += (ori_data[i] - mean1) * (data[i] - mean2);
        sum3 += (ori_data[i] - mean1) * (ori_data[i] - mean1);
        sum4 += (data[i] - mean2) * (data[i] - mean2);
        sum += err * err;
    }
    double std1 = sqrt(sum3 / num_elements);
    double std2 = sqrt(sum4 / num_elements);
    double ee = prodSum / num_elements;
    double acEff = ee / std1 / std2;

    double mse = sum / num_elements;
    double range = Max - Min;
    psnr = 20 * log10(range) - 10 * log10(mse);
    nrmse = sqrt(mse) / range;

    printf("Min=%.20G, Max=%.20G, range=%.20G\n", Min, Max, range);
    printf("Max absolute error = %.2G\n", diffMax);
    printf("Max relative error = %.2G\n", diffMax / (Max - Min));
    printf("Max pw relative error = %.2G\n", maxpw_relerr);
    printf("PSNR = %f, NRMSE= %.10G\n", psnr, nrmse);
    printf("acEff=%f\n", acEff);
    printf("errAutoCorr=%.10f\n", autocorrelation1D<double>(diff, num_elements, diff_sum / num_elements));
    free(diff);
}


template<typename Type>
void verify(Type *ori_data, Type *data, size_t num_elements) {
    double psnr, nrmse;
    verify(ori_data, data, num_elements, psnr, nrmse);
}

#endif