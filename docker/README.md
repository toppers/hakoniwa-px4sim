# これは何？

hakoniwa-px4sim のビルド確認をするための docker イメージを管理しています。

# 使い方

以下のコマンドを実行すると docker コンテナに入ります。

```
bash docker/build.bash
```

```
root@docker-desktop:~/workspace# ls
CMakeLists.txt  airsim         demo    drone_api      drone_sensors          hakoniwa   tools
README-ja.md    cmake-build    docker  drone_control  hako-setup-check.bash  matlab-if
README.md       cmake-options  docs    drone_physics  hako-start.bash        px4
```

# ビルド方法

```
cd cmake-build
```

```
cmake ..
```

```
make
```

