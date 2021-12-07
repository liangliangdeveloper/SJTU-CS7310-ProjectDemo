#include "../hFiles/ResourceScheduler.h"
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

ResourceScheduler::ResourceScheduler(int tasktype,int caseID) {
	taskType = tasktype;
	string filePath = "../input/task" + to_string(taskType) + "_case"+to_string(caseID)+".txt";
	freopen(filePath.c_str(), "r", stdin);

	cin >> numJob >> numHost >> alpha;
	if (taskType == 2)
		cin >> St;

	hostCore.resize(numHost);
	for (int i = 0; i < numHost; i++)
		cin >> hostCore[i];


	jobBlock.resize(numJob);
	for (int i = 0; i < numJob; i++)
		cin >> jobBlock[i];

	Sc.resize(numJob);
	for (int i = 0; i < numJob; i++)
		cin >> Sc[i];
	dataSize.resize(numJob);
	for (int i = 0; i < numJob; i++) {
		dataSize[i].resize(jobBlock[i]);
		for (int j = 0; j < jobBlock[i]; j++)
			cin >> dataSize[i][j];
	}
	//cout << "OK";
	location.resize(numJob);
	for (int i = 0; i < numJob; i++) {
		location[i].resize(jobBlock[i]);
		for (int j = 0; j < jobBlock[i]; j++){
            cin >> location[i][j];
		}
	}
	jobFinishTime.resize(numJob);
	jobCore.resize(numJob);
	runLoc.resize(numJob);
	for (int i = 0; i < numJob; i++)
		runLoc[i].resize(jobBlock[i]);

	hostCoreTask.resize(numHost);
	for (int i = 0; i < numHost; i++)
		hostCoreTask[i].resize(hostCore[i]);

	hostCoreFinishTime.resize(numHost);
	for (int i = 0; i < numHost; i++)
		hostCoreFinishTime[i].resize(hostCore[i], 0);

    coreNumTask.resize(numHost);
    for (int i = 0; i < numHost; i++){
        coreNumTask[i].resize(hostCore[i]);
    }

}

void ResourceScheduler::oneHostScheduler() {
    if(numHost != 1) {
        cout << "This function only can use in one host!";
        return;
    }
    //cout << "OK!";
    double startTime = 0;
    for (int i = 0; i < numJob; i++) {
        //cout << i
        vector<int> host;
        for(int j = 0; j < hostCore[0]; j++){
            host.push_back(j);
        }
        oneJobScheduler(i, hostCore[0], host, startTime);
        startTime = jobFinishTime[i];
        cout << "starttime: " << startTime << endl;
    }
}

void ResourceScheduler::oneHostGreedyScheduler() {
    if(numHost != 1) {
        cout << "This function only can use in one host!";
        return;
    }
    double startTime = 0;
    int restCore = hostCore[0];
    cout << "hostCore:" << hostCore[0] << endl;
    double stageMaxTime = 0;
    for(int i = 0; i < numJob; i++){
        double minTime = 9999999;
        int minCore = 0;
        if(restCore == 0){
            startTime = stageMaxTime;
            restCore = hostCore[0];
            stageMaxTime = 0;
        }
        for(int j = 1; j <= hostCore[0]; j++){
            double time = oneJobTimeCount(i, j);
            if(time < minTime){
                minTime = time;
                minCore = j;
            }
            if(minCore > restCore) {
                startTime = stageMaxTime;
                restCore = hostCore[0];
                stageMaxTime = 0;
            }
        }
        cout << minCore << endl;
        cout << "rest:" << restCore << endl;
        vector<int> host;
        for(int j = hostCore[0] - restCore; j < hostCore[0] - restCore + minCore; j++){
            //cout << j << endl;
            host.push_back(j);
        }
        restCore -= minCore;
        oneJobScheduler(i, minCore, host, startTime);
        if(stageMaxTime < jobFinishTime[i]) {
            stageMaxTime = jobFinishTime[i];
        }
    }
}

