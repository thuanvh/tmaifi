package chhvht;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
/**
 *
 * @author thuanvh, chhue
 */
//import java.util.Random;
import java.util.*;
import java.io.*;

public class CHHVHTJoueur implements Joueur {

    // match result
    int awin = 0;
    int bwin = 0;
    int nowin = 0;
    int currentStateA = 0;
    int currentStateB = 0;
    int currentMoneyA = 10;
    int currentMoneyB = 10;
    double epsilon = 0.1;//probability of action
    double gamma = 0.9;
    Random random = new Random();
    ArrayList stateList = new ArrayList();
    boolean isMatchHumanEnd = false;
    Joueur opponent = null;
    boolean isLearning = false;

    public void resetResult() {
        awin = 0;
        bwin = 0;
        nowin = 0;
    }

    public void reset() {
//        currentStateA = 1;
//        currentStateB = 2;
//        currentMoneyA = 3;
//        currentMoneyB = 2;
        currentStateA = 0;
        currentStateB = 0;
        currentMoneyA = 10;
        currentMoneyB = 10;
    }
    javax.swing.JTextArea textoutput;

    public void setTextOutput(javax.swing.JTextArea output) {
        textoutput = output;
    }

    public void playHuman(int humanMoney) {
        int ma = ChooseAction();
        int mb = humanMoney;
        if (textoutput != null) {
            textoutput.append("Machine choose:" + ma + "\n");
            textoutput.append("Human choose:" + mb + "\n");
        }
        int oldStateA = currentStateA;
        int oldStateB = currentStateB;
        int oldstate = oldStateA * 3 + oldStateB;
        // Do action
        int reward = 0;
        if (ma > mb) {
            currentStateA++;
            reward = 1;
        } else if (mb > ma) {
            currentStateB++;
            reward = -1;
        }
        int oldMoneyA = currentMoneyA;
        int oldMoneyB = currentMoneyB;
        currentMoneyA -= ma;
        currentMoneyB -= mb;
        double endvalue = valueEndState(currentStateA, currentStateB, currentMoneyA, currentMoneyB);
        int newstate = currentStateA * 3 + currentStateB;
        //boolean endstate = false;
        if (endvalue == 2) {
            endvalue = getValueOfState(currentMoneyA, newstate, currentMoneyB);
        } else {
            isMatchHumanEnd = true;
        }
        // update oldvalue
        AlesiaItem olditem = getItem(oldMoneyA, oldstate, oldMoneyB);
        double alpha = 1;
        if (olditem != null) {
            //alpha = 1 / (double) (olditem.visitedNumber);
            alpha = 0.5;
        }
        // update value V
        double newvalue = getValueOfState(oldMoneyA, oldstate, oldMoneyB) * (1 - alpha) + alpha * (reward + gamma * getValueOfState(currentMoneyA, newstate, currentMoneyB));
        setValue(oldMoneyA, oldMoneyB, oldstate, newvalue);
        // count the score
        if (isMatchHumanEnd) {
//            if (textoutput != null) {
            textoutput.append("End:");
//            }
            if (endvalue < 0) {
                textoutput.append("Human Win");
                bwin++;
            } else if (endvalue > 0) {
                textoutput.append("Machine Win");
                awin++;
            } else {
                textoutput.append("Nobody Win");
                nowin++;
            }
            textoutput.append("\n===============\n");
        }
    }

    public int NextMove(int moneyA, int p, int moneyB) {
        currentMoneyA = moneyA;
        currentMoneyB = moneyB;
        currentStateA = p / 3;
        currentStateB = p % 3;
        return ChooseAction();
    }
    public void endMatch(int r){}
    /*
     * Choose action A
     */

