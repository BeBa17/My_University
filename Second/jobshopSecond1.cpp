#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
//#include <algorithm>

#define SEK 60

using namespace std;

bool stop = false;
int czas = 0;
int ukonczonych = 0;
tuple <int,int,int> zeroTuple = make_tuple(0,0,0);
clock_t startC,stopC;
double duration;
bool first = true;
float obszar = 0.8;
float minut = 0.25;

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
    vector<int> dlKolejki;
    vector<tuple<int,int,int>> obslugiwane;
};

struct Ans
{
    vector<vector<int>> Odp;
    int czasOdp;
    vector<vector<int>> Best;
    int czasBest;
    string fileName;
};
Ans ans;

void addKey(Jobs &instance);
void createMachines(Jobs *instance, Machine *machine, Ans *ans);
void addJobToQueue(Jobs *instance, Machine *machine, int m);
void addJobToMachine(Jobs *instance, Machine *machine, Ans *ans);
void addAns(Ans *ans, int nrJob);
void addTime(Ans *ans);
void compareResults(Ans *ans);
void printMachines(Machine &machine);
void printAns(Ans &ans);

bool runTime(Jobs *instance, Machine *machine, Ans *ans, int ileCzasu)
{
    for (int i = 0; i < machine->obslugiwane.size(); i++)
    {
        //cout<<get<0>(machine->obslugiwane[i]);
        if ( machine->obslugiwane[i] != zeroTuple ) // jeśli maszyna obsługuje jakieś zadanie i nie jest zabronione
        {
            //cout<<i<<" ";
            int temp = get<0>(machine->obslugiwane[i]) - ileCzasu; // czas jobsa na maszynie po odjęciu
            int k = get<1>(machine->obslugiwane[i]); // klucz tego jobsa
            int m = get<2>(machine->obslugiwane[i]); // nr tego jobsa
            machine->obslugiwane[i] = make_tuple(temp,k,m); // aktualnizujemy jobsa
            if ( temp == 0) // jeśli po zaktualizowaniu pozostały czas na maszynie to zero, to uswamy tego jobsa
            {
                machine->obslugiwane[i] = zeroTuple;

                addKey(*instance); // upewnić się czy tutaj
                
                if(instance->order[m][0]!=-1 && instance->pTime[m][0]==0)
                {
                    addAns(ans, m);
                    instance->pTime[m].erase(instance->pTime[m].begin()); //usuwamy dane zapotrzebowanie
                    instance->pTime[m].push_back(0);
                    instance->order[m].erase(instance->order[m].begin()); // -||-
                    instance->order[m].push_back(-1);
                }

                if (instance->order[m][0]!=-1) // jeśli ten jobs chce iść jeszcze na jakąś maszynę to wstawiamy go w kolejkę do tej maszyny
                {
                    //int nrNowejMaszyny = instance->order[m][0];
                    addJobToQueue(instance, machine, m); // wstawiamy go. ale gdyby jego czas to było 0, a maszyna miała numer większy niż aktualna, to nie możemy w tej samej jednostce czasu go obsługiwać
                    
                }
                else // jeśli ten jobs nie chce już iść na żadną maszynę
                {
                    ukonczonych++; // to znaczy że ukończył już wszystkie zadania
                    //cout<<"czas: "<<czas<<" ukonczonych= "<<ukonczonych<<endl;
                    if(ukonczonych == instance->jobs)
                    {
                        return true; // gdy wszystkie ukończyły, kończymy program
                    }
                }
            }
        }
        //addJobToMachine(instance, machine, ans);
    }
    if (ukonczonych == instance->jobs)
    {
        czas++;
        return true;
    }
    return false;
}

void addAns(Ans* ans, int nrJob) //Chyba dobrze
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

void addTime(Ans *ans)
{
    ans->czasOdp = czas;
}

void compareResults(Ans *ans)
{
    if(ans->czasOdp<ans->czasBest || ans->czasBest==-1)
    {
        ans->czasBest = ans->czasOdp;
        for (int i = 0; i < ans->Odp.size(); i++)
        {
            for ( int j=0; j<ans->Odp[i].size(); j++)
            {
                ans->Best[i][j] = ans->Odp[i][j];
            }
        }
    }
}

