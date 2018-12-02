/********** jobshop.cpp **********/

#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <algorithm>
#include <cinttypes>
#include "localsolver.h"

using namespace localsolver;

class JobshopInstance {
public:
    // Number of jobs
    int nbJobs;

    // Number of machines
    int nbMachines;

    // Number of activities, i.e. nbJobs*nbMachines
    int nbActivities;

    // processing order of machines for each job
    std::vector<std::vector<lsint> > jobMachineOrder;

    // processing time on each machine for each job (given in the processing order)
    std::vector<std::vector<lsint> > processingTime;

    JobshopInstance(const std::string& fileName) {
        readInstance(fileName);
        nbActivities = nbJobs * nbMachines;
    }

private:

    // The input files follow the "Taillard" format.
    void readInstance(const std::string& fileName) {
        std::ifstream infile;
        infile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        infile.open(fileName.c_str());

        infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        infile >> nbJobs;
        infile >> nbMachines;
        infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // processing times for each jobs on each activity, index starts at 0
        infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        processingTime.resize(nbJobs);
        for (int j = 0; j < nbJobs; j++) {
            processingTime[j].resize(nbMachines);
            for (int m = 0; m < nbMachines; m++) {
                infile >> processingTime[j][m];
            }
        }
        // machine order for each job, index starts at 0
        infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        jobMachineOrder.resize(nbJobs);
        for (int j = 0; j < nbJobs; j++) {
            jobMachineOrder[j].resize(nbMachines);
            for (int m = 0; m < nbMachines; m++) {
                int x;
                infile >> x;
                jobMachineOrder[j][m] = x - 1;
            }
        }

        if (processingTime.size() != nbJobs || processingTime[0].size() != nbMachines) {
            std::cerr << "Error in reading input file. Wrong processingTime matrix size." << std::endl;
            exit(1);
        }
        if (jobMachineOrder.size() != nbJobs || jobMachineOrder[0].size() != nbMachines) {
            std::cerr << "Error in reading input file. Wrong jobMachineOrder matrix size." << std::endl;
            exit(1);
        }

        infile.close();
    }

};

class Scheduler : public LSNativeFunction {
    // Scheduler allows to obtain a schedule in output from the priority
    // list variables. It is based on the simple J. Giffler and G.L. Thompson
    // algorithm (1960).

private:
    const JobshopInstance* instance;

    // To maintain thread-safety property, thread_local (since C++11) is used
    // here. Each thread must have have independant following variables.
    static thread_local std::vector<int> jobProgress;
    static thread_local std::vector<lsint> machineProgressTime;
    static thread_local std::vector<lsint> jobProgressTime;
    static thread_local std::vector<std::vector<int>> priorityList;

public:

    Scheduler(const JobshopInstance* instance) : instance(instance) {
    }

    void schedule() {
        for (int k = 0; k < instance->nbActivities; k++) {
            int job = chooseJob();
            scheduleNextActivity(job);
        }
    }

    lsdouble getMakespan() {
        return *std::max_element(machineProgressTime.begin(), machineProgressTime.end());
    }

    lsdouble call(const LSNativeContext& context) {
        initPriorityList();
        bool parsingSucess = readFlattenedMatrix(context);
        if (!parsingSucess) return std::numeric_limits<lsdouble>::quiet_NaN();
        initStaticVectors();
        schedule();
        return getMakespan();
    }

    // Return the schedule from the final values of the priority list
    // variables. It is only called once at the end if an output file is
    //  mentionned.
    void writeSolution(std::ofstream& outfile, std::vector<std::vector<int> >& finalPriorityList) {
        priorityList.swap(finalPriorityList);
        std::vector<std::vector<int> > solution(instance->nbMachines, std::vector<int>(0));
        initStaticVectors();
        for (int k = 0; k < instance->nbActivities; k++) {
            int job = chooseJob();
            int machine = nextMachine(job);
            solution[machine].push_back(job);
            scheduleNextActivity(job);
        }

        for (int m = 0; m < instance->nbMachines; m++) {
            for (int j = 0; j < instance->nbJobs; j++) {
                outfile << solution[m][j] << " ";
            }
            outfile << std::endl;
        }
    }

private:

    // Get the candidate activity with earliest starting time and
    // schedule it, unless it would delay jobs of higher priority
    // whose next machine is this one, in which case we schedule
    // the job with highest priority among these.
    int chooseJob() {
        int job = selectJobWithEarliestStartingNextActivity();
        int machine = nextMachine(job);
        lsint end = jobNextActivityEnd(job);
        return selectNextPriorityJob(machine, end);
    }

    // Return the job whose next activity has the earliest start date
    int selectJobWithEarliestStartingNextActivity() {
        lsint earliestStart = std::numeric_limits<lsint>::max();
        int selectedJob;
        for (int job = 0; job < instance->nbJobs; job++) {
            if (!hasNextActivity(job)) continue;
            int machine = nextMachine(job);
            lsint start = std::max(machineProgressTime[machine], jobProgressTime[job]);
            if (start < earliestStart) {
                earliestStart = start;
                selectedJob = job;
            }
        }
        return selectedJob;
    }


    // Select the job of highest priority among jobs
    // - whose next machine is machine
    // - whose next activity earliest start is before given time
    int selectNextPriorityJob(int machine, lsint time) {
        for (int k = 0; k < instance->nbJobs; k++) {
            int job = priorityList[machine][k];
            if (nextMachine(job) != machine) continue;
            if (jobProgressTime[job] <= time) return job;
        }
        return -1;
    }