    public int ChooseAction() {
        if (currentMoneyA == 0) {
            return 0;
        } else if (currentMoneyB == 0) {
            return 1;
        }
        double prob = random.nextDouble();
        int nextaction = 0;
        if (prob < epsilon) {
            nextaction = ChooseRandomAction(currentMoneyA);
        } else {
            double maxV = -1e6;
            ArrayList maxlist = new ArrayList();
//            int nexta=0;
//            int currentState = currentStateA * 3 + currentStateB;
            for (int ma = 1; ma <= currentMoneyA; ma++) {
                double v = 0;
                for (int mb = 1; mb <= currentMoneyB; mb++) {
                    int nextstateA = currentStateA;
                    int nextstateB = currentStateB;
                    if (ma < mb) {
                        nextstateB++;
                    } else if (ma > mb) {
                        nextstateA++;
                    }
                    int nextstate = nextstateA * 3 + nextstateB;
                    v += getValueOfState(currentMoneyA - ma, nextstate, currentMoneyB - mb);
//                    if (maxV <= v) {
//                        if (maxV < v) {
//                            maxlist.clear();
//                        }
//                        maxlist.add(ma);
//                        maxV = v;
//                    }
                }
                v = v / currentMoneyB;
                if (maxV <= v) {
                    if (maxV < v) {
                        maxlist.clear();
                    }
                    maxlist.add(ma);
                    maxV = v;
                }
            }
            if (maxlist.size() == 1) {
                nextaction = (Integer) maxlist.get(0);
            } else if (maxlist.size() > 1) {
                int rand = random.nextInt(maxlist.size() - 1);
                nextaction = (Integer) maxlist.get(rand);
            }
        }
        return nextaction;
    }

    public int ChooseRandomAction(int currentMoney) {
        if (currentMoney <= 0) {
            return 0;
        }
        if (currentMoney == 1) {
            return currentMoney;
        }
        return random.nextInt(currentMoney - 1) + 1;
    }

    public int valueEndState(int stateA, int stateB, int moneyA, int moneyB) {
        int v = 2;
        if (currentStateA == 3) {
            // A gagne
            v = 1;
        } else if (currentStateB == 3) {
            // B gagne
            v = -1;
        } else if (currentMoneyA == 0 && currentMoneyB == 0) {
            //NULL
            v = 0;
        }
        return v;
    }


    /*
     * Run with a randomize jouer
     */
    public void NewMatch(int nbParties) {
        int localawin = 0;
        int localbwin = 0;
        int localnowin = 0;
        for (int i = 0; i < nbParties; i++) {
            reset();
            //
            while (true) {
                int oldStateA = currentStateA;
                int oldStateB = currentStateB;
                int oldstate = oldStateA * 3 + oldStateB;

//                System.out.println("A decide "+currentMoneyA+","+ oldstate+","+ currentMoneyB);
                int ma = NextMove(currentMoneyA, oldstate, currentMoneyB);//ChooseAction();
//                System.out.println(ma);
                int stateB = oldStateB * 3 + oldStateA;
//                System.out.println("B decide "+currentMoneyB+","+ stateB+","+ currentMoneyA);
                int mb = opponent.NextMove(currentMoneyB, stateB, currentMoneyA);//ChooseRandomAction(currentMoneyB);
//                System.out.println(mb);

                // Do action
                int reward = 0;
                if (ma > mb) {
                    currentStateA++;
                    reward = 1;
                } else if (mb > ma) {
                    currentStateB++;
                    reward = -1;
                }
                int oldMoneyA = currentMoneyA;
                int oldMoneyB = currentMoneyB;
                currentMoneyA -= ma;
                currentMoneyB -= mb;
                double r = valueEndState(currentStateA, currentStateB, currentMoneyA, currentMoneyB);
                int newstate = currentStateA * 3 + currentStateB;
                boolean endstate = false;
                if (r == 2) {
                    r = getValueOfState(currentMoneyA, newstate, currentMoneyB);
                } else {
                    endstate = true;
                }
                if (isLearning) {
                    // update oldvalue
                    AlesiaItem olditem = getItem(oldMoneyA, oldstate, oldMoneyB);
                    double alpha = 1;
                    if (olditem != null) {
//                    alpha = 1 / (double) (olditem.visitedNumber);
                        alpha = 0.2;
                    }
                    // update value V
                    double newvalue = getValueOfState(oldMoneyA, oldstate, oldMoneyB) * (1 - alpha) + alpha * (reward + gamma * getValueOfState(currentMoneyA, newstate, currentMoneyB));
                    setValue(oldMoneyA, oldMoneyB, oldstate, newvalue);
                }
                // count the score
                if (endstate) {
                    opponent.endMatch((int)-r);
                    if (r < 0) {
                        localbwin++;
                    } else if (r > 0) {
                        localawin++;
                    } else {
                        localnowin++;
                    }
                    break;
                }
            }
        }
        awin += localawin;
        bwin += localbwin;
        nowin += localnowin;
        textoutput.append("===============\n");
        if (isLearning) {
            textoutput.append("Learn: " + nbParties + " matches \n");
        } else {
            textoutput.append("Test: " + nbParties + " matches \n");
        }
        textoutput.append("CHHVHT (A) vs " + opponent.getAuteur() + "(B)\n");
        textoutput.append("A Win:" + localawin + "\n");
        textoutput.append("B Win:" + localbwin + "\n");
        textoutput.append("Noone Win:" + localnowin + "\n");
        textoutput.append("===============\n");
    }

