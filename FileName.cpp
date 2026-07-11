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

bool CheckWin(int y, int x, int stone) {
    for (int d = 0; d < 4; d++) {
        int count = 1;
        int len = 1;

        while (len < 6) {
            int nx = x + dx[d] * len;
            int ny = y + dy[d] * len;

            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && Map[ny][nx] == stone) {
                count++;
            }
            else {
                break;
            }
            len++;
        }

        if (count == 5) {
            int bx = x - dx[d];
            int by = y - dy[d];
            int ax = x + dx[d] * 5;
            int ay = y + dy[d] * 5;

            if ((bx >= 0 && bx < BOARD_SIZE && by >= 0 && by < BOARD_SIZE && Map[by][bx] == stone) ||
                (ax >= 0 && ax < BOARD_SIZE && ay >= 0 && ay < BOARD_SIZE && Map[ay][ax] == stone)) {
                continue;
            }
            return true;
        }
    }
    return false;
}

bool IsValidOpening(int x, int y) {
    if (stone_count == 0) return (x == 7 && y == 7);

    if (stone_count == 1) return (x >= 6 && x <= 8 && y >= 6 && y <= 8);

    if (stone_count == 2) {
        bool inside_two_cells = (x >= 5 && x <= 9 && y >= 5 && y <= 9);
        bool inside_one_cell = (x >= 6 && x <= 8 && y >= 6 && y <= 8);
        return inside_two_cells && !inside_one_cell;
    }

    return true;
}

void Error(const char* msg) {
    gotoxy(0, 18); printf("                                                                    ");
    gotoxy(0, 18); printf("%s", msg);
    Sleep(1200);
    gotoxy(0, 18); printf("                                                                    ");
}

int main() {
    printf("   ");
    for (int i = 1; i <= BOARD_SIZE; i++) {
        printf("%2d", i);
    }
    printf("\n");

    for (int i = 1; i <= BOARD_SIZE; i++) {
        printf("%2d ", i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("- ");
        }
        printf("\n");
    }

    while (1) {
        int x = 0, y = 0;
        int turn = (stone_count % 2 == 0) ? 2 : 1;

        gotoxy(40, 2); printf("현재 차례: %s", turn == 2 ? "흑 (○)" : "백 (●)");
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

        if (!IsValidOpening(map_x, map_y)) {
            Error("렌즈룰 오프닝 위반입니다");
            continue;
        }

        Map[map_y][map_x] = turn;

        gotoxy(map_x * 2 + 3, map_y + 1);
        printf("%s", turn == 2 ? "○" : "●");

        if (CheckWin(map_y, map_x, turn)) {
            gotoxy(0, 19);
            printf("결과: %s 승리 게임이 종료됩니다\n", turn == 2 ? "흑돌(○)" : "백돌(●)");
            break;
        }

        stone_count++;
    }

    return 0;
}