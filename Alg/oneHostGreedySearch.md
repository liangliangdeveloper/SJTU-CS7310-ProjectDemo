## oneHostGreedySearch

Off Policy算法

```c++
Search(int i, int cores, int st):
  // Given the order of each job
  // i: current job to be selected
  // cores: current number of  cores left
  // st: start_time for job i
  // Returns: the least time cost after start_time, best solution for number cores and start time of job i
  // if cores==0, we can only start job j after next_st
  if cores == 0:
    next_st <- min {jobFinishTime[]};
    next_cores <- jobCore[ arg min{jobFinishTime[]| jobFinishTime > st}];
    return next_st - st + Search(i, next_cores ,next_st);
	
	search_sol <- MAXFLOAT
  // Choice 1: start job i at time st
	for j <- 1 to cores:
		time_cost <- oneHostJobCostTable[i, j];
		jobFinishTime[order[i]] <- st + time_cost;
		jobCores[order[i]] <- j;
		search_sol <- min {Search(i+1, cores - j, st), search_sol}
	
	jobCores[order[i] <- 0
  jobFinishTime[order[i]] <- MAXFLOAT
		

  // Choice 2: start job i after next_st
	next_st <- min {jobFinishTime[]};
  next_cores <- jobCore[ arg min{jobFinishTime[] | jobFinishTime > s}];
	search_sol<- {Search(i, next_cores, next_st), search_sol}
	
	update jobFinishTime[order[i]], jobCore[order[i]] with the best condition.
    
return max{jobFinishTime[order[i]] , st + search_sol}


order <- Arrange the order of each job (Sort)
sol <- Search(0, hostCore[0], 0, )
return sol
```





1. 剪枝1(保证正确的剪枝)

	for  j <- cores downto 1:

	如果在某个时刻，第i 个job的时间超过了search(i + 1, cores - j, st, rs)的时间，那么久不再往下搜索了

2. global optimal(保证正确的剪枝): 如果某刻时刻的时间超过了全局最优解，那么cut掉
3. 每个i, jobs的st如果大于历史记录，那么剪掉(不一定正确的剪枝)

4. 蒙特卡洛剪枝：到一定深度后，最后只用一种简单的策略