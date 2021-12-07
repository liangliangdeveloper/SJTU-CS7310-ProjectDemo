#include<iostream>
#include "../hFiles/ResourceScheduler.h"
#include "../hFiles/Util.h"

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
	ResourceScheduler rs(taskType,caseID);
	//generator(rs,taskType);
	//rs.schedule();
	//cout << "OK\n";
    //rs.oneHostScheduler();
	//rs.outputSolutionFromBlock();
	rs.oneHostGreedyScheduler();
	//float max = rs.simpleJobSchduler();
	float max = rs.outputSolutionFromCore();
	cout << "max:" << max;
	rs.visualization(max);

//    of.flush();
//    of.close();
//
//    // 恢复cout原来的流缓冲区指针
//    cout.rdbuf(coutBuf);
	return 0;
}

