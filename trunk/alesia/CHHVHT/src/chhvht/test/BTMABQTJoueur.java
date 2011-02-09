/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package chhvht.test;

import btmabqtv01.*;

/**
 *
 * @author thuanvh
 */
public class BTMABQTJoueur implements chhvht.Joueur {

    btmabqtv01.PlayerA joueur;

    public BTMABQTJoueur() {
       joueur = new btmabqtv01.PlayerA();
    }

    public void NewMatch(int nbParties) {
    }

    public int NextMove(int moneyA, int p, int moneyB) {
        if (moneyA == 0) {
            return 0;
        } else if (moneyB == 0) {
            return 1;
        }
        int a=joueur.nextMove(moneyA, p, moneyB);
        return a;
    }

    public String getAuteur() {
        return "BTMABQT";
    }

    public void LoadPlayer(String file) {
    }

    public void SaveMe(String filename) {
    }
    public void endMatch(int r){}
}
