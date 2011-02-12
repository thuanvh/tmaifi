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
        learning = new LEARNING("test/data_1.dat");
        player=new PLAYER("B",MATCH.TYPE_10);
    }
    public void NewMatch(int nbParties) {

    }

    public int NextMove(int moneyA, int p, int moneyB) {
        if (moneyA == 0) {
            return 0;
        } else if (moneyB == 0) {
            return 1;
        }
        int point_P=moneyB;
        int etat_P=p/3+(p%3)*3;
        try{
            return player.next_move(learning, point_P, etat_P);
        }catch(Exception e){
            System.out.println(e.getMessage());
            System.out.println(moneyA+","+p+","+moneyB);
        }
        return 1;
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