void saveAns(Ans* ans) // Chyba dobrze
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
 
    file << ans->czasBest <<endl;
 
    for (int i = 0; i < ans->Best.size(); i++)
    {
        for ( int j=0; j<ans->Best[i].size(); j++)
        {
            file << ans->Best[i][j]<<"\t";
        }
        file <<endl;
    }
 
    file.close();
   
}

void printInstance(Jobs &instance) // Chyba dobrze
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

void printMachines(Machine &machine) // Chyba dobrze
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

void printAns(Ans &ans) // Chyba dobrze
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

void printBestAns(Ans &ans) // Chyba dobrze
{
    vector<vector<int>> Best = ans.Best;

    cout<<ans.czasBest<<endl;

    for (int i = 0; i < Best.size(); i++)
    {
        for ( int j=0; j<Best[i].size(); j++)
        {
            cout<< Best[i][j]<<"\t";
        }
        cout<<endl;
    }
}

int znajdzNajbizszy(Machine *machine) //używane by przeliczyć ile czasu maksymalnie można odjąć (aby nie odejmować po jednej jednostce)
{
    int temp;
    int minT;

    for (int i = 0; i < machine->obslugiwane.size(); i++)
    {
        if ( machine->obslugiwane[i] != zeroTuple )
        {
            temp = get<0>(machine->obslugiwane[i]);
            if(temp < minT || !minT)
            {
                minT = temp;
                if (minT==0)
                {
                    return minT;
                }
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
    vector<int> dlKolejki(machines); 
    vector<tuple<int,int,int>> obslugiwane(machines);
    vector<int> keyI(jobs); // for Jobs
    vector<vector<tuple<int,int,int>>> mJobs(machines); // for Machines
    vector<vector<int>> Odp(jobs);
    vector<vector<int>> Best(jobs);

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
    machine.dlKolejki = dlKolejki;
    machine.obslugiwane = obslugiwane;

    ans.Odp = Odp;
    ans.fileName = fileName;

    if (first==true)
    {
        ans.Best = Best;
    }

    addKey(instance); // dodajemy klucze
    createMachines(&instance, &machine, &ans); // tworzymy maszyny, tzn początkowe kolejki do nic
    //printMachines(machine);
    addJobToMachine(&instance,&machine,&ans);
    //printMachines(machine);
    //printAns(ans);
    while ( stop == false )
    {
        
        int ileCzasu = znajdzNajbizszy(&machine);
        czas = czas + ileCzasu;
        stop = runTime(&instance, &machine, &ans, ileCzasu);
        addJobToMachine(&instance,&machine,&ans);
        //printMachines(machine);
    }
    addTime(&ans);
    //cout<< czas << endl;
    //printAns(ans);
    //saveAns(&ans);
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
    vector<int> dlKolejki(machines); 
    vector<tuple<int,int,int>> obslugiwane(machines);
    vector<vector<int>> Best(jobs);

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
    machine.dlKolejki = dlKolejki;
    machine.obslugiwane = obslugiwane;

    ans.Odp = Odp;
    ans.fileName = fileName;

    if (first==true)
    {
        ans.Best = Best;
    }

    addKey(instance); // dodajemy klucze
    createMachines(&instance, &machine, &ans); // tworzymy maszyny, tzn początkowe kolejki do nic
    //printMachines(machine);
    addJobToMachine(&instance,&machine,&ans);
    //printMachines(machine);
    //printAns(ans);
    while ( stop == false )
    {
        
        int ileCzasu = znajdzNajbizszy(&machine);
        czas = czas + ileCzasu;
        stop = runTime(&instance, &machine, &ans, ileCzasu);
        addJobToMachine(&instance,&machine,&ans);
        //printMachines(machine);
    }
    addTime(&ans);
    //cout<< czas << endl;
    //printAns(ans);
    //saveAns(&ans);
}

void addJobToQueue(Jobs *instance, Machine *machine, int nrJ) // nrJ - nrJobsa ktory skonczyl zadanie na jakiejs maszynie
{
   int k = 0, a = 0, c=0;
   int i = nrJ;

   int nrMaszyny = instance->order[nrJ][0]; // na tą maszynę chce teraz iść Jobs

    if (machine->mJobs[nrMaszyny][k] == zeroTuple) // jesli maszyna jest wolna
	{
        machine->mJobs[nrMaszyny][k] = make_tuple(instance->pTime[i][a],instance->keyI[i],i); //tworzymy tupla na pierwszym miejscu
        machine->dlKolejki[nrMaszyny]++;
        instance->pTime[i].erase(instance->pTime[i].begin()); //usuwamy dane zapotrzebowanie
        instance->pTime[i].push_back(0);
        instance->order[i].erase(instance->order[i].begin()); // -||-
        instance->order[i].push_back(-1);
        
        return ;
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
        machine->dlKolejki[nrMaszyny]++;
    }
    return ;
}

void addJobToMachine(Jobs *instance, Machine *machine, Ans *ans)
{
    int wybrany;
    for (int i = 0; i<machine->obslugiwane.size(); i++)
    {
        if(machine->obslugiwane[i]==zeroTuple && machine->mJobs[i][0]!=zeroTuple)
        {
            int zakres = (machine->dlKolejki[i])*obszar ;
            //cout<<"zakres: "<<zakres<<" ";
            if (zakres==0)
            {
                wybrany = 0;
            }
            else{
                wybrany = rand()%zakres;
            }
            //cout<<wybrany<<" "<<zakres<<endl;
            //cout<<get<2>(machine->mJobs[i][wybrany])<<endl;
            //cout<<wybrany<<endl;
            int nrJob = get<2>(machine->mJobs[i][wybrany]);
            machine->obslugiwane[i] = make_tuple(get<0>(machine->mJobs[i][wybrany]),get<1>(machine->mJobs[i][wybrany]),nrJob);
            //cout<<get<0>(machine->obslugiwane[i])<<" "<<get<1>(machine->obslugiwane[i])<<" "<<get<2>(machine->obslugiwane[i])<<endl;
            machine->mJobs[i].erase(machine->mJobs[i].begin()+wybrany);
            machine->mJobs[i].push_back(zeroTuple);
            machine->dlKolejki[i]--;
            addAns(ans, nrJob);
        }
        //printMachines(*machine);
    }
}

void addKey(Jobs &instance)
{
    int jobs = instance.jobs;
    int machines = instance.machines;
    int timeSum=0;

    vector<vector<int>> order = instance.order;
    vector<vector<int>> pTime = instance.pTime;
    vector<int> keyI = instance.keyI;

    for (int i = 0; i < jobs; i++)
    {
    	for (int j = 0; j < machines; j++)
    	{
            timeSum += pTime[i][j];
    	}
        keyI[i] = timeSum;
        timeSum = 0;
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

    if (first==true)
    {
        for (int i=0; i<instance->jobs; i++) // ilosc jobsow X ilosc maszyn
        {
            ans->Best[i].resize(instance->machines);
        }

        for (int i=0; i<instance->jobs; i++)
        {
            for (int j = 0; j<instance->machines; j++)
            {
                ans->Best[i][j]=-1; // poczatkowo wszędzie wpisujemy -1, ilosc jobsow X ilosc maszyn
            }
        }
    }

    for (int i = 0; i < instance->machines ; i++)
    {
        machine->dlKolejki[i] = 0;
    }

    for (int i=0; i<instance->machines; i++)
    {
        machine->obslugiwane[i] = zeroTuple;
    }

    for (int i = 0; i<instance->jobs; i++) // zczytujemy pierwszą potrzebną maszynę dla każdego jobsa i dodajemy do kolejki
    {
        addJobToQueue(instance,machine,i);
    }
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    startC=clock();
    ans.czasBest = -1;
    //cout<<t<<endl;
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
    while(((duration) / CLOCKS_PER_SEC)<minut*SEK)
    {
        initBeasley(input);
        compareResults(&ans);
        czas = 0;
        stop = false;
        first = false;
        ukonczonych = 0;
        stopC = clock();
        duration =(double)(stopC - startC);
    }
    cout<<"Best:"<<endl;
    printBestAns(ans);
    saveAns(&ans);
        break;
    
    case 't':
    while(((duration) / CLOCKS_PER_SEC)<minut*SEK)
    {
        initTaillard(input);
        compareResults(&ans);
        czas = 0;
        stop = false;
        first = false;
        ukonczonych = 0;
        stopC = clock();
        duration =(double)(stopC - startC);
    }
    cout<<"Best:"<<endl;
    printBestAns(ans);
    saveAns(&ans);
        break;
    
    default:
        cout<<"ERROR: Wrong format flag\n";
        return 0;
        break;
    }
}