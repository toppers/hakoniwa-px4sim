#include "hako_asset.h"
#include "hako_msgs/pdu_ctype_ManualPosAttControl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <thread>
#include <atomic>
#include "utils/hako_osdep.h"

#define PDU_CHANNEL_ID    3
#define DEGREE2RADIAN(v)    ( (v) * M_PI / (180.0) )
static int my_on_initialize(hako_asset_context_t*)
{
    return 0;
}
static int my_on_reset(hako_asset_context_t*)
{
    return 0;
}

static int my_on_manual_timing_control(hako_asset_context_t*)
{
    int result = 0;
    while (result == 0) {
        result = hako_asset_usleep(3000);
        if (result != 0) {
            break;
        }
    }
    return 0;
}


struct Angles {
    float roll = 0.0;
    float pitch = 0.0;
    float yaw = 0.0;

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
    void set(const char axis, const float angle) {
        switch (axis)
        {
            case 'r': roll = angle; break;
            case 'p': pitch = angle; break;
            case 'y': yaw = angle; break;
            default: break;
        }
    }

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
    std::string lastInput;

    help();
    while (running) {

        std::getline(std::cin, input);
        if (input.empty()) {
            if (lastInput.empty()) {
                angles.reset();
            } else {
                input = lastInput;
            }
        }
        if (input.size() > 1 && isalpha(input[0]) && isdigit(input[1])) {
            char axis = input[0];
            float angle = std::stof(input.substr(1));
            angles.set(axis, angle);
        }
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
        tmp.posatt.angular.x = DEGREE2RADIAN(angles.roll);
        tmp.posatt.angular.y = DEGREE2RADIAN(angles.pitch);
        tmp.posatt.angular.z = DEGREE2RADIAN(angles.yaw);
        tmp.do_operation = true;
        int ret = hako_asset_pdu_write("Robot", PDU_CHANNEL_ID, (const char*)(&tmp), sizeof(tmp));
        if (ret != 0) {
            printf("ERROR: hako_asset_pdu_write erro: %d\n", ret);
        }
        printf("manual set\n");
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


    std::thread inputThread(keyboardInputHandler, std::ref(running), 5.0);

    int ret = hako_asset_register(asset_name, config_path, &my_callback, delta_time_usec, HAKO_ASSET_MODEL_CONTROLLER);
    if (ret != 0) {
        printf("ERORR: hako_asset_register() returns %d.", ret);
        return 1;
    }
    ret = hako_asset_start();
    printf("INFO: hako_asset_start() returns %d\n", ret);

    return 0;
}
