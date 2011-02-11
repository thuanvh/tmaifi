/*
 * CHHVHTView.java
 */
package chhvht;

import java.awt.Color;
import org.jdesktop.application.Action;
import org.jdesktop.application.ResourceMap;
import org.jdesktop.application.SingleFrameApplication;
import org.jdesktop.application.FrameView;
import org.jdesktop.application.TaskMonitor;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.Timer;
import javax.swing.Icon;
import javax.swing.JDialog;
import javax.swing.JFrame;
import java.awt.Graphics;
import javax.swing.JFileChooser;

/**
 * The application's main frame.
 */
public class CHHVHTView extends FrameView {

    int state = 0;
    CHHVHTJoueur jouer;

    public void drawCanvas(int stateA, int stateB) {
        //canvas1.repaint();
        Graphics g = canvas1.getGraphics();

        int A = stateA;// / 3;
        int B = stateB;// % 3;
        int padding = 30;
        //draw lines
        int h = canvas1.getSize().height;
        int w = canvas1.getSize().width;
        g.clearRect(0, 0, w, h);
        g.setColor(Color.BLUE);

        for (int i = 1; i <= 5; i++) {
            int lx = padding + i * (w - 2 * padding) / 6;
            //g.drawLine(lx, h/4, lx, h*3/4);
            if (i == 3) {
                g.fillRect(lx, h / 6, 4, h * 2 / 3);
            } else {
                g.fillRect(lx, h / 4, 2, h / 2);
            }
        }
        int Ax = padding + A * (w - 2 * padding) / 6;
        int r = 20;
        int Bx = padding + (6 - B) * (w - 2 * padding) / 6;

        g.setColor(Color.WHITE);
        g.fillOval(Ax - r / 2, h / 2 - r / 2, r, r);
        g.fillOval(Bx - r / 2, h / 2 - r / 2, r, r);
        g.setColor(Color.BLUE);
        g.drawOval(Ax - r / 2, h / 2 - r / 2, r, r);
        g.drawOval(Bx - r / 2, h / 2 - r / 2, r, r);
        g.drawString("A", Ax - 5, h / 2);

        g.drawString("B", Bx - 5, h / 2);

        //canvas1.repaint();
        //canvas1.paint(g);

    }

    public CHHVHTView(SingleFrameApplication app) {
        super(app);

        initComponents();

        jouer = new CHHVHTJoueur();

        // status bar initialization - message timeout, idle icon and busy animation, etc
        ResourceMap resourceMap = getResourceMap();
        int messageTimeout = resourceMap.getInteger("StatusBar.messageTimeout");
        messageTimer = new Timer(messageTimeout, new ActionListener() {

            public void actionPerformed(ActionEvent e) {
                statusMessageLabel.setText("");
            }
        });
        messageTimer.setRepeats(false);
        int busyAnimationRate = resourceMap.getInteger("StatusBar.busyAnimationRate");
        for (int i = 0; i < busyIcons.length; i++) {
            busyIcons[i] = resourceMap.getIcon("StatusBar.busyIcons[" + i + "]");
        }
        busyIconTimer = new Timer(busyAnimationRate, new ActionListener() {

            public void actionPerformed(ActionEvent e) {
                busyIconIndex = (busyIconIndex + 1) % busyIcons.length;
                statusAnimationLabel.setIcon(busyIcons[busyIconIndex]);
            }
        });
        idleIcon = resourceMap.getIcon("StatusBar.idleIcon");
        statusAnimationLabel.setIcon(idleIcon);
        progressBar.setVisible(false);

        // connecting action tasks to status bar via TaskMonitor
        TaskMonitor taskMonitor = new TaskMonitor(getApplication().getContext());
        taskMonitor.addPropertyChangeListener(new java.beans.PropertyChangeListener() {

            public void propertyChange(java.beans.PropertyChangeEvent evt) {
                String propertyName = evt.getPropertyName();
                if ("started".equals(propertyName)) {
                    if (!busyIconTimer.isRunning()) {
                        statusAnimationLabel.setIcon(busyIcons[0]);
                        busyIconIndex = 0;
                        busyIconTimer.start();
                    }
                    progressBar.setVisible(true);
                    progressBar.setIndeterminate(true);
                } else if ("done".equals(propertyName)) {
                    busyIconTimer.stop();
                    statusAnimationLabel.setIcon(idleIcon);
                    progressBar.setVisible(false);
                    progressBar.setValue(0);
                } else if ("message".equals(propertyName)) {
                    String text = (String) (evt.getNewValue());
                    statusMessageLabel.setText((text == null) ? "" : text);
                    messageTimer.restart();
                } else if ("progress".equals(propertyName)) {
                    int value = (Integer) (evt.getNewValue());
                    progressBar.setVisible(true);
                    progressBar.setIndeterminate(false);
                    progressBar.setValue(value);
                }
            }
        });
    }

