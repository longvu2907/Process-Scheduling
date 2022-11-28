#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

using namespace std;

void readFile(int& nProcess, int& quantum, vector<string>& pList, vector<int>& atList, vector<int>& btList, vector<int>& priList) {
	ifstream iFile("Input.txt");

	iFile >> nProcess;
	iFile >> quantum;

	for (int i = 0; i < nProcess; i++) {
		string strTemp;
		int numTemp;

		iFile >> strTemp;
		pList.push_back(strTemp);

		iFile >> numTemp;
		atList.push_back(numTemp);

		iFile >> numTemp;
		btList.push_back(numTemp);

		iFile >> numTemp;
		priList.push_back(numTemp);
	}
}

void writeRes(int nProcess, vector<string> pList, vector<int> ttList, vector<int> wtList, ofstream& oFile) {
	double avgTT = 0;
	double avgWT = 0;

	//calculate avg wait time
	for (int wt : wtList) {
		avgWT += wt;
	}
	avgWT /= double(wtList.size());

	//calculate avg turnaround time
	for (int tt : ttList) {
		avgTT += tt;
	}
	avgTT /= double(ttList.size());

	for (int i = 0; i < nProcess; i++) {
		oFile << pList[i] << "\t\t" << "TT = " << ttList[i] << "\t" << "WT = " << wtList[i] << endl;
	}
	oFile << fixed << setprecision(2);
	oFile << "Average:\t" << "TT = " << avgTT << "\t" << "WT = " << avgWT << endl;
}

void FCFS(int nProcess, vector<string>pList, vector<int> atList, vector<int> btList) {
	ofstream oFile("FCFS.txt");

	vector<int> dtList; //Done time list
	vector<int> ttList; //Turnaround time list
	vector<int> wtList; //Wait time list
	dtList.resize(nProcess);
	ttList.resize(nProcess);
	wtList.resize(nProcess);

	//calculate done time
	dtList[0] = atList[0] + btList[0];
	for (int i = 1; i < nProcess; i++) {
		dtList[i] = dtList[i - 1] + btList[i];
	}

	//calculate wait time
	wtList[0] = 0;
	for (int i = 1; i < nProcess; i++) {
		wtList[i] = max(dtList[i - 1] - atList[i], 0);
	}

	//calculate turnaround time
	for (int i = 0; i < nProcess; i++) {
		ttList[i] = dtList[i] - atList[i];
	}

	//write to file
	//Scheduling chart
	oFile << "Scheduling chart: ";
	oFile << 0;
	for (int i = 0; i < nProcess; i++) {
		oFile << "~" << pList[i] << "~" << dtList[i];
	}
	oFile << "\n\n";

	writeRes(nProcess, pList, ttList, wtList, oFile);
}

void RR(int nProcess, vector<string> pList, vector<int> atList, vector<int>btList, int quantum) {
	ofstream oFile("RR.txt");

	vector<int> remainBT(btList); //remaing burst time copy from burst time list
	vector<int> dtList; //Done time list
	vector<int> ttList; //Turnaround time list
	vector<int> wtList; //Wait time list
	dtList.resize(nProcess);
	ttList.resize(nProcess);
	wtList.resize(nProcess);

	double avgTT = 0;
	double avgWT = 0;

	oFile << "Scheduling chart: ";
	oFile << 0;

	bool done = false;
	int t = atList[0]; //current time
	int prevP = -1;
	bool firstRun = true;
	bool flagT = false; //flag check is time write at the end
	while (!done) {
		bool flag = true; //flag check is done
		for (int i = 0; i < nProcess; i++) {

			if ((remainBT[i] > 0 && atList[i] < t) || firstRun) {
				if (!firstRun) {
					if (prevP != i) {
						oFile << "~" << t;
						flagT = true;
					}
					else flagT = false;
				}

				flag = false;
				firstRun = false;

				if (remainBT[i] > quantum) {
					t += quantum;
					remainBT[i] -= quantum;
				}
				else {
					t = t + remainBT[i];
					dtList[i] = t;
					wtList[i] = t - btList[i] - atList[i];
					ttList[i] = t - atList[i];
					remainBT[i] = 0;
				}

				//Scheduling chart
				if (prevP != i) {
					oFile << "~" << pList[i];
				}
				prevP = i;
			}
		}
		if (flag) done = true;
	}

	if (!flagT) {
		oFile << "~" << t;
	}
	oFile << "\n\n";

	//write to file
	writeRes(nProcess, pList, ttList, wtList, oFile);
}


