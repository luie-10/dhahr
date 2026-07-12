#include <stdio.h>
#include <windows.h>
#include <stdbool.h>

#define BOARD_SIZE 15
int Map[BOARD_SIZE][BOARD_SIZE];
int stone_count = 0;

int dx[4] = { 1, 0, 1, 1 };
int dy[4] = { 0, 1, 1, -1 };

void gotoxy(int x, int y) {
    COORD pos;
    pos.X = (short)x;
    pos.Y = (short)y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void Error(const char* msg) {
    gotoxy(0, 18); printf("                                                                    ");
    gotoxy(0, 18); printf("%s", msg);
    Sleep(1200);
    gotoxy(0, 18); printf("                                                                    ");
}

// 오프닝 주형 규칙 (1수: 천원, 2수: 1칸이내, 3수: 2칸이내)
bool IsValidOpening(int x, int y) {
    if (stone_count == 0) return (x == 7 && y == 7);
    if (stone_count == 1) return (x >= 6 && x <= 8 && y >= 6 && y <= 8);
    if (stone_count == 2) return (x >= 5 && x <= 9 && y >= 5 && y <= 9);
    return true;
}

// 특정 방향으로 연속된 돌의 개수를 세는 함수
int CountContinuousStones(int y, int x, int d, int stone) {
    int count = 1;
    // 정방향
    int len = 1;
    while (1) {
        int nx = x + dx[d] * len;
        int ny = y + dy[d] * len;
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && Map[ny][nx] == stone) count++;
        else break;
        len++;
    }
    // 역방향
    len = 1;
    while (1) {
        int nx = x - dx[d] * len;
        int ny = y - dy[d] * len;
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && Map[ny][nx] == stone) count++;
        else break;
        len++;
    }
    return count;
}

// 흑의 '열린 3' 여부를 판정하는 고도의 함수
bool IsOpenThree(int y, int x, int d) {
    // 임시로 돌을 놓아봄
    Map[y][x] = 1;

    int count = CountContinuousStones(y, x, d, 1);
    if (count != 3) {
        Map[y][x] = 0;
        return false;
    }

    // 양 끝이 비어있어 '열린 4'로 발전할 수 있는지 확인
    int left_len = 1;
    while (x - dx[d] * left_len >= 0 && y - dy[d] * left_len >= 0 && Map[y - dy[d] * left_len][x - dx[d] * left_len] == 1) left_len++;
    int lx = x - dx[d] * left_len;
    int ly = y - dy[d] * left_len;

    int right_len = 1;
    while (x + dx[d] * right_len < BOARD_SIZE && y + dy[d] * right_len < BOARD_SIZE && Map[y + dy[d] * right_len][x + dx[d] * right_len] == 1) right_len++;
    int rx = x + dx[d] * right_len;
    int ry = y + dy[d] * right_len;

    bool open_left = (lx >= 0 && lx < BOARD_SIZE && ly >= 0 && ly < BOARD_SIZE && Map[ly][lx] == 0);
    bool open_right = (rx >= 0 && rx < BOARD_SIZE && ry >= 0 && ry < BOARD_SIZE && Map[ry][rx] == 0);

    Map[y][x] = 0; // 원상복구
    return (open_left && open_right);
}

// 흑돌(1)의 금수(반칙수) 여부를 완벽하게 체크하는 함수
bool IsForbiddenMove(int y, int x) {
    int three_count = 0;
    int four_count = 0;

    for (int d = 0; d < 4; d++) {
        // 가상으로 돌을 놓았다고 가정하고 연속 개수 파악
        int count = CountContinuousStones(y, x, d, 1);

        // 1. 장목 금수 체크 (6목 이상)
        if (count >= 6) {
            return true;
        }

        // 2. 4목 카운트
        if (count == 4) {
            four_count++;
        }

        // 3. 3목 카운트 (열린 3이어야 함)
        if (count == 3) {
            if (IsOpenThree(y, x, d)) {
                three_count++;
            }
        }
    }

    // 3·3 금수 및 4·4 금수 적용
    if (three_count >= 2) return true;
    if (four_count >= 2) return true;

    return false;
}

// 승리 판정 (흑은 정확히 5개만 가능, 백은 5개 및 장목 가능)
bool CheckWin(int y, int x, int stone) {
    for (int d = 0; d < 4; d++) {
        int count = CountContinuousStones(y, x, d, stone);

        if (stone == 1 && count == 5) return true;       // 흑은 정확히 5개일 때만 승리
        if (stone == 2 && count >= 5) return true;       // 백은 5개 이상(장목 포함) 승리
    }
    return false;
}

int main() {
    printf("   ");
    for (int i = 1; i <= BOARD_SIZE; i++) printf("%2d", i);
    printf("\n");

    for (int i = 1; i <= BOARD_SIZE; i++) {
        printf("%2d ", i);
        for (int j = 0; j < BOARD_SIZE; j++) printf("- ");
        printf("\n");
    }

    while (1) {
        int x = 0, y = 0;
        int turn = (stone_count % 2 == 0) ? 1 : 2; // 1: 흑(●), 2: 백(○)

        gotoxy(40, 2); printf("현재 차례: %s", turn == 1 ? "흑 (●)" : "백 (○)");
        gotoxy(0, 17); printf("                                                                    ");
        gotoxy(0, 17); printf("좌표 입력 (X,Y 예: 8,8): ");

        if (scanf_s("%d,%d", &x, &y) != 2) {
            while (getchar() != '\n');
            Error("올바른 숫자 형식이 아닙니다 (예: 8,8)");
            continue;
        }

        int map_x = x - 1;
        int map_y = y - 1;

        if (map_x < 0 || map_x >= BOARD_SIZE || map_y < 0 || map_y >= BOARD_SIZE) {
            Error("범위를 벗어났습니다 (1 ~ 15 사이 입력)");
            continue;
        }

        if (Map[map_y][map_x] != 0) {
            Error("이미 돌이 놓인 자리입니다");
            continue;
        }

        // 1. 오프닝 룰 체크
        if (!IsValidOpening(map_x, map_y)) {
            Error("렌주룰 오프닝 위반입니다 (초반 배치 제한)");
            continue;
        }

        // 2. 흑의 경우 금수 룰 체크
        if (turn == 1) {
            if (IsForbiddenMove(map_y, map_x)) {
                Error("★렌주룰 금수(3·3 / 4·4 / 장목) 위반입니다! 둘 수 없습니다.★");
                continue;
            }
        }

        // 돌 놓기
        Map[map_y][map_x] = turn;
        gotoxy(map_x * 2 + 3, map_y + 1);
        printf("%s", turn == 1 ? "●" : "○");

        // 승리 판정
        if (CheckWin(map_y, map_x, turn)) {
            gotoxy(0, 19);
            printf("결과: %s 승리! 게임이 종료됩니다.\n", turn == 1 ? "흑돌(●)" : "백돌(○)");
            break;
        }

        stone_count++;
    }

    return 0;
}