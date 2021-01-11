#include "basic.h"
#include "wiener.h"
#include "autocorrelation.h"

int N, n;
string filename, filenameNn;
bool isNmovecs;

int initialize ();
void qnum_3_space ();
void qnum_2_3_time ();
void qnum_2_1_mocom ();
void qnum_1_3_mocomAuto ();
void qnum_1_2_auto ();

vector<vector<vector<int>>> luma (frame, vector<vector<int>> (height, vector<int> (width, 0)));
vector<vector<vector<int>>> reconstLuma (frame, vector<vector<int>> (height, vector<int> (width, 0)));
vector<vector<vector<int>>> residual (frame, vector<vector<int>> (height, vector<int>(width)));
vector<double> SNR (frame);
double minW, maxW;
map<int, int> freq;
int symbolNum;
vector<int> bitsNum (frame);
int totalBits;

int main() {
    // if (initialize() == -1) {return 0;}

    filename = "MOBILE";
    qnum_1_2_auto ();
    filename = "AKIYO";
    qnum_1_2_auto ();

    filename = "MOBILE";
    qnum_1_3_mocomAuto ();
    filename = "AKIYO";
    qnum_1_3_mocomAuto ();

    qnum_2_1_mocom();
    qnum_2_3_time();
    qnum_3_space();
}

void qnum_1_2_auto () {
    // int data;
    // cout << "Input 1 (AKIYO) or 2 (MOBILE) ";
    // cin >> data;
    // if (data == 1) {filename = "AKIYO";}
    // else if (data == 2) {filename = "MOBILE";}
    // else {cout << "Error: filename choose error"; return;}

    vector<vector<vector<int>>> mu0luma (frame, vector<vector<int>> (height, vector<int> (width)));
    vector<vector<vector<double>>> rxx (21, vector<vector<double>> (height/2, vector<double> (width/2)));
    vector<vector<vector<int>>> normalized_rxx (21, vector<vector<int>> (height/2, vector<int> (width/2)));
    luma = readLumas(filename);
    cout << filename << endl;

    mu0luma = minusMean (luma);
    rxx = getAutoRxx (mu0luma);
    normalized_rxx = normalizeRxx (rxx);
    outputGrayVideo("1_2_auto/"+filename+"_1_2_auto", normalized_rxx);
    cout << "End " << filename << endl << endl << endl;
}

void qnum_1_3_mocomAuto () {
    // int data;
    // cout << "Input 1 (AKIYO) or 2 (MOBILE) ";
    // cin >> data;
    // if (data == 1) {filename = "AKIYO";}
    // else if (data == 2) {filename = "MOBILE";}
    // else {cout << "Error: filename choose error"; return;}
    
    vector<vector<vector<double>>> rxx (21, vector<vector<double>> (height/2, vector<double> (width/2)));
    vector<vector<vector<int>>> normalized_rxx (21, vector<vector<int>> (height/2, vector<int> (width/2)));
    luma = readLumas(filename);
    cout << filename << endl;

    rxx = getAutoRxxFromMocom (luma);

    normalized_rxx = normalizeRxx (rxx);
    outputGrayVideo("1_3_mocomAuto/"+filename+"_1_3_mocomAuto", normalized_rxx);
    cout << "End " << filename << endl << endl << endl;
}

void qnum_2_1_mocom () {
    // 2_1. mocom
    isNmovecs = false;
    for (int testCase = 0; testCase < 2; testCase++){
        if (testCase == 0) {filename = "AKIYO";}
        else if (testCase == 1) {filename = "MOBILE";}
        luma = readLumas(filename);
        cout << filename;

        for (int sampleNum = 0; sampleNum < 3; sampleNum++) {
            if (sampleNum == 0) {N = 1;}
            else if (sampleNum == 1) {N = 2;}
            else if (sampleNum == 2) {N = 4;}
            for (int blockSize = 0; blockSize < 3; blockSize++){
                if (blockSize == 0) {n = 16;}
                else if (blockSize == 1) {n = 8;}
                else if (blockSize == 2) {n = 4;}
                vector<vector<vector<vector<movec>>>> movecs (frame, vector<vector<vector<movec>>> (height/n, vector<vector<movec>>(width/n)));
                vector<vector<vector<vector<double>>>> wiener_o (frame, vector<vector<vector<double>>> (height/n, vector<vector<double>>(width/n, vector<double> (N))));
                freq.clear();
                filenameNn = filename+"_N"+to_string(N)+"n"+to_string(n);

                movecs = motionEst (N, n, luma, isNmovecs);
                reconstLuma = getReconstLuma (N, n, luma, movecs);
                residual = getResidual (N, n, luma, reconstLuma);
                SNR = getSNR("2_1_mocom/"+filenameNn+"_mean_var_SNR", luma, residual);
                freq = countFreq(movecs, residual, N, n, symbolNum);
                bitsNum = getTotalBits(freq, movecs, residual, symbolNum, totalBits);
                output_SNR_Bits_CSV ("2_1_mocom/"+filenameNn+"_SNR_Bits", SNR, bitsNum, totalBits, symbolNum);
                outputGrayVideo("2_1_mocom/"+filenameNn+"_residual", residual);
                outputGrayVideo("2_1_mocom/"+filenameNn+"_reconstLuma", reconstLuma);
                cout << "End " << filename << ", N/n:" << N << "/" << n << ", isNmovecs: " << (isNmovecs?"true":"false") << endl;
                cout << endl << endl;
            }
        }
    }

}

