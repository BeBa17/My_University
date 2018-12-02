########## jobshop.py ##########

import localsolver
import sys

if len(sys.argv) < 2:
    print ("Usage: python jobshop.py inputFile [outputFile] [timeLimit]")
    sys.exit(1)


class JobshopInstance :
    def __init__(self, filename):
        self.read_instance(filename)
        self.nb_activities = self.nb_jobs * self.nb_machines

    # The input files follow the "Taillard" format.
    def read_instance(self, filename):
        with open(filename) as f:
            lines = f.readlines()
        first_line = lines[1].split()
        # Number of jobs
        self.nb_jobs = int(first_line[0])
        # Number of machines
        self.nb_machines = int(first_line[1])
        # processing time on each machine for each job (given in the processing order)
        self.processing_time = [[int(lines[i].split()[j]) for j in range(self.nb_machines)] for i in range(3,3+self.nb_jobs)]
        # processing order of machines for each job
        self.job_machine_order = [[int(lines[i].split()[j])-1 for j in range(self.nb_machines)] for i in range(4+self.nb_jobs,4+2*self.nb_jobs)]

class Scheduler :
    """Scheduler allows to obtain a schedule in output from the priority
        list variables. It is based on the simple J. Giffler and G.L. Thompson
        algorithm (1960)."""
    def __init__(self, instance):
        self.instance = instance
        self.priority_list = [[0 for j in range(instance.nb_jobs)] for m in range(instance.nb_machines)]
        self.job_progress = [0 for j in range(instance.nb_jobs)]
        self.job_progress_time = [0 for j in range(instance.nb_jobs)]
        self.machine_progress_time = [0 for j in range(instance.nb_machines)]

    def schedule(self):
        for k in range(self.instance.nb_activities):
            job = self.choose_job()
            self.schedule_next_activity(job)

    def get_make_span(self):
        return max(self.machine_progress_time)

    def call(self, context):
        parsingSucces = self.read_flattened_matrix(context)
        if not parsingSucces:
            return float("nan")
        self.init_lists_attibutes()
        self.schedule()
        return self.get_make_span()

    # Get the candidate activity with earliest starting time and
    # schedule it, unless it would delay jobs of higher priority
    # whose next machine is this one, in which case we schedule
    # the job with highest priority among these.
    def choose_job(self):
        job = self.select_job_with_earliest_starting_next_activity()
        machine = self.next_machine(job)
        end = self.job_next_activity_end(job)
        return self.select_next_priority_job(machine, end)

    # Return the job whose next activity has the earliest start date
    def select_job_with_earliest_starting_next_activity(self):
        earliest_start = float("inf")
        selected_job = -1
        for job in range(self.instance.nb_jobs):
            if not self.has_next_activity(job):
                continue
            machine = self.next_machine(job)
            start = max(self.machine_progress_time[machine], self.job_progress_time[job])
            if start < earliest_start:
                earliest_start = start
                selected_job = job
        return selected_job

    # Select the job of highest priority among jobs
    # - whose next machine is machine
    # - whose next activity earliest start is before given time
    def select_next_priority_job(self, machine, time):
        for k in range(self.instance.nb_jobs):
            job = self.priority_list[machine][k]
            if self.next_machine(job)!=machine :
                continue
            if self.job_progress_time[job] <= time :
                return job

    # Schedule the next activity of the given job,
    # and update progression variables accordingly.
    def schedule_next_activity(self, job):
        m = self.next_machine(job)
        end = self.job_next_activity_end(job)
        self.job_progress[job] += 1
        self.machine_progress_time[m] = end
        self.job_progress_time[job] = end

    def has_next_activity(self, job):
        return self.job_progress[job] < self.instance.nb_machines

    def job_next_activity_end(self, job):
        next_activity = self.job_progress[job]
        machine = instance.job_machine_order[job][next_activity]
        duration = instance.processing_time[job][next_activity]
        start = max(self.machine_progress_time[machine], self.job_progress_time[job])
        return start + duration

    def next_machine(self, job):
        next_activity = self.job_progress[job]
        if next_activity == instance.nb_machines:
            return -1
        return instance.job_machine_order[job][next_activity]

    # Fill the priorityList matrix by parsing the LSNativeContext object.
    # Return false if the list variables aren't full, what happens
    # when the solver hasn't reached feasibility yet.
    def read_flattened_matrix(self, context):
        for m in range(instance.nb_machines):
            for j in range(instance.nb_jobs):
                val = context[m*instance.nb_jobs + j]
                if val < 0:
                    return False
                self.priority_list[m][j] = val
        return True

    def init_lists_attibutes(self):
                self.job_progress = [0 for j in range(instance.nb_jobs)]
                self.job_progress_time = [0 for j in range(instance.nb_jobs)]
                self.machine_progress_time = [0 for j in range(instance.nb_machines)]

    # Return the schedule from the final values of the priority list
    # variables. It is only called once at the end if an output file is
    #  mentionned.
    def write_solution(self, f, final_priority_list):
        self.priority_list = final_priority_list
        solution = [[] for m in range(self.instance.nb_machines)]
        self.init_lists_attibutes()
        for k in range(self.instance.nb_activities):
            job = self.choose_job()
            machine = self.next_machine(job)
            solution[machine].append(job)
            self.schedule_next_activity(job)
        solution_string = str(solution).replace("]","\n")
        solution_string = filter(lambda ch: ch not in "[,]", solution_string)
        f.write(solution_string)


with localsolver.LocalSolver() as ls:

    instance = JobshopInstance(sys.argv[1])

    #
    # Declares the optimization model
    #
    model = ls.model

    # For each machine the decision variable is a permuation of all jobs
    priority_lists_variables = [model.list(instance.nb_jobs) for m in range(instance.nb_machines)]

    # All activities must be processed
    for m in range(instance.nb_machines):
        model.constraint(model.eq(model.count(priority_lists_variables[m]), instance.nb_jobs))

    # Creation of a "Scheduler" object, a native funciton
    # for decoding the priority list variables (that is to say computing
    # the makespan from these priority lists).
    sch = Scheduler(instance)

    # Minimize the makespan: end of the last job on the last machine
    makespan_func = model.create_native_function(sch.call);
    call_makespan_func = model.call(makespan_func)

    # To call the native function, the priorityListsVariables matrix is
    # flatenned in one vector and each element of that vector is added
    # as operand.
    for m in range(instance.nb_machines):
        for t in range(instance.nb_jobs):
            call_makespan_func.add_operand(priority_lists_variables[m][t])

    model.minimize(call_makespan_func)

    model.close()

    #
    # Parameterizes the solver
    #
    if len(sys.argv) >= 4: ls.create_phase().time_limit = int(sys.argv[3])
    else: ls.create_phase().time_limit = 60
    ls.param.set_nb_threads(1)

    ls.solve()

    #
    # Writes the solution in a file
    #
    if len(sys.argv) >= 3:
        final_priority_list = [list(priority_lists_variables[m].value) for m in range(instance.nb_machines)]
        with open(sys.argv[2],"w") as f:
            sch.write_solution(f, final_priority_list)
