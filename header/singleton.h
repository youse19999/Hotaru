#include <iostream>

template <typename T>
class SingletonBase {
public:
    // インスタンス取得のための共通関数
    static T& getInstance() {
        static T instance;
        return instance;
    }

protected:
    // 派生クラスでのみ生成・破棄できるようにprotectedにする
    SingletonBase() = default;
    virtual ~SingletonBase() = default;

public:
    // コピー・ムーブの禁止
    SingletonBase(const SingletonBase&) = delete;
    SingletonBase& operator=(const SingletonBase&) = delete;
    SingletonBase(SingletonBase&&) = delete;
    SingletonBase& operator=(SingletonBase&&) = delete;
};