void qnum_2_3_time () {
    // 2_3. time
    isNmovecs = true;
    for (int testCase = 0; testCase < 2; testCase++){
        if (testCase == 0) {filename = "AKIYO";}
        else if (testCase == 1) {filename = "MOBILE";}
        luma = readLumas(filename);
        cout << filename;

        for (int sampleNum = 0; sampleNum < 3; sampleNum++) {
            if (sampleNum == 0) {N = 1;}
            else if (sampleNum == 1) {N = 2;}
            else if (sampleNum == 2) {N = 4;}
            for (int blockSize = 0; blockSize < 3; blockSize++){
                if (blockSize == 0) {n = 16;}
                else if (blockSize == 1) {n = 8;}
                else if (blockSize == 2) {n = 4;}
                vector<vector<vector<vector<movec>>>> movecs (frame, vector<vector<vector<movec>>> (height/n, vector<vector<movec>>(width/n)));
                vector<vector<vector<vector<double>>>> wiener_o (frame, vector<vector<vector<double>>> (height/n, vector<vector<double>>(width/n, vector<double> (N))));
                freq.clear();
                filenameNn = filename+"_N"+to_string(N)+"n"+to_string(n);

                movecs = motionEst (N, n, luma, isNmovecs);
                wiener_o = getWienerCoef_Time (N, n, luma, movecs, minW, maxW);
                reconstLuma = getReconstLuma_Time (N, n, luma, movecs, wiener_o);
                residual = getResidual (N, n, luma, reconstLuma);
                SNR = getSNR("2_3_time/"+filenameNn+"_mean_var_SNR", luma, residual);
                freq = countFreq_Time(movecs, residual, N, n, wiener_o, minW, maxW, symbolNum);
                bitsNum = getTotalBits_Time(freq, n, movecs, residual, wiener_o, minW, maxW, symbolNum, totalBits);
                output_SNR_Bits_CSV ("2_3_time/"+filenameNn+"_SNR_Bits", SNR, bitsNum, totalBits, symbolNum);
                outputGrayVideo("2_3_time/"+filenameNn+"_residual", residual);
                outputGrayVideo("2_3_time/"+filenameNn+"_reconstLuma", reconstLuma);
                cout << "End " << filename << ", N/n:" << N << "/" << n << ", isNmovecs: " << (isNmovecs?"true":"false") << endl;
                cout << endl << endl;
            }
        }
    }
}

void qnum_3_space () {
    // 3. space
    isNmovecs = true;
    for (int testCase = 0; testCase < 2; testCase++){
        if (testCase == 0) {filename = "AKIYO";}
        else if (testCase == 1) {filename = "MOBILE";}
        luma = readLumas(filename);
        cout << filename;

        for (int sampleNum = 0; sampleNum < 3; sampleNum++) {
            if (sampleNum == 0) {N = 1;}
            else if (sampleNum == 1) {N = 2;}
            else if (sampleNum == 2) {N = 4;}
            for (int blockSize = 0; blockSize < 3; blockSize++){
                if (blockSize == 0) {n = 4;}
                else if (blockSize == 1) {n = 8;}
                else if (blockSize == 2) {n = 16;}
                vector<vector<vector<vector<movec>>>> movecs (frame, vector<vector<vector<movec>>> (height/n, vector<vector<movec>>(width/n)));
                vector<vector<vector<vector<double>>>> wiener_o (frame, vector<vector<vector<double>>> (height/n, vector<vector<double>>(width/n, vector<double> (N))));
                freq.clear();
                filenameNn = filename+"_N"+to_string(N)+"n"+to_string(n);

                wiener_o = getWienerCoef_Space (N, n, luma, minW, maxW);
                reconstLuma = getReconstLuma_Space (N, n, luma, wiener_o);
                residual = getResidual (N, n, luma, reconstLuma);
                SNR = getSNR("3_space/"+filenameNn+"_mean_var_SNR", luma, residual);
                freq = countFreq_Space(residual, wiener_o, minW, maxW, symbolNum);
                bitsNum = getTotalBits_Space(freq, residual, wiener_o, minW, maxW, symbolNum, totalBits);
                output_SNR_Bits_CSV ("3_space/"+filenameNn+"_SNR_Bits", SNR, bitsNum, totalBits, symbolNum);
                outputGrayVideo("3_space/"+filenameNn+"_residual", residual);
                outputGrayVideo("3_space/"+filenameNn+"_reconstLuma", reconstLuma);
                cout << "End " << filename << ", N/n:" << N << "/" << n << ", isNmovecs: " << (isNmovecs?"true":"false") << endl;
                cout << endl << endl;
            }
        }
    }
}

int initialize () {
    // choose file
    int data;
    cout << "Input 1 (AKIYO) or 2 (MOBILE) ";
    cin >> data;
    if (data == 1) {filename = "AKIYO";}
    else if (data == 2) {filename = "MOBILE";}
    else {cout << "Error: filename choose error"; return -1;}

    // choose N, n
    cout << "Input N(1, 2, 4) and n(4, 8, 16): ";
    cin >> N >> n; 
    if (cin.fail()) {cout << "Error: input error"; return -1;}
    if (N != 1 && N != 2 && N != 4) {cout << "Error: N (frame sample) error"; return -1;}
    if (n != 4 && n != 8 && n != 16) {cout << "Error: n (block size) error"; return -1;}
    
    // choose isNmovecs
    cout << "Input isNmovecs T(1) or F(0): ";
    cin >> isNmovecs;
    if (cin.fail()) {cout << "Error: input error"; return -1;}

    return 0;
}