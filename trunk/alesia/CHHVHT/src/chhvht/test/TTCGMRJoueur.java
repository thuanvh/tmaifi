/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package chhvht.test;
import Joueur.JoueurExperience;
/**
 *
 * @author thuanvh
 */
public class TTCGMRJoueur implements chhvht.Joueur{
    JoueurExperience joueur;
    public TTCGMRJoueur(){
        joueur= new JoueurExperience();//)new Joueur.Interface().loadPlayer(new JoueurExperience());
        joueur.loadPlayer("test/MRTTCGV02.txt");
    }
    public void NewMatch(int nbParties) {
    }

    public int NextMove(int moneyA, int p, int moneyB) {
        if (moneyA == 0) {
            return 0;
        } else if (moneyB == 0) {
            return 1;
        }
        return joueur.nextMove(moneyA, p, moneyB);
    }

    public String getAuteur() {
        return "TTCGMR";
    }

    public void LoadPlayer(String file) {
    }

    public void SaveMe(String filename) {
    }
    public void endMatch(int r){}
}