    // Schedule the next activity of the given job,
    // and update progression variables accordingly.
    void scheduleNextActivity(int job) {
        int m = nextMachine(job);
        lsint end = jobNextActivityEnd(job);
        jobProgress[job]++;
        machineProgressTime[m] = end;
        jobProgressTime[job] = end;
    }

    bool hasNextActivity(int job) {
        return jobProgress[job] < instance->nbMachines;
    }

    lsint jobNextActivityEnd(int job) {
        int nextActivity = jobProgress[job];
        int machine = instance->jobMachineOrder[job][nextActivity];
        lsint duration = instance->processingTime[job][nextActivity];
        lsint start = std::max(machineProgressTime[machine], jobProgressTime[job]);
        return start + duration;
    }

    int nextMachine(int job) {
        int nextActivity = jobProgress[job];
        if (nextActivity == instance->nbMachines) return -1;
        return instance->jobMachineOrder[job][nextActivity];
    }

    // Fill the priorityList matrix by parsing the LSNativeContext object.
    // Return false if the list variables aren't full, what happens
    // when the solver hasn't reached feasibility yet.
    bool readFlattenedMatrix(const LSNativeContext& context) {
        for (int m = 0; m < instance->nbMachines; m++) {
            for (int j = 0; j < instance->nbJobs; j++) {
                int val = context.getIntValue(m * instance->nbJobs + j);
                if (val < 0) return false;
                priorityList[m][j] = val;
            }
        }
        return true;
    }

     void initPriorityList() {
        if(priorityList.size()==0){
            priorityList.resize(instance->nbMachines);
            for(int m=0; m < instance->nbMachines; m++) {
                priorityList[m].resize(instance->nbJobs);
            }
        }
    }
    void initStaticVectors() {
        jobProgress.clear();
        jobProgress.resize(instance->nbJobs, 0);
        jobProgressTime.clear();
        jobProgressTime.resize(instance->nbJobs, 0);
        machineProgressTime.clear();
        machineProgressTime.resize(instance->nbMachines, 0);
    }

};

class JobshopProblem {
public:
    // LocalSolver
    LocalSolver localsolver;

    // Instance of the JSSP:
    const JobshopInstance* instance;

    // Decision variables
    std::vector<LSExpression> priorityListsVariables;

    // Native function for computing the makespan from the priority lists.
    LSExpression callMakespanFunc;

    // Constructor
    JobshopProblem(const JobshopInstance* jsi) : localsolver() {
        instance = jsi;
    }

    void solve(int limit) {
        // Declares the optimization model.
        LSModel model = localsolver.getModel();

        // For each machine the decision variable is a permuation of all jobs
        priorityListsVariables.resize(instance->nbMachines);
        for (int m = 0; m < instance->nbMachines; m++) {
            priorityListsVariables[m] = model.listVar(instance->nbJobs);
        }

        for (int m = 0; m < instance->nbMachines; m++) {
            model.constraint(model.count(priorityListsVariables[m]) == instance->nbJobs);
        }

        // Creation of a "Scheduler" object, a native funciton
        // for decoding the priority list variables (that is to say computing
        // the makespan from these priority lists).
        Scheduler sch(instance);

        LSExpression makespanFunc = model.createNativeFunction(&sch);
        callMakespanFunc = model.call(makespanFunc);

        // To call the native function, the priorityListsVariables matrix is
        // flatenned in one vector and each element of that vector is added
        // as operand.
        for (int m = 0; m < instance->nbMachines; m++) {
            for (int j = 0; j < instance->nbJobs; j++) {
                callMakespanFunc.addOperand(priorityListsVariables[m][j]);
            }
        }

        // Objective: minimize the total makespan.
        model.minimize(callMakespanFunc);

        model.close();

        // Parameterizes the solver.
        LSPhase phase = localsolver.createPhase();
        phase.setTimeLimit(limit);

        LSParam params = localsolver.getParam();
        params.setNbThreads(4);

        localsolver.solve();


    }

    // Writes the solution in a file with the following format :
    //  - for each machine, the job sequence
    void writeSolution(const std::string& fileName) {
        std::ofstream outfile(fileName.c_str());
        if (!outfile.is_open()) {
            std::cerr << "File " << fileName << " cannot be opened." << std::endl;
            exit(1);
        }
        std::cout << "Solution written in file " << fileName << std::endl;

        std::vector<std::vector<int> > finalPriorityList(instance->nbMachines, std::vector<int>(instance->nbJobs, 0));
        for (int m = 0; m < instance->nbMachines; m++) {
            LSCollection jobsCollection = priorityListsVariables[m].getCollectionValue();
            for (int j = 0; j < instance->nbJobs; j++) {
                finalPriorityList[m][j] = jobsCollection[j];
            }
        }
        Scheduler sch(instance);
        sch.writeSolution(outfile, finalPriorityList);

        outfile.close();
    }

};

thread_local std::vector<int> Scheduler::jobProgress;
thread_local std::vector<lsint> Scheduler::machineProgressTime;
thread_local std::vector<lsint> Scheduler::jobProgressTime;
thread_local std::vector<std::vector<int>> Scheduler::priorityList;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: jobshop inputFile [outputFile] [timeLimit]" << std::endl;
        exit(1);
    }

    const char* instanceFile = argv[1];
    const char* solFile = argc > 2 ? argv[2] : NULL;
    const char* strTimeLimit = argc > 3 ? argv[3] : "60";

    JobshopInstance instance(instanceFile);
    JobshopProblem model(&instance);

    try {
        model.solve(atoi(strTimeLimit));
        if(solFile != NULL) model.writeSolution(solFile);
        return 0;
    } catch (const std::exception& e){
        std::cerr << "Error occurred: " << e.what() << std::endl;
        return 1;
    }
}
