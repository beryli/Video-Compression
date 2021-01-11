#include "basic.h"
#include "wiener.h"

vector<int> getTotalBits (map<int, int> &freq
, const vector<vector<vector<vector<movec>>>> &movecs, const vector<vector<vector<int>>> &residual
, int &symbolNum, int &totalBits) {
    vector<int> bitsNum (frame, 0);
    double bitsSum;
    totalBits = 0;
    int i, j, k, t;
    for (i = 4; i < frame; i++){
        bitsSum = 0;
        // motion vector
        for (j = 0; j < movecs[i].size(); j++) {
            for (k = 0; k < movecs[i][j].size(); k++) {
                for (t = 0; t < movecs[i][j][k].size(); t++) {
                    bitsSum += log2( (double)  (double) symbolNum / freq[movecs[i][j][k][t].t]);
                    bitsSum += log2( (double)  (double) symbolNum / freq[movecs[i][j][k][t].x]);
                    bitsSum += log2( (double)  (double) symbolNum / freq[movecs[i][j][k][t].y]);
                }
            }
        }
        // residual
        for (j = 0; j < height; j++) {
            for (k = 0; k < width; k++) {
                bitsSum += log2( (double) symbolNum / freq[residual[i][j][k]]);
            }
        }
        bitsNum[i] = round(bitsSum);
        totalBits += bitsNum[i];
    }
    cout << "Total required bits: " << totalBits << endl;
    return bitsNum;
}

map<int, int> countFreq (const vector<vector<vector<vector<movec>>>> &movecs
, const vector<vector<vector<int>>> &residual, const int N, const int n
, int &symbolNum) {
    cout << "Start countFreq" << endl;
    movec motion;
    map<int, int> freq;
    int i, j, k, t;
    for (i = N; i < frame; i++){
        // motion vector
        for (j = 0; j < height; j=j+n) {
            for (k = 0; k < width; k=k+n) {
                motion = movecs[i][j/n][k/n][0];
                freq[motion.t]++;
                freq[motion.x]++;
                freq[motion.y]++;
            }
        }
        // residual
        for (j = 0; j < height; j++) {
            for (k = 0; k < width; k++) {
                freq[residual[i][j][k]]++;
            }
        }
    }
    // calculate total symbol number
    symbolNum = 0;
    for (i = -255; i <= 255; i++){
        if (freq[i] == 0) {freq[i]++;}
        symbolNum += freq[i];
    }
    // block size 16: (101376 + 18*22*3) * 96 = 9846144
    cout << "Total symbol number: " << symbolNum << endl;
    return freq;
}

vector<vector<vector<int>>> getReconstLuma (const int N, const int n
, const vector<vector<vector<int>>> &luma, const vector<vector<vector<vector<movec>>>> &movecs) {
    cout << "Start getReconstLuma" << endl;
    vector<vector<vector<int>>> reconstLuma (frame, vector<vector<int>> (height, vector<int> (width, 0)));
    int i, j, k;
    for (i = 0; i < N; i++){
        for (j = 0; j < height; j++){
            for (k = 0; k < width; k++){ 
                reconstLuma[i][j][k] = luma[i][j][k];
            }
        }
    }
    int t;
    for (i = N; i < frame; i++){ cout << i << " ";
        for (j = 0; j < height; j++){
            for (k = 0; k < width; k++){ // for each block in search range 32 * 32
                movec mv = movecs[i][j/n][k/n][0];
                reconstLuma[i][j][k] = luma[i-mv.t][j+mv.x][k+mv.y];
            }
        }
    }
    cout << "End getReconstLuma" << endl;
    return reconstLuma;
}



void output_SNR_Bits_CSV (const string &filename, const vector<double> &SNR, const vector<int> &bits
, int totalBits, int symbolNum) {
    cout << "Start output_SNR_Bits_CSV" << endl;
    ofstream fout("result/"+filename+".csv");
    if(fout.is_open()) {
        cout << "output_SNR_Bits_CSV ing..." << endl;
        int i;
        fout << "SNR" << "," << "bits" << endl;
        for (i = 0; i < frame; i++) { 
            fout << SNR[i] <<  "," << bits[i] << endl;
        }
        fout << "totalBits" << "," << totalBits << endl;
        fout << "totalSymbols" << "," << symbolNum << endl;
    }
    fout.close();
    cout << "End output_SNR_Bits_CSV" << endl;
    return;
}

