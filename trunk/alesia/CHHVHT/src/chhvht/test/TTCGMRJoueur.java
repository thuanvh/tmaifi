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
        joueur= (JoueurExperience)new Joueur.Interface().loadPlayer(new JoueurExperience());
    }
    public void NewMatch(int nbParties) {
    }

    public int NextMove(int moneyA, int p, int moneyB) {
        return joueur.NextMove(moneyA, p, moneyB);
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
