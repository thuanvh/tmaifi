/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package chhvht.test;

import alesia.Player;

/**
 *
 * @author thuanvh
 */
public class KVCDVDJoueur implements chhvht.Joueur {

    Player joueur;

    public KVCDVDJoueur() {
        joueur = new Player();
        joueur.loadPlayer("test/KVCDVDV02.txt");
    }

    public void NewMatch(int nbParties) {
    }

    public int NextMove(int moneyA, int p, int moneyB) {
        if (moneyA == 0) {
            return 0;
        } else if (moneyB == 0) {
            return 1;
        }
        int pay= moneyA-joueur.nextMove(moneyA, p, moneyB);
        if(pay>moneyA){
            return 1;
        }
        return pay;
    }

    public String getAuteur() {
        return "KVCDVD";
    }

    public void LoadPlayer(String file) {
    }

    public void SaveMe(String filename) {
    }

    public void endMatch(int r) {
    }
}
