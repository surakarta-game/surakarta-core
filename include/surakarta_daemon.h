#pragma once

#include "surakarta_agent_mine.h"
#include "surakarta_agent_random.h"
#include "surakarta_board.h"
#include "surakarta_game.h"

class SurakartaDaemon {
   public:
    class AgentFactory {
       public:
        virtual ~AgentFactory() = default;
        virtual std::unique_ptr<SurakartaAgentBase> CreateAgent(SurakartaDaemon& daemon, PieceColor my_color) = 0;
    };

    SurakartaDaemon(
        int board_size = BOARD_SIZE,
        int max_no_capture_round = MAX_NO_CAPTURE_ROUND,
        std::shared_ptr<AgentFactory> black_agent_factory = nullptr,
        std::shared_ptr<AgentFactory> white_agent_factory = nullptr);

    void Execute();

    virtual void OnUpdateBoard() {};

    std::shared_ptr<SurakartaGameInfo> GameInfo() const { return game_.GetGameInfo(); }
    std::shared_ptr<SurakartaBoard> Board() const { return game_.GetBoard(); }
    std::shared_ptr<SurakartaRuleManager> RuleManager() const { return game_.GetRuleManager(); }

   private:
    SurakartaGame game_;
    std::unique_ptr<SurakartaAgentBase> black_agent_;
    std::unique_ptr<SurakartaAgentBase> white_agent_;
};

class SurakartaAgentMineFactory : public SurakartaDaemon::AgentFactory {
   public:
    class SurakartaMoveWeightUtilBaseFactory {
       public:
        virtual ~SurakartaMoveWeightUtilBaseFactory() = default;
        virtual std::unique_ptr<SurakartaMoveWeightUtilBase> CreateUtil(SurakartaDaemon& daemon, PieceColor my_color) = 0;
    };

    class SurakartaMoveWeightUtilFactory : public SurakartaMoveWeightUtilBaseFactory {
       public:
        SurakartaMoveWeightUtilFactory(
            int depth = SurakartaMoveWeightUtil::DefaultDepth,
            double alpha = SurakartaMoveWeightUtil::DefaultAlpha,
            double beta = SurakartaMoveWeightUtil::DefaultBeta)
            : depth_(depth), alpha_(alpha), beta_(beta) {}

        virtual std::unique_ptr<SurakartaMoveWeightUtilBase> CreateUtil(SurakartaDaemon& daemon, PieceColor my_color) override {
            return std::make_unique<SurakartaMoveWeightUtil>(
                daemon.Board(),
                my_color,
                depth_,
                alpha_,
                beta_);
        }

       private:
        const int depth_;
        const double alpha_;
        const double beta_;
    };

    SurakartaAgentMineFactory(std::shared_ptr<SurakartaMoveWeightUtilBaseFactory> weight_util_factory)
        : weight_util_factory_(weight_util_factory){};

    virtual std::unique_ptr<SurakartaAgentBase> CreateAgent(SurakartaDaemon& daemon, PieceColor color) override {
        return std::make_unique<SurakartaAgentMine>(
            daemon.Board(),
            daemon.GameInfo(),
            daemon.RuleManager(),
            color,
            weight_util_factory_->CreateUtil(daemon, color));
    }

   private:
    const std::shared_ptr<SurakartaMoveWeightUtilBaseFactory> weight_util_factory_;
};

class SurakartaAgentRandomFactory : public SurakartaDaemon::AgentFactory {
   public:
    virtual std::unique_ptr<SurakartaAgentBase> CreateAgent(SurakartaDaemon& daemon, PieceColor color [[maybe_unused]]) override {
        return std::make_unique<SurakartaAgentRandom>(
            daemon.Board(),
            daemon.GameInfo(),
            daemon.RuleManager());
    }
};
