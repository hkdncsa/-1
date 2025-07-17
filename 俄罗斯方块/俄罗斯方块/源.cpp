#include <graphics.h>
#include <conio.h>
#include <ctime>
#include <windows.h>

const int BLOCK_SIZE = 30;
const int WIDTH = 10;
const int HEIGHT = 20;
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 600;

int grid[HEIGHT][WIDTH] = { 0 };
bool paused = false;
int score = 0;
bool started = false;

struct Block {
    int x, y;
    int shape[4][4];
};

Block current;

int shapes[7][4][4] = {
    { {0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0} }, // O
    { {0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0} }, // I
    { {0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0} }, // S
    { {1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0} }, // Z
    { {1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0} }, // L
    { {0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0} }, // J
    { {0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0} }  // T
};

void drawBlock(int x, int y, COLORREF color) {
    setfillcolor(color);
    setlinecolor(RGB(80, 80, 80));
    fillrectangle(x * BLOCK_SIZE, y * BLOCK_SIZE, (x + 1) * BLOCK_SIZE, (y + 1) * BLOCK_SIZE);
    rectangle(x * BLOCK_SIZE, y * BLOCK_SIZE, (x + 1) * BLOCK_SIZE, (y + 1) * BLOCK_SIZE);
}

void drawGrid() {
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            if (grid[y][x])
                drawBlock(x, y, RGB(100, 100, 255));
}

void drawCurrent() {
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            if (current.shape[y][x])
                drawBlock(current.x + x, current.y + y, RGB(255, 0, 0));
}

bool isValid(Block b) {
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++) {
            if (!b.shape[y][x]) continue;
            int newX = b.x + x;
            int newY = b.y + y;
            if (newX < 0 || newX >= WIDTH || newY >= HEIGHT) return false;
            if (newY >= 0 && grid[newY][newX]) return false;
        }
    return true;
}

void mergeBlock() {
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            if (current.shape[y][x]) {
                int gx = current.x + x;
                int gy = current.y + y;
                if (gy >= 0 && gx >= 0)
                    grid[gy][gx] = 1;
            }
}

void clearLines() {
    for (int y = HEIGHT - 1; y >= 0; y--) {
        bool full = true;
        for (int x = 0; x < WIDTH; x++) {
            if (!grid[y][x]) {
                full = false;
                break;
            }
        }
        if (full) {
            for (int yy = y; yy > 0; yy--)
                for (int x = 0; x < WIDTH; x++)
                    grid[yy][x] = grid[yy - 1][x];
            for (int x = 0; x < WIDTH; x++) grid[0][x] = 0;
            score += 100;
            y++;
        }
    }
}

void rotate(Block& b) {
    int tmp[4][4];
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            tmp[x][3 - y] = b.shape[y][x];
    memcpy(b.shape, tmp, sizeof(tmp));
}

void spawnBlock() {
    current.x = WIDTH / 2 - 2;
    current.y = 0;
    int id = rand() % 7;
    memcpy(current.shape, shapes[id], sizeof(current.shape));
}

void drawUI() {
    settextcolor(BLACK);
    settextstyle(18, 0, _T("Consolas"));
    outtextxy(WIDTH * BLOCK_SIZE + 10, 20, _T("操作说明："));
    outtextxy(WIDTH * BLOCK_SIZE + 10, 50, _T("A: 左移"));
    outtextxy(WIDTH * BLOCK_SIZE + 10, 80, _T("D: 右移"));
    outtextxy(WIDTH * BLOCK_SIZE + 10, 110, _T("S: 快速下落"));
    outtextxy(WIDTH * BLOCK_SIZE + 10, 140, _T("W: 旋转"));
    outtextxy(WIDTH * BLOCK_SIZE + 10, 170, _T("P: 暂停/继续"));

    // 分数
    TCHAR buf[64];
    _stprintf_s(buf, _T("当前得分：%d"), score);
    outtextxy(WIDTH * BLOCK_SIZE + 10, 220, buf);

    if (paused)
        outtextxy(WIDTH * BLOCK_SIZE + 10, 260, _T("游戏暂停中"));
}

void waitForStart() {
    cleardevice();
    setbkcolor(WHITE);
    settextcolor(BLACK);
    settextstyle(30, 0, _T("Consolas"));
    outtextxy(100, 250, _T("按空格键开始游戏"));
    while (true) {
        if (_kbhit()) {
            char key = _getch();
            if (key == ' ') {
                break;
            }
        }
        Sleep(10);
    }
}

int main() {
    initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
    setbkcolor(WHITE);
    cleardevice();
    srand((unsigned)time(0));

    waitForStart();
    spawnBlock();
    DWORD lastTick = GetTickCount();

    while (true) {
        // 处理按键
        if (_kbhit()) {
            char key = _getch();
            if (key == 'a' || key == 'A') {
                Block tmp = current;
                tmp.x--;
                if (isValid(tmp)) current = tmp;
            }
            else if (key == 'd' || key == 'D') {
                Block tmp = current;
                tmp.x++;
                if (isValid(tmp)) current = tmp;
            }
            else if (key == 's' || key == 'S') {
                Block tmp = current;
                tmp.y++;
                if (isValid(tmp)) current = tmp;
            }
            else if (key == 'w' || key == 'W') {
                Block tmp = current;
                rotate(tmp);
                if (isValid(tmp)) current = tmp;
            }
            else if (key == 'p' || key == 'P') {
                paused = !paused;
            }
        }

        // 自动下落
        if (!paused && GetTickCount() - lastTick > 500) {
            Block tmp = current;
            tmp.y++;
            if (isValid(tmp)) {
                current = tmp;
            }
            else {
                mergeBlock();
                clearLines();
                spawnBlock();
                if (!isValid(current)) break;
            }
            lastTick = GetTickCount();
        }

        // 绘制界面
        cleardevice();
        drawGrid();
        drawCurrent();
        drawUI();

        Sleep(20);
    }

    MessageBox(NULL, _T("游戏结束！"), _T("俄罗斯方块"), MB_OK);
    closegraph();
    return 0;
}
