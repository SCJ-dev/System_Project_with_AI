# 간단한 적 표적 및 표적타격 프로그램
# 1. 적 표적 목록 (표적번호: [표적명, 위협등급(1~3)])
targets = {
    101: ["적 99사단 본부", 1],
    102: ["적 123레이더기지", 2],
    103: ["100사단 민경부대", 3]
}

# 2. 보유 타격 자원 (자원명: 수량)
weapons = {"K-9자주포": 12, "현무미사일": 4, "자폭드론": 8}

try: 
    while(1):
        print("=== 야전 지휘 통제 시스템 ===")
        print("1. 적 표적 현황 조회")
        print("2. 화력 타격 명령 (자원 차감)")
        print("3. 정찰 드론 신규 표적 등록")
        print("4. 작전 일지 출력 및 파일 저장 (log.txt)")
        print("0. 작전 종료")

        choice = int(input("\n번호를 입력하세요> "))

        match(choice):
            case 0:
                print("작전을 종료합니다..")
                break
            case 1:
                for k, t in targets.items():
                    print(f"{k}: {t[0]} 위험등급: {t[1]}")
            case 2:
                target = int(input("타격할 표적입력바랍니다> "))
                print("=== 타격 자원 입력 ===\n1. K-9자주포\n2. 현무미사일\n3. 자폭드론\n4. 공격취소")
                num = int(input("번호를 입력해주세요> "))
                match num:
                    case 1:
                        if weapons["K-9자주포"] == 0:
                            print("[경고] 포탄이 부족합니다!")
                        else:
                            weapons["K-9자주포"]-=1
                            print(f"{target} [{targets[target]}]에 K-9자주포로 타격 실시했습니다.\n남은 수량: {weapons['K-9자주포']}개입니다.")
                            
                    case 2:
                        if weapons["현무미사일"] == 0:
                            print("[경고] 미사일이 부족합니다!")
                        else:
                            weapons["현무미사일"]-=1
                            print(f"{target} [{targets[target]}]에 현무미사일로 타격 실시했습니다.\n남은 수량: {weapons['현무미사일']}개입니다.")

                    case 3:
                        if weapons["자폭드론"] == 0:
                            print("[경고] 드론이 부족합니다!")
                        else:
                            weapons["자폭드론"]-=1
                            print(f"{target} [{targets[target]}]에 드론으로 타격 실시했습니다.\n남은 수량: {weapons['자폭드론']}개입니다.")

                    case 4:
                        continue

            case 3:
                print("=== 신규 표적 등록 ===")
                num1 = int(input("신규 표적 번호를 입력하세요> "))
                num1_name = input("표적명을 입력해주세요> ")
                num1_level = int(input("위험등급을 입력해주세요> "))
                targets[num1] = [num1_name, num1_level]

            case 4:
                save = open("log.txt", "w", enconding="utf-8")
                save.write("=== 현재 잔여 표적 현황===")
                print("=== 현재 잔여 표적 현황 ===")
                for k, w in targets.items():
                    print(f"타겟번호: {k} | 제대명: {w[0]} | 위험등급: {w[1]}")
                    save.write(f"\n타겟번호: {k} | 제대명: {w[0]} | 위험등급: {w[1]}")

                print("\n=== 현재 화력 자원 수량 ===")
                save.write("\n\n=== 현재 화력 자원 수량 ===\n")
                for a, b in weapons.items():
                    print(f"자원명: {a} | 남은수량: {b}")
                    save.write(f"자원명: {a} | 남은수량: {b}\n")
                save.close()
                print("\n")
except ValueError:
    print("숫자만 입력바랍니다...")