vector<int> getTotalBits_Space (map<int, int> &freq
, const vector<vector<vector<int>>> &residual
, const vector<vector<vector<vector<double>>>> &wiener_o, double &minW, double &maxW, int &symbolNum
, int &totalBits) {
    vector<int> bitsNum (frame, 0);
    double bitsSum;
    totalBits = 0;
    int i, j, k, t;
    double diffW = maxW - minW;
    for (i = 4; i < frame; i++){
        bitsSum = 0;
        // residual
        for (j = 0; j < height; j++) {
            for (k = 0; k < width; k++) {
                bitsSum += log2( (double) symbolNum / freq[residual[i][j][k]]);
            }
        }
        // wiener_o
        for (j = 0; j < wiener_o[i].size(); j++) {
            for (k = 0; k < wiener_o[i][j].size(); k++) {
                for (t = 0; t < wiener_o[i][j][k].size(); t++) {
                    bitsSum += log2( (double) symbolNum / freq[ round (255 * (wiener_o[i][j][k][t] - minW) / diffW)]);
                }
            }
        }
        bitsNum[i] = round(bitsSum);
        totalBits += bitsNum[i];
    }
    cout << "Total required bits: " << totalBits << endl;
    return bitsNum;
}

vector<int> getTotalBits_Time (map<int, int> &freq, const int n
, const vector<vector<vector<vector<movec>>>> &movecs, const vector<vector<vector<int>>> &residual
, const vector<vector<vector<vector<double>>>> &wiener_o, double &minW, double &maxW, int &symbolNum
, int &totalBits) {
    vector<int> bitsNum (frame, 0);
    double bitsSum;
    totalBits = 0;
    int i, j, k, t;
    double diffW = maxW - minW;
    for (i = 4; i < frame; i++){
        bitsSum = 0;
        // motion vector
        for (j = 0; j < height; j=j+n) {
            for (k = 0; k < width; k=k+n) {
                for (t = 0; t < movecs[i][j/n][k/n].size(); t++) {
                    // bitsSum += log2( (double)  (double) symbolNum / freq[movecs[i][j/n][k/n][t].t]);
                    bitsSum += log2( (double)  (double) symbolNum / freq[movecs[i][j/n][k/n][t].x]);
                    bitsSum += log2( (double)  (double) symbolNum / freq[movecs[i][j/n][k/n][t].y]);
                }
            }
        }
        // residual
        for (j = 0; j < height; j++) {
            for (k = 0; k < width; k++) {
                bitsSum += log2( (double) symbolNum / freq[residual[i][j][k]]);
            }
        }
        // wiener_o
        for (j = 0; j < wiener_o[i].size(); j++) {
            for (k = 0; k < wiener_o[i][j].size(); k++) {
                for (t = 0; t < wiener_o[i][j][k].size(); t++) {
                    bitsSum += log2( (double) symbolNum / freq[ round (255 * (wiener_o[i][j][k][t] - minW) / diffW)]);
                }
            }
        }
        bitsNum[i] = round(bitsSum);
        totalBits += bitsNum[i];
    }
    cout << "Total required bits: " << totalBits << endl;
    return bitsNum;
}

map<int, int> countFreq_Space (const vector<vector<vector<int>>> &residual
, const vector<vector<vector<vector<double>>>> &wiener_o, double &minW, double &maxW, int &symbolNum) {
    cout << "Start countFreq_Space" << endl;
    map<int, int> freq;
    double diffW = maxW - minW;
    int i, j, k, t;
    for (i = 0; i < frame; i++){ cout << i << " ";
        // residual
        for (j = 0; j < height; j++) {
            for (k = 0; k < width; k++) {
                freq[residual[i][j][k]]++;
            }
        }
        // wiener_o
        for (j = 0; j < wiener_o[i].size(); j++) {
            for (k = 0; k < wiener_o[i][j].size(); k++) {
                for (t = 0; t < wiener_o[i][j][k].size(); t++) {
                    freq[ round (255 * (wiener_o[i][j][k][t] - minW) / diffW) ]++;
                }
            }
        }
    }
    // calculate total symbol number
    symbolNum = 0;
    for (i = -255; i <= 255; i++){
        if (freq[i] == 0) {freq[i]++;}
        symbolNum += freq[i];
    }
    cout << "Total symbol number: " << symbolNum << endl;
    return freq;
}

