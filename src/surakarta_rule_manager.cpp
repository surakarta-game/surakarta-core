#include "surakarta_rule_manager.h"
#include <exception>

void SurakartaRuleManager::OnUpdateBoard() {
    throw std::runtime_error("Method not implemented");
}

SurakartaIllegalMoveReason SurakartaRuleManager::JudgeMove(const SurakartaMove& move [[maybe_unused]]) {
    throw std::runtime_error("Method not implemented");
}

std::pair<SurakartaEndReason, SurakartaPlayer> SurakartaRuleManager::JudgeEnd(const SurakartaIllegalMoveReason reason [[maybe_unused]]) {
    throw std::runtime_error("Method not implemented");
}

std::unique_ptr<std::vector<SurakartaPosition>> SurakartaRuleManager::GetAllLegalTarget(const SurakartaPosition postion [[maybe_unused]]) {
    throw std::runtime_error("Method not implemented");
}

void SurakartaRuleManager::HelloWorld() {
    throw std::runtime_error("Method not implemented");
}