void ResourceScheduler::schedule() {

	vector<vector<int>> hostCoreBlock(numHost);
	for (int i = 0; i < numHost; i++)
		hostCoreBlock[i].resize(hostCore[i], 0);

	for (int i = 0; i < numJob; i++) {
		set<pair<int, int>> allocatedJobCore;
		for (int j = 0; j < jobBlock[i]; j++) {
			int hid = rand() % numHost;
			int cid = rand() % hostCore[hid];
			allocatedJobCore.insert({ hid,cid });
			//runLoc[i][j] = make_tuple(hid, cid, hostCoreBlock[hid][cid]++);
		}
		/*for (int j = 0; j < jobBlock[i]; j++)
			finishTime[i]+=*/
		jobFinishTime[i] = rand() % 200;
		jobCore[i] = allocatedJobCore.size();
	}


	for (int i = 0; i < numHost; i++) {
		for (int j = 0; j < hostCore[i]; j++) {
			int numTask = rand() % 10 + 1;
			hostCoreTask[i][j].resize(numTask);
			for (int k = 0; k < numTask; k++) {
				int jid = rand() % numJob;
				int bid = rand() % jobBlock[jid];
				int endTime = hostCoreFinishTime[i][j] + rand() % 100 + 1;
				hostCoreTask[i][j][k] = make_tuple(jid, bid, hostCoreFinishTime[i][j], endTime);
				hostCoreFinishTime[i][j] = endTime;
			}
		}
	}
}

void ResourceScheduler::outputSolutionFromBlock() {
	cout << "\nTask2 Solution (Block Perspective) of Teaching Assistant:\n\n";
	for (int i = 0; i < numJob; i++) {
		double speed = g(jobCore[i]);
		cout << "Job" << i << " obtains " << jobCore[i] << " cores (speed=" << speed << ") and finishes at time " << jobFinishTime[i] << ": \n";
		for (int j = 0; j < jobBlock[i]; j++) {
			cout << "\tBlock" << j << ": H" << get<0>(runLoc[i][j]) << ", C" << get<1>(runLoc[i][j]) << " (time=" << fixed << setprecision(2) << dataSize[i][j] / speed << ")" << " \n";
		}
		cout << "\n";
	}

	cout << "The maximum finish time: " << *max_element(jobFinishTime.begin(), jobFinishTime.end()) << "\n";
	cout << "The total response time: " << accumulate(jobFinishTime.begin(), jobFinishTime.end(), 0.0) << "\n\n";
}

float ResourceScheduler::outputSolutionFromCore() {
	cout << "\nTask2 Solution (Core Perspective) of Teaching Assistant:\n\n";
	double maxHostTime = 0, totalRunningTime = 0.0;
	for (int i = 0; i < numHost; i++) {
		double hostTime = *max_element(hostCoreFinishTime[i].begin(), hostCoreFinishTime[i].end());
		maxHostTime = max(hostTime, maxHostTime);
		totalRunningTime += accumulate(hostCoreFinishTime[i].begin(), hostCoreFinishTime[i].end(), 0.0);
		cout << "Host" << i << " finishes at time " << hostTime << ":\n\n";
		for (int j = 0; j < hostCore[i]; j++) {
			cout << "\tCore" << j << " has " << hostCoreTask[i][j].size() << " tasks and finishes at time " << hostCoreFinishTime[i][j] << ":\n";
			for (int k = 0; k < hostCoreTask[i][j].size(); k++) {
				cout << "\t\tJ" << setw(2) << setfill('0') << get<0>(hostCoreTask[i][j][k]) << ", B" << setw(2) << setfill('0') << get<1>(hostCoreTask[i][j][k]) << ", runTime " << fixed << setprecision(1) << setw(5) << setfill('0') << get<2>(hostCoreTask[i][j][k]) << " to " << fixed << setprecision(1) << setw(5) << setfill('0') << get<3>(hostCoreTask[i][j][k]) << "\n";
			}
			cout << "\n";
		}
		cout << "\n\n";
	}
	cout << "The maximum finish time of hosts: " << maxHostTime << "\n";
	cout << "The total efficacious running time: " << totalRunningTime << "\n";
	cout << "Utilization rate: " << totalRunningTime / accumulate(hostCore.begin(), hostCore.end(), 0.0) / maxHostTime << "\n\n";
	return maxHostTime;
}