map<int, int> countFreq_Time (const vector<vector<vector<vector<movec>>>> &movecs
, const vector<vector<vector<int>>> &residual, const int N, const int n
, const vector<vector<vector<vector<double>>>> &wiener_o, double &minW, double &maxW, int &symbolNum) {
    cout << "Start countFreq_Time" << endl;
    map<int, int> freq;
    double diffW = maxW - minW;
    int i, j, k, t;
    for (i = N; i < frame; i++){ cout << i << " ";
        // motion vector
        for (j = 0; j < height; j=j+n) {
            for (k = 0; k < width; k=k+n) {
                for (t = 0; t < movecs[i][j/n][k/n].size(); t++) {
                    // freq[movecs[i][j/n][k/n][t].t]++;
                    freq[movecs[i][j/n][k/n][t].x]++;
                    freq[movecs[i][j/n][k/n][t].y]++;
                }
            }
        }
        // residual
        for (j = 0; j < height; j++) {
            for (k = 0; k < width; k++) {
                freq[residual[i][j][k]]++;
            }
        }
        // wiener_o
        for (j = 0; j < wiener_o[i].size(); j++) {
            for (k = 0; k < wiener_o[i][j].size(); k++) {
                for (t = 0; t < wiener_o[i][j][k].size(); t++) {
                    freq[ round (255 * (wiener_o[i][j][k][t] - minW) / diffW) ]++;
                }
            }
        }
    }
    // calculate total symbol number
    symbolNum = 0;
    for (i = -255; i <= 255; i++){
        if (freq[i] == 0) {freq[i]++;}
        symbolNum += freq[i];
    }
    // block size 16: (101376 + 18*22*3) * 96 = 9846144
    cout << "Total symbol number: " << symbolNum << endl;
    return freq;
}

vector<double> getSNR (const string filename, const vector<vector<vector<int>>> &luma
, const vector<vector<vector<int>>> &residual) {
    cout << "Start getSNR" << endl;
    vector<double> mean_x (frame, 0);
    vector<double> mean_e (frame, 0);
    vector<double> var_x (frame);
    vector<double> var_e (frame);
    vector<double> SNR (frame);
    int i, j, k;
    // calculate mean;
    for (i = 0; i < frame; i++) { // for each frame
        for (j = 0; j < height; j++) {
            for (k = 0; k < width; k++) {
                mean_x[i] += luma[i][j][k];
                mean_e[i] += residual[i][j][k];
            }
        }
        mean_x[i] /= (height * width);
        mean_e[i] /= (height * width);
    }
    // calculate sigma
    for (i = 0; i < frame; i++) { // for each frame
        var_x[i] = - mean_x[i] * mean_x[i];
        var_e[i] = - mean_e[i] * mean_e[i];
        for (j = 0; j < height; j++) {
            for (k = 0; k < width; k++) {
                var_x[i] += luma[i][j][k] * luma[i][j][k] / (double) (height * width);
                var_e[i] += residual[i][j][k] * residual[i][j][k] / (double) (height * width);
            }
        }
        SNR[i] = var_e[i] / var_x[i];
    }
    // output csv
    ofstream fout("result/"+filename+".csv");
    fout << "mean_x[i]" << "," << "mean_e[i]" << "," 
         << "var_x[i]"  << "," << "var_e[i]"  << ","
         << "SNR[i]"    << endl;
    for (i = 0; i < frame; i++) { 
        fout << mean_x[i] << "," << mean_e[i] << "," 
             << var_x[i]  << "," << var_e[i]  << ","
             << SNR[i]    << endl;
    }
    fout.close();

    cout << "End getSNR" << endl;
    return SNR;
}



