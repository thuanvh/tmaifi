/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package game;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import javax.xml.soap.Node;
import player.Player;
import chhvht.*;

/**
 *
 * @author langthang
 */
public class Test {
    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) throws ClassNotFoundException, InstantiationException, IllegalAccessException {

        int nbPartie = 1000;
        String myProgram = "player.PlayerRandomWekaTree";

        // NOTRE PROGRAMME SE COMPOSE 5 TYPE DE JOUEUR. VOUS POUVEZ TESTER
        // EN CHANGEANT LE NOM DE TYPE JOUEUR COMME LES SUIVANTES:
        //String myProgram = "player.Player5";
        //String myProgram = "player.PlayerAdaptif",;
        //String myProgram = "player.PlayerSimplestTDMethod";
        //String myProgram = "player.Ramdom",
        
        Player p;
        p = (Player)Class.forName(myProgram).newInstance();
        p.nextMatch(nbPartie);


        //----- VOTRE PLAYER----------------
        CHHVHTJoueur joueur;
        joueur = new CHHVHTJoueur();
        //Telecharger le fichier
        joueur.LoadPlayer("CHHVHTV10.txt");
        //----- VOTRE PLAYER----------------

        int totscore = 0;

        for (int i=0; i<nbPartie; i++) {
            int score = 0;
            int pCap = 10;
            int qCap = 10;
            int pPos = 0;
            int qPos = 0;

            while(!((pCap <= 0 && qCap <= 0) || (pPos == 3) || (qPos == 3))){
                    int positionP = definePos(pPos, qPos);
                    int positionQ = definePos(qPos, pPos);

                    int pm = p.nextMove(pCap,positionP,qCap);

                    //----- VOTRE PLAYER----------------
                    int qm =  joueur.NextMove(qCap,positionQ,pCap);
                    //----- VOTRE PLAYER----------------

                    p.stockMove(pCap, qCap, positionP, qm);

                    /**
                     * if player p wins
                     */
                    if(pm > qm)
                            pPos += 1;
                    else if (qm > pm)
                            qPos += 1;
                    pCap -= pm;
                    qCap -= qm;

                    if(pPos == 3) score += 1;
                    if(qPos == 3) score -= 1;
            }

            totscore += score;
            p.saveMe();
        }

            System.out.print("Score: " + totscore);

    }

    public static int definePos(int pPos, int qPos){
        int pos = 3 * pPos + qPos;
        return pos;
    }
}
