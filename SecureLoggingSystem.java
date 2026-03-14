import javax.swing.*; // 윈도우 창(GUI)을 만들기 위한 도구
import java.awt.*; // 색상, 폰트 등 디자인 도구
import java.io.*; // 파일 쓰기(Writer)를 위한 도구
import java.time.LocalDateTime; // 현재 시각 가져오기
import java.time.format.DateTimeFormatter;
import java.util.Random;

public class SecureLoggingSystem extends JFrame {
    private JPanel statusPanel; // 화면 상단 배경 패널
    private JLabel statusLabel; // "정상/EMERGENCY" 글자 라벨
    private DefaultListModel<String> logModel; // 로그 목록을 저장할 리스트 모델
    private boolean isAlertActive = false; // 현재 경고 상태인지 저장

    //관제 서버와 공유할 로그 파일 이름(경로가 같아야 함)
    private final String Log_FILE_NAME = "EMERGENCYLOG.txt";

    public SecureLoggingSystem(){
        setTitle("국방 보안 시스템(기지) v1.0");
        setSize(550, 450);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); // 창 닫으면 종료
        setLayout(new BorderLayout()); // 동서남북 배치 방식 사용

        //1. 상단 상태 표시 패널(기본 회색)
        statusPanel = new JPanel();
        statusPanel.setBackground(Color.GRAY);
        statusLabel = new JLabel("시스템 대기 중...");
        statusLabel.setForeground(Color.BLACK); // 글자 색
        statusLabel.setFont(new Font("맑은 고딕", Font.BOLD, 22));
        statusPanel.add(statusLabel);
        add(statusPanel, BorderLayout.NORTH); // 북쪽에 배치

        //2. 중앙 로그 목록 (스크롤 기능 포함)
        logModel = new DefaultListModel<>();
        JList<String> loglist = new JList<>(logModel);
        add(new JScrollPane(loglist), BorderLayout.CENTER); // 중앙에 배치

        //3. 하단 경보 리셋 버튼
        JButton reseButton = new JButton("경보 해제 및 시스템 재가동");
        reseButton.addActionListener(e -> isAlertActive = false); // 클릭 시 경보 상태 꺼짐
        add(reseButton, BorderLayout.SOUTH); // 남쪽에 배치

        //프로그램 시작과 듕시에 실시간 센서 감시 시작(스레드 실행)
        startSensorMonitoring();
    }

    // [중요] 메인 화면과 별개로 돌아가는 감시 코드
    private void startSensorMonitoring(){
        new Thread(() -> {
            Random random = new Random();
            try {
                while (true) {
                    // 15% 확률로 침입자 발생(1), 없음(0)
                    int sensorValue = (random.nextInt(100) <15) ? 1 : 0;

                    if (sensorValue == 1 && !isAlertActive) {
                        triggerAlarm(); // 침입 감지 시 경보 메서드 실행
                    } else if (sensorValue == 0 && !isAlertActive) {
                        updateUI(Color.green, "상태 : 이상 없음(실시간 감시 중");
                    }
                    Thread.sleep(1000); // 1초마다 센서 체크 (부하 방지)
                }
            } catch (InterruptedException e) { e.printStackTrace(); }
        }).start();
    }

    // 침입 발생시 처리 로직
    private void triggerAlarm(){
        isAlertActive = true; // 경보 상태 켬
        // 현재 시각 포맷 설정 (ex : yyyy-MM-dd hh:mm:ss)
        String time = LocalDateTime.now().format(DateTimeFormatter.ofPattern("날짜 : yyyy-MM-dd | 시간 : HH:mm:ss"));
        String logMessage = "[" + time + "] !!! 미상물체 발견 !!!";

        updateUI(Color.RED, "!!! 경고 !!!");

        //UI 리스트에 로그 메시지 추가(맨 위에 삽입)
        SwingUtilities.invokeLater(() -> logModel.insertElementAt(logMessage, 0));

        // 텍스트 파일에 해당 로그 실시간 저장
        saveLogToFile(logMessage);
    }

    // 한글 깨지지 않게 MS949 방식으로 파일에 기록
    private void saveLogToFile(String message) {
        try (FileOutputStream fos = new FileOutputStream(Log_FILE_NAME, true);
            OutputStreamWriter osw = new OutputStreamWriter(fos, "ms949");
            BufferedWriter bw = new BufferedWriter(osw)) {

                bw.write(message); // 메시지 작성
                bw.newLine(); // 줄바꿈
                bw.flush(); // 버퍼 비우고 즉시 저장
                System.out.println("로그 저장 완료 : " + message);
            } catch (IOException e) {
                System.err.println("파일 기록 오류 : " + e.getLocalizedMessage());
            }
    }
    // UI 색상과 텍스트 변경 보조 매서드
    private void updateUI(Color color, String text) {
        SwingUtilities.invokeLater(() -> {
            statusPanel.setBackground(color);
            statusLabel.setText(text);
        });
        }
        public static void main(String[] args) {
            new SecureLoggingSystem().setVisible(true); // 프로그램 창 띄우기
        }
    
}