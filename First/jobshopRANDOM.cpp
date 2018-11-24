#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <vector>
#include <tuple>
#include <string>
#include <limits>
//#include <algorithm>

using namespace std;

bool stop = false;
int czas = 0;
int ukonczonych = 0;
tuple <int,int,int> zeroTuple = make_tuple(0,0,0);

struct Jobs
{
    int jobs, machines;
    
    vector<vector<int>> order, pTime;
    vector<int> keyI;
};

struct Machine
{
    int machines; 

    vector<vector<tuple<int,int,int>>> mJobs; // Machines x Jobs TUPLE: CZAS; KLUCZ; MASZYNA
};

struct Ans
{
    vector<vector<int>> Odp;
    string fileName;
};

void addKey(Jobs &instance);
void createMachines(Jobs *instance, Machine *machine, Ans *ans);
int addJob(Jobs *instance, Machine *machine, int m, Ans *ans);
void addAns(Ans *ans, int nrJob);
void printMachines(Machine &machine);
void printAns(Ans &ans);

bool runTime(Jobs *instance, Machine *machine, Ans *ans, int ileCzasu)
{
    vector<int> zabronione(instance->machines);

    for(int i=0; i<zabronione.size(); i++)
    {
        zabronione[i] = 0;
    }

    for (int i = 0; i < machine->mJobs.size(); i++)
    {
        if ( machine->mJobs[i][0] != zeroTuple && zabronione[i]==0) // jeśli maszyna obsługuje jakieś zadanie i nie jest zabronione
        {
            int temp = get<0>(machine->mJobs[i][0]) - ileCzasu; // czas jobsa na maszynie po odjęciu
            int k = get<1>(machine->mJobs[i][0]); // klucz tego jobsa
            int m = get<2>(machine->mJobs[i][0]); // nr tego jobsa
            machine->mJobs[i][0] = make_tuple(temp,k,m); // aktualnizujemy jobsa
            if ( temp == 0) // jeśli po zaktualizowaniu pozostały czas na maszynie to zero, to uswamy tego jobsa
            {
                machine->mJobs[i].erase(machine->mJobs[i].begin()); // usuwamy z poczatku
                machine->mJobs[i].push_back(zeroTuple); // dodajemy na koniec zeroTupla
                
                if (instance->order[m][0]!=-1) // jeśli ten jobs chce iść jeszcze na jakąś maszynę to wstawiamy go w kolejkę do tej maszyny
                {
                    int nrNowejMaszyny = instance->order[m][0];
                    if(zabronione[nrNowejMaszyny]==0)
                    {
                        zabronione[nrNowejMaszyny]=addJob(instance, machine, m,ans); // wstawiamy go. ale gdyby jego czas to było 0, a maszyna miała numer większy niż aktualna, to nie możemy w tej samej jednostce czasu go obsługiwać
                    }
                    else
                    {
                        addJob(instance, machine, m,ans); // wstawiamy go. ale gdyby jego czas to było 0, a maszyna miała numer większy niż aktualna, to nie możemy w tej samej jednostce czasu go obsługiwać
                    }
                }
                else // jeśli ten jobs nie chce już iść na żadną maszynę
                {
                    ukonczonych++; // to znaczy że ukończył już wszystkie zadania
                    if(ukonczonych == instance->jobs)
                    {
                        return true; // gdy wszystkie ukończyły, kończymy program
                    }
                }
                if(machine->mJobs[i][0]!=zeroTuple) // jeśli po przesunięciu kolejki, na maszynie jest coś obsługiwane (kolejka nie była pusta)
                {
                    int m2 = get<2>(machine->mJobs[i][0]); // numer Jobsa aktualnie na maszynie 
                    addAns(ans, m2); //dodajemy go do odp
                }
            }
        }
    }
    if (ukonczonych == instance->jobs)
    {
        czas++;
        return true;
    }
    return false;
}

void addAns(Ans* ans, int nrJob)
{
    int c = 0;
    while(ans->Odp[nrJob][c]!=-1) // aż znajdziemy pole gdzie czas jeszcze nie był zapisany
    {
        c++;
    }
    if (ans->Odp[nrJob][c]==-1) // pierwsze napotkane wolne od zapisu pole
    {
        ans->Odp[nrJob][c]=czas; // zapisujemy tam czas rozpoczęcia Jobsa
    }
}

