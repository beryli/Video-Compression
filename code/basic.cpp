#include "basic.h"

vector<vector<vector<int>>> getResidual
(const int N, const int n, const vector<vector<vector<int>>> &luma
, const vector<vector<vector<int>>> &reconstLuma) {
    cout << "Start getResidual" << endl;
    vector<vector<vector<int>>> residual (frame, vector<vector<int>> (height, vector<int>(width)));
    int i, j, k, t;
    movec mv;
    for (i = 0; i < frame; i++){
        for (j = 0; j < height; j++){
            for (k = 0; k < width; k++){
                residual[i][j][k] = luma[i][j][k] - reconstLuma[i][j][k];
            }
        }
    }
    cout << "End getResidual" << endl;
    return residual;
}

vector<vector<vector<vector<movec>>>> motionEst 
(const int N, const int n, const vector<vector<vector<int>>> &luma, const bool isAllN) {
    cout << "Start motionEst" << endl;
    vector<vector<vector<vector<movec>>>> movecs (frame, vector<vector<vector<movec>>> (height/n, vector<vector<movec>>(width/n)));
    movec mv;
    int i, j, k, t, x, y, u, v;
    int currSAD, minSAD, tt, xx, yy;

    cout << frame << ": ";
    for (i = N; i < frame; i++){ cout << i << " ";
        for (j = 0; j < height; j=j+n){
            for (k = 0; k < width; k=k+n){ // for each block in search range 32 * 32

                // motion search
                vector<movec> mvBlock;
                minSAD = INT_MAX;
                for (t = i-1; t >= i-N; t--){
                    if (isAllN) {minSAD = INT_MAX;}
                    for (x = j-16>=0 ? j-16 : 0; x+n <= height && x+n <= j+16; x++) {
                        for (y = k-16>=0 ? k-16 : 0; y+n <= width && y+n <= k+16; y++) {
                            // sum of absolute difference
                            currSAD = 0;
                            for (u = 0; u < n; u++){
                                for (v = 0; v < n; v++){
                                    currSAD += abs(luma[i][j+u][k+v] - luma[t][x+u][y+v]);
                                }
                            }
                            if (currSAD < minSAD) {
                                minSAD = currSAD;
                                tt = t, xx = x, yy = y;
                            }
                        }
                    } // movecs[i][j][k][t]: ith frame, j k block, previous t+1 frame
                    if (isAllN) {
                        mv.t = i-tt, mv.x = xx-j, mv.y = yy-k; // 1~N, -16~+15, -16~+15
                        movecs[i][j/n][k/n].push_back(mv);
                        
                    }
                }
                if (!isAllN) {
                    mv.t = i-tt, mv.x = xx-j, mv.y = yy-k; // 1~N, -16~+15, -16~+15
                    movecs[i][j/n][k/n].push_back(mv);
                    if (mv.t > N) {cout << "Error: mv.t" << endl << endl; return movecs;}
                }

            }
        }
    }
    cout << "End motionEst" << endl;
    return movecs;
}

void outputGrayVideo (const string filename, const vector<vector<vector<int>>> &arr) {
    cout << "Start output video" << endl;
    int t = arr.size(), h = arr[0].size(), w = arr[0][0].size();
    char* buffer = new char[ (int) (w * h * 1.5 * t) ];
    int idx = 0, i, j, k;
    for (i = 0; i < t; i++) {
        for (j = 0; j < h; j++){
            for (k = 0; k < w; k++){
                buffer[idx++] = abs(arr[i][j][k]);
            }
        }
        for (j = 0; j < h * w / 2; j++){
            buffer[idx++] = 128;
        }
    }

    ofstream fout("result/"+filename+".yuv", ios::out|ios::binary);
    fout.write(buffer,  (int) (w * h * 1.5 * t) );
    fout.close();
    cout << "End output video" << endl;
}

vector<vector<vector<int>>> readLumas (const string filename) {
    cout << "Start readLumas" << endl;
    // luma contains all y values; 0: black, 127:gray, 255(=-1): white
    ifstream fin;
    vector<vector<vector<int>>> luma (frame, vector<vector<int>> (height, vector<int> (width, 0)));
    fin.open("data/"+filename+"_352x288_10.yuv", ios_base::in|ios_base::binary);
    if (fin.fail()) { cout << "Error: open file failed" << endl << endl << endl; return luma; }

    fin.seekg(0, ios::beg);
    int idx = 0, i = 0, j = 0, k = 0;
    for (i = 0; i < frame; i++) {
        fin.seekg (i * pixelPerFrame, fin.beg);
        char * buffer = new char [lumaNum];
        fin.read (buffer, lumaNum);
        idx = 0;
        for (j = 0; j < height; j++){
            for (k = 0; k < width; k++){
                if ((int)buffer[idx] >= 0){
                    luma[i][j][k] = (int)buffer[idx];}
                else{ // negative value + 256 (2's complement)
                    luma[i][j][k] = (int)buffer[idx]+256;}
                idx++;
            }
        }
    }
    fin.close();
    cout << "End readLumas" << endl;
    return luma;
}