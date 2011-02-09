/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package chhvht;

/**
 *
 * @author thuanvh
 */
public class JoueurLoader {
    public static Joueur load(String joueurName){
        if(joueurName.compareTo("RadomActionJoueur")==0){
            return new RandomActionJoueur();
        }
        else if(joueurName.compareTo("TTCGMR")==0){
            return new chhvht.test.TTCGMRJoueur();
        }else if(joueurName.compareTo("BTMABQT")==0){
            return new chhvht.test.BTMABQTJoueur();
        }else if(joueurName.compareTo("LEVMLS")==0){
            return new chhvht.test.LEVMLSJoueur();
        }
        return new RandomActionJoueur();
    }
}
