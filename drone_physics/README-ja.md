[English](README.md) ｜ 日本語

# Hakoniwa Drone Physics Library(math, physics and dynamics)

## これは何？

箱庭プロジェクト（サイバー物理システムのシミュレーションのためのオープンソースランタイム環境）において作成された、
ドローンプラントモデルのための数学、物理、および動力学ライブラリです。

地上と機体フレーム座標系でドローンの速度、加速度などが計算できます。
しかし、このライブラリはより汎用的であり、さまざまなドローンのシミュレーションプロジェクトで使用することができることに気づきました。
より使いやすくし、文献との対応をとって公開することにしました。
すべての関数は、以下の書籍の式の C++ 実装であり、ソースコードコメントに式番号が記載されています。

- [『ドローン工学入門』工学博士 野波健蔵【著】](https://www.coronasha.co.jp/np/isbn/9784339032307/)


[![image](https://github.com/toppers/hakoniwa-px4sim/assets/1093925/c92d3d96-25f9-4b6a-ae4e-25d898b75a28)](https://www.coronasha.co.jp/np/isbn/9784339032307/)

基礎的なドローンの動力学の参考実装となることを期待しています。

## Hello World

```cpp
#include <iostream>

// include the library header
#include "drone_physics.hpp"

int main() {
    // このライブラリの名前空間
    using namespace hako::drone_physics;

    // 機体座標系を Euler 角で指定
    VectorType frame = {0, 0, M_PI/2};
    VelocityType body_velocity = {100, 200, 300};
    
    // 機体座標系から地上座標系への速度変換
    VelocityType ground_velocity = velocity_body_to_ground(body_velocity, frame);

    // x,y,z 座標を取り出す
    auto [u, v, w] = ground_velocity;

    std::cout << "u = " << u << ", v = " << v << ", w = " << w << std::endl;
    // output: u = 200, v = -100, w = 300

    // このように、明示的にコンストラクタを使うこともできる
    // 逆変換して戻す
    VelocityType body_velocity2 = velocity_ground_to_body(
        VelocityType(u, v, w),
        AngleType(0, 0, M_PI/2)
    );

    auto [u2, v2, w2] = body_velocity2;
    std::cout << "u2 = " << u2 << ", v2 = " << v2 << ", w2 = " << w2 << std::endl;
    // output: u2 = 100, v2 = 200, w2 = 300, back again.
}
```
## 関数リスト

関数は以下のカテゴリから構成され、書籍の式番号が記載されています。

### Frame conversion functions:
| Function | equation | note |
|----------|-----------|------|
|`velocity_body_to_ground`  | (1.71), (1.124) | 機体座標の速度を地上座標に変換 |
|`velocity_ground_to_body`  | (1.69), inverse of (1.124) | 地上座標の速度を機体座標に変換 |
|`angular_velocity_body_to_ground` | (1.109) | 機体角速度を地上座標に変換 |
|`angular_velocity_ground_to_body` | (1.106) | 地上角速度を機体座標に変換 |

### Body dynamics(Acceleration) functions:
| Function | equations in the book | note |
|----------|-----------|------|
|`acceleration_in_body_frame` | (1.136),(2.31) | 機体座標系での加速度計算 |
|`angular_acceleration_in_body_frame` | (1.37),(2.31) | 機体座標系での角加速度計算 |

### Rotor dynamics functions:
| Function | equations in the book | note |
|----------|-----------|------|
|`rotor_omega_acceleration` | (2.48) | ローター角速度の加速度計算 |
|`rotor_thrust` | (2.50) | ローター推力計算 |

### Body dynamics(n rotors) functions:
| Function | equations in the book | note |
|----------|-----------|------|
|`body_thrust` | (2.61) | $n$ 個のローターの推力の合力 |
|`body_torque` | (2.60)-(2.62) | $n$ 個のローターのトルクの合力。ローターの設置位置も関係する。 |
|`rotor_anti_torque` | (2.56) | ローターの反トルク計算。これにより機体は z 軸周りに回転する。 |

## 利用法

examples.cpp と utest.cpp に利用例があります。

## 数式

地上座標系は、右手系で定義されており、 $z$ 軸は下向きです。
機体座標系は、右手系で定義されており、 $x$ 軸は機体の前方向、 $y$ 軸は機体の右方向、 $z$ 軸は機体の下方向です。
機体座標系の原点は機体の重心です。機体座標系は機体に固定されており、機体座標系は機体とともに移動します。
基本的な力学方程式は、機体座標系で以下のようになります eq.(2.31)。

$m \dot{v} + \omega \times m v = F$

$I \dot{\omega} + \omega \times I \omega = \tau$

ここで、

- $m$ - 機体の質量
- $I$ - 機体の慣性行列
- $v$ - 機体の速度 $v=(u, v, w)$
- $\omega$ - 機体の角速度 $\omega = (p, q, r)$
- $F$ - 機体に働く力のベクトル。重力($mg$)、空気抵抗($-dv)$、推力($T$)からなる。
- $\tau$ - 機体に働くトルクのベクトル。ローターの推力によるものと、反トルクの合力。

機体座標系の動力学方程式は、機体座標系のオイラー角 $\phi, \theta, \psi$ に基づいて計算されます。

$\dot{u} = -g \sin{\theta} -(qw -rv) -\frac{d}{m}u$

$\dot{v} = g \cos{\theta}\sin{\phi} -(ru -pw) -\frac{d}{m}v$

$\dot{w} = -\frac{T}{m} + g \cos{\theta}cos{\phi} -(pv-qu)-\frac{d}{m}w$

$\dot{p} = (\tau_{\phi} -qr(I_{zz}-I_{yy}))/I_{xx} $

$\dot{q} = (\tau_{\theta}-rp(I_{xx}-I_{zz}))/I_{yy}$

$\dot{r} = (\tau_{\psi}-pq(I_{yy}-I_{xx}))/I_{zz}$

ここで、

- $g$ は重力加速度
- $\phi, \theta, \psi$ は機体座標系のオイラー角。ロール($x$-軸)、ピッチ($y$-軸)、ヨー($z$-軸)角。
- $d$ は空気抵抗係数。速度項に影響する空気抵抗係数である「抗力」を表す。
- $I_{xx}​,I_{yy}, I_{zz}$ は機体座標系の慣性モーメント。$x, y, z$ 軸は機体の主軸に沿っているものとする。他の斜めの慣性モーメント $I_{xy}, I_{yz}, I_{zx}$ はゼロとする。

機体座標系と地上座標系の変換は以下のようになります。

速度の変換: 

![スクリーンショット 2023-12-05 10 04 44](https://github.com/toppers/hakoniwa-px4sim/assets/164193/992bb7fe-0d50-47a5-aab5-e17aba4f716d)


角速度の変換:

![スクリーンショット 2023-12-05 10 05 51](https://github.com/toppers/hakoniwa-px4sim/assets/164193/9b036e35-6ed5-4fd0-8ceb-05364e5cccdb)

1つ1つのモーターの角速度は、$\Omega(t)$ は、デューティー比 $d(t)$ によって1次遅れ系としてモデル化できます。
書籍の式 (2.48) で記述されている伝達関数 G(s) によって記述され、

$\frac{G(s)}{D(s)} = \frac{K_r/}{Tr s + 1}$

時間領域の微分方程式は以下のようになります。

$\dot{\Omega}(t) = K_r (d(t) - \Omega / T_r)$

ここで、

- $K_r$ はローターのゲイン定数です。
- $T_r$ はローターの時定数です。
- $d(t)$ はローターのデューティー比です。 ($0.0 - 1.0$)

1つのロータ推力 $T$ は、ローターの角速度 $\Omega$ の2乗に比例します eq.(2.50)。
$A$ はローターのサイズと空気密度に関連するパラメータです。

$T = A \Omega^2 $

反トルク $\tau_i$ はとローターの角速度 $\Omega$ の関係式は以下のようになります eq.(2.56)。
 
$\tau_i = B \Omega^2 + Jr \dot{\Omega}$

ここで、 $B$ と $Jr$ はローターの特性に関連するパラメータです。
これによって、機体は $z$ 軸周りに回転します。

## 実験

私たちは、Hakoniwa を PX4 SITL シミュレータに接続し、ライブラリを以下の実験でテストしました。
全体アーキテクチャはこのようなものです。

![image](https://github.com/toppers/hakoniwa-px4sim/assets/1093925/2ffd9cd6-e8b4-4c75-9328-69c6a8aebba4)

実験のミッション:
- 離陸し、高度10mでホバリングする。
- 右へ10m移動して停止する。

![image](https://github.com/toppers/hakoniwa-px4sim/assets/1093925/afa89bfd-e873-4cee-b4f1-606c6fed409e)

<img width="1072" alt="image" src="https://github.com/toppers/hakoniwa-px4sim/assets/1093925/ef02a826-4ba8-4dbb-86d2-090b9de1919e">

## ユニットテスト

全関数のユニットテストが、 utest.cpp にあります。読みにくいですが、使い方の参考にもどうぞ。

## 実装方針
- すべての関数は、標準 C++17 で実装されています。
- std:: 名前空間以外の外部ライブラリは使用していません。
- 他のライブラリ（std:: 名前空間を除く）に依存ていません。
- すべて関数として実装され、クラスはありません。すなわち内部状態を持ちません。

## 謝辞

難しい数学と力学について、非常に丁寧な解説書を書いていただいた、野波先生に感謝します。
また、箱庭プロジェクトをリードし、実際に PX4, QGroundControl と接続し、長い時間を掛けて Unity でビジュアル表示しながら
仮想的にドローンの飛行テストを行ってくれた、箱庭ラボの森さん（[@tmori](https://github.com/tmori)）に感謝します！
