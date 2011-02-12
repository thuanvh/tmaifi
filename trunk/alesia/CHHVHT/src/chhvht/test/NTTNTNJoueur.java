/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package chhvht.test;
import nttntnv01.*;
/**
 *
 * @author thuanvh
 */
public class NTTNTNJoueur implements chhvht.Joueur {

    Player joueur;

    public NTTNTNJoueur() {
        joueur = new Player();
        joueur.load("test/NTTNTNV01.txt");
    }

    public void NewMatch(int nbParties) {
    }

    public int NextMove(int moneyA, int p, int moneyB) {
        if (moneyA == 0) {
            return 0;
        } else if (moneyB == 0) {
            return 1;
        }
        return joueur.nextMove(moneyA, p/3, p%3, moneyB);
    }

    public String getAuteur() {
        return "NTTNTN";
    }

    public void LoadPlayer(String file) {
    }

    public void SaveMe(String filename) {
    }

    public void endMatch(int r) {
    }
}