void saveAns(Ans* ans)
{
    /*namespace fs = std::experimental::filesystem;
    if (!fs::is_directory("output") || !fs::exists("output"))
    {
        fs::create_directory("output");
    }*/
 
 
    ofstream file;
    string fileName = ans->fileName;
   
    int index = fileName.find_last_of("/\\");
    string fileName2 = fileName.substr(index+1);
    string str = "output/ans_";
    str.append(fileName2);
    cout << str << endl;
    file.open(str.c_str());
    if (!file.good())
    {
        cout<<"ERROR: Output file not opened"<<endl;
        return;
    }
 
    file << czas <<endl;
 
    for (int i = 0; i < ans->Odp.size(); i++)
    {
        for ( int j=0; j<ans->Odp[i].size(); j++)
        {
            file << ans->Odp[i][j]<<"\t";
        }
        file <<endl;
    }
 
    file.close();
   
}

void printInstance(Jobs &instance)
{
    int jobs = instance.jobs;
    int machines = instance.machines;

    vector<vector<int>> order = instance.order;
    vector<vector<int>> pTime = instance.pTime;
    vector<int> keyI = instance.keyI;

    cout << "Jobs: " << jobs << "\nMachines: " << machines << endl;

    cout<<order.size()<<"****"<<order[1].size()<<endl;

    for (int i = 0; i < order.size(); i++)
    {
    	cout << "Job " << i << ": \n" << "Machine/Time: ";
    	for (int j = 0; j < order[i].size(); j++)
    	{
    		cout<< order[i][j] <<"/" << pTime[i][j] <<"\t";
    	}
        cout << "Key: " << keyI[i] << endl;
    }
}

void printMachines(Machine &machine)
{
    vector<vector<tuple<int,int,int>>> mJobs = machine.mJobs;

    for (int i = 0; i < mJobs.size(); i++)
    {
        for (int j = 0; j < mJobs[i].size(); j++)
        {
            cout<< get<0>(mJobs[i][j])<<",";
            cout<< get<1>(mJobs[i][j])<<",";
            cout<< get<2>(mJobs[i][j])<<"  ";
        }
        cout<< endl;
    }
}

void printAns(Ans &ans)
{
    vector<vector<int>> Odp = ans.Odp;

    for (int i = 0; i < Odp.size(); i++)
    {
        for ( int j=0; j<Odp[i].size(); j++)
        {
            cout<< Odp[i][j]<<"\t";
        }
        cout<<endl;
    }
}

int znajdzNajbizszy(Machine *machine) //używane by przeliczyć ile czasu maksymalnie można odjąć (aby nie odejmować po jednej jednostce)
{
    int temp;
    int minT;

    for (int i = 0; i < machine->mJobs.size(); i++)
    {
        if ( machine->mJobs[i][0] != zeroTuple )
        {
            temp = get<0>(machine->mJobs[i][0]);
            if(temp < minT || !minT)
            {
                minT = temp;
            }
        }
    }
    return minT;
}

void initBeasley(string fileName)
{
    ifstream file;
    int jobs, machines;
    int x, y;

    file.open(fileName.c_str());
    if (!file.good())
    {
    	cout << "ERROR: File not opened" << endl;
    	return;
    }

    file >> jobs >> machines;
    
    vector<vector<int>> order(jobs); // for Jobs
    vector<vector<int>> pTime(jobs); // for Jobs
    vector<int> keyI(jobs); // for Jobs
    vector<vector<tuple<int,int,int>>> mJobs(machines); // for Machines
    vector<vector<int>> Odp(jobs);

    for (int i = 0; i < jobs; i++)
    {
    	pTime[i].resize(machines);
        order[i].resize(machines);
        keyI[i] = 0;

        for (int j = 0; j < machines; j++)
    	{
    		file >> order [i][j] >> pTime [i][j];
    	}
    }
    file.close();

    Jobs instance;
    instance.jobs = jobs;
    instance.machines = machines;
    instance.pTime = pTime;
    instance.order = order;
    instance.keyI = keyI;

    Machine machine;
    machine.mJobs = mJobs;
    machine.machines = machines;

    Ans ans;
    ans.Odp = Odp;
    ans.fileName = fileName;

    addKey(instance); // dodajemy klucze
    createMachines(&instance, &machine, &ans); // tworzymy maszyny, tzn początkowe kolejki do nic
    while ( stop == false )
    {
        int ileCzasu = znajdzNajbizszy(&machine);
        czas = czas + ileCzasu;
        stop = runTime(&instance, &machine, &ans, ileCzasu);
    }
    cout<< czas << endl;
    printAns(ans);
    saveAns(&ans);
}

