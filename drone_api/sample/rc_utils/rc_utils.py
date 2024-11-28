#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import json

class RcConfig:
    # スティック操作の定数定義
    STICK_TURN_LR = 0  # Turn Left/Right (Left Stick - LR)
    STICK_UP_DOWN = 1  # Up/Down (Left Stick - UD)
    STICK_MOVE_LR = 2  # Move Left/Right (Right Stick - LR)
    STICK_MOVE_FB = 3  # Move Forward/Back (Right Stick - UD)

    # スイッチイベントの定数定義
    SWITCH_RADIO_CONTROL_ENABLE = 0
    SWITCH_GRAB_BAGGAGE =1
    SWITCH_CAMERA_SHOT = 2
    SWITCH_CAMERA_MOVE_UP = 11
    SWITCH_CAMERA_MOVE_DOWN = 12
    SWITCH_RETURN_HOME = 13
    
    def __init__(self, filepath):
        self.config = self._load_json(filepath)

    def _load_json(self, path):
        try:
            with open(path, 'r') as file:
                return json.load(file)
        except FileNotFoundError:
            print(f"ERROR: File not found '{path}'")
        except json.JSONDecodeError:
            print(f"ERROR: Invalid Json fromat '{path}'")
        except PermissionError:
            print(f"ERROR: Permission denied '{path}'")
        except Exception as e:
            print(f"ERROR: {e}")
        return None


    def get_event_op_index(self, switch_index):
        """
        Get the event index based on switch index.
        
        Returns the corresponding index.
        """
        event_op_map = {
            self.config['Event']['RadioControlEnable']['index']: self.SWITCH_RADIO_CONTROL_ENABLE,
            self.config['Event']['GrabBaggage']['index']: self.SWITCH_GRAB_BAGGAGE,
            self.config['Event']['Camera']['index']: self.SWITCH_CAMERA_SHOT,
            self.config['Event']['CameraMoveUp']['index']: self.SWITCH_CAMERA_MOVE_UP,
            self.config['Event']['CameraMoveDown']['index']: self.SWITCH_CAMERA_MOVE_DOWN,
            self.config['Event']['ReturnHome']['index']: self.SWITCH_RETURN_HOME
        }
        return event_op_map.get(switch_index, None)

    def get_switch_feature(self, switch_index):
        """
        Get the switch feature based on switch index.
        
        Returns the corresponding switch feature dictionary.
        """
        feature_map = {
            self.config['Event']['RadioControlEnable']['index']: self.config['Event']['RadioControlEnable'],
            self.config['Event']['Camera']['index']: self.config['Event']['Camera'],
            self.config['Event']['GrabBaggage']['index']: self.config['Event']['GrabBaggage'],
            self.config['Event']['CameraMoveUp']['index']: self.config['Event']['CameraMoveUp'],
            self.config['Event']['CameraMoveDown']['index']: self.config['Event']['CameraMoveDown'],
            self.config['Event']['ReturnHome']['index']: self.config['Event']['ReturnHome']
        }
        feature = feature_map.get(switch_index, None)
        if feature:
            feature['type'] = feature.get('type', 'toggle')
            feature['off'] = feature.get('off', 'up')
            feature['on'] = feature.get('on', 'down')
        else:
            print(f"WARNING: Feature for switch index {switch_index} not found.")
            return None
 
        return feature

    def get_op_index(self, stick_index):
        """
        Get the operation index based on stick index.
        
        Returns the corresponding index.
        """
        if self.config['mode'] == 2:
            op_map = {
                self.config['stick']['Left']['LR']['index']: self.STICK_TURN_LR,
                self.config['stick']['Left']['UD']['index']: self.STICK_UP_DOWN,
                self.config['stick']['Right']['LR']['index']: self.STICK_MOVE_LR,
                self.config['stick']['Right']['UD']['index']: self.STICK_MOVE_FB
            }
        else: # mode1
            op_map = {
                self.config['stick']['Left']['LR']['index']: self.STICK_TURN_LR,
                self.config['stick']['Left']['UD']['index']: self.STICK_MOVE_FB,
                self.config['stick']['Right']['LR']['index']: self.STICK_MOVE_LR,
                self.config['stick']['Right']['UD']['index']: self.STICK_UP_DOWN
            }

        return op_map.get(stick_index, None)

    def get_stick_feature(self, stick_index):
        """
        Get the stick feature based on stick index.
        
        Returns the corresponding stick feature dictionary.
        """
        feature_map = {
            self.config['stick']['Left']['LR']['index']: self.config['stick']['Left']['LR'],
            self.config['stick']['Left']['UD']['index']: self.config['stick']['Left']['UD'],
            self.config['stick']['Right']['LR']['index']: self.config['stick']['Right']['LR'],
            self.config['stick']['Right']['UD']['index']: self.config['stick']['Right']['UD']
        }
        feature = feature_map.get(stick_index, None)
        if feature:
            feature['conversion'] = feature.get('conversion', None)
            feature['discretize'] = feature.get('discretize', None)
            feature['average'] = feature.get('average', False)
            feature['valueInverse'] = feature.get('valueInverse', False)
        else:
            raise ValueError(f"Feature for stick index {stick_index} not found.")
 
        return feature

