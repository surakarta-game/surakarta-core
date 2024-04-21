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
        daemon_.OnUpdateBoard.Invoke();
    }

   private:
    SurakartaDaemon& daemon_;
};

SurakartaDaemon::SurakartaDaemon(
    int board_size,
    int max_no_capture_round,
    std::shared_ptr<AgentFactory> black_agent_factory,
    std::shared_ptr<AgentFactory> white_agent_factory)
    : game_(board_size, max_no_capture_round),
      black_agent_factory_(black_agent_factory),
      white_agent_factory_(white_agent_factory) {
    game_.SetRuleManager(
        std::make_shared<SurakartaRuleManagerImplForwardingSignalToSurakartaDaemon>(
            game_.GetBoard(), game_.GetGameInfo(), *this));
}

void SurakartaDaemon::Execute() {
    game_.StartGame();
    auto black_agent = black_agent_factory_->CreateAgent(*this, PieceColor::BLACK);
    auto white_agent = white_agent_factory_->CreateAgent(*this, PieceColor::WHITE);
    auto current = PieceColor::BLACK;
    while (!game_.IsEnd()) {
        status_ = current == PieceColor::BLACK ? ExecuteStatus::WAITING_FOR_BLACK_AGENT : ExecuteStatus::WAITING_FOR_WHITE_AGENT;
        game_.Move(current == PieceColor::BLACK ? black_agent->CalculateMove() : white_agent->CalculateMove());
        current = ReverseColor(current);
    }
    status_ = ExecuteStatus::ENDED;
}
