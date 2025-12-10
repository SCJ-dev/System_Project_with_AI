#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // read, write, close
#include <arpa/inet.h> // socket, bind, accept
#include <sys/socket.h>

int main(){

  // 1. 변수 준비
  int myphone; // 내 전화기(듣는 용도)
  int frephone; // 친구 전화기(말하는 용도 - 연결되면 생성)

  struct sockaddr_in my_teldir; // 내 전화번호부 (ip, port 저장)
  struct sockaddr_in fre_teldir; // 친구 전화번호부
  socklen_t freadd_size; // 친구 주소 크기 변수

  char message[1024]; // 대화 내용을 담을 그릇
  
  // 2. 전화기 구입 (socket 함수)
  // PF_INET : IPv4 쓰겠다 / SOCK_STREAM : TOP 쓰겠다
  myphone = socket(PF_INET, SOCK_STREAM, 0);
  if(myphone == -1){
    printf("Soket Error Try Again\n");
  }
  
  //3. 내 번호 설정 (주소 구조체로 채움)
  // meset으로 구조체를 싹 0으로 비우고 시작해야 안전함(이유 : 예상치 못한 오류를 방지하고 안전한 통신을 보장하기 위해)
  memset(&my_teldir, 0, sizeof(my_teldir));
  my_teldir.sin_family = AF_INET;
  my_teldir.sin_port = htons(9000); // 포트번호 9000번을 htons로 포트번호를 변환
  my_teldir.sin_addr.s_addr = htonl(INADDR_ANY); // 내 IP 자동 할당(htonl : 컴퓨터 숫자 -> 네트워크숫자로 변환)
  //INADDR_ANY : 모든 네트워크 인터페이스로부터의 연결을 수락하도록 할 때 사용
  // 용도는 서버가 클라이언트의 연결 요청을 받을 IP 주소를 지정할 때 사
  
  //4. 개통하기 (bind 함수)
  //bind함수 : 서버 프로그램이 생성한 소켓에 IP 주소와 포트번호를 결합하여, 해당 소켓이 특정 주소와 포트로 들어오는 데이터를 수신할 수 있도록 준비하는데 사용
  // 전화기(소켓)에 전화번호(IP주소+포트번호)를 부여하는 과정
  // 내 전화기에 위에서 적은 번호표를 붙인다
  if(bind(myphone, (struct sockaddr*) & my_teldir, sizeof(my_teldir)) == -1){ // 이미 9000번 포트를 사용하고 있거나 권한이 없으면 -1이 반환
    printf("Bind Error Try Again\n");
    return 1; // <--- 프로그램 강제 종료 (안전장치)
    // ind 에러가 나도 printf만 하고 밑으로 쭉 내려가서 listen을 시도하다가 또 에러 발생
  }

  // 5. 연결 대기 (listen 함수)
  // listen함수 : 소켓의 전원을 켜고 전화를 받을 준비를 시작하는 단
  // 5명까지 대기열을 만들어줌
  if(listen(myphone, 5) == -1){
    printf("Listen Error Try Again\n");
  }
  printf(" ✉️ 채팅 서버 대기 중...(포트 : 9000)\n");

  // 6. 수화기 돌기 (accept함수 - 여기서 멈춰있음!)
  // 친구가 연결 요청하면 'frephone'라는 새로운 소켓을 만들어줌(이걸로 대화함)
  freadd_size = sizeof(fre_teldir);
  frephone = accept(myphone, (struct sockaddr*)&fre_teldir, &freadd_size);

  if(frephone == -1){
    printf("Accept Error Try Again\n");
  } else {
    printf("✅ 친구가 접속했어요 !\n");
  }
  
    // 7. 대화 나누기 (Echo)
    // 친구 말을 듣고 (read) -> 그대로 말해줌(write)
    while(1) {
    // read(소켓, 담을곳, 크기) : 친구가 보낸 데이터 읽기
    // 반환값은 읽은 글자 수. 0이면 친구가 끊은 것
    int str_len = read(frephone, message, 1024);

    if (str_len == 0) break; // 친구가 끊으면 종료
    //write(소켓, 보낼것, 크기) : 친구에게 그대로 돌려주기(Echo)
    write(frephone, message, str_len);
    // 내 화면에도 출력해보기 (문자열 끝에 0 넣어야 깨짐 방지)
    message[str_len] = 0;
    printf("받은 메시지 : %s \n", message);
    }

    // 8. 전화끊기
    close(frephone);
    close(myphone);
    return 0;
}    
