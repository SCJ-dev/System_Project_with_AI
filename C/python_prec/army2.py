# 적 현재 미사일 위치 탐지 및 요격

import random, time

chunmu = 5

while True:
    danger = random.randint(1, 10)
    target= random.uniform(10.0, 50.0)
    yo = random.uniform(10.0, 50.0)

    if chunmu == 0:
            print("미사일이 전부 소진되었습니다..")
            print("프로그램을 종료하겠습니다..")
            break
    
    if danger >= 5:
        print(f"[경고] 현재 위험도: {danger}% | 적 비행물체 좌표: {target:.3f}")
        print("요격 미사일 발사합니다\n")
        
        if yo - 10 <= target <= yo + 10:
            print("적 미사일 요격했습니다.")
            print("계속해서 감시하겠습니다.")
        else:
            print("미사일 요격에 실패했습니다.")
            chunmu-=1
            print(f"미사일 보유개수: {chunmu}")
            
    else:
        print(f"[안전] 현재 위험도: {danger}% 안전합니다. | 적 비행물체 좌표: {target:.3f}")
        
    time.sleep(3)