void ResourceScheduler::visualization(float max) {
    int maxTime = max;
    int height = 2000, length = 4000;
    int bound = 100;
    Mat picture(2000, 4000, CV_8UC3, Scalar(255, 255, 255,0.5));
//    imshow("Drawing Function", picture);
//    waitKey(-1);
    int allCore = accumulate(hostCore.begin(), hostCore.end(), 0);
    int coreHeight = (height - 2 * bound) / allCore;
    int beforeCore = 0;
	for(int i = 0; i < numHost; i++){
	    for(int j = 0; j < hostCore[i]; j++){
	        for(int k = 0; k < hostCoreTask[i][j].size(); k++){
				int job = get<0>(hostCoreTask[i][j][k]);
				int block = get<1>(hostCoreTask[i][j][k]);
				float startTime = get<2>(hostCoreTask[i][j][k]);
				float endTime = get<3>(hostCoreTask[i][j][k]);
				int leftupx = (beforeCore + j) * coreHeight + bound;
				int leftupy = (startTime/maxTime) * (length - 2 * bound) + bound;
				int rightdownx = leftupx + coreHeight;
				int rightdowny = (endTime/maxTime) * (length - 2 * bound) + bound;
                rectangle(picture, Point(leftupy, leftupx), Point(rightdowny, rightdownx), Scalar(255, 0, 0), 2);
                putText(picture, "J" + to_string(job) + ",B" + to_string(block) , Point(leftupy, rightdownx), 2, 1, Scalar::all(0));
			}
	    }
	    beforeCore += hostCore[i];
	}
    imshow("Drawing Function", picture);
    waitKey(-1);
}

double ResourceScheduler::g(int e) {
	return 1 - alpha * (e - 1);
}

void ResourceScheduler::oneJobScheduler(int job, int coreNum, vector<int> coreID, double startTime) {
    int down = min(coreNum, jobBlock[job]);
    double speed = Sc[job] * g(down);
    set<pair<int, int>> allocatedJobCore;
    sort(dataSize[job].begin(), dataSize[job].end(), cmp);
    vector<double> coreFinishTime;
    coreFinishTime.resize(coreNum, 0);
    int hid = 0;
    for (int j = 0; j < jobBlock[job]; j++) {
        int unreal_cid = distance(coreFinishTime.begin(), min_element(coreFinishTime.begin(), coreFinishTime.end()));
        //int cid = 0;
        //cout  << "cid:" << unreal_cid << endl;
        int cid = coreID[unreal_cid];
        allocatedJobCore.insert({ hid,cid });
        runLoc[job][j] = make_tuple(hid, cid);
        double dataTime = dataSize[job][j] / speed;
        double start_Time = startTime + coreFinishTime[unreal_cid];
        //cout << "stlast:" << startTime;
        double endTime = start_Time + dataTime;
        hostCoreTask[hid][cid].resize(coreNumTask[hid][cid] + 1);
        hostCoreTask[hid][cid][coreNumTask[hid][cid]] = make_tuple(job, j, start_Time, endTime);
        cout << job << " " << j << " " << start_Time << " " << endTime << endl;
        hostCoreFinishTime[hid][cid] = endTime;
        coreFinishTime[unreal_cid] += dataTime;
        coreNumTask[hid][cid]++;
    }
    jobCore[job] = allocatedJobCore.size();
    jobFinishTime[job] = startTime + *max_element(coreFinishTime.begin(), coreFinishTime.end());
    cout << job << ":" << jobFinishTime[job] << endl;
}


