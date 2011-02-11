/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package testcollection;

/**
 *
 * @author hientx
 */
import txhvqhv01.*;

public class Main {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        Node nodeCurrent = new Node();
        // 5- Votre prix exist, 6 - Mon prix exist, 1 - Votre status, 0 - Ma status, status = 3 --> gagne
        nodeCurrent.setValueNode(5, 6, 1, 0); 

        TXHVQHJoueur joueur = new TXHVQHJoueur();

        int choice = joueur.ChoiceBestPrix(nodeCurrent);
        System.out.print(choice);
    }

}
