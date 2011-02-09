/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package alesiatest;

import Joueur.JoueurExperience;
import alesia.model.Lvminhsdrludovic;
import alesia.model.Status;

/**
 *
 * @author lvminh
 */
public class ThinhLuan {
    public static void main(String[] args) {
        // TODO code application logic here
        int numberOfMatch = 1000;
        JoueurExperience joueurB= new JoueurExperience();
        Lvminhsdrludovic joueurA = new Lvminhsdrludovic();
        try{
            joueurA.loadPlayer("LEVMLSV02.txt");
        }
        catch(Exception e){
            System.out.println(e.getMessage());
            return;
        }
        String leftSideName = "Minh";
        String rightSideName = "Giang";
        int leftSideWin = 0;
        int rightSideWin = 0;
        int leftBet = 0;
        int rightBet = 0;
        for (int i=1; i<=numberOfMatch; i++){
            System.out.println("Match "+ i);
            Status status = new Status(10, 0, 10);
            while (true){
                int winner = status.getWinner();
                if (winner != 0){
                    if (winner == 2){
                        System.out.println("Drawn! ");
                    }
                    else if (winner == -1) {
                        System.out.println(leftSideName + " won!");
                        leftSideWin++;
                        joueurA.endMatch(1);
                    }
                    else if (winner == 1){
                        System.out.println(rightSideName + " won!");
                        rightSideWin++;
                        joueurA.endMatch(-1);
                    }
                    System.out.println(leftSideName + ": " + leftSideWin);
                    System.out.println(rightSideName + ": " + rightSideWin);
                    break;
                }
                else{
                    rightBet = joueurB.NextMove(status.getLeftSideAvalable(), status.getP(), status.getRightSideAvalable());
                    if (rightBet > status.getRightSideAvalable()) rightBet = status.getRightSideAvalable();
                    if (rightBet < 1) rightBet = 1;
                    leftBet = joueurA.nextMove(status.getRightSideAvalable(), status.getInverseP(), status.getLeftSideAvalable());
                    if (leftBet > status.getLeftSideAvalable()) leftBet = status.getLeftSideAvalable();
                    if (leftBet < 1) leftBet = 1;
                    System.out.println(leftSideName + " bets: " + leftBet);
                    System.out.println(rightSideName + " bets: " + rightBet);
                    status = new Status(status, leftBet, rightBet);
                }
            }
        }
        try{
            joueurA.writeToFile("LEVMLSV03.txt");
        }
        catch(Exception e){
            System.out.println(e.getMessage());
        }
    }

}
