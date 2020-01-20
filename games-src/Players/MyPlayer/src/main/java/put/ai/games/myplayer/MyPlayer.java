/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

// TODO: dlaczego wyskakuje 'Oszyst!' ? modyfikowałam funkcję tryToDoAMove. zwraca ona my_move który przychodzi z checkSquere gdzie ma
// na sztywno wpisane 4 argumenty -> posprawdzać to
package put.ai.games.myplayer;

import java.util.List;
import java.util.Random;
import put.ai.games.game.Board;
import put.ai.games.game.Move;
import put.ai.games.game.Player;
import put.ai.games.pentago.impl.PentagoMove;

public class MyPlayer extends Player {

    int N = 6;
    int W = ((N/2) + (((N/2) + 1)/2));
    int D = N/2;
    int S = N - W + 1; // ile jest możliwych ułożeń W-pionków pod rząd w jednej kolumnie/rzędzie
    int boundaryD = D;

    int maxDeng = 0;
    int maxPoss = 0;


    int potential_x = 0;
    int potential_y = 0;
    boolean in_row = false;

    int x_to_put_counter = 5;
    int y_to_put_counter = 5;
    boolean availableField = false;

    private Random random = new Random(0xdeadbeef);

    public void checkRowDeng(int x1, int y1, Board b){
        // sprawdzam wiersz o podanym wierzchołku początkowym (x1, y1) i długości=W
        int number_of_searched_counters = 0;
        for(int x=x1; x<(x1+W); x++){
            if(b.getState(x, y1).equals( getColor() )){
                return;
            }
            if(b.getState(x, y1).equals( opponentsColor() )){
                number_of_searched_counters = number_of_searched_counters + 1;
            }
        }
        if(number_of_searched_counters>=boundaryD && number_of_searched_counters>maxDeng){
            maxDeng = number_of_searched_counters;
            potential_x = x1;
            potential_y = y1;
            in_row = true;
        }
        return;

    }

    public void checkRowPoss(int x1, int y1, Board b){
        // sprawdzam wiersz o podanym wierzchołku początkowym (x1, y1) i długości=W
        int number_of_searched_counters = 0;
        for(int x=x1; x<(x1+W); x++){
            if( b.getState(x, y1).equals( opponentsColor() ) ){
                return;
            }
            if( b.getState(x, y1).equals( getColor() ) ){
                number_of_searched_counters = number_of_searched_counters + 1;
            }

        }

        if(number_of_searched_counters>maxPoss){
            maxPoss = number_of_searched_counters;
            potential_x = x1;
            potential_y = y1;
            in_row = true;
        }
        return;

    }

    public void checkColumnDeng(int x1, int y1, Board b){
        // sprawdzam kolumnę o podanym wierzchołku początkowym (x1, y1) i długości=W
        int number_of_searched_counters = 0;
        for(int y=y1; y<(y1+W); y++){ // x1, y
            if(b.getState(x1, y).equals( getColor() )){
                return;
            }
            if(b.getState(x1, y).equals( opponentsColor() )){
                number_of_searched_counters = number_of_searched_counters + 1;
            }

        }

        if(number_of_searched_counters>=boundaryD && number_of_searched_counters>maxDeng){
            maxDeng = number_of_searched_counters;
            potential_x = x1;
            potential_y = y1;
            in_row = false;
        }

        return;
    }

    public void checkColumnPoss(int x1, int y1, Board b){
        // sprawdzam kolumnę o podanym wierzchołku początkowym (x1, y1) i długości=W
        int number_of_searched_counters = 0;
        for(int y=y1; y<(y1+W); y++){
            if( b.getState(x1, y).equals( opponentsColor() ) ){
                return;
            }
            if( b.getState(x1, y).equals( getColor() ) ){
                number_of_searched_counters = number_of_searched_counters + 1;
            }
        }

        if(number_of_searched_counters>maxPoss){
            maxPoss = number_of_searched_counters;
            potential_x = x1;
            potential_y = y1;
            in_row = false;
        }

        return;
    }

    public void checkForDanger(Board b){
        for(int shift=0; shift<S; shift++){
            for(int r=0; r<N; r++){
                // check Row
                checkRowDeng(shift, r, b);
            }
            for(int c=0; c<N; c++){
                // check Column
                checkColumnDeng(c, shift, b);
            }
        }
        return;
    }

    public Move checkForPossibility(Board b, List<Move> moves){
        for(int shift=0; shift<S; shift++){
            for(int r=0; r<N; r++){
                checkRowPoss(shift, r, b);
            }
            for(int c=0; c<N; c++){
                checkColumnPoss(c, shift, b);
            }
        }
        return freeField(potential_x, potential_y, in_row, b, moves);
    }

    // DOBRZE DZIALA
    public Move tryToDoAMove(PentagoMove my_move, Board b, List<Move> moves){

        for (Move allowed : moves) {
            PentagoMove possibly_move = (PentagoMove) allowed;
            if( my_move.getPlaceX() == possibly_move.getPlaceX() &&  my_move.getPlaceY() == possibly_move.getPlaceY() )
                return possibly_move;
        }
        throw new IllegalArgumentException();
    }

    public Color opponentsColor(){
        Color c1 = Player.Color.PLAYER1;
        Color c2 = Player.Color.PLAYER2;

        Color myC = getColor();

        if(myC == c1)
            return c2;
        else
            return c1;
    }

    public Move freeField(int x1, int y1, boolean in_row, Board b, List<Move> moves){
        if( in_row == true ){
            for( int x=x1; x<(x1 + W); x++){
                if( b.getState(x, y1) == Player.Color.EMPTY ){
                    PentagoMove my_move = new PentagoMove(x, y1, 1, 1, 1, 5, getColor()) ;
                    return tryToDoAMove(my_move, b, moves);
                }
            }
        }
        else{
            for( int y=y1; y<(y1 + W); y++){
                if( b.getState(x1, y) == Player.Color.EMPTY ){
                    PentagoMove my_move = new PentagoMove(x1, y, 1, 1, 1, 5, getColor()) ;
                    return tryToDoAMove(my_move, b, moves);
                }
            }
        }
        return moves.get(3);
    }

    @Override
    public String getName() {
        return "Moj algorytm (Beata)";
    }


    @Override
    public Move nextMove(Board b) {
        N = b.getSize();
        W = ((N/2) + (((N/2) + 1)/2));
        D = N/2;
        S = N - W + 1; // ile jest możliwych ułożeń W-pionków pod rząd w jednej kolumnie/rzędzie
        boundaryD = D;
        List<Move> moves = b.getMovesFor(getColor());
        maxDeng = 0;
        maxPoss = 0;
        checkForDanger( b );
        if( maxDeng > 0)
            return freeField(potential_x, potential_y, in_row, b, moves);
        else
            return checkForPossibility( b , moves);
        //return my_move;
        //return moves.get(random.nextInt(moves.size()));
    }
}


