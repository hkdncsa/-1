# 项目简介

本项目是一个基于 C++ 和 EasyX 图形库开发的经典俄罗斯方块游戏。通过图形化界面展示游戏内容，实现了方块自由下落、左右移动、旋转、消行、暂停、计分、界面提示、背景音乐等核心功能。

---

##  1 需求分析

### 1.1 功能模块
<img width="865" height="424" alt="image" src="https://github.com/user-attachments/assets/6b761327-2ec6-499d-947f-0e87c450b1b2" />

（图 1.1 功能模块图）

---

### 1.2 需求说明

本项目为基于 C++ 和 EasyX 图形库开发的俄罗斯方块游戏，主要实现以下功能需求：

1. **游戏启动与界面初始化**
   - 启动程序后显示启动界面，提示“按空格键开始游戏”。
   - 背景颜色为白色，界面简洁，包含标题和操作说明。

2. **游戏主界面与交互**
   - 游戏主界面包含：
     - 主游戏区域（用于显示下落的方块）。
     - 得分区域（实时显示玩家当前得分）。
     - 操作提示区域（提示控制按键：←、→、↓、↑、空格键等）。

3. **俄罗斯方块核心玩法**
   - 方块自动从上方下落，玩家可以使用键盘进行操作：
     - ←：方块左移。
     - →：方块右移。
     - ↓：加速下落。
     - ↑：顺时针旋转方块。
     - 空格键：开始游戏或暂停/恢复游戏。
   - 方块到达底部或与其他方块接触后停止移动，并检查是否有可消除的整行。
   - 消除一行后，所有上方方块下移，同时增加得分。

4. **暂停与继续**
   - 玩家在游戏过程中按空格键可以随时暂停/恢复游戏，暂停状态下游戏区域不刷新，并显示“游戏已暂停”字样。

5. **游戏结束判断**
   - 当新的方块无法在初始位置生成时，判定为“游戏结束”。
   - 显示“游戏结束”提示，等待玩家按任意键退出或重新开始。

6. **背景音乐功能**
   - 游戏开始后自动播放背景音乐，循环播放。

---

## 2 系统设计

### 2.1 界面设计（UI）
<img width="377" height="465" alt="image" src="https://github.com/user-attachments/assets/f665bfa2-f427-4716-9b1b-6b0eee6911a8" />

（图 2.1 界面原型图）

### 2.2 流程设计
<img width="865" height="1269" alt="image" src="https://github.com/user-attachments/assets/cbecbd95-ecc7-4849-8080-115a4291f568" />

（图 2.2 系统流程图）

---

## 3 系统实现

### 3.1 实现效果
<img width="422" height="542" alt="image" src="https://github.com/user-attachments/assets/92241945-f25d-489a-aa25-dfed9a4a3f7e" />

- 图 3.1 开始界面
- <img width="435" height="548" alt="image" src="https://github.com/user-attachments/assets/3ac8e290-8051-48cd-9c00-c00e241e0879" />

- 图 3.2 游戏过程
- 
## 2 核心代码
```C++
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
