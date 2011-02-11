/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package chhvht.test;
import NDTDTLV02.*;
/**
 *
 * @author thuanvh
 */
public class NDTDTLJoueur implements chhvht.Joueur{
    PLAYER player;
    LEARNING learning;
    public NDTDTLJoueur(){
        learning = new LEARNING("data_1.dat");
        player=new PLAYER("B",MATCH.TYPE_10);
    }
    public void NewMatch(int nbParties) {

    }

    public int NextMove(int moneyA, int p, int moneyB) {
//        int point_P=moneyB;
//        int etat_P=p;//p/3+(p%3)*4;
//        return player.next_move(learning, point_P, etat_P);
        
        return 0;
    }

    public String getAuteur() {
        return "NDTDTL";
    }
    public void endMatch(int r){}

    public void LoadPlayer(String file) {
    }

    public void SaveMe(String filename) {
    }
}
