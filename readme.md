parallelsort
======

https://qiita.com/Nabetani/items/2dc2264764e2c68e7bcf
を書くために作ったリポジトリ

## requirements

* macOS + homebrew 以外は考慮されていない
* cmake 3.16 以降が必要
* tbb が必要( `brew install tbb` )
* tbb が新しい場合、 `CMakeLists.txt` を書き換える必要があるかも
* gcc-9 が必要
* python 3.7 以降が必要

## how to build

```
$ cmake .
$ make release
```

## how to run

```
$ pip install -r requirements.txt
$ release/sample.out > result.csv
$ python plot.py
```