vector<vector<vector<int>>> getReconstLuma_Space (const int N, const int n
, const vector<vector<vector<int>>> &luma, const vector<vector<vector<vector<double>>>> &wiener_o) {
    cout << "Start getReconstLuma_Space" << endl;
    vector<vector<vector<int>>> reconstLuma (frame, vector<vector<int>> (height, vector<int> (width, 0)));
    int i, j, k, t;
    double pred;
    for (i = 0; i < frame; i++){ cout << i << " ";
        for (j = 0; j < height; j++){
            for (k = 0; k < width; k++){ // for each block in search range 32 * 32
                reconstLuma[i][j][k] = 0;
                pred = 0;
                for (t = 0; t < N; t++){
                    if (t == 0) { // D (left)
                        if (k-1 < 0) {pred += 128 * wiener_o[i][j/n][k/n][t];}
                        else {pred += luma[i][j][k-1] * wiener_o[i][j/n][k/n][t];} }
                    if (t == 1) {
                        if (j-1 < 0) {pred += 128 * wiener_o[i][j/n][k/n][t];}
                        else {pred += luma[i][j-1][k] * wiener_o[i][j/n][k/n][t];} }
                    if (t == 2) {
                        if (j-1 < 0 || k-1 < 0) {pred += 128 * wiener_o[i][j/n][k/n][t];}
                        else {pred += luma[i][j-1][k-1] * wiener_o[i][j/n][k/n][t];} }
                    if (t == 3) {
                        if (j-1 < 0 || k+1 >= width) {pred += 128 * wiener_o[i][j/n][k/n][t];}
                        else {pred += luma[i][j-1][k+1] * wiener_o[i][j/n][k/n][t];} }
                }
                if (pred > 255) {pred = 255;}
                if (pred < 0) {pred = 0;}
                reconstLuma[i][j][k] = round(pred);
            }
        }
    }
    cout << "End getReconstLuma_Space" << endl;
    return reconstLuma;
}

vector<vector<vector<vector<double>>>> getWienerCoef_Space (const int N, const int n
, const vector<vector<vector<int>>> &luma, double &minW, double &maxW) {
    cout << "Start getWienerCoef_Space" << endl;
    double minInvR=0, maxInvR=0, minR=n*n*256*256, maxR=0, minP=N*256*256, maxP=0;
    minW=0, maxW=0;
    vector<vector<vector<vector<double>>>> wiener_o (frame, vector<vector<vector<double>>> (height/n, vector<vector<double>>(width/n, vector<double> (N))));
    vector<vector<double>> invR (N, vector<double> (N));
    vector<vector<double>> R (N, vector<double> (N));
    vector<double> p (N);
    movec mv;
    bool isInverse;
    int i, j, k, x, y, u, v, idx;
    vector<vector<int>> input (N, vector<int> (n*n));

    for (i = 0; i < frame; i++){ cout << i << " ";
        for (j = 0; j < height; j=j+n){
            for (k = 0; k < width; k=k+n){ 
                // for each block in search range 32 * 32
                // intput[N][n*n]
                for (u = 0; u < N; u++) {
                    idx = 0;
                    for (x = j; x < j+n; x++) {
                        for (y = k; y < k+n; y++) {
                            if (u == 0) {
                                if (y-1 < 0) {input[u][idx] = 128;}
                                else {input[u][idx] = luma[i][x][y-1];} }
                            if (u == 1) {
                                if (x-1 < 0) {input[u][idx] = 128;}
                                else {input[u][idx] = luma[i][x-1][y];} }
                            if (u == 2) {
                                if (x-1 < 0 || y-1 < 0) {input[u][idx] = 128;}
                                else {input[u][idx] = luma[i][x-1][y-1];} }
                            if (u == 3) {
                                if (x-1 < 0 || y+1 >= width) {input[u][idx] = 128;}
                                else {input[u][idx] = luma[i][x-1][y+1];} }
                            idx++;
                        }
                    }
                }
                // R[N][N]
                for (u = 0; u < N; u++) {
                    for (v = 0; v < N; v++) {
                        R[u][v] = 0;
                        for (x = 0; x < n*n; x++) {
                            R[u][v] += (input[u][x] * input[v][x] / (double) (n*n));
                        }
                        // testminmax
                        if (minR > R[u][v]) {minR = R[u][v];}
                        if (maxR < R[u][v]) {maxR = R[u][v];}
                    }
                }
                // p[N]
                for (u = 0; u < N; u++) {
                    p[u] = 0;
                    idx = 0;
                    for (x = j; x < j+n; x++) {
                        for (y = k; y < k+n; y++) {
                            p[u] += (input[u][idx++] * luma[i][x][y] / (double) (n*n));
                        }
                    }
                    // testminmax
                    if (minP > p[u]) {minP = p[u];}
                    if (maxP < p[u]) {maxP = p[u];}
                }
                // invR[N][N]
                if (N == 4) {isInverse = inv4 (R, invR);}
                else if (N == 2) {isInverse = inv2 (R, invR);}
                else {invR[0][0] = (double) 1 / R[0][0];}
                // wiener filter
                if (isInverse == false) {
                    for (u = 0; u < N; u++) {
                        wiener_o[i][j/n][k/n][u] = 1 / (double)N;
                    }
                } else {
                    for (u = 0; u < N; u++) {
                        wiener_o[i][j/n][k/n][u] = 0;
                        for (v = 0; v < N; v++) {
                            wiener_o[i][j/n][k/n][u] += p[v] * invR[u][v];
                            if (minInvR > invR[u][v]) {minInvR = invR[u][v];}
                            if (maxInvR < invR[u][v]) {maxInvR = invR[u][v];}
                        }
                        if (minW > wiener_o[i][j/n][k/n][u]) {minW = wiener_o[i][j/n][k/n][u];}
                        if (maxW < wiener_o[i][j/n][k/n][u]) {maxW = wiener_o[i][j/n][k/n][u];}
                    }
                } // end
            }
        }
    }
    cout << endl;
    cout << "minR:    " << setw(10) << minR    << ",   maxR:    " << setw(10) << maxR    << endl;
    cout << "minInvR: " << setw(10) << minInvR << ",   maxInvR: " << setw(10) << maxInvR << endl;
    cout << "minP:    " << setw(10) << minP    << ",   maxP:    " << setw(10) << maxP    << endl;
    cout << "minW:    " << setw(10) << minW    << ",   maxW:    " << setw(10) << maxW    << endl;
    cout << "End getWienerCoef_Space" << endl;
    return wiener_o;
}