void initTaillard(string fileName)
{
    ifstream file;
    int jobs, machines;
    
    file.open(fileName.c_str());
    if (!file.good())
	{
        cout<<"ERROR: File not opened"<<endl;
        return;
    }
    
    file >> jobs >> machines;
    file.ignore(numeric_limits<streamsize>::max(), '\n');

    file.ignore(numeric_limits<streamsize>::max(), '\n');
    
    vector<vector<int>> pTime(jobs);
    vector<int> keyI(jobs); // for Jobs
    vector<vector<tuple<int,int,int>>> mJobs(machines); // for Machines
    vector<vector<int>> Odp(jobs);

    for (int i = 0; i < jobs; i++)
    {
        pTime[i].resize(machines);
        for (int j = 0; j < machines; j++)
        {
            file >> pTime[i][j];
            keyI[i]=0;
        }
    }

    file.ignore(numeric_limits<streamsize>::max(), '\n');
    file.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<vector<int>> order(jobs);
    for (int i = 0; i < jobs; i++)
    {
        order[i].resize(machines);
        for (int j = 0; j < machines; j++)
        {
            int x;
            file >> x;
            order[i][j] = x - 1;
        }
    }

    Jobs instance;
    instance.jobs = jobs;
    instance.machines = machines;
    instance.pTime = pTime;
    instance.order = order;
    instance.keyI = keyI;

    Machine machine;
    machine.mJobs = mJobs;
    machine.machines = machines;

    Ans ans;
    ans.Odp = Odp;
    ans.fileName = fileName;

    addKey(instance);
    createMachines(&instance, &machine, &ans); // tworzymy maszyny, tzn początkowe kolejki do nich
    while ( stop == false )
    {
        int ileCzasu = znajdzNajbizszy(&machine);
        czas = czas + ileCzasu;
        czas ++;
        stop = runTime(&instance, &machine, &ans, ileCzasu);
    }
    cout<< czas << endl;
    printAns(ans);
    file.close();
    saveAns(&ans);
}

int addJob(Jobs *instance, Machine *machine, int nrM, Ans *ans) // nrM - nrJobsa ktory skonczyl zadanie na jakiejs maszynie
{
   int k = 0, a = 0, c=0;
   int i = nrM;

   int nrMaszyny = instance->order[nrM][0]; // na tą maszynę chce teraz iść Jobs

    if (machine->mJobs[nrMaszyny][k] == zeroTuple) // jesli maszyna jest wolna
	{
        machine->mJobs[nrMaszyny][k] = make_tuple(instance->pTime[i][a],instance->keyI[i],i); //tworzymy tupla na pierwszym miejscu
        instance->pTime[i].erase(instance->pTime[i].begin()); //usuwamy dane zapotrzebowanie
        instance->pTime[i].push_back(0);
        instance->order[i].erase(instance->order[i].begin()); // -||-
        instance->order[i].push_back(-1);
        addAns(ans, nrM); // dodajemy czas do Odp, ponieważ ten Jobs już rusza
        
        return 1;
    }
    else { // jeśli kolejka do potrzebnej maszyny nie była wolne
        int m = k + 1;
        while (machine->mJobs[nrMaszyny][m] != zeroTuple && get<1>(machine->mJobs[nrMaszyny][m]) > instance->keyI[i] ) // klucz jest mniejszy
        { m++; } 
        if (machine->mJobs[nrMaszyny][m] == zeroTuple) // gdy znajdziemy koniec kolejki
	    {
            machine->mJobs[nrMaszyny][m] = make_tuple(instance->pTime[i][a],instance->keyI[i],i); // wstawiamy tam tupla
            instance->pTime[i].erase(instance->pTime[i].begin()); // usuwamy dane zapotrzebowanie
            instance->pTime[i].push_back(0);
            instance->order[i].erase(instance->order[i].begin()); // -||-
            instance->order[i].push_back(-1);
        
        }
        else{ // gdy to nie jest koniec kolejki tylko miejsce według klucza (musimy poprzesuwać jobsy 'za nami')
            tuple<int,int,int> ack = make_tuple(instance->pTime[i][a],instance->keyI[i],i); // jobs który chcemy wstawić
            bool finish = false;
            while (finish == false)
            {
                if (machine->mJobs[nrMaszyny][m+1] == zeroTuple) // jeśli kolejne miejsce jest wolne, tylko swapujemy
                {
                    tuple<int,int,int> temp = machine->mJobs[nrMaszyny][m]; 
                    machine->mJobs[nrMaszyny][m] = ack;
                    machine->mJobs[nrMaszyny][m+1] = temp ;
                    finish = true; // i wychodzimy - dodaliśmy Jobsa do kolejki
                    
                }
                else{
                    tuple<int,int,int> temp = machine->mJobs[nrMaszyny][m]; // temp to to co było na miejscu w które chcemy wstawić nasze zadanie
                    machine->mJobs[nrMaszyny][m] = ack; // wstawiamy w to miejsce nasze zadanie
                    ack = temp; // teraz to zadanie które 'zdjeliśmy' z kolejki jest tym które chcemy gdzieś wstawić
                    m++; // wykonujemy pętlę while, aż nie będziemy w ifie
                    
                }
            }
            // finalnie i tak wstawiliśmy gdzieś to zadanie więc możemy usunąć zapotrzebowanie z kolejki 
            instance->pTime[i].erase(instance->pTime[i].begin());
            instance->pTime[i].push_back(0);
            instance->order[i].erase(instance->order[i].begin());
            instance->order[i].push_back(-1);
        }
    }
    return 0;
}

