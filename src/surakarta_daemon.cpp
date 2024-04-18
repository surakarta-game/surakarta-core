#include "surakarta_daemon.h"

class SurakartaRuleManagerImplForwardingSignalToSurakartaDaemon : public SurakartaRuleManagerImpl {
   public:
    SurakartaRuleManagerImplForwardingSignalToSurakartaDaemon(
        std::shared_ptr<SurakartaBoard> board,
        std::shared_ptr<SurakartaGameInfo> game_info,
        SurakartaDaemon& daemon)  // Lifecycle of this class should be the same as the lifecycle of SurakartaDaemon
        : SurakartaRuleManagerImpl(board, game_info),
          daemon_(daemon) {}

    virtual void OnUpdateBoard() override {
        daemon_.OnUpdateBoard();
    }

   private:
    SurakartaDaemon& daemon_;
};

SurakartaDaemon::SurakartaDaemon(
    int board_size,
    int max_no_capture_round,
    std::shared_ptr<AgentFactory> black_agent_factory,
    std::shared_ptr<AgentFactory> white_agent_factory)
    : game_(board_size, max_no_capture_round) {
    game_.SetRuleManager(
        std::make_shared<SurakartaRuleManagerImplForwardingSignalToSurakartaDaemon>(
            game_.GetBoard(), game_.GetGameInfo(), *this));
    black_agent_ = black_agent_factory->CreateAgent(*this, PieceColor::BLACK);
    white_agent_ = white_agent_factory->CreateAgent(*this, PieceColor::WHITE);
}

void SurakartaDaemon::Execute() {
    game_.StartGame();
    auto current = PieceColor::BLACK;
    while (!game_.IsEnd()) {
        game_.Move(current == PieceColor::BLACK ? black_agent_->CalculateMove() : white_agent_->CalculateMove());
        current = ReverseColor(current);
    }
}
