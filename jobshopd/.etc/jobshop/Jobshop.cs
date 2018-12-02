/********** JobShop.cs **********/

using System;
using System.IO;
using System.Threading;
using System.Linq;
using localsolver;

public class JobshopInstance {
    // Number of jobs
    public int nbJobs;

    // Number of machines
    public int nbMachines;

    // Number of activities, i.e. nbJobs*nbMachines
    public int nbActivities;

    // processing order of machines for each job
    public int[,] jobMachineOrder;

    // processing time on each machine for each job (given in the processing order)
    public long[,] processingTime;

    public JobshopInstance(string fileName) {
      ReadInstance(fileName);
      nbActivities = nbJobs * nbMachines;
    }

    // The input files follow the "Taillard" format.
    void ReadInstance(string fileName) {
        using (StreamReader input = new StreamReader(fileName)) {
            input.ReadLine();
            string[] splitted = input.ReadLine().Split(' ');
            nbJobs = int.Parse(splitted[0]);
            nbMachines = int.Parse(splitted[1]);

            input.ReadLine();
            processingTime = new long[nbJobs, nbMachines];
            for (int j = 0; j < nbJobs; j++)
            {
                splitted = input.ReadLine().Trim().Split(' ');
                if (splitted.Length != nbMachines)
                {
                    Console.WriteLine("Error in reading input file. Wrong processingTime matrix size.");
                    System.Environment.Exit(1);
                }
                for (int m = 0; m < nbMachines; m++)
                {
                    processingTime[j, m] = long.Parse(splitted[m]);
                }
            }

            input.ReadLine();
            jobMachineOrder = new int[nbJobs, nbMachines];
            for (int j = 0; j < nbJobs; j++)
            {
                splitted = input.ReadLine().Trim().Split(' ');
                if (splitted.Length != nbMachines)
                {
                    Console.WriteLine("Error in reading input file. Wrong jobMachineOrder matrix size.");
                    System.Environment.Exit(1);
                }
                for (int m = 0; m < nbMachines; m++)
                {
                    jobMachineOrder[j, m] = int.Parse(splitted[m]) - 1;
                }
            }
        }
    }
}

public class Scheduler {
    // Scheduler allows to obtain a schedule in output from the priority
    // list variables. It is based on the simple J. Giffler and G.L. Thompson
    // algorithm (1960).

    JobshopInstance instance;

    // To maintain thread-safety property, ThreadStatic is used
    // here. Each thread must have have independant following variables.
    [ThreadStatic]
    static int[] jobProgress;
    [ThreadStatic]
    static long[] machineProgressTime;
    [ThreadStatic]
    static long[] jobProgressTime;
    [ThreadStatic]
    static int[,] priorityList;

    public Scheduler(JobshopInstance instance) {
      this.instance = instance;
    }

    public void Schedule() {
        for (int k = 0; k < instance.nbActivities; k++) {
            int job = ChooseJob();
            ScheduleNextActivity(job);
        }
    }

    public double GetMakespan() {
        return machineProgressTime.Max();
    }

    public double Call(LSNativeContext context) {
        InitStaticVectors();
        bool parsingSucess = ReadFlattenedMatrix(context);
        if (!parsingSucess) return Double.NaN;
        ResetStaticVectors();
        Schedule();
        return GetMakespan();
    }

    // Return the schedule from the final values of the priority list
    // variables. It is only called once at the end if an output file is
    //  mentionned.
    public void WriteSolution(StreamWriter output, int[,] finalPriorityList) {
        priorityList = finalPriorityList;
        int[,] solution = new int[instance.nbMachines, instance.nbJobs];
        int[] machineProgress = new int[instance.nbMachines];
        ResetStaticVectors();
        for (int k = 0; k < instance.nbActivities; k++) {
            int job = ChooseJob();
            int machine = NextMachine(job);
            solution[machine, machineProgress[machine]] = job;
            machineProgress[machine]++;
            ScheduleNextActivity(job);
        }

        for (int m = 0; m < instance.nbMachines; m++) {
            for (int j = 0; j < instance.nbJobs; j++) {
                output.Write(solution[m, j] + " ");
            }
            output.WriteLine();
        }
    }

    // Get the candidate activity with earliest starting time and
    // schedule it, unless it would delay the job of higher priority
    // whose next machine is this one, in which case we schedule
    // the job with highest priority.
    int ChooseJob() {
        int job = SelectJobWithEarliestStartingNextActivity();
        int machine = NextMachine(job);
        long end = JobNextActivityEnd(job);
        return SelectNextPriorityJob(machine, end);
    }

