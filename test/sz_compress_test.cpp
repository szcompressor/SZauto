#include "sz_autotuning_3d.hpp"
#include "sz_def.hpp"
#include "sz_utils.hpp"

using namespace std;


int main(int argc, char **argv) {
    size_t num_elements = 0;
    float *data = readfile<float>(argv[1], num_elements);
    float max = data[0];
    float min = data[0];
    for (int i = 1; i < num_elements; i++) {
        if (max < data[i]) max = data[i];
        if (min > data[i]) min = data[i];
    }
    int r1 = atoi(argv[2]);
    int r2 = atoi(argv[3]);
    int r3 = atoi(argv[4]);
    float eb = atof(argv[5]);
    float precision = eb * (max - min);

    if (argc <= 6) {
        cout << "precision = " << precision << endl;
        sz_compress_manualtuning_3d(data, num_elements, r1, r2, r3, eb, eb * (max - min));
        free(data);
        return 0;
    }

    int block_size = atoi(argv[6]);
    int pred_dim = atoi(argv[7]);
    int lorenzo_op = atoi(argv[8]);
    int regression_op = atoi(argv[9]);
    int capacity = atoi(argv[10]);

    bool lorenzo = lorenzo_op == 1 || lorenzo_op == 3;
    bool lorenzo_2layer = lorenzo_op == 2 || lorenzo_op == 3;
    bool regression_linear = regression_op == 1 || regression_op == 3;
    bool regression_poly = regression_op == 2 || regression_op == 3;

    if (argc <= 11) {
        cout << "precision = " << precision << endl;
        cout << "Options: block_size = " << block_size << ", pred_dim = " << pred_dim
             << ", lorenzo = " << lorenzo
             << ", lorenzo2 = " << lorenzo_2layer
             << ", regression = " << regression_linear
             << ", regression2 = " << regression_poly
             << ", capacity = " << capacity
             << endl;
        sz_params params(false, block_size, pred_dim, 0, lorenzo, lorenzo_2layer,
                         regression_linear, regression_poly, precision);
        params.capacity = capacity;
        params.filename = argv[1];
        params.eb = eb;
        sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3, eb * (max - min), params, true);
        free(data);
        return 0;
    }
    float reg_eb_base = atof(argv[11]);
    float reg_eb_1 = atof(argv[12]);
    float poly_reg_eb_base = atof(argv[13]);
    float poly_reg_eb_1 = atof(argv[14]);
    float poly_reg_eb_2 = atof(argv[15]);
    float poly_reg_noise = atof(argv[16]);
    cout << "precision = " << precision << endl;
    cout << "Options: block_size = " << block_size << ", pred_dim = " << pred_dim
         << ", lorenzo = " << lorenzo
         << ", lorenzo2 = " << lorenzo_2layer
         << ", regression = " << regression_linear
         << ", regression2 = " << regression_poly
         << endl
         << " reg_eb_base=" << reg_eb_base
         << ", reg_eb_1=" << reg_eb_1
         << ", poly_reg_eb_base=" << poly_reg_eb_base
         << ", poly_reg_eb_1=" << poly_reg_eb_1
         << ", poly_reg_eb_2=" << poly_reg_eb_2
         << ", poly_regression_noise=" << poly_reg_noise
         << endl;
    sz_params params(false, block_size, pred_dim, 0, lorenzo, lorenzo_2layer,
                     regression_linear, regression_poly, precision, reg_eb_base, reg_eb_1,
                     poly_reg_eb_base, poly_reg_eb_1, poly_reg_eb_2, poly_reg_noise);
    params.filename = argv[1];
    params.eb = eb;
    sz_compress_decompress_highorder_3d(data, num_elements, r1, r2, r3, eb * (max - min), params, true);

    free(data);
    return 0;

}