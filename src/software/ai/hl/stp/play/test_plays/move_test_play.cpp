#include "software/ai/hl/stp/play/test_plays/move_test_play.h"

#include "software/ai/hl/stp/tactic/move/move_tactic.h"
#include "software/util/generic_factory/generic_factory.h"

MoveTestPlay::MoveTestPlay(std::shared_ptr<const PlayConfig> config) : Play(config, false)
{
}

bool MoveTestPlay::isApplicable(const World &world) const
{
    return world.ball().position().x() >= 0;
}

bool MoveTestPlay::invariantHolds(const World &world) const
{
    return world.ball().position().x() >= 0;
}

void MoveTestPlay::getNextTactics(TacticCoroutine::push_type &yield, const World &world)
{
    auto move_test_tactic_friendly_goal = std::make_shared<MoveTactic>();
    auto move_test_tactic_enemy_goal    = std::make_shared<MoveTactic>();
    auto move_test_tactic_center_field  = std::make_shared<MoveTactic>();

    do
    {
        move_test_tactic_friendly_goal->updateControlParams(
            world.field().friendlyGoalCenter(), Angle::zero(), 0,
            MaxAllowedSpeedMode::PHYSICAL_LIMIT);
        move_test_tactic_enemy_goal->updateControlParams(
            world.field().enemyGoalCenter(), Angle::zero(), 0,
            MaxAllowedSpeedMode::PHYSICAL_LIMIT);
        move_test_tactic_center_field->updateControlParams(
            Point(0, 0), Angle::zero(), 0, MaxAllowedSpeedMode::PHYSICAL_LIMIT);

        yield({{move_test_tactic_center_field, move_test_tactic_friendly_goal,
                move_test_tactic_enemy_goal}});
    } while (!move_test_tactic_center_field->done());
}

// Register this play in the genericFactory
static TGenericFactory<std::string, Play, MoveTestPlay, PlayConfig> factory;
