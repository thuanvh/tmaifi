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

    public void drawCanvas(int stateA,int stateB) {
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
        btnReset = new javax.swing.JButton();
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
        btnRun = new javax.swing.JButton();
        txtMatchNumber = new javax.swing.JTextField();
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
        btnReset.setText(resourceMap.getString("btnReset.text")); // NOI18N
        btnReset.setName("btnReset"); // NOI18N
        btnReset.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                btnResetMouseClicked(evt);
            }
        });

        btnNext.setText(resourceMap.getString("btnNext.text")); // NOI18N
        btnNext.setName("btnNext"); // NOI18N
        btnNext.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                btnNextMouseClicked(evt);
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
        btnLoadPlayer.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                btnLoadPlayerMouseClicked(evt);
            }
        });

        btnSavePlayer.setLabel(resourceMap.getString("btnSavePlayer.label")); // NOI18N
        btnSavePlayer.setName("btnSavePlayer"); // NOI18N
        btnSavePlayer.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                btnSavePlayerMouseClicked(evt);
            }
        });

        btnRun.setLabel(resourceMap.getString("btnRun.label")); // NOI18N
        btnRun.setName("btnRun"); // NOI18N
        btnRun.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                btnRunMouseClicked(evt);
            }
        });

        txtMatchNumber.setText(resourceMap.getString("txtMatchNumber.text")); // NOI18N
        txtMatchNumber.setName("txtMatchNumber"); // NOI18N

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

        javax.swing.GroupLayout mainPanelLayout = new javax.swing.GroupLayout(mainPanel);
        mainPanel.setLayout(mainPanelLayout);
        mainPanelLayout.setHorizontalGroup(
            mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(mainPanelLayout.createSequentialGroup()
                .addGap(22, 22, 22)
                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addGroup(mainPanelLayout.createSequentialGroup()
                        .addComponent(canvas1, javax.swing.GroupLayout.PREFERRED_SIZE, 184, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jScrollPane1))
                    .addGroup(mainPanelLayout.createSequentialGroup()
                        .addGap(6, 6, 6)
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
                                    .addComponent(lblHumanPos)))
                            .addGroup(mainPanelLayout.createSequentialGroup()
                                .addGap(12, 12, 12)
                                .addComponent(btnNext)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                .addComponent(btnReset)))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addGroup(mainPanelLayout.createSequentialGroup()
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(lblCurrentMachineMoney)
                                    .addComponent(txtHumanInput, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                                .addGap(109, 109, 109)
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                                    .addGroup(mainPanelLayout.createSequentialGroup()
                                        .addComponent(jLabel1)
                                        .addGap(16, 16, 16)
                                        .addComponent(txtEpsilon, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                                    .addGroup(mainPanelLayout.createSequentialGroup()
                                        .addComponent(jLabel2)
                                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                        .addComponent(txtGamma)))
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                                    .addGroup(mainPanelLayout.createSequentialGroup()
                                        .addComponent(jLabel3)
                                        .addGap(32, 32, 32)
                                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                            .addComponent(jLabel5)
                                            .addComponent(jLabel4)))
                                    .addComponent(jLabel6))
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(lblNULL)
                                    .addComponent(lblLost)
                                    .addComponent(lblWin)))
                            .addGroup(mainPanelLayout.createSequentialGroup()
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(lblCurrentHumanMoney)
                                    .addGroup(mainPanelLayout.createSequentialGroup()
                                        .addGap(3, 3, 3)
                                        .addComponent(jLabel12, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE)))
                                .addGap(39, 39, 39)
                                .addComponent(btnLoadPlayer)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                .addComponent(btnSavePlayer)
                                .addGap(18, 18, 18)
                                .addComponent(jLabel11)
                                .addGap(18, 18, 18)
                                .addComponent(txtMatchNumber, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addComponent(btnRun)))))
                .addContainerGap())
        );
        mainPanelLayout.setVerticalGroup(
            mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(mainPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 121, Short.MAX_VALUE)
                    .addComponent(canvas1, javax.swing.GroupLayout.PREFERRED_SIZE, 121, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                        .addComponent(btnRun)
                        .addComponent(txtMatchNumber, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addComponent(jLabel11)
                        .addComponent(btnSavePlayer)
                        .addComponent(btnLoadPlayer))
                    .addGroup(mainPanelLayout.createSequentialGroup()
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(jLabel8)
                            .addComponent(jLabel12))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(jLabel9)
                            .addComponent(lblHumanPos)
                            .addComponent(lblCurrentHumanMoney))))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(mainPanelLayout.createSequentialGroup()
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(jLabel3)
                            .addComponent(jLabel4)
                            .addComponent(lblWin)
                            .addComponent(txtEpsilon, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(jLabel1))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                .addComponent(jLabel2)
                                .addComponent(txtGamma, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                            .addGroup(mainPanelLayout.createSequentialGroup()
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                    .addComponent(jLabel5)
                                    .addComponent(lblLost))
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addGroup(mainPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                                    .addComponent(lblNULL)
                                    .addComponent(jLabel6)))))
                    .addGroup(mainPanelLayout.createSequentialGroup()
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
                            .addComponent(btnReset)
                            .addComponent(btnNext))))
                .addContainerGap(63, Short.MAX_VALUE))
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
            .addComponent(statusPanelSeparator, javax.swing.GroupLayout.DEFAULT_SIZE, 599, Short.MAX_VALUE)
            .addGroup(statusPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(statusMessageLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 413, Short.MAX_VALUE)
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

    private void btnNextMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_btnNextMouseClicked
        //drawCanvas((state++)%9);
        jouer.epsilon = Double.parseDouble(txtEpsilon.getText());
        jouer.gamma = Double.parseDouble(txtGamma.getText());
        jouer.setTextOutput(jTextArea1);
        int humanPlay = Integer.parseInt(txtHumanInput.getText());
        if (jouer.currentMoneyB > 0) {
            if (humanPlay > jouer.currentMoneyB) {
                javax.swing.JOptionPane.showMessageDialog(mainPanel, "Human Input has to be small than Human Money");
                return;
            }
        }else{
            txtHumanInput.setText("0");
            humanPlay=0;
        }
        jouer.playHuman(humanPlay);
        lblCurrentHumanMoney.setText(jouer.currentMoneyB + "");
        lblCurrentMachineMoney.setText(jouer.currentMoneyA + "");
        lblHumanPos.setText(jouer.currentStateB + "");
        lblMachinePos.setText(jouer.currentStateA + "");
//        int curstate = jouer.currentStateA * 3 + jouer.currentStateB;
        drawCanvas(jouer.currentStateA,jouer.currentStateB);
        if (jouer.isMatchHumanEnd) {
            btnNext.setEnabled(false);
        }
    }//GEN-LAST:event_btnNextMouseClicked

    private void btnLoadPlayerMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_btnLoadPlayerMouseClicked
        // load file player
        javax.swing.JFileChooser filedialog = new JFileChooser(".");
        filedialog.showOpenDialog(this.mainPanel);
        java.io.File file = filedialog.getSelectedFile();
        String path = file.getAbsolutePath();
        jouer.LoadPlayer(path);
    }//GEN-LAST:event_btnLoadPlayerMouseClicked

    private void btnSavePlayerMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_btnSavePlayerMouseClicked
        // save file player
        javax.swing.JFileChooser filedialog = new JFileChooser(".");
        filedialog.showSaveDialog(this.mainPanel);
        java.io.File file = filedialog.getSelectedFile();
        String path = file.getAbsolutePath();
        jouer.SaveMe(path);
    }//GEN-LAST:event_btnSavePlayerMouseClicked

    private void btnRunMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_btnRunMouseClicked
        // run 1000 times
        int times = Integer.parseInt(this.txtMatchNumber.getText());
        jouer.epsilon = Double.parseDouble(txtEpsilon.getText());
        jouer.gamma = Double.parseDouble(txtGamma.getText());
        jouer.NewMatch(times);

        lblLost.setText(jouer.bwin + "");
        lblWin.setText(jouer.awin + "");
        lblNULL.setText(jouer.nowin + "");
    }//GEN-LAST:event_btnRunMouseClicked

    private void btnResetMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_btnResetMouseClicked
        // TODO add your handling code here:
        jouer.reset();
        jouer.epsilon = Double.parseDouble(txtEpsilon.getText());
        jouer.gamma = Double.parseDouble(txtGamma.getText());
        jouer.setTextOutput(jTextArea1);
        int humanPlay = Integer.parseInt(txtHumanInput.getText());


        lblCurrentHumanMoney.setText(jouer.currentMoneyB + "");
        lblCurrentMachineMoney.setText(jouer.currentMoneyA + "");
        lblHumanPos.setText(jouer.currentStateB + "");
        lblMachinePos.setText(jouer.currentStateA + "");
        int curstate = jouer.currentStateA * 3 + jouer.currentStateB;
        jouer.isMatchHumanEnd = false;
