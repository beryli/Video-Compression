#include "basic.h"
#include "autocorrelation.h"

vector<vector<vector<double>>> rxx (21, vector<vector<double>> (height/2, vector<double> (width/2, 0)));

vector<vector<vector<double>>> getAutoRxxFromMocom (const vector<vector<vector<int>>> &luma) {
    cout << "Start getAutoRxxFromMocom" << endl;
    
    vector<vector<vector<int>>> comluma (frame, vector<vector<int>> (height, vector<int> (width, 0)));
    vector<vector<vector<int>>> mu0luma (frame, vector<vector<int>> (height, vector<int> (width)));
    vector<vector<vector<vector<int>>>> mu0comluma (11, vector<vector<vector<int>>> (frame, vector<vector<int>> (height, vector<int> (width))));
    mu0luma = minusMean (luma);

    thread mythread[11];
    for (int i = 0; i <= 10; i++) { cout << "i: " << i << endl;
        if (i == 0) {mu0comluma[i] = mu0luma;}
        else {comluma = motionEstFromPrevN (i, luma); mu0comluma[i] = minusMean (comluma);}
        mythread[i] = thread(runOneRxxFrame, i, 0, mu0luma, mu0comluma[i]);
    }
    for (int i = 0; i <= 10; i++) {
        mythread[i].join();
    }
    /*
    int j, k, value;
    // for (j = 0; j <= 1; j++){ // test
    for (j = -72; j <= 71; j++){ cout << j << " ";
        // for (k = 0; k <= 1; k++){ // test
        for (k = -88; k <= 87; k++){
            // for (int a = 0; a < 10; a++){ // test
            for (int a = 0; a < frame; a++){
                value = 0;
                // for (int b = 0; b < 10; b++){ // test
                for (int b = j>0?0:-j; b < height && b+j < height; b++){
                    // for (int c = 0; c < 10; c++){ // test
                    for (int c = k>0?0:-k; c < width && c+k < width; c++){
                        value += mu0luma[a][b][c] * mu0comluma[a][b+j][c+k];
                    }
                }
            rxx[10-N][j+72][k+88] += value / (double)((height-j) * (width-k));
            }
        rxx[10-N][j+72][k+88] /= (frame);
        }
    }
    */
    cout << "End getAutoRxxFromMocom" << endl;
    return rxx;
}

vector<vector<vector<int>>> motionEstFromPrevN (int N, const vector<vector<vector<int>>> &luma) { // N from 0 ~ 10
    cout << "Start motionEstFromPrevN" << endl;
    vector<vector<vector<int>>> comluma (frame, vector<vector<int>> (height, vector<int> (width, 0)));
    int i, j, k, n = 4;
    for (i = 0; i < N; i++){
        for (j = 0; j < height; j++){
            for (k = 0; k < width; k++){
                comluma[i][j][k] = luma[i][j][k];
            }
        }
    }

    int x, y, u, v;
    int currSAD, minSAD, xx, yy;
    // for (i = N; i < N+2; i++){ // test
    for (i = N; i < frame; i++){  cout << i << " ";
        for (j = 0; j < height; j=j+n){
            for (k = 0; k < width; k=k+n){ // motion search for each block in range 32 * 32
                minSAD = INT_MAX;
                for (x = j-16>=0 ? j-16 : 0; x+n <= height && x+n <= j+16; x++) {
                    for (y = k-16>=0 ? k-16 : 0; y+n <= width && y+n <= k+16; y++) {
                        // sum of absolute difference
                        currSAD = 0;
                        for (u = 0; u < n; u++){
                            for (v = 0; v < n; v++){
                                currSAD += abs(luma[i][j+u][k+v] - comluma[i-N][x+u][y+v]);
                            }
                        }
                        if (currSAD < minSAD) {
                            minSAD = currSAD;
                            xx = x, yy = y;
                        }
                    }
                }
                for (u = 0; u < n; u++){
                    for (v = 0; v < n; v++){
                        comluma[i][j+u][k+v] = luma[i-N][xx+u][yy+v];
                    }
                }
            }
        }
    }
    cout << "End motionEstFromPrevN" << endl;
    return comluma;
}





void runOneRxxFrame (int N, int i
, const vector<vector<vector<int>>> &mu0luma, const vector<vector<vector<int>>> &mu0comluma) {
    int j, k;
    double value = 0;
    for (j = -72; j <= 71; j++){ cout << "N" << N << "i" << i << ":" << j << " ";
        // for (k = 0; k <= 1; k++){ // test
        for (k = -88; k <= 87; k++){
            // for (int a = 0; a < 10; a++){ // test
            for (int a = 0; a < frame && a+i < frame; a++){
                value = 0;
                // for (int b = 0; b < 10; b++){ // test
                for (int b = j>0?0:-j; b < height && b+j < height; b++){
                    // for (int c = 0; c < 10; c++){ // test
                    for (int c = k>0?0:-k; c < width && c+k < width; c++){
                        value += mu0luma[a][b][c] * mu0comluma[a+i][b+j][c+k];
                    }
                }
            rxx[10-N][j+72][k+88] += value / (double)((height-j) * (width-k));
            }
        rxx[10-N][j+72][k+88] /= (frame);
        }
    }
}

