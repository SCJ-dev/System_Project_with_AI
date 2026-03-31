import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.net.*;
import javax.crypto.Cipher;
import javax.crypto.spec.SecretKeySpec;
import java.util.Base64;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.Properties;

public class Roka extends JFrame {
    private String secretKey;
    private int serverPort;

    private JTextArea logArea;
    private JLabel statusLabel;
    private JPanel statusPanel;
    // 로그 파일 이름 설정 (초소와 동일하게 맞췄습니다)
    private final String LOG_FILE_NAME = "EMERGENCYLOG.txt";

    public Roka() {
        if (!loadConfig()) {
            JOptionPane.showMessageDialog(null, "사단 설정 파일(config.properties)을 찾을 수 없습니다.");
            System.exit(0);
        }
        initUI();
        startServer();
    }

    private boolean loadConfig() {
        File configFile = new File(System.getProperty("user.dir"), "config.properties");
        try (InputStream in = new FileInputStream(configFile)) {
            Properties p = new Properties();
            p.load(in);
            serverPort = Integer.parseInt(p.getProperty("ROKA_SERVER_PORT"));
            secretKey = p.getProperty("AES_SECRET_KEY");
            return true;
        } catch (Exception e) {
            return false;
        }
    }

    private void initUI() {
        setTitle("대한민국 육군 사단 본부 보안 통제 시스템");
        setSize(600, 500);
        setDefaultCloseOperation(EXIT_ON_CLOSE);

        statusPanel = new JPanel();
        statusPanel.setBackground(new Color(30, 50, 30));
        statusLabel = new JLabel("전 부대 경계 태세 이상 없음");
        statusLabel.setForeground(Color.WHITE);
        statusLabel.setFont(new Font("맑은 고딕", Font.BOLD, 20));
        statusPanel.add(statusLabel);

        logArea = new JTextArea();
        logArea.setEditable(false);
        logArea.setBackground(Color.BLACK);
        logArea.setForeground(Color.CYAN);
        logArea.setFont(new Font("돋움체", Font.PLAIN, 14));

        add(statusPanel, BorderLayout.NORTH);
        add(new JScrollPane(logArea), BorderLayout.CENTER);
        setLocationRelativeTo(null);
        setVisible(true);
    }

    private void startServer() {
        new Thread(() -> {
            try (ServerSocket ss = new ServerSocket(serverPort)) {
                while (true) {
                    try (Socket s = ss.accept();
                         BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()))) {
                        String encryptedMsg = in.readLine();
                        if (encryptedMsg != null) {
                            String decryptedMsg = decrypt(encryptedMsg);
                            
                            // [추가된 로직] 수신된 메시지를 파일에 저장
                            saveToFile(decryptedMsg);

                            if (decryptedMsg.contains("리셋")) {
                                updateStatus(new Color(30, 50, 30), "전 부대 경계 태세 이상 없음");
                            } else {
                                updateStatus(Color.RED, "!!! 비상 상황 발생 !!!");
                            }
                            SwingUtilities.invokeLater(() -> logArea.append(decryptedMsg + "\n"));
                        }
                    } catch (Exception e) { e.printStackTrace(); }
                }
            } catch (IOException e) { 
                JOptionPane.showMessageDialog(null, "포트 충돌: 이미 서버가 실행 중이거나 포트가 점유되었습니다.");
            }
        }).start();
    }

    // [추가된 로직] 사단용 파일 저장 함수
    private void saveToFile(String msg) {
        File logFile = new File(System.getProperty("user.dir"), LOG_FILE_NAME);
        try (BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(
                new FileOutputStream(logFile, true), StandardCharsets.UTF_8))) {
            bw.write(msg); bw.newLine();
        } catch (IOException e) { e.printStackTrace(); }
    }

    private void updateStatus(Color c, String t) {
        SwingUtilities.invokeLater(() -> {
            statusPanel.setBackground(c);
            statusLabel.setText(t);
        });
    }

    private String decrypt(String data) throws Exception {
        byte[] keyBytes = Arrays.copyOf(secretKey.getBytes(StandardCharsets.UTF_8), 16);
        SecretKeySpec keySpec = new SecretKeySpec(keyBytes, "AES");
        Cipher c = Cipher.getInstance("AES");
        c.init(Cipher.DECRYPT_MODE, keySpec);
        return new String(c.doFinal(Base64.getDecoder().decode(data)), StandardCharsets.UTF_8);
    }

    public static void main(String[] args) { new Roka(); }
}
