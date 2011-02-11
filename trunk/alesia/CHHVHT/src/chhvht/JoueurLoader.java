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

    public static Joueur load(String joueurName) {
        if (joueurName.compareTo("RadomActionJoueur") == 0) {
            return new RandomActionJoueur();
        } else if (joueurName.compareTo("TTCGMR") == 0) {
            return new chhvht.test.TTCGMRJoueur();
        } else if (joueurName.compareTo("BTMABQT") == 0) {
            return new chhvht.test.BTMABQTJoueur();
        } else if (joueurName.compareTo("LEVMLS") == 0) {
            return new chhvht.test.LEVMLSJoueur();
        } else if (joueurName.compareTo("TXHVQH") == 0) {
            return new chhvht.test.TXHVQHJoueur();
        }else if (joueurName.compareTo("PVDLVMHTN.Weka") == 0) {
            return new chhvht.test.PVDLVMHTNJoueurWeka();
        }else if (joueurName.compareTo("PVDLVMHTN.5") == 0) {
            return new chhvht.test.PVDLVMHTNJoueur5();
        }else if (joueurName.compareTo("PVDLVMHTN.Adaptif") == 0) {
            return new chhvht.test.PVDLVMHTNJoueurAdaptif();
        }else if (joueurName.compareTo("PVDLVMHTN.TDSimplest") == 0) {
            return new chhvht.test.PVDLVMHTNJoueurTDSimplest();
        }



        return new RandomActionJoueur();
    }
}
