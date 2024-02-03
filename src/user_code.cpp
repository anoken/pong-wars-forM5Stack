
#include <M5Unified.h>

// 画像の幅と高さ、四角形のサイズ、四角形の数を定義
int img_width = 320;
int img_height = 240;
int SQUARE_SIZE = 16;
int numSquaresX = img_width / SQUARE_SIZE + 1;
int numSquaresY = img_height / SQUARE_SIZE + 1;

// 四角形の位置情報を格納する2次元ベクトル
std::vector<std::vector<int>> squares;

// M5UnifiedのCanvasオブジェクト
M5Canvas canvas(&M5.Display);

// ボールの情報を格納する構造体のベクトル
struct _pong {
    double x;
    double y;
    double dx;
    double dy;
    int class_num;
};
double vel = SQUARE_SIZE / 2;

std::vector<_pong> pong;

// 乱数生成関数
double randomNum(double min, double max) {
    int rand_int = rand() % 100;
    double rand_uni = (rand_int / 100.0) * (max - min) + min;
    return rand_uni;
}
// 数値の符号を返す関数
double sign(double A) {
    if(A == 0)
        return 0;
    else
        return A / abs(A);
}

// 四角形とボールの衝突判定と反射を処理する関数
int updateSquareAndBounce(int numSquaresX, int numSquaresY, double x, double y, double &dx, double &dy, int class_num) {
    double updatedDx = dx;
    double updatedDy = dy;

    // ボールの円周上の複数のポイントをチェックする
    for(double angle = 0; angle < M_PI * 2; angle += M_PI / 4) {
        int checkX = x + cos(angle) * (SQUARE_SIZE / 2);
        int checkY = y + sin(angle) * (SQUARE_SIZE / 2);

        // チェックしたポイントが画面内かどうか確認
        int i = floor(checkX / SQUARE_SIZE);
        int j = floor(checkY / SQUARE_SIZE);

        if(i >= 0 && i < numSquaresX && j >= 0 && j < numSquaresY) {
            if(squares[i][j] != class_num) {
                squares[i][j] = class_num;

                // 角度からバウンド方向を決定
                if(abs(cos(angle)) > abs(sin(angle))) {
                    updatedDx = -updatedDx;
                } else {
                    updatedDy = -updatedDy;
                }
            }
        }
    }

    // ボールがループにはまらないように、バウンドにノイズを加える
    double theta = M_PI / 4 * (1 + randomNum(-0.1, 1));
    dx = sign(updatedDx) * vel * cos(theta);
    dy = sign(updatedDy) * vel * sin(theta);

    return 0;
}

// 画面の境界とボールの衝突を判定し、必要に応じて反射させる関数
int checkBoundaryCollision(int image_width, int img_height, double x, double y, double &dx, double &dy) {
    if(x + dx > image_width - SQUARE_SIZE / 2 || x + dx < SQUARE_SIZE / 2) {
        dx = -dx;
    }
    if(y + dy > img_height - SQUARE_SIZE / 2 || y + dy < SQUARE_SIZE / 2) {
        dy = -dy;
    }
    return 0;
}

