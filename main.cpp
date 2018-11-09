#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
//class to hold a process object
class process{
public:
	/*Variables fullforms:
	AT=arrival-time,ST=service-time,iniWT=initial-waiting-time,intWT=intermediate-waiting-time
	Pr=priority,FT=finish-time,TAT=turn-around-time,cpuUsage=cpu-usage-time,QOrder=order-in-queue,RemainingST=remaining-service-time
	StartTime=time at which the process has started,isDone={1(if process is terminated),0(if process is not terminated)},timesSch=number of times the process has been scheduled*/
	string processID;
	int AT,ST,iniWT,intWT,Pr,FT,TAT,StartTime,isDone,cpuUsage,RemainingST,QOrder,timesSch;
	process(){
		AT=0,ST=0,iniWT=0,intWT=0,Pr=0,FT=0,TAT=0,StartTime=0,isDone=0,cpuUsage=0,RemainingST=ST,QOrder=0,timesSch=0;
	}
};
//function which calculates the max time that it would take for all the processes to get completed which is the sum of ST of all processes
int calculateMaxFinalTime(vector<process> processesVector){
	int maxTime=0;
	for(int i=0;i<processesVector.size();i++){
		maxTime+=processesVector[i].ST;
	}
	//sum of ST of all processes gives us maxTime
	return maxTime;
}
//function to print the required output
void printOutput(vector<process> processesVector,string schPolicy,vector<string> processSchedule,int isPriorityReversed=0){
	float totalTAT=0,totalWT=0;
	//calculating total TAT and total WT
	for(int i=0;i<processesVector.size();i++){
		totalWT+=processesVector[i].iniWT+processesVector[i].intWT;
		totalTAT+=processesVector[i].TAT;
	}
	//calculating mean TAT and mean WT
	float meanTAT=totalTAT/(processesVector.size());
	float meanWT=totalWT/(processesVector.size());
	//writing output to output file
	string filename=schPolicy;
	filename=filename+"-output.txt";
	ofstream myfile (filename);
  	if (myfile.is_open()){
  		myfile<<"Sheduling Policy:\n"<<schPolicy<<"\n";
  		myfile<<"Output parameters for all processes:\n";
    	for(int i=0;i<processesVector.size();i++){
    		if(isPriorityReversed==0)
				myfile<<(processesVector[i].processID)<<" "<<(processesVector[i].AT)<<" "<<(processesVector[i].ST)<<" "<<(processesVector[i].Pr)<<" "<<(processesVector[i].FT)<<" "<<(processesVector[i].TAT)<<" "<<(meanTAT)<<" "<<(processesVector[i].iniWT)<<" "<<(processesVector[i].intWT)<<" "<<(processesVector[i].iniWT+processesVector[i].intWT)<<" "<<(meanWT)<<"\n";
			else if(isPriorityReversed==1)
				myfile<<(processesVector[i].processID)<<" "<<(processesVector[i].AT)<<" "<<(processesVector[i].ST)<<" "<<(-processesVector[i].Pr)<<" "<<(processesVector[i].FT)<<" "<<(processesVector[i].TAT)<<" "<<(meanTAT)<<" "<<(processesVector[i].iniWT)<<" "<<(processesVector[i].intWT)<<" "<<(processesVector[i].iniWT+processesVector[i].intWT)<<" "<<(meanWT)<<"\n";
		}
		myfile<<"Order of sheduling processes:\n";
		for(int i=0;i<processSchedule.size();i++)
			myfile<<processSchedule[i]<<" ";
		myfile<<"\n";
		//calculating the number of times a process has been scheduled
		for(int i=0;i<processSchedule.size();i++){
			for(int j=0;j<processesVector.size();j++){
				if(processSchedule[i]==processesVector[j].processID)
					processesVector[j].timesSch++;
			}
		}
		myfile<<"Number of times each process has been scheduled:\n";
		for(int i=0;i<processesVector.size();i++){
			myfile<<processesVector[i].processID<<" "<<processesVector[i].timesSch<<endl;
		}
    	myfile.close();
  	}
  	else 
  		cout << "Unable to open file";
}
//function which selects best sch policy for given input by looking at output files of each sch policy
void compareSchPolicies(){
	//this function selects best Sch policy for given input by taking minimum meanTAT of all processes
	ifstream infile;
	//dummy variables to retrieve required value from output file
	int a,b,c,d,e,index=0;
	float f,minTAT;
	string ch1,ch2,ch3,ch4,ch5,ch6,ch7,ch8;
	string ch,schPolicies[8]={"FCFS","SJF","Priority","HRRN","SRTN","RoundRobin","MLQ","MLFQ"};
	string filename=schPolicies[0];
	filename=filename+"-output.txt";
    infile.open(filename);
    infile>>ch1>>ch2>>ch3>>ch4>>ch5>>ch6>>ch7>>ch8;
    infile>>ch>>a>>b>>c>>d>>e>>f;
    minTAT=f;
    index=0;
    infile.close();
	for(int i=1;i<8;i++){
	string filename=schPolicies[i];
	filename=filename+"-output.txt";
    infile.open(filename);
    infile>>ch1>>ch2>>ch3>>ch4>>ch5>>ch6>>ch7>>ch8;
    infile>>ch>>a>>b>>c>>d>>e>>f;
    if(f<minTAT){
    		minTAT=f;
    		index=i;
    	}
    infile.close();
	}
    cout<<"Best scheduling policy for given input is "<<schPolicies[index]<<"\n";
}
// function which updates the parameters of every process
vector<process> updateQueue(vector<process> queue,int time,int isUsingCpu,int indexOfCpuUsingProcessInQueue){
	for(int i=0;i<queue.size();i++){
		if(indexOfCpuUsingProcessInQueue!=-1){
			if(i==indexOfCpuUsingProcessInQueue){
				queue[i].cpuUsage++;
			}
			else{
				if(queue[i].cpuUsage>=1){
					queue[i].intWT++;
				}
				else{
					queue[i].iniWT++;
				}
			}
		}
		else{
			if(queue[i].cpuUsage>=1){
				queue[i].intWT++;
			}
			else{
				queue[i].iniWT++;
			}
		}
		queue[i].RemainingST=queue[i].ST-queue[i].cpuUsage;
	}
	return queue;
}
//function that adds new processes into queue
vector<process> addNewProcesses(vector<process> queue,int time,vector<process> processesVector,int *newProcessAdded=0,string limits=""){
	if(limits==""){
		for(int i=0;i<processesVector.size();i++){
			if(processesVector[i].AT==time){
				processesVector[i].QOrder=queue.size();
				queue.push_back(processesVector[i]);
				*newProcessAdded=1;
			}
		}
	}
	else if(limits[0]=='-'){
		for(int i=0;i<processesVector.size();i++){
			if(processesVector[i].AT==time&&processesVector[i].ST<(int(limits[1])-48)){
				queue.push_back(processesVector[i]);
				*newProcessAdded=1;
			}
		}
	}
	else if(limits[1]=='+'){
		for(int i=0;i<processesVector.size();i++){
			if(processesVector[i].AT==time&&processesVector[i].ST>=(int(limits[0])-48)){
				queue.push_back(processesVector[i]);
				*newProcessAdded=1;
			}
		}
	}
	else{
		for(int i=0;i<processesVector.size();i++){
			if(processesVector[i].AT==time&&(processesVector[i].ST>=(int(limits[0])-48)&&processesVector[i].ST<=(int(limits[1])-48))){
				queue.push_back(processesVector[i]);
				*newProcessAdded=1;
			}
		}
	}
	return queue;
}
//function(used for MLQ,MLFQ) which gets next process in a queue based on the sch policy which the queue is following
int getNextProcess(vector<process> queue,string schPolicy,int unnecessaryTQ=0){
	int indexOfCpuUsingProcessInQueue=0;
	if(schPolicy=="FCFS"){
		if(queue.size()>1){
			int minAT=queue[0].AT;
			indexOfCpuUsingProcessInQueue=0;
			for(int i=0;i<queue.size();i++){
				if(queue[i].AT<minAT){
					indexOfCpuUsingProcessInQueue=i;
					minAT=queue[i].AT;
				}
			}
		}
		else if(queue.size()==1){
			indexOfCpuUsingProcessInQueue=0;
		}
	}
	else if(schPolicy=="SJF"){
		if(queue.size()>1){
			int minST=queue[0].ST;
			indexOfCpuUsingProcessInQueue=0;
			for(int i=1;i<queue.size();i++){
				if(queue[i].ST<minST){
					indexOfCpuUsingProcessInQueue=i;
					minST=queue[i].ST;
				}
			}
		}
		else if(queue.size()==1){
			indexOfCpuUsingProcessInQueue=0;
		}
	}
	else if(schPolicy=="Priority"){
		if(queue.size()>1){
					int minPr=queue[0].Pr;
					indexOfCpuUsingProcessInQueue=0;
					for(int i=1;i<queue.size();i++){
						if(queue[i].Pr<minPr){
							indexOfCpuUsingProcessInQueue=i;
							minPr=queue[i].Pr;
						}
					}
				}
				else if(queue.size()==1){
					indexOfCpuUsingProcessInQueue=0;
				}
	}
	else if(schPolicy=="HRRN"){
		if(queue.size()>1){
					int maxRR=(queue[0].iniWT+queue[0].intWT+queue[0].ST-unnecessaryTQ)/(queue[0].ST-unnecessaryTQ);
					indexOfCpuUsingProcessInQueue=0;
					for(int i=1;i<queue.size();i++){
						if(((queue[i].iniWT+queue[i].intWT+queue[i].ST-unnecessaryTQ)/(queue[i].ST-unnecessaryTQ))>maxRR){
							indexOfCpuUsingProcessInQueue=i;
							maxRR=(queue[i].iniWT+queue[i].intWT+queue[i].ST-unnecessaryTQ)/(queue[i].ST-unnecessaryTQ);
						}
					}
				}
				else if(queue.size()==1){
					indexOfCpuUsingProcessInQueue=0;
				}
	}
	else if(schPolicy=="SRTN"){
		if(queue.size()>1){
					int minRST=queue[0].ST-queue[0].cpuUsage;
					indexOfCpuUsingProcessInQueue=0;
					for(int i=1;i<queue.size();i++){
						if((queue[i].ST-queue[i].cpuUsage)<minRST){
							indexOfCpuUsingProcessInQueue=i;
							minRST=queue[i].ST-queue[i].cpuUsage;

						}
					}
				}
				else if(queue.size()==1){
					indexOfCpuUsingProcessInQueue=0;
				}
	}
	else if(schPolicy=="RR"){
		if(indexOfCpuUsingProcessInQueue==-1){
			for(int i=0;i<queue.size();i++){
				if(i!=indexOfCpuUsingProcessInQueue){
					queue[i].QOrder--;
				}
				else{
					queue[i].QOrder=queue.size()-1;
				}
			}
		}
		else{
			for(int i=0;i<queue.size();i++){
				queue[i].QOrder--;
			}
		}
		for(int i=0;i<queue.size();i++){
			if(queue[i].QOrder==0){
				indexOfCpuUsingProcessInQueue=i;
				break;
			}
		}
	}
	return indexOfCpuUsingProcessInQueue;
}
//function which implements FCFS scheduling policy
void FCFS(vector<process> processesVector,int tieBreaker=0,int isPriorityReversed=0){
	vector<string> processSchedule;
	int time=-1,indexOfCpuUsingProcessInQueue=-1,isUsingCpu=0,newProcessAdded=0;
	int maximumFinalTime=calculateMaxFinalTime(processesVector);
	vector<process> queue;
	ofstream infile ("FCFSQTrace.txt");
  	while(time<=maximumFinalTime){
	newProcessAdded=0;
		time++;
		if(time>=1)
			queue=updateQueue(queue,time,isUsingCpu,indexOfCpuUsingProcessInQueue);
		queue=addNewProcesses(queue,time,processesVector,&newProcessAdded);
		if(isUsingCpu==1){
			if(queue[indexOfCpuUsingProcessInQueue].cpuUsage==queue[indexOfCpuUsingProcessInQueue].ST){
				queue[indexOfCpuUsingProcessInQueue].FT=time;
				queue[indexOfCpuUsingProcessInQueue].TAT=time-queue[indexOfCpuUsingProcessInQueue].AT;
				queue[indexOfCpuUsingProcessInQueue].isDone=1;
				for(int i=0;i<processesVector.size();i++){
					if(queue[indexOfCpuUsingProcessInQueue].processID==processesVector[i].processID){
						processesVector[i]=queue[indexOfCpuUsingProcessInQueue];
						break;
					}
				}
				queue.erase(queue.begin()+indexOfCpuUsingProcessInQueue);
				indexOfCpuUsingProcessInQueue=-1;
				isUsingCpu=0;
			}
		}
		if(isUsingCpu==0){
				if(queue.size()>1){
					int minAT=queue[0].AT;
					indexOfCpuUsingProcessInQueue=0;
					for(int i=1;i<queue.size();i++){
						if(queue[i].AT<minAT){
							indexOfCpuUsingProcessInQueue=i;
							minAT=queue[i].AT;
						}
						else if(queue[i].AT==minAT){
							if(tieBreaker==0)
								{
								if(queue[i].Pr<queue[indexOfCpuUsingProcessInQueue].Pr){
									indexOfCpuUsingProcessInQueue=i;
									minAT=queue[i].AT;
								}
							}
							else if(tieBreaker==1)
								{
								if(queue[i].Pr<queue[indexOfCpuUsingProcessInQueue].Pr){
									indexOfCpuUsingProcessInQueue=i;
									minAT=queue[i].AT;
								}
							}
							else if(tieBreaker==2){
								if(queue[i].ST<queue[indexOfCpuUsingProcessInQueue].ST){
									indexOfCpuUsingProcessInQueue=i;
									minAT=queue[i].AT;
								}
							}
							else if(tieBreaker==3){
								if(queue[i].Pr<queue[indexOfCpuUsingProcessInQueue].Pr){
									indexOfCpuUsingProcessInQueue=i;
									minAT=queue[i].AT;
								}
							}
						}
					}
					processSchedule.push_back(queue[indexOfCpuUsingProcessInQueue].processID);
					queue[indexOfCpuUsingProcessInQueue].StartTime=time;
					isUsingCpu=1;
				}
				else if(queue.size()==1){
					indexOfCpuUsingProcessInQueue=0;
					processSchedule.push_back(queue[indexOfCpuUsingProcessInQueue].processID);
					queue[indexOfCpuUsingProcessInQueue].StartTime=time;
					isUsingCpu=1;
				}
			}
			if(time<=20){
			infile<<time<<"||"<<queue[indexOfCpuUsingProcessInQueue].processID<<"||";
			string a;
			for(int i=0;i<queue.size();i++){
				if(i!=indexOfCpuUsingProcessInQueue)
					a=a+queue[i].processID+" ";
			}
			infile<<a<<"\n";
		}
	}
	printOutput(processesVector,"FCFS",processSchedule,isPriorityReversed);
	infile.close();
}
//function which implements SJF scheduling policy
void SJF(vector<process> processesVector,int tieBreaker=0,int isPriorityReversed=0){
	vector<string> processSchedule;
	int time=-1,indexOfCpuUsingProcessInQueue=-1,isUsingCpu=0,newProcessAdded=0;
	int maximumFinalTime=calculateMaxFinalTime(processesVector);
	vector<process> queue;
	ofstream infile ("SJFQTrace.txt");
	while(time<=maximumFinalTime){
	newProcessAdded=0;
		time++;
		if(time>=1)
			queue=updateQueue(queue,time,isUsingCpu,indexOfCpuUsingProcessInQueue);
		queue=addNewProcesses(queue,time,processesVector,&newProcessAdded);
		if(isUsingCpu==1){
			if(queue[indexOfCpuUsingProcessInQueue].cpuUsage==queue[indexOfCpuUsingProcessInQueue].ST){
				queue[indexOfCpuUsingProcessInQueue].FT=time;
				queue[indexOfCpuUsingProcessInQueue].TAT=time-queue[indexOfCpuUsingProcessInQueue].AT;
				queue[indexOfCpuUsingProcessInQueue].isDone=1;
				for(int i=0;i<processesVector.size();i++){
					if(queue[indexOfCpuUsingProcessInQueue].processID==processesVector[i].processID){
						processesVector[i]=queue[indexOfCpuUsingProcessInQueue];
						break;
					}
				}
				queue.erase(queue.begin()+indexOfCpuUsingProcessInQueue);
				indexOfCpuUsingProcessInQueue=-1;
				isUsingCpu=0;
			}
		}
		if(isUsingCpu==0){			
				if(queue.size()>1){
					int minST=queue[0].ST;
					indexOfCpuUsingProcessInQueue=0;
					for(int i=1;i<queue.size();i++){
						if(queue[i].ST<minST){
							indexOfCpuUsingProcessInQueue=i;
							minST=queue[i].ST;

						}
						else if(queue[i].ST==minST){

							if(tieBreaker==0)
								{
								if(queue[i].Pr<queue[indexOfCpuUsingProcessInQueue].Pr){
									indexOfCpuUsingProcessInQueue=i;
									minST=queue[i].ST;
								}
							}
							else if(tieBreaker==1){
								if(queue[i].AT<queue[indexOfCpuUsingProcessInQueue].AT){
									indexOfCpuUsingProcessInQueue=i;
									minST=queue[i].ST;
								}
							}
							else if(tieBreaker==2)
								{
								if(queue[i].Pr<queue[indexOfCpuUsingProcessInQueue].Pr){
									indexOfCpuUsingProcessInQueue=i;
									minST=queue[i].ST;
								}
							}	
							
							else if(tieBreaker==3){
								if(queue[i].Pr<queue[indexOfCpuUsingProcessInQueue].Pr){
									indexOfCpuUsingProcessInQueue=i;
									minST=queue[i].ST;
								}
							}
						}
					}
					processSchedule.push_back(queue[indexOfCpuUsingProcessInQueue].processID);
					queue[indexOfCpuUsingProcessInQueue].StartTime=time;
					isUsingCpu=1;
				}
				else if(queue.size()==1){
					indexOfCpuUsingProcessInQueue=0;
					processSchedule.push_back(queue[indexOfCpuUsingProcessInQueue].processID);
					queue[indexOfCpuUsingProcessInQueue].StartTime=time;
					isUsingCpu=1;
				}
			}
			if(time<=20){
			infile<<time<<"||"<<queue[indexOfCpuUsingProcessInQueue].processID<<"||";
			string a;
			for(int i=0;i<queue.size();i++){
				if(i!=indexOfCpuUsingProcessInQueue)
					a=a+queue[i].processID+" ";
			}
			infile<<a<<"\n";
		}
	}
	printOutput(processesVector,"SJF",processSchedule,isPriorityReversed);
	infile.close();
}
//function which implements Priority scheduling policy
void Priority(vector<process> processesVector,int tieBreaker=0,int isPriorityReversed=0){
	vector<string> processSchedule;
	int time=-1,indexOfCpuUsingProcessInQueue=-1,isUsingCpu=0,newProcessAdded=0;
	int maximumFinalTime=calculateMaxFinalTime(processesVector);
	vector<process> queue;
	ofstream infile ("PriorityQTrace.txt");
	while(time<=maximumFinalTime){
	newProcessAdded=0;
		time++;
		if(time>=1)
			queue=updateQueue(queue,time,isUsingCpu,indexOfCpuUsingProcessInQueue);
		queue=addNewProcesses(queue,time,processesVector,&newProcessAdded);
		if(isUsingCpu==1){
			if(queue[indexOfCpuUsingProcessInQueue].cpuUsage==queue[indexOfCpuUsingProcessInQueue].ST){
				queue[indexOfCpuUsingProcessInQueue].FT=time;
				queue[indexOfCpuUsingProcessInQueue].TAT=time-queue[indexOfCpuUsingProcessInQueue].AT;
				queue[indexOfCpuUsingProcessInQueue].isDone=1;
				for(int i=0;i<processesVector.size();i++){
					if(queue[indexOfCpuUsingProcessInQueue].processID==processesVector[i].processID){
						processesVector[i]=queue[indexOfCpuUsingProcessInQueue];
						break;
					}
				}
				queue.erase(queue.begin()+indexOfCpuUsingProcessInQueue);
				indexOfCpuUsingProcessInQueue=-1;
				isUsingCpu=0;
			}
		}
		if(isUsingCpu==0){
				if(queue.size()>1){
					int minPr=queue[0].Pr;
					indexOfCpuUsingProcessInQueue=0;
					for(int i=1;i<queue.size();i++){
						if(queue[i].Pr<minPr){
							indexOfCpuUsingProcessInQueue=i;
							minPr=queue[i].Pr;
						}
						else if(queue[i].Pr==minPr){
							if(tieBreaker==0)
								{
								if(queue[i].AT<queue[indexOfCpuUsingProcessInQueue].AT){
									indexOfCpuUsingProcessInQueue=i;
									minPr=queue[i].Pr;
								}
							}
							else if(tieBreaker==1){
								if(queue[i].AT<queue[indexOfCpuUsingProcessInQueue].AT){
									indexOfCpuUsingProcessInQueue=i;
									minPr=queue[i].Pr;
								}
							}
							else if(tieBreaker==2){
								if(queue[i].ST<queue[indexOfCpuUsingProcessInQueue].ST){
									indexOfCpuUsingProcessInQueue=i;
									minPr=queue[i].Pr;
								}
							}
							else if(tieBreaker==3)
							{
								if(queue[i].AT<queue[indexOfCpuUsingProcessInQueue].AT){
									indexOfCpuUsingProcessInQueue=i;
									minPr=queue[i].Pr;
								}
							}
						}
					}
					processSchedule.push_back(queue[indexOfCpuUsingProcessInQueue].processID);
					queue[indexOfCpuUsingProcessInQueue].StartTime=time;
					isUsingCpu=1;
				}
				else if(queue.size()==1){
					indexOfCpuUsingProcessInQueue=0;
					processSchedule.push_back(queue[indexOfCpuUsingProcessInQueue].processID);
					queue[indexOfCpuUsingProcessInQueue].StartTime=time;
					isUsingCpu=1;
				}
			}
			if(time<=20){
			infile<<time<<"||"<<queue[indexOfCpuUsingProcessInQueue].processID<<"||";
			string a;
			for(int i=0;i<queue.size();i++){
				if(i!=indexOfCpuUsingProcessInQueue)
					a=a+queue[i].processID+" ";
			}
			infile<<a<<"\n";
		}
	}
	printOutput(processesVector,"Priority",processSchedule,isPriorityReversed);
	infile.close();
}
//function which implements HRRN scheduling policy
void HRRN(vector<process> processesVector,int tieBreaker=0,int isPriorityReversed=0){
	vector<string> processSchedule;
	int time=-1,indexOfCpuUsingProcessInQueue=-1,isUsingCpu=0,newProcessAdded=0;
	int maximumFinalTime=calculateMaxFinalTime(processesVector);
	vector<process> queue;
	ofstream infile ("HRRNQTrace.txt");
	while(time<=maximumFinalTime){
	newProcessAdded=0;
		time++;
		if(time>=1)
			queue=updateQueue(queue,time,isUsingCpu,indexOfCpuUsingProcessInQueue);
		queue=addNewProcesses(queue,time,processesVector,&newProcessAdded);
		if(isUsingCpu==1){
			if(queue[indexOfCpuUsingProcessInQueue].cpuUsage==queue[indexOfCpuUsingProcessInQueue].ST){
				queue[indexOfCpuUsingProcessInQueue].FT=time;
				queue[indexOfCpuUsingProcessInQueue].TAT=time-queue[indexOfCpuUsingProcessInQueue].AT;
				queue[indexOfCpuUsingProcessInQueue].isDone=1;
				for(int i=0;i<processesVector.size();i++){
					if(queue[indexOfCpuUsingProcessInQueue].processID==processesVector[i].processID){
						processesVector[i]=queue[indexOfCpuUsingProcessInQueue];
						break;
					}
				}
				queue.erase(queue.begin()+indexOfCpuUsingProcessInQueue);
				indexOfCpuUsingProcessInQueue=-1;
				isUsingCpu=0;
			}
		}
		if(isUsingCpu==0){
				if(queue.size()>1){
					int maxRR=(queue[0].iniWT+queue[0].ST)/(queue[0].ST);
					indexOfCpuUsingProcessInQueue=0;
					for(int i=1;i<queue.size();i++){
						if(((queue[i].iniWT+queue[i].ST)/(queue[i].ST))>maxRR){
							indexOfCpuUsingProcessInQueue=i;
							maxRR=(queue[i].iniWT+queue[i].ST)/(queue[i].ST);
						}
						else if(((queue[i].iniWT+queue[i].ST)/(queue[i].ST))==maxRR){
							if(tieBreaker==0)
								{
								if(queue[i].AT<queue[indexOfCpuUsingProcessInQueue].AT){
									indexOfCpuUsingProcessInQueue=i;
									maxRR=(queue[i].iniWT+queue[i].ST)/(queue[i].ST);
								}
							}
							else if(tieBreaker==1){
								if(queue[i].AT<queue[indexOfCpuUsingProcessInQueue].AT){
									indexOfCpuUsingProcessInQueue=i;
									maxRR=(queue[i].iniWT+queue[i].ST)/(queue[i].ST);
								}
							}
							else if(tieBreaker==2){
								if(queue[i].ST<queue[indexOfCpuUsingProcessInQueue].ST){
									indexOfCpuUsingProcessInQueue=i;
									maxRR=(queue[i].iniWT+queue[i].ST)/(queue[i].ST);
								}
							}	
							else if(tieBreaker==3){
								if(queue[i].Pr<queue[indexOfCpuUsingProcessInQueue].Pr){
									indexOfCpuUsingProcessInQueue=i;
									maxRR=(queue[i].iniWT+queue[i].ST)/(queue[i].ST);
								}
							}
						}
					}
					processSchedule.push_back(queue[indexOfCpuUsingProcessInQueue].processID);
					queue[indexOfCpuUsingProcessInQueue].StartTime=time;
					isUsingCpu=1;
				}
				else if(queue.size()==1){
					indexOfCpuUsingProcessInQueue=0;
					processSchedule.push_back(queue[indexOfCpuUsingProcessInQueue].processID);
					queue[indexOfCpuUsingProcessInQueue].StartTime=time;
					isUsingCpu=1;
				}
			}
			if(time<=20){
			infile<<time<<"||"<<queue[indexOfCpuUsingProcessInQueue].processID<<"||";
			string a;
			for(int i=0;i<queue.size();i++){
				if(i!=indexOfCpuUsingProcessInQueue)
					a=a+queue[i].processID+" ";
			}
			infile<<a<<"\n";
		}
	}
	printOutput(processesVector,"HRRN",processSchedule,isPriorityReversed);
	infile.close();
}
//function which implements SRTN scheduling policy
void SRTN(vector<process> processesVector,int tieBreaker=0,int isPriorityReversed=0){
	vector<string> processSchedule;
	int time=-1,indexOfCpuUsingProcessInQueue=-1,isUsingCpu=0,newProcessAdded=0;
	int maximumFinalTime=calculateMaxFinalTime(processesVector);
	vector<process> queue;
	ofstream infile ("SRTNQTrace.txt");
	while(time<=maximumFinalTime){
		newProcessAdded=0;
		time++;
		if(time>=1)
			queue=updateQueue(queue,time,isUsingCpu,indexOfCpuUsingProcessInQueue);
		queue=addNewProcesses(queue,time,processesVector,&newProcessAdded);
		if(isUsingCpu==1){
			if(queue[indexOfCpuUsingProcessInQueue].cpuUsage==queue[indexOfCpuUsingProcessInQueue].ST){
				queue[indexOfCpuUsingProcessInQueue].FT=time;
				queue[indexOfCpuUsingProcessInQueue].TAT=time-queue[indexOfCpuUsingProcessInQueue].AT;
				queue[indexOfCpuUsingProcessInQueue].isDone=1;
				for(int i=0;i<processesVector.size();i++){
					if(queue[indexOfCpuUsingProcessInQueue].processID==processesVector[i].processID){
						processesVector[i]=queue[indexOfCpuUsingProcessInQueue];
						break;
					}
				}
				queue.erase(queue.begin()+indexOfCpuUsingProcessInQueue);
				indexOfCpuUsingProcessInQueue=-1;
				isUsingCpu=0;
			}
			else if(newProcessAdded==1){
				if(queue[indexOfCpuUsingProcessInQueue].RemainingST>queue[queue.size()-1].ST){
					indexOfCpuUsingProcessInQueue=queue.size()-1;
					isUsingCpu=1;
				}
				else if(queue[indexOfCpuUsingProcessInQueue].RemainingST==queue[queue.size()-1].ST){
					if(tieBreaker==0)
								{
								if(queue[queue.size()-1].AT<queue[indexOfCpuUsingProcessInQueue].AT){
									indexOfCpuUsingProcessInQueue=queue.size()-1;
									isUsingCpu=1;
									queue[queue.size()-1].StartTime=time;
								}
							}
							else if(tieBreaker==1){
								if(queue[queue.size()-1].AT<queue[indexOfCpuUsingProcessInQueue].AT){
									indexOfCpuUsingProcessInQueue=queue.size()-1;
									isUsingCpu=1;
									queue[queue.size()-1].StartTime=time;
								}
							}
							else if(tieBreaker==2){
								if(queue[queue.size()-1].ST<queue[indexOfCpuUsingProcessInQueue].ST){
									indexOfCpuUsingProcessInQueue=queue.size()-1;
									isUsingCpu=1;
									queue[queue.size()-1].StartTime=time;
								}
							}
							else if(tieBreaker==3){
								if(queue[queue.size()-1].Pr<queue[indexOfCpuUsingProcessInQueue].Pr){
									indexOfCpuUsingProcessInQueue=queue.size()-1;
									isUsingCpu=1;
									queue[queue.size()-1].StartTime=time;
								}
							}
				}
			}
		}
		if(isUsingCpu==0){		
				if(queue.size()>1){
					int minRemainingST=queue[0].RemainingST;
					indexOfCpuUsingProcessInQueue=0;
					for(int i=1;i<queue.size();i++){
						if(queue[i].RemainingST<minRemainingST){
							indexOfCpuUsingProcessInQueue=i;
							minRemainingST=queue[i].RemainingST;
						}
						else if(queue[i].RemainingST==minRemainingST){

							if(tieBreaker==0)
								{
								if(queue[i].AT<queue[indexOfCpuUsingProcessInQueue].AT){
									indexOfCpuUsingProcessInQueue=i;
									minRemainingST=queue[i].RemainingST;
								}
							}
							else if(tieBreaker==1){
								if(queue[i].AT<queue[indexOfCpuUsingProcessInQueue].AT){
									indexOfCpuUsingProcessInQueue=i;
									minRemainingST=queue[i].RemainingST;
								}
							}
							else if(tieBreaker==2){
								if(queue[i].ST<queue[indexOfCpuUsingProcessInQueue].ST){
									indexOfCpuUsingProcessInQueue=i;
									minRemainingST=queue[i].RemainingST;
								}
							}		
							else if(tieBreaker==3){
								if(queue[i].Pr<queue[indexOfCpuUsingProcessInQueue].Pr){
									indexOfCpuUsingProcessInQueue=i;
									minRemainingST=queue[i].RemainingST;
								}
							}
						}
					}
					processSchedule.push_back(queue[indexOfCpuUsingProcessInQueue].processID);
					if(queue[indexOfCpuUsingProcessInQueue].StartTime==0)
						queue[indexOfCpuUsingProcessInQueue].StartTime=time;
					isUsingCpu=1;
				}
				else if(queue.size()==1){
					indexOfCpuUsingProcessInQueue=0;
					processSchedule.push_back(queue[indexOfCpuUsingProcessInQueue].processID);
					if(queue[indexOfCpuUsingProcessInQueue].StartTime==0)
						queue[indexOfCpuUsingProcessInQueue].StartTime=time;
					isUsingCpu=1;
				}
			}
			if(time<=20){
			infile<<time<<"||"<<queue[indexOfCpuUsingProcessInQueue].processID<<"||";
			string a;
			for(int i=0;i<queue.size();i++){
				if(i!=indexOfCpuUsingProcessInQueue)
					a=a+queue[i].processID+" ";
			}
			infile<<a<<"\n";
		}
	}
	printOutput(processesVector,"SRTN",processSchedule,isPriorityReversed);
	infile.close();
}
//function which implements RR scheduling policy
void RR(vector<process> processesVector,int timeQ=3,int isPriorityReversed=0){
	vector<string> processSchedule;
	int time=-1,indexOfCpuUsingProcessInQueue=-1,isUsingCpu=0,newProcessAdded=0,timeQuantum=timeQ,timeQuantumUsage=0;
	int maximumFinalTime=calculateMaxFinalTime(processesVector);
	vector<process> queue;
	ofstream infile ("RoundRobinQTrace.txt");
	while(time<=maximumFinalTime){
		time++;
		if(time>=1)
			queue=updateQueue(queue,time,isUsingCpu,indexOfCpuUsingProcessInQueue);
		queue=addNewProcesses(queue,time,processesVector,&newProcessAdded);
		if(isUsingCpu==1){
			if(time>=0)
				timeQuantumUsage++;
			if(queue[indexOfCpuUsingProcessInQueue].cpuUsage==queue[indexOfCpuUsingProcessInQueue].ST){
				queue[indexOfCpuUsingProcessInQueue].FT=time;
				queue[indexOfCpuUsingProcessInQueue].TAT=time-queue[indexOfCpuUsingProcessInQueue].AT;
				queue[indexOfCpuUsingProcessInQueue].isDone=1;
				for(int i=0;i<processesVector.size();i++){
					if(queue[indexOfCpuUsingProcessInQueue].processID==processesVector[i].processID){
						processesVector[i]=queue[indexOfCpuUsingProcessInQueue];
						break;
					}
				}
				queue.erase(queue.begin()+indexOfCpuUsingProcessInQueue);
				for(int i=0;i<queue.size();i++){			
						queue[i].QOrder--;		
				}
				indexOfCpuUsingProcessInQueue=-1;
				isUsingCpu=0;
			}
			else if(timeQuantumUsage==timeQuantum){
				for(int i=0;i<queue.size();i++){
					if(i!=indexOfCpuUsingProcessInQueue){
						queue[i].QOrder--;
					}
					else{
						queue[i].QOrder=queue.size()-1;
					}
				}
				indexOfCpuUsingProcessInQueue=-1;
				isUsingCpu=0;
			}
		}
		if(isUsingCpu==0){	
				if(queue.size()>=1){
					for(int i=0;i<queue.size();i++){
						if(queue[i].QOrder==0){
							indexOfCpuUsingProcessInQueue=i;
							break;
						}
					}
					processSchedule.push_back(queue[indexOfCpuUsingProcessInQueue].processID);
					if(queue[indexOfCpuUsingProcessInQueue].StartTime==0)
						queue[indexOfCpuUsingProcessInQueue].StartTime=time;
					isUsingCpu=1;
					timeQuantumUsage=0;
				}
			}
			if(time<=20){
			infile<<time<<"||"<<queue[indexOfCpuUsingProcessInQueue].processID<<"||";
			string a;
			for(int i=0;i<queue.size();i++){
				if(i!=indexOfCpuUsingProcessInQueue)
					a=a+queue[i].processID+" ";
			}
			infile<<a<<"\n";
		}
	}
	printOutput(processesVector,"RoundRobin",processSchedule,isPriorityReversed);
	infile.close();
}
// function which implements MLQ scheduling policy
void MLQ(vector<process> processesVector,int numOfQs,int schPolicies[],int isPriorityReversed=0){
	vector<string> processSchedule;
	int time=-1,indexOfCpuUsingProcessInQueue=-1,isUsingCpu=0,newProcessAdded=0,timeQuantum1=1,timeQuantum2=2,timeQuantum3=3,timeQuantumUsage=0;
	int maximumFinalTime=calculateMaxFinalTime(processesVector);
	vector<process> queue1,queue2,queue3;
	ofstream infile ("MLQQTrace.txt");
	while(time<=maximumFinalTime){
		time++;
		if(time>=1){
			if(isUsingCpu==1){
				queue1=updateQueue(queue1,time,isUsingCpu,indexOfCpuUsingProcessInQueue);
				queue2=updateQueue(queue2,time,isUsingCpu,-1);
				queue3=updateQueue(queue3,time,isUsingCpu,-1);
			}
			else if(isUsingCpu==2){
				queue1=updateQueue(queue1,time,isUsingCpu,-1);
				queue2=updateQueue(queue2,time,isUsingCpu,indexOfCpuUsingProcessInQueue);
				queue3=updateQueue(queue3,time,isUsingCpu,-1);
			}
			else if(isUsingCpu==3){
				queue1=updateQueue(queue1,time,isUsingCpu,-1);
				queue2=updateQueue(queue2,time,isUsingCpu,-1);
				queue3=updateQueue(queue3,time,isUsingCpu,indexOfCpuUsingProcessInQueue);
			}
			else{
				queue1=updateQueue(queue1,time,isUsingCpu,-1);
				queue2=updateQueue(queue2,time,isUsingCpu,-1);
				queue3=updateQueue(queue3,time,isUsingCpu,-1);
			}		
		}
		queue1=addNewProcesses(queue1,time,processesVector,&newProcessAdded,"-3");
		queue2=addNewProcesses(queue2,time,processesVector,&newProcessAdded,"34");
		queue3=addNewProcesses(queue3,time,processesVector,&newProcessAdded,"5+");	
		if(isUsingCpu!=0){
			if(time>=0)
 				timeQuantumUsage++;
			if(isUsingCpu==1){
				if(queue1[indexOfCpuUsingProcessInQueue].cpuUsage==queue1[indexOfCpuUsingProcessInQueue].ST){
					queue1[indexOfCpuUsingProcessInQueue].FT=time;
					queue1[indexOfCpuUsingProcessInQueue].TAT=time-queue1[indexOfCpuUsingProcessInQueue].AT;
					queue1[indexOfCpuUsingProcessInQueue].isDone=1;
				for(int i=0;i<processesVector.size();i++){
					if(queue1[indexOfCpuUsingProcessInQueue].processID==processesVector[i].processID){
						processesVector[i]=queue1[indexOfCpuUsingProcessInQueue];
						break;
					}
				}
				queue1.erase(queue1.begin()+indexOfCpuUsingProcessInQueue);
				indexOfCpuUsingProcessInQueue=-1;
				isUsingCpu=0;
				}
				else if(timeQuantumUsage==timeQuantum1){
					indexOfCpuUsingProcessInQueue=-1;
					isUsingCpu=0;
				}
			}
			else if(isUsingCpu==2){
				if(queue2[indexOfCpuUsingProcessInQueue].cpuUsage==queue2[indexOfCpuUsingProcessInQueue].ST){
					queue2[indexOfCpuUsingProcessInQueue].FT=time;
					queue2[indexOfCpuUsingProcessInQueue].TAT=time-queue2[indexOfCpuUsingProcessInQueue].AT;
					queue2[indexOfCpuUsingProcessInQueue].isDone=1;
				for(int i=0;i<processesVector.size();i++){
					if(queue2[indexOfCpuUsingProcessInQueue].processID==processesVector[i].processID){
						processesVector[i]=queue2[indexOfCpuUsingProcessInQueue];
						break;
					}
				}
				queue2.erase(queue2.begin()+indexOfCpuUsingProcessInQueue);
				indexOfCpuUsingProcessInQueue=-1;
				isUsingCpu=0;
				}
				else if(timeQuantumUsage==timeQuantum2){
					indexOfCpuUsingProcessInQueue=-1;
					isUsingCpu=0;
				}
			}
			else{
				if(queue3[indexOfCpuUsingProcessInQueue].cpuUsage==queue3[indexOfCpuUsingProcessInQueue].ST){
					queue3[indexOfCpuUsingProcessInQueue].FT=time;
					queue3[indexOfCpuUsingProcessInQueue].TAT=time-queue3[indexOfCpuUsingProcessInQueue].AT;
					queue3[indexOfCpuUsingProcessInQueue].isDone=1;
				for(int i=0;i<processesVector.size();i++){
					if(queue3[indexOfCpuUsingProcessInQueue].processID==processesVector[i].processID){
						processesVector[i]=queue3[indexOfCpuUsingProcessInQueue];
						break;
					}
				}
				queue3.erase(queue3.begin()+indexOfCpuUsingProcessInQueue);
				indexOfCpuUsingProcessInQueue=-1;
				isUsingCpu=0;
				}
				else if(timeQuantumUsage==timeQuantum3){
					indexOfCpuUsingProcessInQueue=-1;
					isUsingCpu=0;
				}
			}
		}
		if(isUsingCpu==0){
			if(queue1.size()!=0){
				indexOfCpuUsingProcessInQueue=getNextProcess(queue1,"FCFS",0);
				if(queue1[indexOfCpuUsingProcessInQueue].StartTime==0)
					queue1[indexOfCpuUsingProcessInQueue].StartTime=time;
				processSchedule.push_back(queue1[indexOfCpuUsingProcessInQueue].processID);
				isUsingCpu=1;
				timeQuantumUsage=0;
			}
			else{
				if(queue2.size()!=0){
					indexOfCpuUsingProcessInQueue=getNextProcess(queue2,"SJF",timeQuantum1);
					if(queue2[indexOfCpuUsingProcessInQueue].StartTime==0)
						queue2[indexOfCpuUsingProcessInQueue].StartTime=time;
					processSchedule.push_back(queue2[indexOfCpuUsingProcessInQueue].processID);
					isUsingCpu=2;
					timeQuantumUsage=0;
				}
				else{
					if(queue3.size()!=0){
						indexOfCpuUsingProcessInQueue=getNextProcess(queue3,"FCFS",timeQuantum1+timeQuantum2);
						if(queue3[indexOfCpuUsingProcessInQueue].StartTime==0)
							queue3[indexOfCpuUsingProcessInQueue].StartTime=time;
						processSchedule.push_back(queue3[indexOfCpuUsingProcessInQueue].processID);
						isUsingCpu=3;
						timeQuantumUsage=0;
					}
				}
			}

		}
		if(time<=20){
		if(isUsingCpu==1){
			infile<<time<<"||"<<queue1[indexOfCpuUsingProcessInQueue].processID<<"||";
			string a;
			for(int i=0;i<queue1.size();i++){
				if(i!=indexOfCpuUsingProcessInQueue)
					a=a+queue1[i].processID+" ";
			}
			a=a+"||";
			for(int i=0;i<queue2.size();i++){
					a=a+queue2[i].processID+" ";
			}
			a=a+"||";
			for(int i=0;i<queue3.size();i++){
					a=a+queue3[i].processID+" ";
			}
			infile<<a<<"\n";
		}
		else if(isUsingCpu==2){
			infile<<time<<"||"<<queue2[indexOfCpuUsingProcessInQueue].processID<<"||";
			string a;
			for(int i=0;i<queue1.size();i++){
					a=a+queue1[i].processID+" ";
			}
			a=a+"||";
			for(int i=0;i<queue2.size();i++){
				if(i!=indexOfCpuUsingProcessInQueue)
					a=a+queue2[i].processID+" ";
			}
			a=a+"||";
			for(int i=0;i<queue3.size();i++){
					a=a+queue3[i].processID+" ";
			}
			infile<<a<<"\n";
		}
		else if(isUsingCpu==3){
			infile<<time<<"||"<<queue3[indexOfCpuUsingProcessInQueue].processID<<"||";
			string a;
			for(int i=0;i<queue1.size();i++){
					a=a+queue1[i].processID+" ";
			}
			a=a+"||";
			for(int i=0;i<queue2.size();i++){
					a=a+queue2[i].processID+" ";
			}
			a=a+"||";
			for(int i=0;i<queue3.size();i++){
				if(i!=indexOfCpuUsingProcessInQueue)
					a=a+queue3[i].processID+" ";
			}
			infile<<a<<"\n";
		}
		}
	}
	printOutput(processesVector,"MLQ",processSchedule,isPriorityReversed);
	infile.close();
}
//function which implements MLFQ scheduling policy
void MLFQ(vector<process> processesVector,int numOfQs,int schPolicies[],int isPriorityReversed=0){
	vector<string> processSchedule;
	int time=-1,indexOfCpuUsingProcessInQueue=-1,isUsingCpu=0,newProcessAdded=0,timeQuantum1=1,timeQuantum2=2,timeQuantum3=3,timeQuantumUsage=0;
	int maximumFinalTime=calculateMaxFinalTime(processesVector);
	vector<process> queue1,queue2,queue3;
	ofstream infile ("MLFQQTrace.txt");
	while(time<=maximumFinalTime){
		time++;
		if(time>=1){
			if(isUsingCpu==1){
				queue1=updateQueue(queue1,time,isUsingCpu,indexOfCpuUsingProcessInQueue);
				queue2=updateQueue(queue2,time,isUsingCpu,-1);
				queue3=updateQueue(queue3,time,isUsingCpu,-1);
			}
			else if(isUsingCpu==2){
				queue1=updateQueue(queue1,time,isUsingCpu,-1);
				queue2=updateQueue(queue2,time,isUsingCpu,indexOfCpuUsingProcessInQueue);
				queue3=updateQueue(queue3,time,isUsingCpu,-1);
			}
			else if(isUsingCpu==3){
				queue1=updateQueue(queue1,time,isUsingCpu,-1);
				queue2=updateQueue(queue2,time,isUsingCpu,-1);
				queue3=updateQueue(queue3,time,isUsingCpu,indexOfCpuUsingProcessInQueue);
			}
			else{
				queue1=updateQueue(queue1,time,isUsingCpu,-1);
				queue2=updateQueue(queue2,time,isUsingCpu,-1);
				queue3=updateQueue(queue3,time,isUsingCpu,-1);
			}
		}
		queue1=addNewProcesses(queue1,time,processesVector,&newProcessAdded,"");
		if(isUsingCpu!=0){
			if(time>=0)
  				timeQuantumUsage++;
			if(isUsingCpu==1){
				if(queue1[indexOfCpuUsingProcessInQueue].cpuUsage==queue1[indexOfCpuUsingProcessInQueue].ST){
					queue1[indexOfCpuUsingProcessInQueue].FT=time;
					queue1[indexOfCpuUsingProcessInQueue].TAT=time-queue1[indexOfCpuUsingProcessInQueue].AT;
					queue1[indexOfCpuUsingProcessInQueue].isDone=1;
				for(int i=0;i<processesVector.size();i++){
					if(queue1[indexOfCpuUsingProcessInQueue].processID==processesVector[i].processID){
						processesVector[i]=queue1[indexOfCpuUsingProcessInQueue];
						break;
					}
				}
				queue1.erase(queue1.begin()+indexOfCpuUsingProcessInQueue);
				indexOfCpuUsingProcessInQueue=-1;
				isUsingCpu=0;
				}
				else if(timeQuantumUsage==timeQuantum1){
					if(queue1.size()+queue2.size()+queue3.size()!=1){
						queue2.push_back(queue1[indexOfCpuUsingProcessInQueue]);
						queue1.erase(queue1.begin()+indexOfCpuUsingProcessInQueue);
					}
					indexOfCpuUsingProcessInQueue=-1;
					isUsingCpu=0;
				}
			}
			else if(isUsingCpu==2){
				if(queue2[indexOfCpuUsingProcessInQueue].cpuUsage==queue2[indexOfCpuUsingProcessInQueue].ST){
					queue2[indexOfCpuUsingProcessInQueue].FT=time;
					queue2[indexOfCpuUsingProcessInQueue].TAT=time-queue2[indexOfCpuUsingProcessInQueue].AT;
					queue2[indexOfCpuUsingProcessInQueue].isDone=1;
				for(int i=0;i<processesVector.size();i++){
					if(queue2[indexOfCpuUsingProcessInQueue].processID==processesVector[i].processID){
						processesVector[i]=queue2[indexOfCpuUsingProcessInQueue];
						break;
					}
				}
				queue2.erase(queue2.begin()+indexOfCpuUsingProcessInQueue);
				indexOfCpuUsingProcessInQueue=-1;
				isUsingCpu=0;
				}
				else if(timeQuantumUsage==timeQuantum2){
					if(queue1.size()+queue2.size()+queue3.size()!=1){
						queue3.push_back(queue2[indexOfCpuUsingProcessInQueue]);
						queue2.erase(queue2.begin()+indexOfCpuUsingProcessInQueue);
					}
					indexOfCpuUsingProcessInQueue=-1;
					isUsingCpu=0;
				}
			}
			else{
				if(queue3[indexOfCpuUsingProcessInQueue].cpuUsage==queue3[indexOfCpuUsingProcessInQueue].ST){
					queue3[indexOfCpuUsingProcessInQueue].FT=time;
					queue3[indexOfCpuUsingProcessInQueue].TAT=time-queue3[indexOfCpuUsingProcessInQueue].AT;
					queue3[indexOfCpuUsingProcessInQueue].isDone=1;
				for(int i=0;i<processesVector.size();i++){
					if(queue3[indexOfCpuUsingProcessInQueue].processID==processesVector[i].processID){
						processesVector[i]=queue3[indexOfCpuUsingProcessInQueue];
						break;
					}
				}
				queue3.erase(queue3.begin()+indexOfCpuUsingProcessInQueue);
				indexOfCpuUsingProcessInQueue=-1;
				isUsingCpu=0;
				}
				else if(timeQuantumUsage==timeQuantum3){
					indexOfCpuUsingProcessInQueue=-1;
					isUsingCpu=0;
				}
			}
		}
		if(isUsingCpu==0){
			if(queue1.size()!=0){
				indexOfCpuUsingProcessInQueue=getNextProcess(queue1,"FCFS");
				if(queue1[indexOfCpuUsingProcessInQueue].StartTime==0)
 					queue1[indexOfCpuUsingProcessInQueue].StartTime=time;
 				processSchedule.push_back(queue1[indexOfCpuUsingProcessInQueue].processID);
				isUsingCpu=1;
				timeQuantumUsage=0;
			}
			else{
				if(queue2.size()!=0){
					indexOfCpuUsingProcessInQueue=getNextProcess(queue2,"SJF");
					if(queue2[indexOfCpuUsingProcessInQueue].StartTime==0)
 						queue2[indexOfCpuUsingProcessInQueue].StartTime=time;
 					processSchedule.push_back(queue2[indexOfCpuUsingProcessInQueue].processID);
					isUsingCpu=2;
					timeQuantumUsage=0;
				}
				else{
					if(queue3.size()!=0){
						indexOfCpuUsingProcessInQueue=getNextProcess(queue3,"FCFS");
						if(queue3[indexOfCpuUsingProcessInQueue].StartTime==0)
							queue3[indexOfCpuUsingProcessInQueue].StartTime=time;
						processSchedule.push_back(queue3[indexOfCpuUsingProcessInQueue].processID);
						isUsingCpu=3;
						timeQuantumUsage=0;
					}
				}
			}

		}
		if(time<=20){
		if(isUsingCpu==1){
			infile<<time<<"||"<<queue1[indexOfCpuUsingProcessInQueue].processID<<"||";
			string a;
			for(int i=0;i<queue1.size();i++){
				if(i!=indexOfCpuUsingProcessInQueue)
					a=a+queue1[i].processID+" ";
			}
			a=a+"||";
			for(int i=0;i<queue2.size();i++){
					a=a+queue2[i].processID+" ";
			}
			a=a+"||";
			for(int i=0;i<queue3.size();i++){
					a=a+queue3[i].processID+" ";
			}
			infile<<a<<"\n";
		}
		else if(isUsingCpu==2){
			infile<<time<<"||"<<queue2[indexOfCpuUsingProcessInQueue].processID<<"||";
			string a;
			for(int i=0;i<queue1.size();i++){
					a=a+queue1[i].processID+" ";
			}
			a=a+"||";
			for(int i=0;i<queue2.size();i++){
				if(i!=indexOfCpuUsingProcessInQueue)
					a=a+queue2[i].processID+" ";
			}
			a=a+"||";
			for(int i=0;i<queue3.size();i++){
					a=a+queue3[i].processID+" ";
			}
			infile<<a<<"\n";
		}
		else if(isUsingCpu==3){
			infile<<time<<"||"<<queue3[indexOfCpuUsingProcessInQueue].processID<<"||";
			string a;
			for(int i=0;i<queue1.size();i++){
					a=a+queue1[i].processID+" ";
			}
			a=a+"||";
			for(int i=0;i<queue2.size();i++){
					a=a+queue2[i].processID+" ";
			}
			a=a+"||";
			for(int i=0;i<queue3.size();i++){
				if(i!=indexOfCpuUsingProcessInQueue)
					a=a+queue3[i].processID+" ";
			}
			infile<<a<<"\n";
		}
		}
	}
	printOutput(processesVector,"MLFQ",processSchedule,isPriorityReversed);	
	infile.close();
}
//main function
int main(int argc,char *argv[]){
	//vector to store process objects
	vector<process> inputProcessesVector;
	string filename;
	cout<<"Enter input file name:\n";
	cin>>filename;
	ifstream infile; 
    infile.open(filename);
    int tieBreaker=0,timeQuantum=3,numOfQs=3,schPolicies[numOfQs],flag=0,isPriorityReversed=0;
    for(int i=0;i<numOfQs;i++)
    	schPolicies[i]=0;
    //dummy variables used for additional parameters checking
    string e,l,t,g;
    int r;
    //reading and storing input files
    //additional parameters checking
    infile>>e;
    	if(e!="/*"){
    		if(e=="q"){
    			infile>>timeQuantum>>t;
    		}
    		else if(e=="ql"){
    			infile>>numOfQs;
    			infile>>e;
    			if(e!="/*"){
    				schPolicies[0]=int(e[0])-48;
    				for(int i=1;i<numOfQs;i++){
    					infile>>r;
    					schPolicies[i]=r;
    				}
    				infile>>t;
    			}
    		}
    		else if(e=="tc"){
    			infile>>tieBreaker>>t;
    		}
    		else if(e=="pl"){
    			infile>>l;
    			if(l!="/*")
    			{
    				isPriorityReversed=1;
    				infile>>t>>g>>e;
    			}
    		}
    	}
    while(!infile.eof()){
    	process p;
    	//dummy variables to help us store input into process objects
    	string a;
    	int b,c,d;
    	infile>>a>>b>>c>>d;
    	p.processID=a;
    	p.AT=b;
    	p.ST=c;
    	if(isPriorityReversed==1)
    		p.Pr=-d;
    	else if(isPriorityReversed==0)
    		p.Pr=d;
    	//storing process object into vector of process objects
    	inputProcessesVector.push_back(p);	
    	infile>>a;
    	}
    //calling appropiate functions
	FCFS(inputProcessesVector,tieBreaker,isPriorityReversed);
	SJF(inputProcessesVector,tieBreaker,isPriorityReversed);
	Priority(inputProcessesVector,tieBreaker,isPriorityReversed);
	HRRN(inputProcessesVector,tieBreaker,isPriorityReversed);
	SRTN(inputProcessesVector,tieBreaker,isPriorityReversed);
	RR(inputProcessesVector,timeQuantum,isPriorityReversed);
	MLQ(inputProcessesVector,numOfQs,schPolicies,isPriorityReversed);
	MLFQ(inputProcessesVector,numOfQs,schPolicies,isPriorityReversed);
	//calling function to compare sch policies
	compareSchPolicies();
	infile.close();
	return 0;
}