# 箱庭のインストール手順

```
cd hakoniwa-px4sim/hakoniwa
```

## Windows の場合

Windowsの場合は、docker コンテナ内での作業になります。

以下のコマンドで docker イメージを pull してください。

```
bash docker/pull-image.bash 
```

成功した場合は、以下のように`toppersjp/hakoniwa-px4sim`が作成されます。

```
$ docker images
REPOSITORY                        TAG       IMAGE ID       CREATED             SIZE
toppersjp/hakoniwa-px4sim         v1.0.1    c34c696b8007   About an hour ago   3.98GB
```

なお、イメージは一度作成できればWindows内に残り続けますので、一度だけ実行すればOKです。

次に、以下のコマンドで docker コンテナに入ってください。

```
bash docker/run.bash 
```

箱庭環境をビルドします。

```
bash build.bash
```

成功すると、` cmake-build/src/hako-px4sim` というファイルが作成されます。

## Mac の場合

箱庭環境をビルドします。

```
bash build.bash
```

成功すると、` cmake-build/src/hako-px4sim` というファイルが作成されます。
