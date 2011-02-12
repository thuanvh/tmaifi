/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package chhvht.test;

import alesia.model.Lvminhsdrludovic;

/**
 *
 * @author thuanvh
 */
public class LEVMLSJoueur implements chhvht.Joueur {

    Lvminhsdrludovic joueur;

    public LEVMLSJoueur() {
        joueur = new Lvminhsdrludovic();
        try {
            joueur.loadPlayer("test/LEVMLSV02.txt");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    public void endMatch(int r) {
        joueur.endMatch(r);
    }

    public void NewMatch(int nbParties) {
    }

    public int NextMove(int moneyA, int p, int moneyB) {
        if (moneyA == 0) {
            return 0;
        } else if (moneyB == 0) {
            return 1;
        }
        int a = 0;
        try {
            a = joueur.nextMove(moneyA, p, moneyB);
            if (a > moneyA) {
                a = moneyA;
            }
            if (a < 1) {
                a = 1;
            }
        } catch (Exception e) {
            System.out.println(e.getMessage());
            System.out.println(moneyA + "," + p + "," + moneyB);
        }
        return a;
    }

    public String getAuteur() {
        return "LEVMLS";
    }

    public void LoadPlayer(String file) {
    }

    public void SaveMe(String filename) {
    }
}