    public AlesiaItem getItem(int moneyA, int moneyB, int state) {
        AlesiaItem item = new AlesiaItem();
        item.label = moneyA + "," + moneyB + "," + state;

        int index = Collections.binarySearch(stateList, item);
        if (index < 0) {
            return null;
        } else {
            AlesiaItem itemfound = (AlesiaItem) stateList.get(index);

            return itemfound;
        }
    }

    public void setValue(int moneyA, int moneyB, int state, double value) {
        AlesiaItem item = new AlesiaItem();
        item.label = moneyA + "," + moneyB + "," + state;
        item.value = value;
        item.visitedNumber++;
        int index = Collections.binarySearch(stateList, item);
        if (index < 0) {
            stateList.add(-index - 1, item);
        } else {
            AlesiaItem itemfound = (AlesiaItem) stateList.get(index);
            itemfound.value = value;
            itemfound.visitedNumber++;
        }
    }
    /*
     * Get value of state
     */

    public double getValueOfState(int moneyA, int p, int moneyB) {
        //(p vaut 0, 1, 2, ... 8 correspondant aux positions 00 01 02 10 11 12 20 21 22) et player qui possede moneyA
        int stateA = p / 3;
        int stateB = p % 3;
        int r = valueEndState(stateA, stateB, moneyA, moneyB);
        if (r < 2) {
            return r;
        }
        AlesiaItem item = new AlesiaItem();
        item.label = moneyA + "," + moneyB + "," + p;
        int index = Collections.binarySearch(stateList, item);
        if (index < 0) {
            return 0;
        } else {
            AlesiaItem itemfound = (AlesiaItem) stateList.get(index);
            return itemfound.value;
        }
    }

    public String getAuteur() {
        return "Cao Hong Hue et Vu Hong Thuan";
    }

    public void LoadPlayer(String filename) {
        stateList.clear();
        try {
            // Read file
            FileReader fstream = new FileReader(filename);
            BufferedReader out = new BufferedReader(fstream);
            while (out.ready()) {
                AlesiaItem item = new AlesiaItem();
                String itemstr = out.readLine();
                String[] arraystr = itemstr.split(":");
                item.label = arraystr[0];
                item.value = Double.valueOf(arraystr[1]);
                item.visitedNumber = Integer.valueOf(arraystr[2]);
                stateList.add(item);
            }
            //Close the input stream
            out.close();
        } catch (Exception e) {//Catch exception if any
            System.err.println("Error: " + e.getMessage());
        }
    }

    public void SaveMe(String filename) {
        try {
            // Create file
            FileWriter fstream = new FileWriter(filename);
            BufferedWriter out = new BufferedWriter(fstream);
            for (int i = 0; i < stateList.size(); i++) {
                AlesiaItem item = (AlesiaItem) stateList.get(i);
                String itemstr = item.label + ":" + item.value + ":" + item.visitedNumber;
                out.write(itemstr);
                out.newLine();
            }
            //Close the output stream
            out.close();
        } catch (Exception e) {//Catch exception if any
            System.err.println("Error: " + e.getMessage());
        }
    }
}
