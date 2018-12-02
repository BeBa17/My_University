/********** Jobshop.java **********/

import java.util.*;
import java.io.*;
import localsolver.*;

public class Jobshop {

    private static class JobshopInstance {

        // Number of jobs
        int nbJobs;

        // Number of machines
        int nbMachines;

        // Number of activities, i.e. nbJobs*nbMachines
        int nbActivities;

        // processing time on each machine for each job
        long[][] processingTime;

        // processing order of machines for each job
        int[][] jobMachineOrder;

        JobshopInstance(String fileName) {
            readInputJobShop(fileName);
            nbActivities = nbJobs * nbMachines;
        }

        // The input files follow the "Taillard" format.
        private void readInputJobShop(String fileName) {
            try (Scanner input = new Scanner(new File(fileName))) {
                input.nextLine();
                nbJobs = input.nextInt();
                nbMachines = input.nextInt();

                if (nbJobs < 1) {
                    System.out.println("Wrong number of jobs");
                    System.exit(1);
                }
                if (nbMachines < 1) {
                    System.out.println("Wrong number of machines");
                    System.exit(1);
                }
                input.nextLine();
                input.nextLine();
                // processing times for each jobs on each activity, index starts at 0
                processingTime = new long[nbJobs][nbMachines];
                for (int j = 0; j < nbJobs; j++) {
                    for (int m = 0; m < nbMachines; m++) {
                        processingTime[j][m] = input.nextInt();
                    }
                }
                input.nextLine();
                input.nextLine();
                // machine order for each job, index starts at 0
                jobMachineOrder = new int[nbJobs][nbMachines];
                for (int j = 0; j < nbJobs; j++) {
                    for (int m = 0; m < nbMachines; m++) {
                        jobMachineOrder[j][m] = input.nextInt() - 1;
                    }
                }

                input.close();

            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }

    }

    private static class Scheduler implements LSNativeFunction {
        // Scheduler allows to obtain a schedule in output from the priority
        // list variables. It is based on the simple J. Giffler and G.L. Thompson
        // algorithm (1960).

        final JobshopInstance instance;

        // To maintain thread-safety property, ThreadLocal is used here.
        // Each thread must have have independant following variables.
        private static ThreadLocal<int[]> jobProgress;
        private static ThreadLocal<long[]> machineTime;
        private static ThreadLocal<long[]> jobProgressTime;
        private static ThreadLocal<int[][]> priorityList;

        public Scheduler(JobshopInstance instance) {
          this.instance = instance;
        }

        public void schedule() {
            for (int k = 0; k < instance.nbActivities; k++) {
                int job = chooseJob();
                scheduleNextActivity(job);
            }
        }

        public double getMakespan() {
            double makespan = machineTime.get()[0];
            for (int m = 1; m < instance.nbMachines; m++) {
                if (makespan < machineTime.get()[m]) {
                    makespan = machineTime.get()[m];
                }
            }
            return makespan;
        }

        @Override
        public double call(LSNativeContext context) {
            initStaticVectors();
            boolean parsingSucess = readFlattenedMatrix(context);
            if (!parsingSucess) return Double.NaN;
            resetStaticVectors();
            schedule();
            return getMakespan();
        }

        // Return the correct scheduled from the final values of the priority list
        // variables. It is only called once at the end if an output file is
        //  mentionned.s
        public void writeSolution(PrintWriter output, int[][] finalPriorityList) {
            priorityList.set(finalPriorityList.clone());
            int[][] solution = new int[instance.nbMachines][instance.nbJobs];
            int[] machineProgress = new int[instance.nbMachines];
            resetStaticVectors();
            for (int k = 0; k < instance.nbActivities; k++) {
                int job = chooseJob();
                int machine = nextMachine(job);
                solution[machine][machineProgress[machine]] = job;
                machineProgress[machine]++;
                scheduleNextActivity(job);
            }
            for (int m = 0; m < instance.nbMachines; m++) {
                for (int j = 0; j < instance.nbJobs; j++) {
                    output.write(solution[m][j] + " ");
                }
                output.write("\n");
            }
        }

        // Get the candidate activity with earliest starting time and
        // schedule it, unless it would delay the job of higher priority
        // whose next machine is this one, in which case we schedule
        // the job with highest priority.
        private int chooseJob() {
            int job = selectJobWithEarliestStartingNextActivity();
            int machine = nextMachine(job);
            long endJob = jobNextActivityEnd(job);
            int nextPriorityJobOnMachine = selectNextPriorityJob(machine);
            if(jobProgressTime.get()[nextPriorityJobOnMachine] < endJob) return nextPriorityJobOnMachine;
            else return job;
        }