    // Return the job whose next activity has the earliest start date
    int SelectJobWithEarliestStartingNextActivity() {
        long earliestStart = long.MaxValue;
        int selectedJob = -1;
        for (int job = 0; job < instance.nbJobs; job++) {
            if (!HasNextActivity(job)) continue;
            int machine = NextMachine(job);
            long start = Math.Max(machineProgressTime[machine] , jobProgressTime[job] );
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
    int SelectNextPriorityJob(int machine, long time) {
        for (int k = 0; k < instance.nbJobs; k++) {
            int job = priorityList[machine,k];
            if (NextMachine(job) != machine) continue;
            if (jobProgressTime[job] <= time) return job;
        }
        return -1;
    }

    // Schedule the next activity of the given job,
    // and update progression variables accordingly.
    void ScheduleNextActivity(int job) {
        int m = NextMachine(job);
        long end = JobNextActivityEnd(job);
        jobProgress[job]++;
        machineProgressTime[m] = end;
        jobProgressTime[job] = end;
    }

    bool HasNextActivity(int job) {
        return jobProgress[job] < instance.nbMachines;
    }

    long JobNextActivityEnd(int job) {
        int nextActivity = jobProgress[job];
        int machine = instance.jobMachineOrder[job, nextActivity];
        long duration = instance.processingTime[job, nextActivity];
        long start = Math.Max(machineProgressTime[machine], jobProgressTime[job]);
        return start + duration;
    }

    int NextMachine(int job) {
        int nextActivity = jobProgress[job];
        if (nextActivity == instance.nbMachines) return -1;
        return instance.jobMachineOrder[job, nextActivity];
    }

    // Fill the priorityList matrix by parsing the LSNativeContext object.
    // Return false if the list variables aren't full, what happens
    // when the solver hasn't reached feasibility yet.
    bool ReadFlattenedMatrix(LSNativeContext context) {
        for (int m = 0; m < instance.nbMachines; m++) {
            for (int j = 0; j < instance.nbJobs; j++) {
                int val = (int)context.GetIntValue(m * instance.nbJobs + j);
                if (val < 0) return false;
                priorityList[m, j] = val;
            }
        }
        return true;
    }

    void InitStaticVectors() {
        if (priorityList == null) {
            priorityList = new int[instance.nbMachines, instance.nbJobs];
        }
        if(jobProgress == null){
            jobProgress = new int[instance.nbJobs];
        }
        if(jobProgressTime == null){
            jobProgressTime = new long[instance.nbJobs];
        }
        if(machineProgressTime == null){
            machineProgressTime = new long[instance.nbMachines];
        }

    }

    void ResetStaticVectors() {
        for(int j = 0; j < instance.nbJobs; j++){
            jobProgress[j] = 0;
            jobProgressTime[j] = 0;
        }
        for(int m = 0; m < instance.nbMachines; m++){
            machineProgressTime[m] = 0;
        }
    }

  }

  class JobshopProblem  : IDisposable {

    // Solver.
    LocalSolver localsolver;

    // Instance of the JSSP:
    JobshopInstance instance;

    // LS Program variables.
    LSExpression[] priorityListsVariables;

    // Native function for computing the makespan from the priority lists.
    LSExpression callMakespanFunc;

    // Constructor
    public JobshopProblem (JobshopInstance instance) {
        localsolver = new LocalSolver();
        this.instance = instance;
    }


    public void Dispose ()  {
        localsolver.Dispose();
    }

    void Solve(int limit) {
        /* Declares the optimization model. */
        LSModel model = localsolver.GetModel();

        // For each machine the decision variable is a permuation of all jobs
        priorityListsVariables = new LSExpression[instance.nbMachines];
        for (int m = 0; m < instance.nbMachines; m++) {
            priorityListsVariables[m] = model.List(instance.nbJobs);
        }

        // All machines must receive each job
        for (int m = 0; m < instance.nbMachines; m++) {
            model.Constraint(model.Eq(model.Count(priorityListsVariables[m]), instance.nbJobs));
        }

        // Creation of a "Scheduler" object, a native funciton
        // for decoding the priority list variables (that is to say computing
        // the makespan from these priority lists).
        Scheduler sch = new Scheduler(instance);
        LSNativeFunction schedulerFunction = new LSNativeFunction(sch.Call);
        LSExpression makespanFunc = model.CreateNativeFunction(schedulerFunction);
        callMakespanFunc = model.Call(makespanFunc);

        // To call the native function, the priorityListsVariables matrix is
        // flatenned in one vector and each element of that vector is added
        // as operand.
        for (int m = 0; m < instance.nbMachines; m++) {
            for (int j = 0; j < instance.nbJobs; j++) {
                callMakespanFunc.AddOperand(model.At(priorityListsVariables[m], j));
            }
        }

        // Objective: minimize the total makespan.
        model.Minimize(callMakespanFunc);

        model.Close();

        // Parameterizes the solver.
        LSPhase phase = localsolver.CreatePhase();
        phase.SetTimeLimit(limit);

        LSParam param = localsolver.GetParam();
        param.SetNbThreads(4);

        localsolver.Solve();
    }

    // Writes the solution in a file with the following format :
    //  - for each machine, the job sequence
    void WriteSolution(string fileName) {
        using (StreamWriter output = new StreamWriter(fileName)) {
            int[,] finalPriorityList = new int[instance.nbMachines, instance.nbJobs];
            for (int m = 0; m < instance.nbMachines; m++) {
                LSCollection jobsCollection = priorityListsVariables[m].GetCollectionValue();
                for (int j = 0; j < instance.nbJobs; j++) {
                    finalPriorityList[m, j] = (int)jobsCollection.Get(j);
                }
            }
            Scheduler sch = new Scheduler(instance);
            sch.WriteSolution(output, finalPriorityList);
        }
    }

    public static void Main (string[] args) {
        if (args.Length < 1)  {
            Console.WriteLine ("Usage: JobShop inputFile [outputFile] [timeLimit]");
            System.Environment.Exit (1);
        }

        string instanceFile = args [0];
        string outputFile = args.Length > 1 ? args [1] : null;
        string strTimeLimit = args.Length > 2 ? args [2] : "60";

        JobshopInstance instance = new JobshopInstance(instanceFile);
        using (JobshopProblem model = new JobshopProblem(instance)) {
            model.Solve (int.Parse (strTimeLimit));
            if (outputFile != null)
                model.WriteSolution (outputFile);
        }
    }
}
