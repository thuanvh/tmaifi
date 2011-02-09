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

    public int NextMove(int moneyA, int p, int moneyB);

    public String getAuteur();

    public void LoadPlayer(String file);

    public void SaveMe(String filename);

    public void endMatch(int r);
}