void addKey(Jobs &instance)
{
    int jobs = instance.jobs;
    int machines = instance.machines;
    int timeSum=0;

    vector<vector<int>> order = instance.order;
    vector<vector<int>> pTime = instance.pTime;
    vector<int> keyI = instance.keyI;

    srand(time(NULL));

    for (int i = 0; i < jobs; i++)
    {
    	keyI[i] = rand();
    }
    instance.keyI = keyI;
}

void createMachines(Jobs *instance, Machine *machine, Ans *ans)
{
    int a = 0;
    
    for (int i=0; i<instance->machines; i++) // ilosc maszyn X ilosc jobsow
    {
        machine->mJobs[i].resize(instance->jobs);
    }

    for (int i=0; i<instance->jobs; i++) // ilosc jobsow X ilosc maszyn
    {
        ans->Odp[i].resize(instance->machines);
    }

    for (int i=0; i<instance->jobs; i++)
    {
        for (int j = 0; j<instance->machines; j++)
        {
            ans->Odp[i][j]=-1; // poczatkowo wszędzie wpisujemy -1, ilosc jobsow X ilosc maszyn
        }
    }

    for (int i = 0; i<instance->jobs; i++) // zczytujemy pierwszą potrzebną maszynę dla każdego jobsa i dodajemy do kolejki
    {
        int k = 0; 
        int nMaszyny = instance->order[i][a];

        while (machine->mJobs[nMaszyny][k] != zeroTuple && get<1>(machine->mJobs[nMaszyny][k]) > instance->keyI[i] ) // klucz jest mniejszy
        { k++; }
        if (machine->mJobs[nMaszyny][k] == zeroTuple) // w pierwszym wolnym miejscu w kolejce
    	{
            machine->mJobs[nMaszyny][k] = make_tuple(instance->pTime[i][a],instance->keyI[i],i); // wstawiamy tupla jobsa
            instance->pTime[i].erase(instance->pTime[i].begin()); // usuwamy dane zapotrzebowanie
            instance->pTime[i].push_back(0);
            instance->order[i].erase(instance->order[i].begin()); // -||-
            instance->order[i].push_back(-1);
        }
        else {
            bool finish = false;
            int m = k;
            tuple<int,int,int> ack = make_tuple(instance->pTime[i][a],instance->keyI[i],i);
            while (finish == false)
            {
            if (machine->mJobs[nMaszyny][m+1] == zeroTuple) // tylko swapujemy
            {
                tuple<int,int,int> temp = machine->mJobs[nMaszyny][m];
                machine->mJobs[nMaszyny][m] = ack;
                machine->mJobs[nMaszyny][m+1] = temp ;
                finish = true;
            }
            else{
                tuple<int,int,int> temp = machine->mJobs[nMaszyny][m];
                machine->mJobs[nMaszyny][m] = ack; 
                ack = temp; 
                m++; // czekamy aż wejdziemy do ifa
            }
            }
            instance->pTime[i].erase(instance->pTime[i].begin()); // usuwamy dane zapotrzebowanie
            instance->pTime[i].push_back(0);
            instance->order[i].erase(instance->order[i].begin()); // -||-
            instance->order[i].push_back(-1);
        }
    } // wczytywanie do kolejek zakonczone

    for (int i = 0; i<instance->machines; i++) // teraz sprawdzamy ktore jobsy maja pierwsze miejsce w kolejce, i dla nich czas rozpoczęcia zadania to 0, co wpisujemy do Odp
    {
        if (machine->mJobs[i][0]!=zeroTuple)
        {int nrJobs = get<2>(machine->mJobs[i][0]); // czytamy tylko pierwsze miejsce (0), każdego z i maszyn
        ans->Odp[nrJobs][0] = 0;}
    }
}

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        printf("Usage: %s [b|t] inputfile\n", argv[0]);
        return 0;
    }
    int format = *argv[1];
    const char* input = argv[2];
    switch (format)
    {
    case 'b':
        initBeasley(input);
        break;
    
    case 't':
        initTaillard(input);
        break;
    
    default:
        cout<<"ERROR: Wrong format flag\n";
        return 0;
        break;
    }
}