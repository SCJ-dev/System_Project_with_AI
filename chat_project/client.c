#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // read, write, close
#include <arpa/inet.h> // socket, bind, accept
#include <sys/socket.h>

int main(){
  // 1. 변수 준비 (내 전화기, 친구 전화번호부, 문자수 지정)
  int myphone; // 내 전화기
  struct sockaddr_in fre_teldir; // 친구 전화번호부
  char message[1024]; // 버퍼 크기

  // 2. 전화기 구입
  myphone = socket(PF_INET, SOCK_STREAM, 0); // myphone 변수명 안에 소캣을 지정함(기본 주소체계, 데이터 서비스타입 지정, 자동으로 특정 프로토콜 지정)

  // 3. 친구 번호 적기
  memset(&fre_teldir, 0, sizeof(fre_teldir)); // memset(메모리 시작 주소를 가져와야함(이러한 이유로 '&'가 붙음, 메모리를 채울 바이트 값(0으로 해야 안전함), 메모리 채울 바이트 값이 가르키는 주소부터 채울 바이트 수)
  fre_teldir.sin_family = AF_INET; // IPv4 주소 체계 지정
  fre_teldir.sin_port = htons(9000); // 포트번호 9000번 사용
  fre_teldir.sin_addr.s_addr = inet_addr("127.0.0.1"); // 접속할 상대방의 특정 IP 주소 지정

  // 4. 전화 걸기(연결요청)
  if(connect(myphone, (struct sockaddr*)&fre_teldir, sizeof(fre_teldir)) == -1){ // connect(내 전화기, 친구의 전화번호부, 전화번호부 정보의 길이) == -1 통화 시도에 성공/실패 여부
    printf("connect Error\n");
    return 1; // 에러 발생시 프로그램을 종료시킴
  } else {
      printf("✅ 연결에 성공하였습니다!\n");
  }

  // 5. 대화 하기 (반복문 추가)
  while(1) {
    printf(" 나 : "); // 처음에는 "나 : "하고 커서가 깜빡임
    //키보드 입력 받기
    fgets(message, 1024, stdin); // 문자 길이 1024

    if (!strcmp(message, "q\n")) break; // q누르면 종료
  
    write(myphone, message, strlen(message));

    int str_len = read(myphone, message, 1024-1); // 한 번에 최대로 읽을 수 있는 바이트의 수는 0~1023
    if(str_len == 0) break; // 서버가 전화를 뚝 끊었을 때(read 함수는 읽은 글자 수를 반환하는데 0이 나오면 연결이 끊김)

    message[str_len] = 0; // 
    printf("서버 : %s", message);
  }
    
  
  // 6. 전화기 닫기 (함수가 아니라 소켓을 닫아야 함)
  close(myphone);
}
