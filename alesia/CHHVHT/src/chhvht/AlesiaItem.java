package chhvht;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author thuanvh
 */
public class AlesiaItem implements Comparable{

    public String label;
    public double value=0;
    public int visitedNumber=0;
    public int compareTo(Object o){
        AlesiaItem a=(AlesiaItem)o;
        return label.compareTo(a.label);
    }
}
