#include<iostream>
#include "../hFiles/ResourceScheduler.h"
#include "../hFiles/Util.h"
#define MAXFLOAT 10000000.0

float global_opt = MAXFLOAT;
int start_trunc_job = 2;
int start_trunc_job2 = 6;

using namespace  std;

float search(int i, int cores, float st, ResourceScheduler rs){
    // prune
    if (st > global_opt)
        return MAXFLOAT;

    if (i == rs.numJob){
        cout << i <<"  " << st << endl;
        if (global_opt > st) {global_opt = st;}
        return 0.0;
    }
//    if (st > rs.oneHostLocalOpt[i][cores]) return MAXFLOAT;
//    if (st < rs.oneHostLocalOpt[i][cores]) rs.oneHostLocalOpt[i][cores] = st;
//    cout << "***** " << i <<"   "<< cores << "   " << st << " *****" << endl;

    if (cores == 0){
        // calculate next start time
        float next_st = MAXFLOAT;
        int next_cores = -1;
        for (int t = 0; t < i; t++){
            if (rs.jobFinishTime[t] > st){
                if (rs.jobFinishTime[t] < next_st){
                    next_st = rs.jobFinishTime[t];
                    next_cores = rs.jobCore[t];
                }
            }
        }
//        cout << "next_cores: " << next_cores << endl;
        return next_st - st + search(i, next_cores, next_st, rs);
    }

    // Choice 1: start job i at time st
    float search_sol = MAXFLOAT;
    int best_cores = -1;
    float best_time_cost = 0.0;
    bool flag = false;
    int min_cores = 1;
//    if (i < start_trunc_job) min_cores = 1; else min_cores = cores;
    if (i < start_trunc_job) min_cores = 1;
    else if (cores > 1)
            min_cores = cores - 1;
        else min_cores = cores;
    if (i >= start_trunc_job2) min_cores = cores;
    for (int j = cores; j >= min_cores; j-- ) {
//        if (i==19) cout << "Caution！Current cores is " << cores << endl;
//        cout << j << endl;

        float time_cost = rs.oneHostJobCostTable[i][j];
        rs.jobFinishTime[i] = st + time_cost;
        rs.jobCore[i] = j;
        float sol1 = search(i + 1, cores - j, st, rs);
        if (sol1 < time_cost) {sol1 = time_cost; flag = true;}
        // compare and choose the best
        if (sol1 < search_sol) {
            search_sol = sol1;
            best_cores = j;
            best_time_cost = time_cost;
        }
        if (flag) break;
    }

    rs.jobCore[i] = 0;
    rs.jobFinishTime[i] = MAXFLOAT;

    // Choice 2: start job i after next_st
    float next_st = MAXFLOAT;
    int next_cores = -1;
    for (int t = 0; t < i; t++){
        if (rs.jobFinishTime[t] > st){
            if (rs.jobFinishTime[t] < next_st){
                next_st = rs.jobFinishTime[t];
                next_cores = rs.jobCore[t];
            }
        }
    }
//    cout << "next_cores: " << next_cores << endl;
//    cout << "total cores: " << cores + next_cores << endl;
    if (next_cores != -1){
        float sol2 = next_st - st + search(i, cores + next_cores, next_st, rs);
        // compare and choose the best
        if (sol2 < search_sol) {
            search_sol = sol2;
            return search_sol;
        }
    }

    rs.jobCore[i] = best_cores;
    rs.jobFinishTime[i] = st + best_time_cost;

    return search_sol;
}

void oneHostSchedulerGreedy1(ResourceScheduler rs){
    rs.getOneHostJobCostTable();
    cout << rs.hostCore[0] << endl;
    for (int i = 0; i < rs.numJob; i++) {
        for (int j = 1; j <= rs.hostCore[0]; j++){
            rs.oneHostLocalOpt[i][j] = MAXFLOAT;
        }
    }

    cout << "*******The final answer is ********" << endl;
    cout << search(0, rs.hostCore[0], 0.0, rs) << endl;
    return;
}

int main() {
	int taskType=1;
	int caseID=1;

//    streambuf* coutBuf = cout.rdbuf();
//    ofstream of("out.txt");
//    // 获取文件out.txt流缓冲区指针
//    streambuf* fileBuf = of.rdbuf();
//    // 设置cout流缓冲区指针为out.txt的流缓冲区指针
//    cout.rdbuf(fileBuf);

    // main function
	ResourceScheduler rs(taskType, caseID);
	//generator(rs,taskType);
	//rs.schedule();
	//cout << "OK\n";
//    rs.oneHostScheduler();
    oneHostSchedulerGreedy1(rs);
	//rs.outputSolutionFromBlock();
//	float max = rs.outputSolutionFromCore();
//	rs.visualization(max);

//    of.flush();
//    of.close();

//    // 恢复cout原来的流缓冲区指针
//    cout.rdbuf(coutBuf);
	return 0;
}

