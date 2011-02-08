package chhvht;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author thuanvh
 */
public interface Joueur {

    public void NewMatch(int nbParties);

    public double NextMove(int moneyA, int p, int moneyB);
//(p vaut 0, 1, 2, ... 8 correspondant aux positions 00 01 02 10 11 12 20 21 22) et player qui possede moneyA
    public String getAuteur();

    public void LoadPlayer(String file);

    public void SaveMe(String filename);
}
