#ifndef _AUTOCORRELATION_H_
#define _AUTOCORRELATION_H_

#include <thread>

void runOneRxxFrame (int N, int i
, const vector<vector<vector<int>>> &mu0luma, const vector<vector<vector<int>>> &mu0comluma);

vector<vector<vector<double>>> getAutoRxxFromMocom (const vector<vector<vector<int>>> &luma);
vector<vector<vector<int>>> motionEstFromPrevN (int N, const vector<vector<vector<int>>> &luma);

vector<vector<vector<int>>> normalizeRxx (const vector<vector<vector<double>>> &rxx);
vector<vector<vector<double>>> getAutoRxx (const vector<vector<vector<int>>> &mu0luma);
vector<vector<vector<int>>> minusMean (const vector<vector<vector<int>>> &luma);

#endif