//        drawCanvas(curstate);
        drawCanvas(jouer.currentStateA,jouer.currentStateB);
        btnNext.setEnabled(true);
    }//GEN-LAST:event_btnResetMouseClicked
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btnLoadPlayer;
    private javax.swing.JButton btnNext;
    private javax.swing.JButton btnReset;
    private javax.swing.JButton btnRun;
    private javax.swing.JButton btnSavePlayer;
    private java.awt.Canvas canvas1;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel10;
    private javax.swing.JLabel jLabel11;
    private javax.swing.JLabel jLabel12;
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
    private javax.swing.JLabel lblMachinePos;
    private javax.swing.JLabel lblNULL;
    private javax.swing.JLabel lblWin;
    private javax.swing.JPanel mainPanel;
    private javax.swing.JMenuBar menuBar;
    private javax.swing.JProgressBar progressBar;
    private javax.swing.JLabel statusAnimationLabel;
    private javax.swing.JLabel statusMessageLabel;
    private javax.swing.JPanel statusPanel;
    private javax.swing.JTextField txtEpsilon;
    private javax.swing.JTextField txtGamma;
    private javax.swing.JTextField txtHumanInput;
    private javax.swing.JTextField txtMatchNumber;
    // End of variables declaration//GEN-END:variables
    private final Timer messageTimer;
    private final Timer busyIconTimer;
    private final Icon idleIcon;
    private final Icon[] busyIcons = new Icon[15];
    private int busyIconIndex = 0;
    private JDialog aboutBox;
}
