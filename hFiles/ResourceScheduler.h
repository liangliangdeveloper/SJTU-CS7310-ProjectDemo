#pragma once
#include<vector>
#include<algorithm>
#include<iostream>
#include<iomanip>
#include<numeric>
#include<ctime>
#include<set>
#include <sstream>
using namespace std;

class ResourceScheduler {
public:
	int taskType; // 1 or 2
	int caseID;   // 1
	int numJob; // No. 0 ~ numJob-1
	int numHost;// No. 0 ~ numHost-1
	double St;  // Speed of Transimision 
	double alpha;   // g(e)=1-alpha(e-1) alpha>0, e is the number of cores allocated to a single job
	vector<int> hostCore;              // The number of cores for each host
	vector<int> jobBlock;              // The number of blocks for each job
	vector<double> Sc;                    // Speed of calculation for each job
	vector<vector<double>> dataSize;      // Job-> block number-> block size
	vector<vector<int>> location;         // Job-> block number-> block location (host number)
	vector<vector<int>> coreNumTask;

	vector<double> jobFinishTime;                 // The finish time of each job 
	vector<int> jobCore;                       // The number of cores allocated to each job.
	vector<vector<double>> hostFinishTime;
	vector<vector<tuple<int, int>>> runLoc; // Block perspective: job number->block number->(hostID, coreID,rank), rank=1 means that block is the first task running on that core of that host
	vector<vector<vector<tuple<int, int, double, double>>>> hostCoreTask; // Core perspective: host->core->task-> <job,block,startTime,endTime>
	vector<vector<double>> hostCoreFinishTime; // host->core->finishTime

	ResourceScheduler(int,int);
    void oneHostScheduler();
	void schedule();
	void outputSolutionFromBlock();
	float outputSolutionFromCore();
	void visualization(float max); // An optional fuction.
	double g(int);
	void oneJobScheduler(int, int, vector<int>, double); // Job, Cores, CoreAddress, StartTime
	static bool cmp(double, double);
};
