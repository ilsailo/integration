# 컴파일러 설정
CC = gcc
CFLAGS = -O3 -Wall -pthread
# 라이브러리 링크 (pthread, cJSON, Real-time library)
LDFLAGS = -lpthread -lcjson -lrt

# 타겟 실행 파일 이름
TARGET = safety_system

# 소스 파일
SRCS = main.c

# 기본 빌드 규칙
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# 빌드 결과물 삭제
clean:
	rm -f $(TARGET)

# 재빌드
re: clean all