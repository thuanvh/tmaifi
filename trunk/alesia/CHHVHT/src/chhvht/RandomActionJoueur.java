/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package chhvht;
import java.util.*;
/**
 *
 * @author thuanvh
 */
public class RandomActionJoueur implements Joueur {

    Random random=new Random();
    public void NewMatch(int nbParties) {
    }

    public int ChooseRandomAction(int currentMoney) {
        if (currentMoney == 0) {
            return 0;
        }
        if (currentMoney == 1) {
            return currentMoney;
        }
        return random.nextInt(currentMoney - 1) + 1;
    }

    public int NextMove(int moneyA, int p, int moneyB) {
        return ChooseRandomAction(moneyA);
    }

    public String getAuteur() {
        return "Random Action Player-CHH-VHT";
    }

    public void LoadPlayer(String file) {
    }

    public void SaveMe(String filename) {
    }
    public void endMatch(int r){}
}
