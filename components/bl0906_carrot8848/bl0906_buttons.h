#pragma once

#include "esphome/components/button/button.h"

#include "bl0906_carrot8848.h"

namespace esphome {
namespace bl0906_carrot8848 {

enum class Bl0906ButtonAction : uint8_t {
  RESET_ENERGY,
  SAVE_ENERGY,
  DIAGNOSE_PERSISTENCE,
  DIAGNOSE_STATISTICS,
};

// 组件操作按钮：把电量维护/诊断动作暴露为HA按钮。
// header-only，由生成固件实例化，实现调用预编译库中已导出的组件方法。
class BL0906ActionButton : public button::Button, public Component {
 public:
  void set_parent(BL0906Carrot8848 *parent) { this->parent_ = parent; }
  void set_action(Bl0906ButtonAction action) { this->action_ = action; }

 protected:
  void press_action() override {
    switch (this->action_) {
      case Bl0906ButtonAction::RESET_ENERGY:
        this->parent_->reset_energy_data();
        break;
      case Bl0906ButtonAction::SAVE_ENERGY:
        this->parent_->save_energy_data();
        break;
      case Bl0906ButtonAction::DIAGNOSE_PERSISTENCE:
        this->parent_->diagnose_energy_persistence();
        break;
      case Bl0906ButtonAction::DIAGNOSE_STATISTICS:
        this->parent_->diagnose_energy_statistics();
        break;
    }
  }

  BL0906Carrot8848 *parent_{nullptr};
  Bl0906ButtonAction action_{Bl0906ButtonAction::RESET_ENERGY};
};

}  // namespace bl0906_carrot8848
}  // namespace esphome