class StickMonitor:
    def __init__(self, config: RcConfig):
        self.rc_config = config
        self.stick_history = {0: [], 1: [], 2: [], 3: [], 4: [], 5: []}
        self.history_len = 5
        self.switch_states = {}
    
    def stick_value(self, stick_index, stick_value) -> float:
        op_index = self.rc_config.get_op_index(stick_index)
        feature = self.rc_config.get_stick_feature(stick_index)
        v = stick_value

        if feature['average']:
            v = self.average_stick_value(op_index, v)
        
        if feature['conversion'] is not None:
            v = self.cubic_stick_value(v, feature['conversion']['paramA'], feature['conversion']['paramB'], feature['conversion']['paramC'])

        if feature['valueInverse']:
            v = -v

        if feature['discretize'] is not None:
            v = self.discretized_stick_value(v, feature['discretize'])

        return v
    
    def switch_event(self, switch_index: int, down: bool) -> bool:
        """
        Handle switch events.
        
        Returns True if the event is triggered. 
        For 'push': Returns True on down -> up transition.
        For 'toggle': Toggles the state on each down event.
        """
        feature = self.rc_config.get_switch_feature(switch_index)
        if feature is None:
            return False

        # Determine the event_on state based on the feature's 'on' condition
        if feature['on'] == 'down':
            event_on = down
        else:
            event_on = not down

        event_triggered = False

        if feature['type'] == 'push':
            # Push event is triggered on down -> up transition
            previous_state = self.switch_states.get(switch_index, False)
            if previous_state is True and event_on is False:
                event_triggered = True
            self.switch_states[switch_index] = event_on
        
        elif feature['type'] == 'toggle':
            # Toggle event toggles the state on each down event
            if event_on and not self.switch_states.get(switch_index, False):
                self.switch_states[switch_index] = True
            elif event_on and self.switch_states[switch_index]:
                self.switch_states[switch_index] = False
            event_triggered = self.switch_states[switch_index]
        else:
            # Non-toggle, non-push events just return the current state
            event_triggered = event_on

        return event_triggered


    def average_stick_value(self, op_index, new_value: float):
        """
        履歴を使用してスティックの平均値を計算する
        """
        history = self.stick_history[op_index]
        history.append(new_value)
        if len(history) > self.history_len:
            history.pop(0)
        return sum(history) / len(history)
    
    def cubic_stick_value(self, x: float, a_value: float, b_value: float, c_value: float = 0.0, d_value: float = 0.0) -> float:
        """
        ドローンのスティック操作を3次関数で計算し、正規化する関数。
        """
        # 3次関数の計算
        y = a_value * x**3 + b_value * x**2 + c_value * x + d_value

        # 出力を -1 から 1 の範囲に制限（クリッピング）
        y_clipped = max(min(y, 1.0), -1.0)

        return y_clipped


    def discretized_stick_value(self, value, split_value: float):
        return round(value / split_value) * split_value
