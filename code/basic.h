#ifndef _BASIC_H_
#define _BASIC_H_

#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>

#include <climits>
#include <cmath>
#include <vector>
#include <map>
using namespace std;

struct movec { int t, x, y; }; 

const int totalPixel = 15206400;
const int pixelPerFrame = 152064, lumaNum = 101376;
const int frame = 100, height = 288, width = 352;

vector<vector<vector<int>>> getResidual
(const int N, const int n, const vector<vector<vector<int>>> &luma
, const vector<vector<vector<int>>> &reconstLuma);
vector<vector<vector<vector<movec>>>> motionEst (const int N, const int n, const vector<vector<vector<int>>> &luma, const bool isAllN);
void outputGrayVideo (const string filename, const vector<vector<vector<int>>> &arr);
vector<vector<vector<int>>> readLumas (const string filename);

#endif