    @Action
    public void showAboutBox() {
        if (aboutBox == null) {
            JFrame mainFrame = CHHVHTApp.getApplication().getMainFrame();
            aboutBox = new CHHVHTAboutBox(mainFrame);
            aboutBox.setLocationRelativeTo(mainFrame);
        }
        CHHVHTApp.getApplication().show(aboutBox);
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        mainPanel = new javax.swing.JPanel();
        canvas1 = new java.awt.Canvas();
        btnNewMatch = new javax.swing.JButton();
        btnNext = new javax.swing.JButton();
        jScrollPane1 = new javax.swing.JScrollPane();
        jTextArea1 = new javax.swing.JTextArea();
        txtEpsilon = new javax.swing.JTextField();
        txtGamma = new javax.swing.JTextField();
        jLabel1 = new javax.swing.JLabel();
        jLabel2 = new javax.swing.JLabel();
        jLabel3 = new javax.swing.JLabel();
        jLabel4 = new javax.swing.JLabel();
        jLabel5 = new javax.swing.JLabel();
        jLabel6 = new javax.swing.JLabel();
        lblWin = new javax.swing.JLabel();
        lblLost = new javax.swing.JLabel();
        lblNULL = new javax.swing.JLabel();
        btnLoadPlayer = new javax.swing.JButton();
        btnSavePlayer = new javax.swing.JButton();
        btnLearnMatch = new javax.swing.JButton();
        txtLearnMatch = new javax.swing.JTextField();
        jLabel10 = new javax.swing.JLabel();
        jLabel11 = new javax.swing.JLabel();
        txtHumanInput = new javax.swing.JTextField();
        jLabel7 = new javax.swing.JLabel();
        jLabel8 = new javax.swing.JLabel();
        jLabel9 = new javax.swing.JLabel();
        jLabel12 = new javax.swing.JLabel();
        lblHumanPos = new javax.swing.JLabel();
        lblCurrentHumanMoney = new javax.swing.JLabel();
        jLabel15 = new javax.swing.JLabel();
        lblMachinePos = new javax.swing.JLabel();
        lblCurrentMachineMoney = new javax.swing.JLabel();
        lblWinPercent = new javax.swing.JLabel();
        lblLostPercent = new javax.swing.JLabel();
        lblNULLPercent = new javax.swing.JLabel();
        btnResultReset = new javax.swing.JButton();
        cboOpponent = new javax.swing.JComboBox();
        jLabel13 = new javax.swing.JLabel();
        jLabel14 = new javax.swing.JLabel();
        txtTestMatch = new javax.swing.JTextField();
        btnTestMatch = new javax.swing.JButton();
        btnResetPlayer = new javax.swing.JButton();
        menuBar = new javax.swing.JMenuBar();
        javax.swing.JMenu fileMenu = new javax.swing.JMenu();
        javax.swing.JMenuItem exitMenuItem = new javax.swing.JMenuItem();
        javax.swing.JMenu helpMenu = new javax.swing.JMenu();
        javax.swing.JMenuItem aboutMenuItem = new javax.swing.JMenuItem();
        statusPanel = new javax.swing.JPanel();
        javax.swing.JSeparator statusPanelSeparator = new javax.swing.JSeparator();
        statusMessageLabel = new javax.swing.JLabel();
        statusAnimationLabel = new javax.swing.JLabel();
        progressBar = new javax.swing.JProgressBar();

        mainPanel.setName("mainPanel"); // NOI18N

        canvas1.setName("cvsAlesia"); // NOI18N

        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(chhvht.CHHVHTApp.class).getContext().getResourceMap(CHHVHTView.class);
        btnNewMatch.setText(resourceMap.getString("btnNewMatch.text")); // NOI18N
        btnNewMatch.setName("btnNewMatch"); // NOI18N
        btnNewMatch.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnNewMatchActionPerformed(evt);
            }
        });

        btnNext.setText(resourceMap.getString("btnNext.text")); // NOI18N
        btnNext.setName("btnNext"); // NOI18N
        btnNext.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnNextActionPerformed(evt);
            }
        });

        jScrollPane1.setName("jScrollPane1"); // NOI18N

        jTextArea1.setColumns(20);
        jTextArea1.setRows(5);
        jTextArea1.setName("txtHistory"); // NOI18N
        jScrollPane1.setViewportView(jTextArea1);

        txtEpsilon.setText(resourceMap.getString("txtEpsilon.text")); // NOI18N
        txtEpsilon.setName("txtEpsilon"); // NOI18N

        txtGamma.setText(resourceMap.getString("txtGamma.text")); // NOI18N
        txtGamma.setName("txtGamma"); // NOI18N

        jLabel1.setText(resourceMap.getString("lblAlpha.text")); // NOI18N
        jLabel1.setName("lblAlpha"); // NOI18N

        jLabel2.setText(resourceMap.getString("lblGama.text")); // NOI18N
        jLabel2.setName("lblGama"); // NOI18N

        jLabel3.setText(resourceMap.getString("jLabel3.text")); // NOI18N
        jLabel3.setName("jLabel3"); // NOI18N

        jLabel4.setText(resourceMap.getString("jLabel4.text")); // NOI18N
        jLabel4.setName("jLabel4"); // NOI18N

        jLabel5.setText(resourceMap.getString("jLabel5.text")); // NOI18N
        jLabel5.setName("jLabel5"); // NOI18N

        jLabel6.setText(resourceMap.getString("jLabel6.text")); // NOI18N
        jLabel6.setName("jLabel6"); // NOI18N

        lblWin.setText(resourceMap.getString("lblWin.text")); // NOI18N
        lblWin.setName("lblWin"); // NOI18N

        lblLost.setText(resourceMap.getString("lblLost.text")); // NOI18N
        lblLost.setName("lblLost"); // NOI18N

        lblNULL.setText(resourceMap.getString("lblNULL.text")); // NOI18N
        lblNULL.setName("lblNULL"); // NOI18N

        btnLoadPlayer.setText(resourceMap.getString("btnLoadPlayer.text")); // NOI18N
        btnLoadPlayer.setName("btnLoadPlayer"); // NOI18N
        btnLoadPlayer.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnLoadPlayerActionPerformed(evt);
            }
        });

        btnSavePlayer.setLabel(resourceMap.getString("btnSavePlayer.label")); // NOI18N
        btnSavePlayer.setName("btnSavePlayer"); // NOI18N
        btnSavePlayer.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSavePlayerActionPerformed(evt);
            }
        });

        btnLearnMatch.setText(resourceMap.getString("btnLearnMatch.text")); // NOI18N
        btnLearnMatch.setName("btnLearnMatch"); // NOI18N
        btnLearnMatch.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnLearnMatchActionPerformed(evt);
            }
        });

        txtLearnMatch.setText(resourceMap.getString("txtLearnMatch.text")); // NOI18N
        txtLearnMatch.setName("txtLearnMatch"); // NOI18N

        jLabel10.setText(resourceMap.getString("jLabel10.text")); // NOI18N
        jLabel10.setName("jLabel10"); // NOI18N

        jLabel11.setText(resourceMap.getString("jLabel11.text")); // NOI18N
        jLabel11.setName("jLabel11"); // NOI18N

        txtHumanInput.setText(resourceMap.getString("txtHumanInput.text")); // NOI18N
        txtHumanInput.setName("txtHumanInput"); // NOI18N

        jLabel7.setText(resourceMap.getString("jLabel7.text")); // NOI18N
        jLabel7.setName("jLabel7"); // NOI18N

        jLabel8.setText(resourceMap.getString("jLabel8.text")); // NOI18N
        jLabel8.setName("jLabel8"); // NOI18N

        jLabel9.setText(resourceMap.getString("jLabel9.text")); // NOI18N
        jLabel9.setName("jLabel9"); // NOI18N

        jLabel12.setText(resourceMap.getString("jLabel12.text")); // NOI18N
        jLabel12.setName("jLabel12"); // NOI18N

        lblHumanPos.setText(resourceMap.getString("lblHumanPos.text")); // NOI18N
        lblHumanPos.setName("lblHumanPos"); // NOI18N

        lblCurrentHumanMoney.setText(resourceMap.getString("lblCurrentHumanMoney.text")); // NOI18N
        lblCurrentHumanMoney.setName("lblCurrentHumanMoney"); // NOI18N

        jLabel15.setText(resourceMap.getString("jLabel15.text")); // NOI18N
        jLabel15.setName("jLabel15"); // NOI18N

        lblMachinePos.setText(resourceMap.getString("lblMachinePos.text")); // NOI18N
        lblMachinePos.setName("lblMachinePos"); // NOI18N

        lblCurrentMachineMoney.setText(resourceMap.getString("lblCurrentMachineMoney.text")); // NOI18N
        lblCurrentMachineMoney.setName("lblCurrentMachineMoney"); // NOI18N

        lblWinPercent.setText(resourceMap.getString("lblWinPercent.text")); // NOI18N
        lblWinPercent.setName("lblWinPercent"); // NOI18N

        lblLostPercent.setText(resourceMap.getString("lblLostPercent.text")); // NOI18N
        lblLostPercent.setName("lblLostPercent"); // NOI18N

        lblNULLPercent.setText(resourceMap.getString("lblNULLPercent.text")); // NOI18N
        lblNULLPercent.setName("lblNULLPercent"); // NOI18N

        btnResultReset.setText(resourceMap.getString("btnResultReset.text")); // NOI18N
        btnResultReset.setName("btnResultReset"); // NOI18N
        btnResultReset.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnResultResetActionPerformed(evt);
            }
        });

        cboOpponent.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "RandomActionPlayer", "TTCGMR", "NDTDTL", "BTMABQT", "LEVMLS", "TXHVQH", "PVDLVMHTN.Weka", "PVDLVMHTN.5", "PVDLVMHTN.Adaptif", "PVDLVMHTN.TDSimplest" }));
        cboOpponent.setName("cboOpponent"); // NOI18N

        jLabel13.setText(resourceMap.getString("jLabel13.text")); // NOI18N
        jLabel13.setName("jLabel13"); // NOI18N

        jLabel14.setText(resourceMap.getString("jLabel14.text")); // NOI18N
        jLabel14.setName("jLabel14"); // NOI18N

        txtTestMatch.setText(resourceMap.getString("txtTestMatch.text")); // NOI18N
        txtTestMatch.setName("txtTestMatch"); // NOI18N

        btnTestMatch.setText(resourceMap.getString("btnTestMatch.text")); // NOI18N
        btnTestMatch.setName("btnTestMatch"); // NOI18N
        btnTestMatch.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnTestMatchActionPerformed(evt);
            }
        });

        btnResetPlayer.setText(resourceMap.getString("btnResetPlayer.text")); // NOI18N
        btnResetPlayer.setName("btnResetPlayer"); // NOI18N
        btnResetPlayer.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnResetPlayerActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout mainPanelLayout = new javax.swing.GroupLayout(mainPanel);
        mainPanel.setLayout(mainPanelLayout);
        mainPanelLayout.setHorizontalGroup(
            mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(mainPanelLayout.createSequentialGroup()
                .addGap(22, 22, 22)
                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(canvas1, javax.swing.GroupLayout.PREFERRED_SIZE, 184, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addGroup(mainPanelLayout.createSequentialGroup()
                        .addGap(6, 6, 6)
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, mainPanelLayout.createSequentialGroup()
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(jLabel7)
                                    .addGroup(mainPanelLayout.createSequentialGroup()
                                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                            .addComponent(jLabel9)
                                            .addComponent(jLabel15))
                                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                            .addComponent(lblMachinePos)
                                            .addComponent(jLabel8)
                                            .addComponent(lblHumanPos))))
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addGroup(mainPanelLayout.createSequentialGroup()
                                        .addGap(10, 10, 10)
                                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                                            .addComponent(lblCurrentMachineMoney)
                                            .addComponent(txtHumanInput, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                            .addComponent(lblCurrentHumanMoney)))
                                    .addGroup(mainPanelLayout.createSequentialGroup()
                                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                        .addComponent(jLabel12, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE))))
                            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, mainPanelLayout.createSequentialGroup()
                                .addComponent(btnNext)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addComponent(btnNewMatch)))))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addGroup(mainPanelLayout.createSequentialGroup()
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(btnLoadPlayer)
                            .addGroup(mainPanelLayout.createSequentialGroup()
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(jLabel13)
                                    .addComponent(jLabel1))
                                .addGap(22, 22, 22)
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                                        .addComponent(btnTestMatch)
                                        .addComponent(cboOpponent, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                                    .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                                        .addGroup(mainPanelLayout.createSequentialGroup()
                                            .addComponent(btnSavePlayer)
                                            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                            .addComponent(btnResetPlayer)
                                            .addGap(8, 8, 8))
                                        .addGroup(mainPanelLayout.createSequentialGroup()
                                            .addComponent(txtEpsilon, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                            .addComponent(jLabel2)
                                            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                            .addComponent(txtGamma, javax.swing.GroupLayout.PREFERRED_SIZE, 73, javax.swing.GroupLayout.PREFERRED_SIZE)))))
                            .addGroup(mainPanelLayout.createSequentialGroup()
                                .addComponent(jLabel14)
                                .addGap(18, 18, 18)
                                .addComponent(txtTestMatch, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                            .addGroup(mainPanelLayout.createSequentialGroup()
                                .addComponent(jLabel11)
                                .addGap(2, 2, 2)
                                .addComponent(txtLearnMatch, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                .addGap(12, 12, 12)
                                .addComponent(btnLearnMatch))
                            .addGroup(mainPanelLayout.createSequentialGroup()
                                .addComponent(jLabel3)
                                .addGap(12, 12, 12)
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addGroup(mainPanelLayout.createSequentialGroup()
                                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                            .addComponent(jLabel5)
                                            .addComponent(jLabel4)
                                            .addComponent(jLabel6))
                                        .addGap(18, 18, 18)
                                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                            .addGroup(mainPanelLayout.createSequentialGroup()
                                                .addComponent(lblNULL)
                                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                                .addComponent(lblNULLPercent))
                                            .addGroup(mainPanelLayout.createSequentialGroup()
                                                .addComponent(lblWin)
                                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                                .addComponent(lblWinPercent))
                                            .addGroup(mainPanelLayout.createSequentialGroup()
                                                .addComponent(lblLost)
                                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                                .addComponent(lblLostPercent))))
                                    .addComponent(btnResultReset)))))
                    .addComponent(jScrollPane1))
                .addGap(97, 97, 97))
        );
        mainPanelLayout.setVerticalGroup(
            mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(mainPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(canvas1, javax.swing.GroupLayout.PREFERRED_SIZE, 121, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 124, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addGroup(mainPanelLayout.createSequentialGroup()
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(jLabel8)
                            .addComponent(jLabel12))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(jLabel9)
                            .addComponent(lblHumanPos)
                            .addComponent(lblCurrentHumanMoney))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(jLabel15)
                            .addComponent(lblMachinePos)
                            .addComponent(lblCurrentMachineMoney))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(jLabel7)
                            .addComponent(txtHumanInput, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(btnNext)
                            .addComponent(btnNewMatch)))
                    .addGroup(mainPanelLayout.createSequentialGroup()
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(btnLoadPlayer)
                            .addComponent(btnSavePlayer)
                            .addComponent(btnResetPlayer)
                            .addComponent(jLabel11)
                            .addComponent(txtLearnMatch, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(btnLearnMatch))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addGroup(mainPanelLayout.createSequentialGroup()
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                    .addComponent(txtEpsilon, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                    .addComponent(txtGamma, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                    .addComponent(jLabel2)
                                    .addComponent(jLabel1))
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                    .addComponent(jLabel13)
                                    .addComponent(cboOpponent, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                    .addComponent(jLabel14)
                                    .addComponent(txtTestMatch, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                    .addComponent(btnTestMatch)))
                            .addGroup(mainPanelLayout.createSequentialGroup()
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                    .addComponent(jLabel3)
                                    .addComponent(jLabel4)
                                    .addComponent(lblWin)
                                    .addComponent(lblWinPercent))
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                    .addComponent(jLabel5)
                                    .addComponent(lblLost)
                                    .addComponent(lblLostPercent))
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                    .addComponent(jLabel6)
                                    .addComponent(lblNULL)
                                    .addComponent(lblNULLPercent))
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                .addComponent(btnResultReset)))))
                .addContainerGap(55, Short.MAX_VALUE))
        );

        menuBar.setName("menuBar"); // NOI18N

        fileMenu.setText(resourceMap.getString("fileMenu.text")); // NOI18N
        fileMenu.setName("fileMenu"); // NOI18N

        javax.swing.ActionMap actionMap = org.jdesktop.application.Application.getInstance(chhvht.CHHVHTApp.class).getContext().getActionMap(CHHVHTView.class, this);
        exitMenuItem.setAction(actionMap.get("quit")); // NOI18N
        exitMenuItem.setName("exitMenuItem"); // NOI18N
        fileMenu.add(exitMenuItem);

        menuBar.add(fileMenu);

        helpMenu.setText(resourceMap.getString("helpMenu.text")); // NOI18N
        helpMenu.setName("helpMenu"); // NOI18N

        aboutMenuItem.setAction(actionMap.get("showAboutBox")); // NOI18N
        aboutMenuItem.setName("aboutMenuItem"); // NOI18N
        helpMenu.add(aboutMenuItem);

        menuBar.add(helpMenu);

        statusPanel.setName("statusPanel"); // NOI18N

        statusPanelSeparator.setName("statusPanelSeparator"); // NOI18N

        statusMessageLabel.setName("statusMessageLabel"); // NOI18N

        statusAnimationLabel.setHorizontalAlignment(javax.swing.SwingConstants.LEFT);
        statusAnimationLabel.setName("statusAnimationLabel"); // NOI18N

        progressBar.setName("progressBar"); // NOI18N

        javax.swing.GroupLayout statusPanelLayout = new javax.swing.GroupLayout(statusPanel);
        statusPanel.setLayout(statusPanelLayout);
        statusPanelLayout.setHorizontalGroup(
            statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(statusPanelSeparator, javax.swing.GroupLayout.DEFAULT_SIZE, 764, Short.MAX_VALUE)
            .addGroup(statusPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(statusMessageLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 578, Short.MAX_VALUE)
                .addComponent(progressBar, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(statusAnimationLabel)
                .addContainerGap())
        );
        statusPanelLayout.setVerticalGroup(
            statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(statusPanelLayout.createSequentialGroup()
                .addComponent(statusPanelSeparator, javax.swing.GroupLayout.PREFERRED_SIZE, 2, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addGroup(statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(statusMessageLabel)
                    .addComponent(statusAnimationLabel)
                    .addComponent(progressBar, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(3, 3, 3))
        );

        setComponent(mainPanel);
        setMenuBar(menuBar);
        setStatusBar(statusPanel);
    }// </editor-fold>//GEN-END:initComponents

    private void btnResultResetActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnResultResetActionPerformed
        // TODO add your handling code here:
        jouer.resetResult();
        lblLost.setText(jouer.bwin + "");
        lblWin.setText(jouer.awin + "");
        lblNULL.setText(jouer.nowin + "");
        lblLostPercent.setText(jouer.bwin + "%");
        lblWinPercent.setText(jouer.awin + "%");
        lblNULLPercent.setText(jouer.nowin + "%");
    }//GEN-LAST:event_btnResultResetActionPerformed

    private void btnLoadPlayerActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnLoadPlayerActionPerformed
        // load file player
        javax.swing.JFileChooser filedialog = new JFileChooser(".");
        filedialog.showOpenDialog(this.mainPanel);
        java.io.File file = filedialog.getSelectedFile();
        String path = file.getAbsolutePath();
        jouer.LoadPlayer(path);
    }//GEN-LAST:event_btnLoadPlayerActionPerformed

    private void btnSavePlayerActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSavePlayerActionPerformed
        // save file player
        javax.swing.JFileChooser filedialog = new JFileChooser(".");
        filedialog.showSaveDialog(this.mainPanel);
        java.io.File file = filedialog.getSelectedFile();
        String path = file.getAbsolutePath();
        jouer.SaveMe(path);
    }//GEN-LAST:event_btnSavePlayerActionPerformed

    private void btnLearnMatchActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnLearnMatchActionPerformed
        int times = Integer.parseInt(this.txtLearnMatch.getText());
        jouer.epsilon = Double.parseDouble(txtEpsilon.getText());
        jouer.gamma = Double.parseDouble(txtGamma.getText());
        jouer.opponent = JoueurLoader.load("RandomActionJoueur");
        jouer.isLearning=true;
        jouer.setTextOutput(jTextArea1);
        jouer.NewMatch(times);

        lblLost.setText(jouer.bwin + "");
        lblWin.setText(jouer.awin + "");
        lblNULL.setText(jouer.nowin + "");
        double total = jouer.bwin + jouer.awin + jouer.nowin;
        lblLostPercent.setText((int) (100 * jouer.bwin / total) + "%");
        lblWinPercent.setText((int) (100 * jouer.awin / total) + "%");
        lblNULLPercent.setText((int) (100 * jouer.nowin / total) + "%");
    }//GEN-LAST:event_btnLearnMatchActionPerformed

    private void btnNextActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnNextActionPerformed
        //drawCanvas((state++)%9);
        jouer.epsilon = Double.parseDouble(txtEpsilon.getText());
        jouer.gamma = Double.parseDouble(txtGamma.getText());
        jouer.setTextOutput(jTextArea1);
        int humanPlay = Integer.parseInt(txtHumanInput.getText());
        if (jouer.currentMoneyB > 0) {
            if (humanPlay > jouer.currentMoneyB || humanPlay == 0) {
                javax.swing.JOptionPane.showMessageDialog(mainPanel, "Human Input must be positive and small than Human Money");
                return;
            }
        } else {
            txtHumanInput.setText("0");
            humanPlay = 0;
        }
        jouer.playHuman(humanPlay);
        lblCurrentHumanMoney.setText(jouer.currentMoneyB + "");
        lblCurrentMachineMoney.setText(jouer.currentMoneyA + "");
        lblHumanPos.setText(jouer.currentStateB + "");
        lblMachinePos.setText(jouer.currentStateA + "");
