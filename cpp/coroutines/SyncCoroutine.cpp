#include <experimental/coroutine>
#include <iostream>
#include <memory>

template <typename T> struct SyncV0 {
  SyncV0() { std::cout << "Created a sync object" << std::endl; }
  SyncV0(const SyncV0 &s) { std::cout << "Copied a sync object" << std::endl; }
  ~SyncV0() { std::cout << "Sync gone" << std::endl; }
  void get() {
    std::cout << "We got asked for the return value..." << std::endl;
  }
  struct promise_type {
    promise_type() { std::cout << "Promise created" << std::endl; }
    ~promise_type() { std::cout << "Promise died" << std::endl; }
  };
};

template <typename T> struct SyncV1 {
  std::shared_ptr<T> value;
  SyncV1(std::shared_ptr<T> p) : value(p) {
    std::cout << "Created a sync object" << std::endl;
  }
  SyncV1(const SyncV1 &s) : value(s.value) {
    std::cout << "Copied a sync object" << std::endl;
  }
  ~SyncV1() { std::cout << "Sync gone" << std::endl; }
  T get() {
    std::cout << "We got asked for the return value..." << std::endl;
    return *value;
  }
  struct promise_type {
    std::shared_ptr<T> ptr;
    promise_type() : ptr(std::make_shared<T>()) {
      std::cout << "Promise created" << std::endl;
    }
    ~promise_type() { std::cout << "Promise died" << std::endl; }
    auto get_return_object() {
      std::cout << "Send back a sync" << std::endl;
      return ptr;
    }
    auto initial_suspend() {
      std::cout << "Started the coroutine, don't stop now!" << std::endl;
      return std::experimental::suspend_never{};
    }
    auto return_value(T v) {
      std::cout << "Got an answer of " << v << std::endl;
      *ptr = v;
      return std::experimental::suspend_never{};
    }
    auto final_suspend() {
      std::cout << "Finished the coro" << std::endl;
      return std::experimental::suspend_never{};
    }
    void unhandled_exception() { std::exit(1); }
  };
};

SyncV0<int> answer() {
  std::cout << "Thinking deep thoughts..." << std::endl;
  co_return 42;
}

int main() {
  auto a = answer();
  std::cout << "Got a coroutine, let's get a value" << std::endl;
  auto v = a.get();
  std::cout << "And the coroutine value is: " << v << std::endl;

  return 0;
}