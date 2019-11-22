#pragma once
#include "main.h"
#include <atomic>

class TowerBar {
public:
  TowerBar(std::shared_ptr<okapi::AbstractMotor> imotor, okapi::IterativePosPIDController::Gains igains, std::unique_ptr<okapi::Filter> iderivativeFilter = std::make_unique<okapi::PassthroughFilter>());

  ~TowerBar();

  static std::atomic_int minimumPos;
  static std::atomic_int restingPos;
  static std::atomic_int threshold;
  static std::atomic_int raisedPos;


  void intake();

  void raise();

  void lower();

  bool isRaised();

  bool isSettled();

  void waitUntilSettled();

  void trim(int trimDistance);

  void tare();

  std::shared_ptr<okapi::AbstractMotor> getMotor();

  void startThread();

  pros::Task *getTask();

  static void trampoline(void *angler);

private:
  void loop();

  pros::Task *thread{nullptr};
  std::unique_ptr<okapi::IterativePosPIDController> controller{nullptr};
  std::shared_ptr<okapi::AbstractMotor> motor{nullptr};
};