vector<vector<vector<int>>> getReconstLuma_Time (const int N, const int n
, const vector<vector<vector<int>>> &luma, const vector<vector<vector<vector<movec>>>> &movecs
, const vector<vector<vector<vector<double>>>> &wiener_o) {
    cout << "Start getReconstLuma_Time" << endl;
    vector<vector<vector<int>>> reconstLuma (frame, vector<vector<int>> (height, vector<int> (width, 0)));
    int i, j, k;
    for (i = 0; i < N; i++){
        for (j = 0; j < height; j++){
            for (k = 0; k < width; k++){ 
                reconstLuma[i][j][k] = luma[i][j][k];
            }
        }
    }
    int t;
    double pred;
    for (i = N; i < frame; i++){
        for (j = 0; j < height; j++){
            for (k = 0; k < width; k++){ // for each block in search range 32 * 32
                reconstLuma[i][j][k] = 0;
                pred = 0;
                for (t = i-N; t < i; t++){
                    movec mv = movecs[i][j/n][k/n][i-t-1];
                    pred += wiener_o[i][j/n][k/n][i-t-1] * luma[t][j+mv.x][k+mv.y];
                }
                if (pred > 255) {pred = 255;}
                if (pred < 0) {pred = 0;}
                reconstLuma[i][j][k] = round(pred);
            }
        }
    }
    cout << "End getReconstLuma_Time" << endl;
    return reconstLuma;
}

