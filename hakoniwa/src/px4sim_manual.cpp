#include "hako_asset.h"
#include "hako_msgs/pdu_ctype_ManualPosAttControl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <atomic>

#define PDU_CHANNEL_ID    3

static int my_on_initialize(hako_asset_context_t*)
{
    return 0;
}
static int my_on_reset(hako_asset_context_t*)
{
    return 0;
}

static std::atomic<Hako_ManualPosAttControl> manual;

static int my_on_manual_timing_control(hako_asset_context_t*)
{
    printf("INFO: on_manual_timing_control enter\n");
    int result = 0;
    while (result == 0) {
        Hako_ManualPosAttControl buffer = manual;
        int ret = hako_asset_pdu_write("Robot", PDU_CHANNEL_ID, (const char*)(&buffer), sizeof(buffer));
        if (ret != 0) {
            printf("ERROR: hako_asset_pdu_write erro: %d\n", ret);
        }
        result = hako_asset_usleep(1000);
        if (result != 0) {
            break;
        }
    }
    printf("INFO: on_manual_timing_control exit\n");
    return 0;
}


// 角度を管理する構造体
struct Angles {
    float roll = 0.0;
    float pitch = 0.0;
    float yaw = 0.0;

    // 角度の増減
    void adjust(const char axis, const float delta) {
        switch (axis) {
            case 'r': roll += delta; break;
            case 'p': pitch += delta; break;
            case 'y': yaw += delta; break;
            default: break;
        }
    }
    void reset()
    {
        roll = 0.0;
        pitch = 0.0;
        yaw = 0.0;
    }
    // 角度の設定
    void set(const char axis, const float angle) {
        switch (axis) {
            case 'r': roll = angle; break;
            case 'p': pitch = angle; break;
            case 'y': yaw = angle; break;
            default: break;
        }
    }

    // 現在の角度を表示
    void print() {
        std::cout << "==> Roll: " << roll << ", Pitch: " << pitch << ", Yaw: " << yaw << std::endl;
    }
};

static void help()
{
    std::cout << "  r or R for Roll, p or P for Pitch, y or Y for Yaw, c for reset" << std::endl;
    std::cout << "  [r|p|y]<number>: Set specific angle (e.g., r45 sets Roll to 45 degrees)" << std::endl;
}

// キーボード入力を処理する関数
static void keyboardInputHandler(std::atomic<bool>& running, double deg) {
    Angles angles;
    std::string input;
    std::string lastInput;  // 前回の入力を保存する変数

    help();
    while (running) {

        std::getline(std::cin, input);
        if (input.empty()) {
            // Enterキーのみが押された場合
            if (lastInput.empty()) {
                // 前回の入力がない場合はreset
                angles.reset();
            } else {
                // 前回の入力を再利用
                input = lastInput;
            }
        }
        // 数値指定の場合 (例: r30)
        if (input.size() > 1 && isalpha(input[0]) && isdigit(input[1])) {
            char axis = input[0];
            float angle = std::stof(input.substr(1));
            angles.set(axis, angle);
        }
        // 角度の増減の場合
        else if (input == "p") {
            angles.adjust('p', deg);
        }
        else if (input == "P") {
            angles.adjust('p', -deg);
        }
        else if (input == "r") {
            angles.adjust('r', deg);
        }
        else if (input == "R") {
            angles.adjust('r', -deg);
        }
        else if (input == "y") {
            angles.adjust('y', deg);
        }
        else if (input == "Y") {
            angles.adjust('y', -deg);
        }
        else if (input == "c") {
            angles.reset();
        }
        else if (input == "h") {
            help();
        }
        lastInput = input;

        Hako_ManualPosAttControl tmp;
        tmp.posatt.angular.x = angles.roll;
        tmp.posatt.angular.y = angles.pitch;
        tmp.posatt.angular.z = angles.yaw;
        tmp.do_operation = true;
        manual = tmp;
        angles.print();

    }
}

static hako_asset_callbacks_t my_callback;

int main(int argc, const char* argv[])
{
    my_callback.on_initialize = my_on_initialize;
    my_callback.on_manual_timing_control = my_on_manual_timing_control;
    my_callback.on_simulation_step = NULL;
    my_callback.on_reset = my_on_reset;

    std::atomic<bool> running(true);
    if (argc != 2) {
        printf("Usage: %s <config_path>\n", argv[0]);
        return 1;
    }
    const char* asset_name = "ManualController";
    const char* config_path = argv[1];
    hako_time_t delta_time_usec = 3000000;//3msec

    // キーボード入力用のスレッドを作成
    std::thread inputThread(keyboardInputHandler, std::ref(running), 10.0);

    int ret = hako_asset_register(asset_name, config_path, &my_callback, delta_time_usec, HAKO_ASSET_MODEL_CONTROLLER);
    if (ret != 0) {
        printf("ERORR: hako_asset_register() returns %d.", ret);
        return 1;
    }
    ret = hako_asset_start();
    printf("INFO: hako_asset_start() returns %d\n", ret);

    return 0;
}
