#ifndef _WIENER_H_
#define _WIENER_H_

vector<int> getTotalBits (map<int, int> &freq
, const vector<vector<vector<vector<movec>>>> &movecs, const vector<vector<vector<int>>> &residual
, int &symbolNum, int &totalBits);
map<int, int> countFreq (const vector<vector<vector<vector<movec>>>> &movecs
, const vector<vector<vector<int>>> &residual, const int N, const int n
, int &symbolNum);
vector<vector<vector<int>>> getReconstLuma (const int N, const int n
, const vector<vector<vector<int>>> &luma, const vector<vector<vector<vector<movec>>>> &movecs);



void output_SNR_Bits_CSV (const string &filename, const vector<double> &SNR, const vector<int> &bits
, int totalBits, int symbolNum);
vector<int> getTotalBits_Space (map<int, int> &freq
, const vector<vector<vector<int>>> &residual
, const vector<vector<vector<vector<double>>>> &wiener_o, double &minW, double &maxW, int &symbolNum
, int &totalBits);
vector<int> getTotalBits_Time (map<int, int> &freq, const int n
, const vector<vector<vector<vector<movec>>>> &movecs, const vector<vector<vector<int>>> &residual
, const vector<vector<vector<vector<double>>>> &wiener_o, double &minW, double &maxW, int &symbolNum
, int &totalBits);
map<int, int> countFreq_Space (const vector<vector<vector<int>>> &residual
, const vector<vector<vector<vector<double>>>> &wiener_o, double &minW, double &maxW, int &symbolNum);
map<int, int> countFreq_Time (const vector<vector<vector<vector<movec>>>> &movecs
, const vector<vector<vector<int>>> &residual, const int N, const int n
, const vector<vector<vector<vector<double>>>> &wiener_o, double &minW, double &maxW, int &symbolNum);
vector<double> getSNR (const string filename, const vector<vector<vector<int>>> &luma
, const vector<vector<vector<int>>> &residual);



vector<vector<vector<int>>> getReconstLuma_Space (const int N, const int n
, const vector<vector<vector<int>>> &luma, const vector<vector<vector<vector<double>>>> &wiener_o);
vector<vector<vector<vector<double>>>> getWienerCoef_Space (const int N, const int n
, const vector<vector<vector<int>>> &luma, double &minW, double &maxW);

vector<vector<vector<int>>> getReconstLuma_Time (const int N, const int n
, const vector<vector<vector<int>>> &luma, const vector<vector<vector<vector<movec>>>> &movecs
, const vector<vector<vector<vector<double>>>> &wiener_o);
vector<vector<vector<vector<double>>>> getWienerCoef_Time (const int N, const int n
, const vector<vector<vector<int>>> &luma, const vector<vector<vector<vector<movec>>>> &movecs
, double &minW, double &maxW);

bool inv4 (const vector<vector<double>> &R, vector<vector<double>> &invR);
bool inv2 (const vector<vector<double>> &R, vector<vector<double>> &invR);

#endif