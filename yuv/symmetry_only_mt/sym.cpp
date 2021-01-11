#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
using namespace std;

const int frame = 21, height = 144, width = 176;
const int lumaNum = 144*176, pixelPerFrame = lumaNum*1.5;
const int totalPixel = pixelPerFrame*21;

void symmetry (string filename);

int main () {
    symmetry ("AKIYO_1_2_auto.yuv");
    symmetry ("AKIYO_1_3_mocomAuto.yuv");
    symmetry ("MOBILE_1_2_auto.yuv");
    symmetry ("MOBILE_1_3_mocomAuto.yuv");

}

void symmetry (string filename) {
    cout << "Start readLumas" << endl;
    // luma contains all y values; 0: black, 127:gray, 255(=-1): white
    ifstream fin;
    vector<vector<vector<int>>> luma (frame, vector<vector<int>> (height, vector<int> (width, 0)));
    fin.open(filename, ios_base::in|ios_base::binary);
    if (fin.fail()) { cout << "Error: open file failed" << endl << endl << endl; return; }

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



    for (i = 0; i < 11; i++) {
        for (j = 0; j < height/2; j++){
            for (k = 0; k < width/2; k++){
                luma[i][72+j][88-k] = luma[i][72+j][88+k];
                luma[i][72-j][88+k] = luma[i][72+j][88+k];
                luma[i][72-j][88-k] = luma[i][72+j][88+k];
            }
        }
    }
    for (i = 0; i < 11; i++) {
        luma[10+i] = luma[10-i];
    }



    vector<vector<vector<int>>> arr = luma;
    cout << "Start output video" << endl;
    int t = arr.size(), h = arr[0].size(), w = arr[0][0].size();
    char* buffer = new char[ (int) (w * h * 1.5 * t) ];
    idx = 0;
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

    ofstream fout("new_"+filename, ios::out|ios::binary);
    fout.write(buffer,  (int) (w * h * 1.5 * t) );
    fout.close();
    cout << "End output video" << endl;
}