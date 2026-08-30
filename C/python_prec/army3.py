# 적 함정 주파수 탐지 프로그램 및 어뢰 공격 시스템
import random, time

contacts = {501: ["적 상어급 잠수함", 120.5], 502: ["적 유구급 호위함", 85.0],
    503: ["아군 경비함", 60.0]}

boom = 3
ID = 504

while True:
    try:
        print("========== 잠수함 음향 탐지 및 어뢰 공격 시스템 ==========")
        print("1. 음향 탐지 현황 출력\n2. 실시간 자동 수역 수색\n3. 어뢰 발사 및 타격\n4. 작전 보고서 저장\n5. 어뢰 재장전\n0. 작전 종료")
        num = int(input("숫자를 입력하세요> "))
        match num:
            case 1:
                for k, v in contacts.items():
                    print(f"ID: {k} | 함선: {v[0]} | 주파수: {v[1]}")
                print("\n")
            case 2:
                ran = round(random.uniform(0.0, 150.0), 3)
                print("3초 동안 음파 탐지하겠습니다...")
                time.sleep(3)
                if (ran > 50.0):
                    print(f"[경고] 적 주파수의 신호를 가진 잠수함이 식별되었습니다!\n 주파수 번호: {ran}\n해당 주파수를 가진 함선을 ID {ID}로 기록하겠습니다.")
                    ans = input("해당 잠수함의 이름을 입력해주십시오> ")
                    contacts[ID] = [ans, ran]
                    ID+=1
                    print(f"저장 완료되었습니다!")
                    
                else:
                    print(f"주파수: {ran}은 아군 잠수함입니다. ")
                
            case 3:
                att = int(input("공격할 함선 ID를 입력하십시오> "))
                ran = random.randint(1, 500)
                if (ran > 250):
                    print(f"함선: {att}는 현재 우리 함선과 {ran}km 떨어져있으므로 요격 거리를 벗어나있습니다...")
                else:
                    print(f"함선: {contacts[att]}와 요격 거리 {ran}km")
                    an = int(input("요격을 시도하시겠습니까?(0=취소/1=공격)>"))
                    if(an == 0):
                        print("공격을 취소합니다..")
                    elif(an == 1):
                        print("공격을 시도하겠습니다!")
                        if (boom > 0):
                            boom -= 1
                            print(f"{contacts[att]}에게 어뢰 발사!\n남은 어뢰 갯수: {boom}")
                            time.sleep(3)
                            ren = random.randint(1,2)
                            if(ren == 1):
                                print(f"[실패] {att}에게 요격이 실패하였습니다..")
                            else:
                                print(f"[성공] [{contacts[att][0]}]에게 요격 성공하였습니다!")
                                contacts.pop(att)
                        else:
                            print("[경고] 어뢰 개수가 남아있지 않습니다!")

            case 4:
                print("===== 현재 적 잠수함 목록 =====")
                for k, v in contacts.items():
                    print(f"ID: {k} | 함선: {v[0]} | 주파수: {v[1]}")
                print(f"남은 어뢰 개수: {boom}")

                print("\n sonar_log.txt에 저장 중...")
                a = open("sonar_log.txt", "w", encoding="utf-8")
                a.write("===== 현재 적 잠수함 목록 =====")
                for k, v in contacts.items():
                    a.write(f"\nID: {k} | 함선: {v[0]} | 주파수: {v[1]}")
                print(f"\n남은 어뢰 개수: {boom}")
                a.write(f"\n남은 어뢰 개수: {boom}")
                a.close()

            case 5:
                
                if boom < 3:
                    print("어뢰를 재장전합니다!")
                    boom += 3
                    print(f"현재 남은 어뢰 개수 {boom}")
                else:
                    print(f"최대 어뢰 장전 갯수는 5개입니다.\n현재 {boom}개가 장전되어있습니다.")


            case 0:
                print("프로그램을 종료합니다..")
                break

    except ValueError:
        print("타입에 맞는 숫자 또는 문자를 넣어주세요..")