//        int curstate = jouer.currentStateA * 3 + jouer.currentStateB;
        drawCanvas(jouer.currentStateA, jouer.currentStateB);
        if (jouer.isMatchHumanEnd) {
            btnNext.setEnabled(false);
            lblLost.setText(jouer.bwin + "");
            lblWin.setText(jouer.awin + "");
            lblNULL.setText(jouer.nowin + "");
            double total = jouer.bwin + jouer.awin + jouer.nowin;
            lblLostPercent.setText((int) (100 * jouer.bwin / total) + "%");
            lblWinPercent.setText((int) (100 * jouer.awin / total) + "%");
            lblNULLPercent.setText((int) (100 * jouer.nowin / total) + "%");
        }
    }//GEN-LAST:event_btnNextActionPerformed

    private void btnNewMatchActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnNewMatchActionPerformed
        // TODO add your handling code here:
        jouer.reset();
        jouer.epsilon = Double.parseDouble(txtEpsilon.getText());
        jouer.gamma = Double.parseDouble(txtGamma.getText());
        jouer.setTextOutput(jTextArea1);
//        int humanPlay = Integer.parseInt(txtHumanInput.getText());


        lblCurrentHumanMoney.setText(jouer.currentMoneyB + "");
        lblCurrentMachineMoney.setText(jouer.currentMoneyA + "");
        lblHumanPos.setText(jouer.currentStateB + "");
        lblMachinePos.setText(jouer.currentStateA + "");
