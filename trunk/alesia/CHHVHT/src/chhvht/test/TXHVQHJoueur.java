/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package chhvht.test;

import txhvqhv01.*;

/**
 *
 * @author thuanvh
 */
public class TXHVQHJoueur implements chhvht.Joueur {

    txhvqhv01.TXHVQHJoueur player;

    public TXHVQHJoueur() {
        player = new txhvqhv01.TXHVQHJoueur();
        player.LoadPlayer("test/TXHVQHV01.txt");
    }

    public void NewMatch(int nbParties) {
    }

    public int NextMove(int moneyA, int p, int moneyB) {
        if (moneyA == 0) {
            return 0;
        } else if (moneyB == 0) {
            return 1;
        }
        Node nodeCurrent = new Node();
        // 5- Votre prix exist, 6 - Mon prix exist, 1 - Votre status, 0 - Ma status, status = 3 --> gagne
        nodeCurrent.setValueNode(moneyB, moneyA, p%3, p/3);
        int choice = player.ChoiceBestPrix(nodeCurrent);
        return choice;
    }

    public String getAuteur() {
        return "TXHVQH";
    }

    public void endMatch(int r) {
    }

    public void LoadPlayer(String file) {
    }

    public void SaveMe(String filename) {
    }
}