vector<vector<vector<vector<double>>>> getWienerCoef_Time (const int N, const int n
, const vector<vector<vector<int>>> &luma, const vector<vector<vector<vector<movec>>>> &movecs
, double &minW, double &maxW) {
    cout << "Start getWienerCoef_Time" << endl;
    double minInvR=0, maxInvR=0, minR=n*n*256*256, maxR=0, minP=N*256*256, maxP=0;
    minW=0, maxW=0;
    vector<vector<vector<vector<double>>>> wiener_o (frame, vector<vector<vector<double>>> (height/n, vector<vector<double>>(width/n, vector<double> (N))));
    vector<vector<double>> invR (N, vector<double> (N));
    vector<vector<double>> R (N, vector<double> (N));
    vector<double> p (N);
    movec mv;
    bool isInverse;
    int i, j, k, x, y, u, v;
    for (i = N; i < frame; i++){ cout << i << " ";
        for (j = 0; j < height; j=j+n){
            for (k = 0; k < width; k=k+n){ // for each block in search range 32 * 32
                // R[N][N]
                for (u = 0; u < N; u++) {
                    for (v = 0; v < N; v++) {
                        R[u][v] = 0;
                        movec mv1 = movecs[i][j/n][k/n][u];
                        movec mv2 = movecs[i][j/n][k/n][v];
                        for (x = j; x < j+n; x++) {
                            for (y = k; y < k+n; y++) {
                                R[u][v] += luma[i-1-u][x+mv1.x][y+mv1.y] 
                                         * luma[i-1-v][x+mv2.x][y+mv2.y] / (double) (n*n);
                            }
                        }
                        // testminmax
                        if (minR > R[u][v]) {minR = R[u][v];}
                        if (maxR < R[u][v]) {maxR = R[u][v];}
                    }
                }
                // p[N]
                for (u = 0; u < N; u++) {
                    p[u] = 0;
                    movec mv = movecs[i][j/n][k/n][u];
                    for (x = j; x < j+n; x++) {
                        for (y = k; y < k+n; y++) {
                            p[u] += luma[i-1-u][x+mv.x][y+mv.y] * luma[i][x][y] / (double) (n*n);
                        }
                    }
                    // testminmax
                    if (minP > p[u]) {minP = p[u];}
                    if (maxP < p[u]) {maxP = p[u];}
                }
                // invR[N][N]
                if (N == 4) {isInverse = inv4 (R, invR);}
                else if (N == 2) {isInverse = inv2 (R, invR);}
                else {invR[0][0] = (double) 1 / R[0][0];}
                // wiener filter
                if (isInverse == false) {
                    for (u = 0; u < N; u++) {
                        wiener_o[i][j/n][k/n][u] = 1 / (double)N;
                    }
                } else {
                    for (u = 0; u < N; u++) {
                        wiener_o[i][j/n][k/n][u] = 0;
                        for (v = 0; v < N; v++) {
                            wiener_o[i][j/n][k/n][u] += p[v] * invR[u][v];
                            if (minInvR > invR[u][v]) {minInvR = invR[u][v];}
                            if (maxInvR < invR[u][v]) {maxInvR = invR[u][v];}
                        }
                        if (minW > wiener_o[i][j/n][k/n][u]) {minW = wiener_o[i][j/n][k/n][u];}
                        if (maxW < wiener_o[i][j/n][k/n][u]) {maxW = wiener_o[i][j/n][k/n][u];}
                    }
                } //end
            }
        }
    }
    cout << endl;
    cout << "minR:    " << setw(10) << minR    << ",   maxR:    " << setw(10) << maxR    << endl;
    cout << "minInvR: " << setw(10) << minInvR << ",   maxInvR: " << setw(10) << maxInvR << endl;
    cout << "minP:    " << setw(10) << minP    << ",   maxP:    " << setw(10) << maxP    << endl;
    cout << "minW:    " << setw(10) << minW    << ",   maxW:    " << setw(10) << maxW    << endl;
    cout << "End getWienerCoef_Time" << endl;
    return wiener_o;
}