double ResourceScheduler::oneJobTimeCount(int job, int coreNum) {
    double speed = Sc[job] * g(coreNum);
    set<pair<int, int>> allocatedJobCore;
    sort(dataSize[job].begin(), dataSize[job].end(), cmp);
    vector<double> coreFinishTime;
    coreFinishTime.resize(coreNum, 0);
    int hid = 0;
    for (int j = 0; j < jobBlock[job]; j++) {
        int unreal_cid = distance(coreFinishTime.begin(), min_element(coreFinishTime.begin(), coreFinishTime.end()));
        double dataTime = dataSize[job][j] / speed;
        coreFinishTime[unreal_cid] += dataTime;
    }
    return *max_element(coreFinishTime.begin(), coreFinishTime.end());
}

double ResourceScheduler::simpleJobSchduler() {
    vector<double> jobTime;
    jobTime.resize(numJob, 0);
    for(int i = 0; i < numJob; i++){
        for(int j = 0; j < jobBlock[i]; j++){
            jobTime[i] += dataSize[i][j] / Sc[i];
            //cout << jobTime[i] << "\n";
        }
    }
    sort(jobTime.begin(), jobTime.end(), cmp);
    vector<double> coreFinishTime;
    coreFinishTime.resize(hostCore[0], 0);
    for(int i = 0; i < numJob; i++){
        int index = distance(coreFinishTime.begin(), min_element(coreFinishTime.begin(), coreFinishTime.end()));
        double st = coreFinishTime[index];
        coreFinishTime[index] += jobTime[i];
        double et = coreFinishTime[index];
        hostCoreTask[0][index].resize(coreNumTask[0][index] + 1);
        hostCoreTask[0][index][coreNumTask[0][index]] = make_tuple(i, 1, st, et);
        coreNumTask[0][index]++;
    }
    return *max_element(coreFinishTime.begin(), coreFinishTime.end());
}

void ResourceScheduler::partJobScheduler(int job, int host, int coreNum, vector<int> blockID, vector<int> coreID, double startTime) {
    int down = min(coreNum, int(blockID.size()));
    double speed = Sc[job] * g(down);
    set<pair<int, int>> allocatedJobCore;
    vector<double> partDataSize;
    for(int i = 0; i < blockID.size(); i++){
        partDataSize.push_back(dataSize[job][blockID[i]]);
    }
    sort(partDataSize.begin(), partDataSize.end(), cmp);
    vector<double> coreFinishTime;
    coreFinishTime.resize(coreNum, 0);
    int hid = host;
    for (int j = 0; j < blockID.size(); j++) {
        int unreal_cid = distance(coreFinishTime.begin(), min_element(coreFinishTime.begin(), coreFinishTime.end()));
        //int cid = 0;
        //cout  << "cid:" << unreal_cid << endl;
        int cid = coreID[unreal_cid];
        allocatedJobCore.insert({ hid,cid });
        runLoc[job][j] = make_tuple(hid, cid);
        double dataTime = partDataSize[j] / speed;
        double start_Time = startTime + coreFinishTime[unreal_cid];
        //cout << "stlast:" << startTime;
        double endTime = start_Time + dataTime;
        hostCoreTask[hid][cid].resize(coreNumTask[hid][cid] + 1);
        hostCoreTask[hid][cid][coreNumTask[hid][cid]] = make_tuple(job, blockID[j], start_Time, endTime);
        cout << job << " " << j << " " << start_Time << " " << endTime << endl;
        hostCoreFinishTime[hid][cid] = endTime;
        coreFinishTime[unreal_cid] += dataTime;
        coreNumTask[hid][cid]++;
    }
    jobCore[job] = allocatedJobCore.size();
    jobFinishTime[job] = startTime + *max_element(coreFinishTime.begin(), coreFinishTime.end());
    //cout << job << ":" << jobFinishTime[job] << endl;
}

