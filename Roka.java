import java.io.File;
import java.io.RandomAccessFile;

public class Roka {
    public static void main(String[] args){
        //C 프로그램이 fopen으로 열었던 파일명
        File logFile = new File("EMERGENCYLOG.txt");
        // 파일의 읽기 위치를 기억하는 '커서', 이미 읽은 로그 중복 방지
        long lastKnownCursor = 0; 

        System.out.println("=======================================");
        System.out.println("      [사단지휘통제실 실시간 관제 서버]    ");
        System.out.println("    서버 상태 : 정상 가동 중 (감시 시작)    ");
        System.out.println("=======================================");

        try {
            // 관제 서버 다운이 되면 안 되므로 무한 감시
            while(true) {
                //1. 파일이 실제로 존재하는지 확인(C프로그램을 통해 먼저 파일을 생성시켜야함)
                if(logFile.exists()){

                    //2. 현재 디스크에 저장된 파일의 전체 길이 측정
                    long fileLength = logFile.length();
                    
                    //3. 현재 파일 길이가 내가 마지막으로 읽었던 지점보다 크면
                    //-> 즉, C 프로그램이 파일에 새로운 내용을 추가했다는 뜻
                    if (fileLength > lastKnownCursor) {

                        //4. 파일 읽기 전용("r")으로 엽니다.
                        //RandomAccessFile은 파일 내 어디든 점프(seek)할 수 있는 특수 도구
                        RandomAccessFile raf = new RandomAccessFile(logFile, "r");

                        //5. [지점 이동] 마지막으로 읽었던 그 지점으로 이동
                        raf.seek(lastKnownCursor);

                        String line;
                        //6. [신규 로그 추출] 이동한 지점부터 한 줄씩 끝까지 읽어옴
                        while((line = raf.readLine()) != null){

                            line = new String(line.getBytes ("ISO-8859-1"), "EUC-KR"); // 한글 깨짐 방지
                            
                            if(line.contains("미상물체") || line.contains("출현")){// emergency.txt 파일에서 미상물체, 출현이라는 키워드가 있는지 검사
                                System.out.println("\n[!]-----------------------------------[!]");
                                java.awt.Toolkit.getDefaultToolkit().beep();
                                System.out.println("  sc대대 : " + line); // C에서 남긴 타임스탬프와 메시지가 출력.
                                System.out.println("  [경고] ooGP 전방 미상 물체 식별 [경고]  ");
                                System.out.println("[!]-----------------------------------[!]");
                            }
                            else {
                                // 위험 키워드가 없는 일반 정보성 로그일 경우
                                System.out.println(">> 신규 로그 수신 : " + line);
                            }
                        }
                        // 7. [위치 업데이트] 마지막으로 읽기가 끝난 지점의 좌표를 lastKnownCursor에 저장
                        // 다음에 루프를 돌 때 이 지점부터 읽기 시작
                        lastKnownCursor = raf.getFilePointer();

                        // 8. 파일 사용 끝났으므로 자원 반납
                        raf.close();
                    }
                }
                // [부하 방지] 1초 1000ms 동안 휴식
                Thread.sleep(1000);
            }
        } catch (Exception e){
            //예외 발생 시 에러 메시지 출력
            System.out.println("관제 중 오류 발생 : " + e.getMessage());
        }
    }
}
