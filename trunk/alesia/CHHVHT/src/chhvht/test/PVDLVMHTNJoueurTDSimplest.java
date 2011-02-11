/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package chhvht.test;
import player.*;
/**
 *
 * @author thuanvh
 */
public class PVDLVMHTNJoueurTDSimplest  implements chhvht.Joueur{

    player.Player joueur;

    public PVDLVMHTNJoueurTDSimplest() {
//        String myProgram = "player.PlayerRandomWekaTree";

        // NOTRE PROGRAMME SE COMPOSE 5 TYPE DE JOUEUR. VOUS POUVEZ TESTER
        // EN CHANGEANT LE NOM DE TYPE JOUEUR COMME LES SUIVANTES:
        //String myProgram = "player.Player5";
        //String myProgram = "player.PlayerAdaptif",;
        //String myProgram = "player.PlayerSimplestTDMethod";
        //String myProgram = "player.Ramdom",
        joueur = (Player)new PlayerSimplestTDMethod();
        joueur.nextMatch(1000);
    }

    public void NewMatch(int nbParties) {
    }

    public int NextMove(int moneyA, int p, int moneyB) {
        return joueur.nextMove(moneyA, p, moneyB);
    }

    public String getAuteur() {
        return "PVDLVMHTN.TDSimplest";
    }

    public void LoadPlayer(String file) {
    }

    public void SaveMe(String filename) {
    }

    public void endMatch(int r) {
    }
}