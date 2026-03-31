import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.net.Socket;
import java.util.*;
import javax.crypto.Cipher;
import javax.crypto.spec.SecretKeySpec;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.Base64;
import java.nio.charset.StandardCharsets;
import com.fazecast.jSerialComm.*;

public class SecureLoggingSystem extends JFrame {
    private String serverIp;
    private String secretKey;
    private int serverPort;
    private String targetComPort;
    
    private volatile boolean isAlertActive = false;
    private SerialPort comPort;
    private JPanel statusPanel;
    private JLabel statusLabel;
    private DefaultListModel<String> logModel = new DefaultListModel<>();
    private final String LOG_FILE_NAME = "EMERGENCYLOG.txt";

    public SecureLoggingSystem() {
        if (!loadConfig()) {
            JOptionPane.showMessageDialog(null, "초소 설정 파일(config.properties)을 찾을 수 없습니다.");
            System.exit(0);
        }
        initUI();
        startMonitoring();
    }

    private boolean loadConfig() {
        // [보안/안정성] .jar 실행 위치에서 파일을 찾도록 절대 경로 인식 보완
        File configFile = new File(System.getProperty("user.dir"), "config.properties");
        try (InputStream in = new FileInputStream(configFile)) {
            Properties p = new Properties();
            p.load(in);
            serverIp = p.getProperty("ROKA_SERVER_IP");
            serverPort = Integer.parseInt(p.getProperty("ROKA_SERVER_PORT"));
            secretKey = p.getProperty("AES_SECRET_KEY");
            targetComPort = p.getProperty("COM_PORT");
            return true;
        } catch (Exception e) {
            return false;
        }
    }

    private void initUI() {
        setTitle("무인 경계 시스템 - 초소");
        setSize(550, 450);
        setDefaultCloseOperation(EXIT_ON_CLOSE);

        statusPanel = new JPanel();
        statusPanel.setBackground(new Color(30, 50, 30)); 
        statusLabel = new JLabel("경계 감시 중 - 이상 없음");
        statusLabel.setForeground(Color.WHITE);
        statusLabel.setFont(new Font("맑은 고딕", Font.BOLD, 18));
        statusPanel.add(statusLabel);

        JList<String> logList = new JList<>(logModel);
        logList.setBackground(Color.BLACK);
        logList.setForeground(Color.GREEN);
        logList.setFont(new Font("돋움체", Font.PLAIN, 14));

        JButton btnReset = new JButton("경보 해제 및 감시 재개");
        btnReset.setFont(new Font("맑은 고딕", Font.BOLD, 14));
        btnReset.addActionListener(e -> {
            isAlertActive = false;
            if (comPort != null && comPort.isOpen()) comPort.flushIOBuffers();
            updateUI(new Color(30, 50, 30), "경계 감시 중 - 이상 없음");
            String now = LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
            String resetMsg = "[" + now + "] 시스템 리셋: 감시 재개";
            logModel.insertElementAt(resetMsg, 0);
            reportToDivision(resetMsg); 
        });

        add(statusPanel, BorderLayout.NORTH);
        add(new JScrollPane(logList), BorderLayout.CENTER);
        add(btnReset, BorderLayout.SOUTH);
        setLocationRelativeTo(null);
        setVisible(true);
    }

    private void startMonitoring() {
        new Thread(() -> {
            comPort = SerialPort.getCommPort(targetComPort); 
            comPort.setBaudRate(9600);
            comPort.setComPortTimeouts(SerialPort.TIMEOUT_NONBLOCKING, 0, 0);
            if (comPort.openPort()) {
                comPort.addDataListener(new SerialPortDataListener() {
                    @Override
                    public int getListeningEvents() { return SerialPort.LISTENING_EVENT_DATA_AVAILABLE; }
                    @Override
                    public void serialEvent(SerialPortEvent event) {
                        try {
                            Thread.sleep(20); 
                            byte[] readBuffer = new byte[comPort.bytesAvailable()];
                            int numRead = comPort.readBytes(readBuffer, readBuffer.length);
                            String line = new String(readBuffer, 0, numRead, StandardCharsets.UTF_8).trim();
                            if (line.toUpperCase().contains("ALERT") && !isAlertActive) {
                                isAlertActive = true; 
                                processIntrusion(LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss")));
                            }
                        } catch (Exception e) { e.printStackTrace(); }
                    }
                });
            } else {
                SwingUtilities.invokeLater(() -> logModel.insertElementAt("[오류] 아두이노 연결 실패: " + targetComPort, 0));
            }
        }).start();
    }

    private void processIntrusion(String time) {
        String msg = "[" + time + "] 🚨 침입 감지 : 미상물체 식별됨!";
        updateUI(Color.RED, "!!! 비상 상황 : 사단 보고 중 !!!");
        SwingUtilities.invokeLater(() -> logModel.insertElementAt(msg, 0));
        saveToFile(msg);     
        reportToDivision(msg); 
    }

    private void saveToFile(String msg) {
        // [수정] .jar 실행 위치에 로그 파일 생성 및 UTF-8 인코딩 보완
        File logFile = new File(System.getProperty("user.dir"), LOG_FILE_NAME);
        try (BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(logFile, true), StandardCharsets.UTF_8))) {
            bw.write(msg); bw.newLine();
        } catch (IOException e) { e.printStackTrace(); }
    }

    private void reportToDivision(String msg) {
        new Thread(() -> {
            try (Socket s = new Socket(serverIp, serverPort);
                 PrintWriter out = new PrintWriter(s.getOutputStream(), true)) {
                out.println(encrypt(msg));
            } catch (Exception e) { 
                SwingUtilities.invokeLater(() -> logModel.insertElementAt("[오류] 사단 보고 실패: 서버 연결 불가", 0));
            }
        }).start();
    }

    private String encrypt(String data) throws Exception {
        byte[] keyBytes = Arrays.copyOf(secretKey.getBytes(StandardCharsets.UTF_8), 16);
        SecretKeySpec keySpec = new SecretKeySpec(keyBytes, "AES");
        Cipher c = Cipher.getInstance("AES");
        c.init(Cipher.ENCRYPT_MODE, keySpec);
        return Base64.getEncoder().encodeToString(c.doFinal(data.getBytes(StandardCharsets.UTF_8)));
    }

    private void updateUI(Color c, String t) {
        SwingUtilities.invokeLater(() -> {
            statusPanel.setBackground(c);
            statusLabel.setText(t);
        });
    }

    public static void main(String[] args) { new SecureLoggingSystem(); }
}
