# 컴파일러 및 옵션 설정
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lrt

# 생성할 실행 파일(타겟) 목록
TARGETS = main th_module vital_module db_module send_module

# 모든 타겟 빌드
all: $(TARGETS)

# 1. 마스터 관리 프로세스 빌드
main: main.c main.h
	$(CC) $(CFLAGS) -o main main.c $(LDFLAGS)

# 2. 온·습도 수집 모듈 빌드
th_module: th_module.c main.h
	$(CC) $(CFLAGS) -o th_module th_module.c $(LDFLAGS)

# 3. 워치 데이터 수집 모듈 빌드
vital_module: vital_module.c main.h
	$(CC) $(CFLAGS) -o vital_module vital_module.c $(LDFLAGS)

# 4. 내/외부 DB 적재 모듈 빌드
db_module: db_module.c main.h
	$(CC) $(CFLAGS) -o db_module db_module.c $(LDFLAGS)

# 5. 판단 정보 전송 모듈 빌드
send_module: send_module.c main.h
	$(CC) $(CFLAGS) -o send_module send_module.c $(LDFLAGS)

# 빌드 결과물 삭제 (정리)
clean:
	rm -f $(TARGETS)
	@echo "모든 바이너리 파일이 삭제되었습니다."

# 실행 방법 안내
help:
	@echo "사용 가능한 명령어:"
	@echo "  make       : 모든 모듈 빌드"
	@echo "  make clean : 빌드된 파일 삭제"
