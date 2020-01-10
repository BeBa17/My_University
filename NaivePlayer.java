/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
//package put.ai.games.naiveplayer;

import java.util.List;
import java.util.Random;
/*import put.ai.games.game.Board;
import put.ai.games.game.Move;
import put.ai.games.game.Player;*/

int N;
int W = ((N/2) + (((N/2) + 1)/2));
int D = N/2;

public boolean checkForDanger(){
    for(int i=0; i<D; i++){
        for(int r=0; r<N; r++){
            checkRow(0);
        }
        for(int c=0; c<N; c++){
            checkColumn(0);
        }
    }

}

public boolean checkForPossibility(){
    for(int i=0; i<D; i++){
        for(int r=0; r<N; r++){
            checkRow(1);
        }
        for(int c=0; c<N; c++){
            checkColumn(1);
        }
    }
}

public boolean checkRow(int myPositions){
    if(myPositions==0){
        // looking for opponent's counters
    }
    else if(myPosition==1){
        // loking for my counters
    }
}

public boolean checkColumn(int myPositions){
    if(myPositions==0){
        // looking for opponent's counters
    }
    else if(myPosition==1){
        // loking for my counters
    }
}

public int getRequiredNumberOfMyCounters(int number_of_fields, int half_or_all){
    if(half_or_all == 0){
        return number_of_fields/2;
    }
    else{
        return number_of_fields;
    }
}

public void checkSquare(int square, int half_or_all){
    // x1 i x2 - wierzchołkowe wartości dla danego (obramowania) kwadratu
    int x1 = N/2 + square;
    int x2 = N/2 - 1 - square;

    int number_of_my_counters;
    int number_of_opponents_counters;
    int number_of_counters;

    // algorytm: 4 -> 4 + 8 = 12 -> 12 + 8 = 20 -> 20 + 8 = 28 -> 28 + 8 = 36 -> 36 + 8 = 44 ...
    int number_of_fields = 4;
    int required_number_of_my_counters = getRequiredNumberOfMyCounters(number_of_fields, half_or_all);


    for(i=x1; i<=x2; i++){
        for(j=x1; j<=x2; j++){

            // czy tu jest mój pionek
            checkField(i,j,1);
            //czy tu jest pionek przeciwnika
            checkField(i,j,0);
            // w checkFieldsach zliczyć ile jest moich/przeciwnika/w sumie pionków

            if(number_of_my_counters<required_number_of_my_counters && number_of_fields>number_of_counters){
                // gdzieś tutaj mój postaw pionek
            }
            // ...... dokończyć
        }
        number_of_fields = number_of_fields + 8;
        required_number_of_my_counters = getRequiredNumberOfMyCounters(number_of_fields, half_or_all);
    }
}

public void otherStrategy(){
    for(i=0; i<D; i++){
        // będę sprawdzać kwadraty od najbardziej wewnętrznego. ;
        // Będzie ich N/2 = D;
        // Drugi argument - czy dopełniam go do połowy (0) czy do całości (1)
        checkSquare(i, 0);
    }
    for(i=0; i<D; i++){
        checkSquare(i, 1);
    }
}

public void myMove(){
    if(checkForDanger()==false)
    {
        if(checkForPossibility()==false)
        {
            otherStrategy();
        }
    }

}

public class NaivePlayer extends Player {

    private Random random = new Random(0xdeadbeef);


    @Override
    public String getName() {
        return "Gracz Naiwny 84868";
    }


    @Override
    public Move nextMove(Board b) {
        List<Move> moves = b.getMovesFor(getColor());
        return moves.get(random.nextInt(moves.size()));
    }
}