bool inv4 (const vector<vector<double>> &R, vector<vector<double>> &invR) {
    double A2323 = R[2][2] * R[3][3] - R[2][3] * R[3][2] ;
    double A1323 = R[2][1] * R[3][3] - R[2][3] * R[3][1] ;
    double A1223 = R[2][1] * R[3][2] - R[2][2] * R[3][1] ;
    double A0323 = R[2][0] * R[3][3] - R[2][3] * R[3][0] ;
    double A0223 = R[2][0] * R[3][2] - R[2][2] * R[3][0] ;
    double A0123 = R[2][0] * R[3][1] - R[2][1] * R[3][0] ;
    double A2313 = R[1][2] * R[3][3] - R[1][3] * R[3][2] ;
    double A1313 = R[1][1] * R[3][3] - R[1][3] * R[3][1] ;
    double A1213 = R[1][1] * R[3][2] - R[1][2] * R[3][1] ;
    double A2312 = R[1][2] * R[2][3] - R[1][3] * R[2][2] ;
    double A1312 = R[1][1] * R[2][3] - R[1][3] * R[2][1] ;
    double A1212 = R[1][1] * R[2][2] - R[1][2] * R[2][1] ;
    double A0313 = R[1][0] * R[3][3] - R[1][3] * R[3][0] ;
    double A0213 = R[1][0] * R[3][2] - R[1][2] * R[3][0] ;
    double A0312 = R[1][0] * R[2][3] - R[1][3] * R[2][0] ;
    double A0212 = R[1][0] * R[2][2] - R[1][2] * R[2][0] ;
    double A0113 = R[1][0] * R[3][1] - R[1][1] * R[3][0] ;
    double A0112 = R[1][0] * R[2][1] - R[1][1] * R[2][0] ;

    double det = R[0][0] * ( R[1][1] * A2323 - R[1][2] * A1323 + R[1][3] * A1223 ) 
        - R[0][1] * ( R[1][0] * A2323 - R[1][2] * A0323 + R[1][3] * A0223 ) 
        + R[0][2] * ( R[1][0] * A1323 - R[1][1] * A0323 + R[1][3] * A0123 ) 
        - R[0][3] * ( R[1][0] * A1223 - R[1][1] * A0223 + R[1][2] * A0123 ) ;
    if (abs(det) < 1) {return false;}

    invR[0][0] =   ( R[1][1] * A2323 - R[1][2] * A1323 + R[1][3] * A1223 ) / det,
    invR[0][1] = - ( R[0][1] * A2323 - R[0][2] * A1323 + R[0][3] * A1223 ) / det,
    invR[0][2] =   ( R[0][1] * A2313 - R[0][2] * A1313 + R[0][3] * A1213 ) / det,
    invR[0][3] = - ( R[0][1] * A2312 - R[0][2] * A1312 + R[0][3] * A1212 ) / det,
    invR[1][0] = - ( R[1][0] * A2323 - R[1][2] * A0323 + R[1][3] * A0223 ) / det,
    invR[1][1] =   ( R[0][0] * A2323 - R[0][2] * A0323 + R[0][3] * A0223 ) / det,
    invR[1][2] = - ( R[0][0] * A2313 - R[0][2] * A0313 + R[0][3] * A0213 ) / det,
    invR[1][3] =   ( R[0][0] * A2312 - R[0][2] * A0312 + R[0][3] * A0212 ) / det,
    invR[2][0] =   ( R[1][0] * A1323 - R[1][1] * A0323 + R[1][3] * A0123 ) / det,
    invR[2][1] = - ( R[0][0] * A1323 - R[0][1] * A0323 + R[0][3] * A0123 ) / det,
    invR[2][2] =   ( R[0][0] * A1313 - R[0][1] * A0313 + R[0][3] * A0113 ) / det,
    invR[2][3] = - ( R[0][0] * A1312 - R[0][1] * A0312 + R[0][3] * A0112 ) / det,
    invR[3][0] = - ( R[1][0] * A1223 - R[1][1] * A0223 + R[1][2] * A0123 ) / det,
    invR[3][1] =   ( R[0][0] * A1223 - R[0][1] * A0223 + R[0][2] * A0123 ) / det,
    invR[3][2] = - ( R[0][0] * A1213 - R[0][1] * A0213 + R[0][2] * A0113 ) / det,
    invR[3][3] =   ( R[0][0] * A1212 - R[0][1] * A0212 + R[0][2] * A0112 ) / det;
    return true;
}

bool inv2 (const vector<vector<double>> &R, vector<vector<double>> &invR) {
    double det = (R[0][0] * R[1][1] - R[0][1] * R[1][0]);
    if (abs(det) < 1) {return false;}
    det = 1 / det;
    invR[0][0] = det *   R[1][1],
    invR[0][1] = det * - R[0][1],
    invR[1][0] = det * - R[1][0],
    invR[1][1] = det *   R[0][0];
    return true;
}