void SJF(int nProcess, vector<string> pList, vector<int> atList, vector<int>btList) {
	ofstream oFile("SJF.txt");

	vector<int> remainBT(btList); //remaing burst time copy from burst time list
	vector<int> dtList; //Done time list
	vector<int> ttList; //Turnaround time list
	vector<int> wtList; //Wait time list
	dtList.resize(nProcess);
	ttList.resize(nProcess);
	wtList.resize(nProcess);

	int completed = 0;
	int t = atList[0];
	int minM = INT_MAX;
	int shortest = -1;
	bool check = false;
	int prevP = -1;
	oFile << "Scheduling chart: ";
	while (completed != nProcess) {
		prevP = shortest;
		for (int i = 0; i < nProcess; i++) {
			if ((atList[i] <= t) && (remainBT[i] < minM) && remainBT[i] > 0) {
				minM = remainBT[i];
				shortest = i;
				check = true;
			}
		}
		if (check && prevP != shortest) {
			oFile << t << "~" << pList[shortest] << "~";
		}
		if (!check) {
			t++;
			continue;
		}

		remainBT[shortest]--;
		minM = remainBT[shortest];
		if (minM == 0) {
			minM = INT_MAX;

			if (remainBT[shortest] == 0) {
				completed++;
				check = false;
				dtList[shortest] = t + 1;
				wtList[shortest] = dtList[shortest] - btList[shortest] - atList[shortest];
				ttList[shortest] = dtList[shortest] - atList[shortest];
				if (wtList[shortest] < 0) {
					wtList[shortest] = 0;
				}
			}
		}

		t++;
	}
	oFile << t << "\n\n";

	writeRes(nProcess, pList, ttList, wtList, oFile);
}

void Priority(int nProcess, vector<string> pList, vector<int> atList, vector<int>btList, vector<int> priority) {
	ofstream oFile("Priority.txt");

	vector<int> remainBT(btList); //remaing burst time copy from burst time list
	vector<int> dtList; //Done time list
	vector<int> ttList; //Turnaround time list
	vector<int> wtList; //Wait time list
	dtList.resize(nProcess);
	ttList.resize(nProcess);
	wtList.resize(nProcess);

	int completed = 0;
	int t = atList[0];
	int minP = INT_MAX;
	int topPriority = -1;
	bool check = false;
	int prevP = -1;
	oFile << "Scheduling chart: ";
	while (completed != nProcess) {
		prevP = topPriority;
		for (int i = 0; i < nProcess; i++) {
			if ((atList[i] <= t) && (priority[i] < minP) && remainBT[i] > 0) {
				minP = priority[i];
				topPriority = i;
				check = true;
			}
		}
		if (check && prevP != topPriority) {
			oFile << t << "~" << pList[topPriority] << "~";
		}
		if (!check) {
			t++;
			continue;
		}

		remainBT[topPriority]--;
		if (remainBT[topPriority] == 0) {
			minP = INT_MAX;

			if (remainBT[topPriority] == 0) {
				completed++;
				check = false;
				dtList[topPriority] = t + 1;
				wtList[topPriority] = dtList[topPriority] - btList[topPriority] - atList[topPriority];
				ttList[topPriority] = dtList[topPriority] - atList[topPriority];
				if (wtList[topPriority] < 0) {
					wtList[topPriority] = 0;
				}
			}
		}

		t++;
	}
	oFile << t << "\n\n";

	writeRes(nProcess, pList, ttList, wtList, oFile);
}

void main() {
	int nProcess, quantum;
	vector<string> processList;
	vector<int> arrivalTimeList;
	vector<int> burstTimeList;
	vector<int> priorityList;

	readFile(nProcess, quantum, processList, arrivalTimeList, burstTimeList, priorityList);

	FCFS(nProcess, processList, arrivalTimeList, burstTimeList);
	RR(nProcess, processList, arrivalTimeList, burstTimeList, quantum);
	SJF(nProcess, processList, arrivalTimeList, burstTimeList);
	Priority(nProcess, processList, arrivalTimeList, burstTimeList, priorityList);

}