void ResourceScheduler::oneJobMultiHostScheduler(int job, vector<int> coreNum, vector <vector<int>> blockID, vector <vector<int>> coreID,
                                                 double startTime) {
    for(int i = 0; i < numHost; i++){
        partJobScheduler(job, i, coreNum[i], blockID[i], coreID[i], startTime);
    }
}

void ResourceScheduler::transferedJobMultiHostScheduler(int job, int coreNum, vector <vector<int>> coreID, int startTime) {
    vector<vector<double>> coreFinishTime;
    vector<vector<double>> coreTransferTime;
    double speed = Sc[job] * g(coreNum);
    coreFinishTime.resize(numHost);
    for(int i = 0; i < numHost; i++){
        coreFinishTime[i].resize(coreID[i].size(), 0);
    }
    coreTransferTime.resize(numHost);
    for(int i = 0; i < numHost; i++){
        coreTransferTime[i].resize(coreID[i].size(), 0);
    }
    sort(dataSize[job].begin(), dataSize[job].end(), cmp);
    double localMinTime;
    int localMinIndex, localMinIndexReal;
    double globalMinTime;
    int globalMinHost, globalMinIndex, globalMinIndexReal;
    for(int i = 0; i < jobBlock[job]; i++){
        double runtime = dataSize[job][i] / speed;
        globalMinTime = 999999;
        int hostLoc = location[job][i];
        localMinIndex = distance(coreFinishTime[hostLoc].begin(), min_element(coreFinishTime[hostLoc].begin(), coreFinishTime[hostLoc].end()));
        for(int j = 0; j < numHost; j++){
            for(int k = 0; k < coreID[j].size(); k++){
                if(globalMinTime > coreFinishTime[j][k]){
                    globalMinTime = coreFinishTime[j][k];
                    globalMinHost = j;
                    globalMinIndex = k;
                }
            }
        }
        localMinIndexReal = coreID[hostLoc][localMinIndex];
        globalMinIndexReal = coreID[globalMinHost][globalMinIndex];
        if(globalMinHost != hostLoc && globalMinTime < localMinTime) {
            double deltaTime = abs(localMinTime - globalMinTime);
            double transTime = dataSize[job][i] / St;
            // Transfer is a good choice
            if (deltaTime > transTime) {
                double start_Time = startTime + coreFinishTime[globalMinHost][globalMinIndex];
                double endTime = start_Time + runtime + transTime;
                hostCoreTask[globalMinHost][globalMinIndexReal].resize(coreNumTask[globalMinHost][globalMinIndexReal] + 1);
                hostCoreTask[globalMinHost][globalMinIndexReal][coreNumTask[globalMinHost][globalMinIndexReal]] = make_tuple(job, i, start_Time, endTime);
                coreNumTask[globalMinHost][globalMinIndexReal]++;
                coreFinishTime[globalMinHost][globalMinIndex] += runtime + transTime;
                coreTransferTime[globalMinHost][globalMinIndex] += transTime;
            }
            else{
                goto badcase;
            }
        } else {
        badcase:
            double start_Time = startTime + coreFinishTime[hostLoc][localMinIndex];
            //cout << "stlast:" << startTime;
            double endTime = start_Time + runtime;
            hostCoreTask[hostLoc][localMinIndexReal].resize(coreNumTask[globalMinHost][globalMinIndexReal] + 1);
            hostCoreTask[hostLoc][localMinIndexReal][coreNumTask[hostLoc][localMinIndexReal]] = make_tuple(job, i, start_Time, endTime);
            coreNumTask[globalMinHost][globalMinIndex]++;
            coreFinishTime[globalMinHost][globalMinIndex] += runtime;
        }
    }
    int maxTime = 0;
    for(int i = 0; i < numHost; i++){
        for(int j = 0; j < coreID.size(); j++){
            if(coreFinishTime[i][j] > maxTime){
                maxTime = coreFinishTime[i][j];
            }
        }
    }
    jobFinishTime[job] = startTime + maxTime;
}

bool ResourceScheduler::cmp(double i, double j) { return i > j; }