// M5Stackデバイスの初期化や位置情報の設定を行うセットアップ関数
void setup(void) {
    auto cfg = M5.config();
    M5.begin(cfg);
    canvas.createSprite(320, 240);

    // 四角形の位置情報を初期化
    squares.resize(numSquaresX);
    for(int i = 0; i < numSquaresX; i++) {
        squares[i].resize(numSquaresY);
    }

    // 画面を4つのエリアに分割し、それぞれに異なるクラスの四角形を配置
    int pong_no = 4;
    pong.resize(pong_no);

    for(int i = 0; i < numSquaresX; i++) {
        for(int j = 0; j < numSquaresY; j++) {
            if(i <= (numSquaresX / 2)) {
                squares[i][j] = 0;
            } else {
                squares[i][j] = 1;
            }
            if((i <= (numSquaresX / 2)) && (j >= (numSquaresY / 2)) && (pong_no >= 3)) {
                squares[i][j] = 2;
            }
            if((i >= (numSquaresX / 2)) && (j >= (numSquaresY / 2)) && (pong_no >= 4)) {
                squares[i][j] = 3;
            }
        }
    }

    // ボールの初期位置と速度を設定
    pong[0].x = img_width / 4;
    pong[0].y = img_height / 4;
    pong[0].dx = vel;
    pong[0].dy = -vel;
    pong[0].class_num = 0;

    pong[1].x = img_width * 3 / 4;
    pong[1].y = img_height / 4;
    pong[1].dx = -vel;
    pong[1].dy = vel;
    pong[1].class_num = 1;

    pong[2].x = img_width / 4;
    pong[2].y = img_height / 4 * 3;
    pong[2].dx = -vel;
    pong[2].dy = vel;
    pong[2].class_num = 2;

    pong[3].x = img_width * 3 / 4;
    pong[3].y = img_height / 4 * 3;
    pong[3].dx = -vel;
    pong[3].dy = vel;
    pong[3].class_num = 3;
}
void loop(void) {
    // 画面の描画処理を開始
    M5.Display.startWrite();

    // 四角形の描画
    for(int i = 0; i < squares.size(); i++) {
        for(int j = 0; j < squares[i].size(); j++) {

            // 各クラスに応じて四角形を描画
            if(squares[i][j] == 0) {
                uint16_t color = M5.Lcd.color565(237, 223, 214);
                canvas.fillRect(i * SQUARE_SIZE, j * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, color);
            }
            if(squares[i][j] == 1) {
                uint16_t color = M5.Lcd.color565(200, 100, 81);
                canvas.fillRect(i * SQUARE_SIZE, j * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, color);
            }
            if(squares[i][j] == 2) {
                uint16_t color = M5.Lcd.color565(103, 193, 173);
                canvas.fillRect(i * SQUARE_SIZE, j * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, color);
            }
            if(squares[i][j] == 3) {
                uint16_t color = M5.Lcd.color565(00, 139, 171);
                canvas.fillRect(i * SQUARE_SIZE, j * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, color);
            }
        }
    }

    // ボールの描画と更新
    for(int i = 0; i < pong.size(); i++) {
        uint16_t color = M5.Lcd.color565(237, 223, 214);

        // 各クラスに応じてボールの色を設定
        if(pong[i].class_num == 0)
            color = M5.Lcd.color565(200, 100, 81);
        if(pong[i].class_num == 1)
            color = M5.Lcd.color565(237, 223, 214);
        if(pong[i].class_num == 2)
            color = M5.Lcd.color565(00, 139, 171);
        if(pong[i].class_num == 3)
            color = M5.Lcd.color565(103, 193, 173);

        // ボールを描画
        canvas.fillCircle(pong[i].x, pong[i].y, SQUARE_SIZE / 2, color);

        // 四角形との衝突判定と反射、境界との衝突判定
        updateSquareAndBounce(numSquaresX, numSquaresY, pong[i].x, pong[i].y, pong[i].dx, pong[i].dy, pong[i].class_num);
        checkBoundaryCollision(img_width, img_height, pong[i].x, pong[i].y, pong[i].dx, pong[i].dy);

        // ボールの位置を更新
        pong[i].x += pong[i].dx;
        pong[i].y += pong[i].dy;
    }

    // Canvasに描画された内容をスクリーンに転送
    canvas.pushSprite(0, 0);

    // 画面の描画処理を終了
    M5.Display.endWrite();

    // 10ミリ秒待機
    M5.delay(10);

    // M5Stackの状態を更新
    M5.update();
}

#if defined(ESP_PLATFORM) && !defined(ARDUINO)
extern "C" {
int app_main(int, char **) {
    setup();
    for(;;) {
        loop();
    }
    return 0;
}
}
#endif
