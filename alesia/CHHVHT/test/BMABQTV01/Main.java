/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package apprentissage;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Vector;

/**
 *
 * @author ADMIN
 */
public class Main {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws IOException {
        // TODO code application logic here

        // Create the root node
        int iniMoney = 10;
        Node rootNode = new Node(iniMoney, iniMoney, 0, 0, null);

        // Create the data of tree
        Utility.initilizeValue(rootNode);

        float gama = (float) 0.9;
        float alpha = (float) 1;
        for (int i = 0; i < 2000; i++) {
            alpha = (float) 1 / (1 + i);
            Utility.updateValue(rootNode, alpha, gama);
        }
        Utility.saveData(rootNode);

        // Playing
        String command = "c";
        do {
            InputStreamReader reader0 = new InputStreamReader(System.in);
            BufferedReader br0 = new BufferedReader(reader0);
            System.out.print("Do you want continue to play : ");
            String str = br0.readLine();
            command = str;

            //Start game
            Node testNode = new Node(10, 10, 0, 0, null);
            Node var = new Node(0, 0, 0, 0, null);
            boolean gameOver = false;
            while (!gameOver) {
                /** Current Etat */
                System.out.println("----------------------------------------------------");
                System.out.println("Computer's money : " + testNode.myMoney + ", Computer's position : " + testNode.myPosition);
                System.out.println("User 's money    : " + testNode.yourMoney + ", User's position     : " + testNode.yourPosition);

                /** User plays */
                InputStreamReader reader = new InputStreamReader(System.in);
                BufferedReader br = new BufferedReader(reader);
                System.out.print("Enter your number : ");
                String strUserMoney = br.readLine();
                int userMoney = Integer.parseInt(strUserMoney);
                System.out.println("User gives the money : " + userMoney);

                /** Computer plays */
                Utility.findMove(testNode, rootNode, var);
                int computerMoney = var.myMoney;
                System.out.println("Computer gives money : " + computerMoney);

                testNode.showInformation();

                /** Update new etat */
                testNode.myMoney = testNode.myMoney - computerMoney;
                testNode.yourMoney = testNode.yourMoney - userMoney;

                if (userMoney > computerMoney) {
                    testNode.yourPosition = testNode.yourPosition + 1;
                } else {
                    if (userMoney < computerMoney) {
                        testNode.myPosition = testNode.myPosition + 1;
                    }
                }

                testNode.showInformation();

                /** Check game over */
                if (testNode.myPosition == 3) {
                    System.out.println("Computer wins");
                    gameOver = true;
                }else{
                    if ( testNode.yourPosition == 3) {
                        System.out.println("User wins");
                        gameOver = true;
                    }else{
                        if(testNode.myMoney > 0 && testNode.yourMoney == 0){
                            System.out.println("Computer wins");
                            gameOver = true;
                        }else{
                            if(testNode.myMoney == 0 && testNode.yourMoney > 0){
                                System.out.println("User wins");
                                gameOver = true;
                            }
                        }
                    }
                }                
            }
        } while (command.compareToIgnoreCase("c") == 0);
    }
}
