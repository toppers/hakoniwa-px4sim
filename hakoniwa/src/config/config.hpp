#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

/*
 * 有効にすると、物理モデルが航空座標系になります。
 */
//#define ENABLE_AIR_FRAME

/*
 * 有効化すると、PX4と連携できます。
 * 無効の場合は、内部の制御モデルと連携します。
 */
//#define DRONE_PX4_ENABLE
#ifdef DRONE_PX4_ENABLE
/*
 * 現行の物理モデル(drone_phys.cpp)には、以下がない。
 * - 空気摩擦
 * - ローター動力の時間遅延
 * 以下を有効化すると、上記が有効化されます。
 */
#define EXPERIMENTAL_CODE_ENABLE
#define DRONE_PX4_AIRFRAME_ADJUSTMENT_ENABLE
#define DRONE_PX4_SENSOR_ENABLE
#define DRONE_PX4_CONTROL_ENABLE

/*
 * 有効にすると新しいアーキテクチャで動作する
 */
//#define ENABLE_LATEST_ARCHITECTURE

/*
 * 有効化すると、PX4への送信データを標準出力に出力します。
 * 無効の場合は、出力されません。
 */
//#define DRONE_PX4_TX_DEBUG_ENABLE

/*
 * 有効化すると、PX4からの受信データを標準出力に出力します。
 * 無効の場合は、出力されません。
 */
//#define DRONE_PX4_RX_DEBUG_ENABLE

/*
 * 有効化すると、物理モデルのデバッグ情報を標準出力に出力します。
 * 無効の場合は、出力されません。
 */
//#define ENABLE_DRONE_PHYS_DEBUG

#else
/*
 * 有効化すると、緯度経度が東京になる
 * 無効の場合は、アメリカ・ワシントン周辺になる(AirSimと同じ構成)
 */
#define DRON_LOC_TOKYO
#endif


/*
 * 物理モデルのアクチュエータに力の強さのパラメータ値です。
 * PX4制御の場合はパラメータ値を大きくしないと動きません。
 */

#ifdef DRONE_PX4_CONTROL_ENABLE
#define KEISU   4.0f
#else
#define KEISU   1.0f
#endif

/*
 * 現行の物理モデル(drone_phys.cpp)のパラメータです。
 */
#define DRONE_PROPELLER_NUM 4
#define DRONE_PARAM_M       1.0
#define DRONE_PARAM_L       0.3
#define DRONE_PARAM_GRAVITY 9.81
#define DRONE_PARAM_K        0.5
#define DRONE_PARAM_P       0.25


#endif /* _CONFIG_HPP_*/