vector<vector<vector<int>>> normalizeRxx (const vector<vector<vector<double>>> &rxx) {
    vector<vector<vector<int>>> normalized_rxx (21, vector<vector<int>> (height/2, vector<int> (width/2)));
    double minRxx = rxx[0][0][0], maxRxx = rxx[0][0][0];
    int i, j, k;
    for (i = 0; i <= 10; i++){
        for (j = 0; j < height/2; j++){
            for (k = 0; k < width/2; k++){
                if (rxx[i][j][k] > maxRxx)
                    maxRxx = rxx[i][j][k];
                if (rxx[i][j][k] < minRxx)
                    minRxx = rxx[i][j][k];
            }
        }
    } cout << "minRxx:" << minRxx << ", maxRxx:" << maxRxx << endl;

    for (i = 0; i <= 10; i++){
        for (j = 0; j < height/2; j++){
            for (k = 0; k < width/2; k++){
                normalized_rxx[i][j][k] = round ((rxx[i][j][k] - minRxx) / (maxRxx - minRxx) * 255);
            }
        }
    }

    for (i = 0; i <= 10; i++){
        for (j = 0; j < height/2; j++){
            for (k = 0; k < width/2; k++){
                normalized_rxx[10+i][j][k] = normalized_rxx[10-i][j][k];
            }
        }
    }
    return normalized_rxx;
}

vector<vector<vector<double>>> getAutoRxx (const vector<vector<vector<int>>> &mu0luma) {
    cout << "Start getAutoRxx";
    // vector<vector<vector<double>>> rxx (21, vector<vector<double>> (height/2, vector<double> (width/2, 0)));

    thread rxx0 (runOneRxxFrame, 0, 0, mu0luma, mu0luma);
    thread rxx1 (runOneRxxFrame, 1, 1, mu0luma, mu0luma);
    thread rxx2 (runOneRxxFrame, 2, 2, mu0luma, mu0luma);
    thread rxx3 (runOneRxxFrame, 3, 3, mu0luma, mu0luma);
    thread rxx4 (runOneRxxFrame, 4, 4, mu0luma, mu0luma);
    thread rxx5 (runOneRxxFrame, 5, 5, mu0luma, mu0luma);
    thread rxx6 (runOneRxxFrame, 6, 6, mu0luma, mu0luma);
    thread rxx7 (runOneRxxFrame, 7, 7, mu0luma, mu0luma);
    thread rxx8 (runOneRxxFrame, 8, 8, mu0luma, mu0luma);
    thread rxx9 (runOneRxxFrame, 9, 9, mu0luma, mu0luma);
    thread rxx10 (runOneRxxFrame, 10, 10, mu0luma, mu0luma);

    rxx0.join();
    rxx1.join();
    rxx2.join();
    rxx3.join();
    rxx4.join();
    rxx5.join();
    rxx6.join();
    rxx7.join();
    rxx8.join();
    rxx9.join();
    rxx10.join();
    /*
    int i, j, k, value;
    // for (i = 0; i <= 1; i++){ // test
    for (i = 0; i <= 10; i++){ cout << endl << "i" << i << ":";
        // for (j = 0; j <= 1; j++){ // test
        for (j = -72; j <= 71; j++){ cout << j << " ";
            // for (k = 0; k <= 1; k++){ // test
            for (k = -88; k <= 87; k++){
                // for (int a = 0; a < 10; a++){ // test
                for (int a = 0; a < frame && a+i < frame; a++){
                    value = 0;
                    // for (int b = 0; b < 10; b++){ // test
                    for (int b = j>0?0:-j; b < height && b+j < height; b++){
                        // for (int c = 0; c < 10; c++){ // test
                        for (int c = k>0?0:-k; c < width && c+k < width; c++){
                            value += mu0luma[a][b][c] * mu0luma[a+i][b+j][c+k];
                        }
                    }
                rxx[10-i][j+72][k+88] += value / (double)((height-j) * (width-k));
                }
            rxx[10-i][j+72][k+88] /= (frame - i);
            }
        }
    }
    */
    cout << "End getAutoRxx" << endl;
    return rxx;
}

vector<vector<vector<int>>> minusMean (const vector<vector<vector<int>>> &luma) {
    vector<double> mean (frame, 0);
    int i, j, k;
    for (i = 0; i < frame; i++){
        for (j = 0; j < height; j++){
            for (k = 0; k < width; k++){
                mean[i] += luma[i][j][k];
            }
        }
        mean[i] /= 101376;
    }
    vector<vector<vector<int>>> mu0luma (frame, vector<vector<int>> (height, vector<int> (width)));
    for (i = 0; i < frame; i++){
        for (j = 0; j < height; j++){
            for (k = 0; k < width; k++){
                mu0luma[i][j][k] = round(luma[i][j][k] - mean[i]);
            }
        }
    }
    return mu0luma;
}