#ifndef _DRONE_SERVICE_RC_HPP_
#define _DRONE_SERVICE_RC_HPP_

#include "service/drone/impl/idrone_service_operation.hpp"
#include "service/drone/impl/drone_service.hpp"
#include <cstdint>
#include <thread>

namespace hako::service::impl {

#define GAME_CTRL_CHECK_COUNT_MAX       10
#define GAME_CTRL_BUTTON_RADIO_CONTROL  0
#define GAME_CTRL_BUTTON_NUM            4

#define GAME_CTRL_AXIS_LR_RR            0
#define GAME_CTRL_AXIS_UP_DOWN          1
#define GAME_CTRL_AXIS_LEFT_RIGHT       2
#define GAME_CTRL_AXIS_FORWARD_BACK     3

class DroneServiceRC: public IDroneServiceOperation {
public:
    DroneServiceRC() {}
    ~DroneServiceRC() {}

    void reset() override
    {
        radio_control_on_ = false;
        for (int i = 0; i < GAME_CTRL_BUTTON_NUM; ++i) {
            button_check_count[i] = 0;
            lastButtonState[i] = false;
        }
    }
    bool can_advanceTimeStep_for_controller() override { return radio_control_on_; }
    void setup_controller_inputs(mi_aircraft_control_in_t& in, std::array<HakoniwaDronePduDataControlType, HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM>& pdu_data) override
    {
        auto& pdu_entry = pdu_data[HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_GAME_CTRL];

        // 排他制御を行いながらデータにアクセス
        while (pdu_entry.is_busy.exchange(true)) {
            std::this_thread::yield(); // CPU負荷を軽減
        }

        // 更新がなければ処理をスキップ
        if (!pdu_entry.is_dirty) {
            pdu_entry.is_busy.store(false);
            return;
        }

        // データを取得
        const auto& game_in = pdu_entry.data.pdu.game_ctrl;

        // ボタン状態の変更をチェック
        if (is_button_state_change(game_in.button[GAME_CTRL_BUTTON_RADIO_CONTROL], GAME_CTRL_BUTTON_RADIO_CONTROL)) {
            radio_control_on_ = !radio_control_on_;
            std::cout << "radio_control: " << radio_control_on_ << std::endl;
        }

        // ラジオコントロールが有効な場合、入力を更新
        in.radio_control = (radio_control_on_ == false) ? 0 : 1;
        if (radio_control_on_) {
            in.target.attitude.roll = game_in.axis[GAME_CTRL_AXIS_LEFT_RIGHT];
            in.target.attitude.pitch = game_in.axis[GAME_CTRL_AXIS_FORWARD_BACK];
            in.target.throttle.power = game_in.axis[GAME_CTRL_AXIS_UP_DOWN];
            in.target.direction_velocity.r = game_in.axis[GAME_CTRL_AXIS_LR_RR];
        }

        // フラグをリセット
        pdu_entry.is_dirty.store(false);
        pdu_entry.is_busy.store(false);
    }

    void write_controller_pdu() override {}
private:
    bool radio_control_on_ = false;
    int button_check_count[GAME_CTRL_BUTTON_NUM] = {};
    bool lastButtonState[GAME_CTRL_BUTTON_NUM] = {};

    bool is_button_state_change(int button, uint32_t index)
    {
        if (index >= GAME_CTRL_BUTTON_NUM) {
            throw std::runtime_error("is_button_state_change index out of range");
        }
        bool currentButtonState = (button != 0);
        if (currentButtonState && !lastButtonState[index]) {
            this->button_check_count[index] = 1;
        } else if (!currentButtonState && lastButtonState[index]) {
            if (this->button_check_count[index] >= GAME_CTRL_CHECK_COUNT_MAX) {
                lastButtonState[index] = currentButtonState;
                return true;
            }
            this->button_check_count[index] = 0;
        } else if (currentButtonState) {
            if (this->button_check_count[index] < GAME_CTRL_CHECK_COUNT_MAX) {
                this->button_check_count[index]++;
            }
        }
        lastButtonState[index] = currentButtonState;
        return false;
    }
};

}
#endif /* _DRONE_SERVICE_RC_HPP_ */