        // Return the job whose next activity has the earliest start date
        private int selectJobWithEarliestStartingNextActivity() {
            long earliestStart = Integer.MAX_VALUE;
            int selectedJob = -1;
            for (int job = 0; job < instance.nbJobs; job++) {
                if (!hasNextActivity(job)) continue;
                int machine = nextMachine(job);
                long start;
                if( machineTime.get()[machine] < jobProgressTime.get()[job]) start = jobProgressTime.get()[job];
                else start = machineTime.get()[machine];
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
        private int selectNextPriorityJob(int machine) {
            for (int k = 0; k < instance.nbJobs; k++) {
                int job = priorityList.get()[machine][k];
                if (nextMachine(job) == machine) return job;
            }
            return -1;
        }

        // Schedule the next activity of the given job,
        // and update progression variables accordingly.
        private void scheduleNextActivity(int job) {
            int m = nextMachine(job);
            long end = jobNextActivityEnd(job);
            jobProgress.get()[job]++;
            machineTime.get()[m] = end;
            jobProgressTime.get()[job] = end;
        }

        private boolean hasNextActivity(int job) {
            return jobProgress.get()[job] < instance.nbMachines;
        }

        private long jobNextActivityEnd(int job) {
            int nextActivity = jobProgress.get()[job];
            int machine = instance.jobMachineOrder[job][nextActivity];
            long duration = instance.processingTime[job][nextActivity];
            long start;
            if( machineTime.get()[machine] < jobProgressTime.get()[job]) start = jobProgressTime.get()[job];
            else start = machineTime.get()[machine];
            return start + duration;
        }

        private int nextMachine(int job) {
            int nextActivity = jobProgress.get()[job];
            if (nextActivity == instance.nbMachines) return -1;
            return instance.jobMachineOrder[job][nextActivity];
        }

        // Fill the priorityList matrix by parsing the LSNativeContext object.
        // Return false if the list variables aren't full, what happens
        // when the solver hasn't reached feasibility yet.
        private boolean readFlattenedMatrix(LSNativeContext context) {
            for (int m = 0; m < instance.nbMachines; m++) {
                for (int j = 0; j < instance.nbJobs; j++) {
                    long val = context.getIntValue(m * instance.nbJobs + j);
                    if (val < 0) {
                        return false;
                    }
                    priorityList.get()[m][j] = Math.toIntExact(val);
                }
            }
            return true;
        }

        private void initStaticVectors() {
            if (priorityList==null) {
                priorityList = ThreadLocal.withInitial( () -> new int[instance.nbMachines][instance.nbJobs]);
            }
            if(jobProgress == null){
                jobProgress = ThreadLocal.withInitial( () -> new int[instance.nbJobs]);
            }
            if(jobProgressTime == null){
                jobProgressTime = ThreadLocal.withInitial( () -> new long[instance.nbJobs]);
            }
            if(machineTime == null){
                machineTime = ThreadLocal.withInitial( () -> new long[instance.nbMachines]);
            }

        }

        private void resetStaticVectors() {
            for(int j = 0; j < instance.nbJobs; j++){
                jobProgress.get()[j] = 0;
                jobProgressTime.get()[j] = 0;
            }
            for(int m = 0; m < instance.nbMachines; m++){
                machineTime.get()[m] = 0;
            }
        }

    }

    private static class JobshopProblem {

        // Solver
        private LocalSolver localsolver;

        // instance of the JSSP :
        public JobshopInstance instance;

        // Decision variables
        private LSExpression[] priorityListsVariables;

        // Native function for computing the makespan from the priority lists.
        private LSExpression callMakespanFunc;

        public JobshopProblem(JobshopInstance instance) {
            localsolver = new LocalSolver();
            this.instance = instance;
        }

        void solve(int limit) {

            // Declares the optimization model.
            LSModel model = localsolver.getModel();

            // For each machine the decision variable is a permuation of all jobs
            priorityListsVariables = new LSExpression[instance.nbMachines];
            for (int m = 0; m < instance.nbMachines; m++) {
                priorityListsVariables[m] = model.listVar(instance.nbJobs);
            }

            // All machines must receive each job
            for (int m = 0; m < instance.nbMachines; m++) {
                model.constraint(model.eq(model.count(priorityListsVariables[m]), instance.nbJobs));
            }

            // Creation of a "Scheduler" object, a native funciton
            // for decoding the priority list variables (that is to say computing
            // the makespan from these priority lists).
            Scheduler sch = new Scheduler(instance);
            LSExpression makespanFunc = model.createNativeFunction(sch);
            callMakespanFunc = model.call(makespanFunc);

            // To call the native function, the priorityListsVariables matrix is
            // flatenned in one vector and each element of that vector is added
            // as operand.
            for (int m = 0; m < instance.nbMachines; m++) {
                for (int j = 0; j < instance.nbJobs; j++) {
                    callMakespanFunc.addOperand(model.at(priorityListsVariables[m], j));
                }
            }

            // Objective: minimize the total makespan.
            model.minimize(callMakespanFunc);

            model.close();

            // Parameterizes the solver.
            LSPhase phase = localsolver.createPhase();
            phase.setTimeLimit(limit);

            LSParam param = localsolver.getParam();
            param.setNbThreads(4);

            localsolver.solve();

        }

        // Writes the solution in a file with the following format :
        //  - for each machine, the job sequence
        void writeSolution(String fileName) throws IOException {
            try (PrintWriter output = new PrintWriter(fileName)) {
                System.out.println("Solution written in file " + fileName);

                int[][] finalPriorityList = new int[instance.nbMachines][instance.nbJobs];
                for (int m = 0; m < instance.nbMachines; m++) {
                    LSCollection jobsCollection = priorityListsVariables[m].getCollectionValue();
                    for (int j = 0; j < instance.nbJobs; j++) {
                        finalPriorityList[m][j] = Math.toIntExact(jobsCollection.get(j));
                    }
                }
                Scheduler sch = new Scheduler(instance);
                sch.writeSolution(output, finalPriorityList);

                output.close();
            }
        }

    }

    public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println("Usage: java Jobshop inputFile [outputFile] [timeLimit]");
            System.exit(1);
        }

        String instanceFile = args[0];
        String outputFile = args.length > 1 ? args[1] : null;
        String strTimeLimit = args.length > 2 ? args[2] : "60";


        try {
            JobshopInstance instance = new JobshopInstance(instanceFile);
            JobshopProblem model = new JobshopProblem(instance);
            model.solve(Integer.parseInt(strTimeLimit));

            if (outputFile != null) {
                model.writeSolution(outputFile);
            }
        } catch (Exception ex) {
            System.err.println(ex);
            ex.printStackTrace();
            System.exit(1);
        }


    }

}