//        int curstate = jouer.currentStateA * 3 + jouer.currentStateB;
        jouer.isMatchHumanEnd = false;
//        drawCanvas(curstate);
        drawCanvas(jouer.currentStateA, jouer.currentStateB);
        btnNext.setEnabled(true);
    }//GEN-LAST:event_btnNewMatchActionPerformed

    private void btnTestMatchActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnTestMatchActionPerformed
        int times = Integer.parseInt(this.txtTestMatch.getText());
//        jouer.epsilon = Double.parseDouble(txtEpsilon.getText());
//        jouer.gamma = Double.parseDouble(txtGamma.getText());
        jouer.opponent = JoueurLoader.load(cboOpponent.getSelectedItem().toString());
        jouer.isLearning = false;
        jouer.setTextOutput(jTextArea1);
        jouer.NewMatch(times);

        lblLost.setText(jouer.bwin + "");
        lblWin.setText(jouer.awin + "");
        lblNULL.setText(jouer.nowin + "");
        double total = jouer.bwin + jouer.awin + jouer.nowin;
        lblLostPercent.setText((int) (100 * jouer.bwin / total) + "%");
        lblWinPercent.setText((int) (100 * jouer.awin / total) + "%");
        lblNULLPercent.setText((int) (100 * jouer.nowin / total) + "%");
    }//GEN-LAST:event_btnTestMatchActionPerformed

    private void btnResetPlayerActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnResetPlayerActionPerformed
        jouer.resetPlayer();
    }//GEN-LAST:event_btnResetPlayerActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btnLearnMatch;
    private javax.swing.JButton btnLoadPlayer;
    private javax.swing.JButton btnNewMatch;
    private javax.swing.JButton btnNext;
    private javax.swing.JButton btnResetPlayer;
    private javax.swing.JButton btnResultReset;
    private javax.swing.JButton btnSavePlayer;
    private javax.swing.JButton btnTestMatch;
    private java.awt.Canvas canvas1;
    private javax.swing.JComboBox cboOpponent;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel10;
    private javax.swing.JLabel jLabel11;
    private javax.swing.JLabel jLabel12;
    private javax.swing.JLabel jLabel13;
    private javax.swing.JLabel jLabel14;
    private javax.swing.JLabel jLabel15;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JLabel jLabel6;
    private javax.swing.JLabel jLabel7;
    private javax.swing.JLabel jLabel8;
    private javax.swing.JLabel jLabel9;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JTextArea jTextArea1;
    private javax.swing.JLabel lblCurrentHumanMoney;
    private javax.swing.JLabel lblCurrentMachineMoney;
    private javax.swing.JLabel lblHumanPos;
    private javax.swing.JLabel lblLost;
    private javax.swing.JLabel lblLostPercent;
    private javax.swing.JLabel lblMachinePos;
    private javax.swing.JLabel lblNULL;
    private javax.swing.JLabel lblNULLPercent;
    private javax.swing.JLabel lblWin;
    private javax.swing.JLabel lblWinPercent;
    private javax.swing.JPanel mainPanel;
    private javax.swing.JMenuBar menuBar;
    private javax.swing.JProgressBar progressBar;
    private javax.swing.JLabel statusAnimationLabel;
    private javax.swing.JLabel statusMessageLabel;
    private javax.swing.JPanel statusPanel;
    private javax.swing.JTextField txtEpsilon;
    private javax.swing.JTextField txtGamma;
    private javax.swing.JTextField txtHumanInput;
    private javax.swing.JTextField txtLearnMatch;
    private javax.swing.JTextField txtTestMatch;
    // End of variables declaration//GEN-END:variables
    private final Timer messageTimer;
    private final Timer busyIconTimer;
    private final Icon idleIcon;
    private final Icon[] busyIcons = new Icon[15];
    private int busyIconIndex = 0;
    private JDialog aboutBox;
}
