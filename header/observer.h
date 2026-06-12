/**
 * @file observer.h
 * @brief observerのテンプレートとobserverたち
 * @date 2026/06/10
 * @version 1.0
 */
#pragma once

#include <singleton.h>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
 /**
  * @class IObserver
  * @brief オブザーバーのテンプレート
  */
template <typename EventType>
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onNotify(const EventType& event) = 0;
};

/**
 * @class IObserver
 * @brief オブザーバーのサブジェクトのテンプレート
 */
template <typename EventType>
class Subject {
private:
    std::vector<std::weak_ptr<IObserver<EventType>>> observers_;

public:
    void registerObserver(std::weak_ptr<IObserver<EventType>> observer) {
        std::cout << "OBSERVER CRETED\n";
        observers_.push_back(observer);
    }

    void notify(const EventType& event) {
        auto it = observers_.begin();
        std::cout << "OBSERVER MONITOR START\n";
        while (it != observers_.end()) {
            if (auto observerSmart = it->lock()) {
                observerSmart->onNotify(event);
                ++it;
            }
            else {
                it = observers_.erase(it);
                std::cout << "ERASE\n";
            }
        }
    }
};


/**
 * @class DebugMessageObserver
 * @brief デバッグメッセージのオブザーバー
 * @author Saitou Yousei
 */
class DebugMessageObserver :public IObserver<std::string> {
private:
	std::string name_;
public:
    DebugMessageObserver(std::string name) :name_(std::move(name)) {};

	//破棄されたら
	~DebugMessageObserver() override {
		std::cout << name_ << " DISPOSE\n";
	}

	//通知されたら
	void onNotify(const std::string& message) override {
		std::cout << name_ << " RECIVE " << message << "\n";
	}
};

/**
 * @class ObserverManager
 * @brief オブザーバーのシングルトン
 * @author Saitou Yousei
 */
class ObserverManager : public SingletonBase<ObserverManager> {
    friend class SingletonBase<ObserverManager>;
private:
    Subject<std::string> subject;
    std::shared_ptr<DebugMessageObserver> logObserver = std::make_shared<DebugMessageObserver>("MAIN DEBUG");

    ObserverManager() { 
        subject.registerObserver(logObserver);
        std::cout << "ObserverManager Created." << std::endl; 
    }
    ~ObserverManager() { std::cout << "ObserverManager Destroyed." << std::endl; }
public:
    Subject<std::string>  GetLogSubject() {
        return subject;
    }
};