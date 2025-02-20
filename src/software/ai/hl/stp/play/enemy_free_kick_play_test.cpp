#include "software/ai/hl/stp/play/enemy_free_kick_play.h"

#include <gtest/gtest.h>

#include "software/simulated_tests/non_terminating_validation_functions/robots_avoid_ball_validation.h"
#include "software/simulated_tests/simulated_play_test_fixture.h"
#include "software/simulated_tests/terminating_validation_functions/robot_halt_validation.h"
#include "software/simulated_tests/terminating_validation_functions/robot_in_polygon_validation.h"
#include "software/simulated_tests/validation/validation_function.h"
#include "software/test_util/test_util.h"
#include "software/time/duration.h"
#include "software/world/game_state.h"
#include "software/world/world.h"

class EnemyFreekickPlayTest : public SimulatedPlayTestFixture,
                              public ::testing::WithParamInterface<unsigned int>
{
   protected:
    Field field = Field::createSSLDivisionBField();
};

TEST_P(EnemyFreekickPlayTest, test_enemy_free_kick_play)
{
    BallState ball_state(Point(0.9, 2.85), Vector(0, 0));
    auto friendly_robots = TestUtil::createStationaryRobotStatesWithId(
        {Point(-4.5, 0), Point(-3, 1.5), Point(-3, 0.5), Point(-3, -0.5), Point(-3, -1.5),
         Point(-3, -3.0)});
    setFriendlyGoalie(0);
    auto enemy_robots = TestUtil::createStationaryRobotStatesWithId(
        {Point(-2, -1.25), Point(-1, -0.25), field.enemyDefenseArea().negXPosYCorner(),
         field.enemyDefenseArea().negXNegYCorner(), Point(1, 3),
         field.enemyGoalCenter()});
    unsigned int num_enemy_robots = GetParam();
    ASSERT_TRUE(GetParam() <= enemy_robots.size());
    if (enemy_robots.size() - num_enemy_robots)
    {
        enemy_robots.erase(enemy_robots.begin(),
                           enemy_robots.begin() + enemy_robots.size() - num_enemy_robots);
    }

    setEnemyGoalie(5);
    setAIPlay(TYPENAME(EnemyFreekickPlay));
    setRefereeCommand(RefereeCommand::NORMAL_START, RefereeCommand::DIRECT_FREE_THEM);

    std::vector<ValidationFunction> terminating_validation_functions = {
        [num_enemy_robots](std::shared_ptr<World> world_ptr,
                           ValidationCoroutine::push_type& yield) {
            if (num_enemy_robots == 6)
            {
                // Wait for all robots to come to a halt
                robotHalt(world_ptr, yield);

                // Two robots defending close to the enemy robot performing the free kick
                Rectangle shadowing_free_kicker_rect(Point(0, 3), Point(1, 2));
                robotInPolygon(1, shadowing_free_kicker_rect, world_ptr, yield);
                robotInPolygon(3, shadowing_free_kicker_rect, world_ptr, yield);

                // Two friendly robots in position to shadow enemy robots. Rectangles are
                // chosen to be generally in the way of the the front 2 enemy robots and
                // the enemy robot performing the free kick, based on where the enemy
                // robots are initialized in the test.
                Rectangle robot_four_shadowing_rect(Point(-1, 0.25), Point(-0.75, -0.25));
                Rectangle robot_five_shadowing_rect(Point(-2, -1), Point(-1.75, -1.25));
                robotInPolygon(4, robot_four_shadowing_rect, world_ptr, yield);
                robotInPolygon(5, robot_five_shadowing_rect, world_ptr, yield);

                // One Friendly robot defending the exterior of defense box to the right
                // of the goalie
                Point goalie_position = world_ptr->friendlyTeam().goalie()->position();
                Rectangle crease_defender_rect(
                    goalie_position,
                    Point(goalie_position.x() + 0.2, goalie_position.y() + 0.4));
                robotInPolygon(2, crease_defender_rect, world_ptr, yield);
            }
            else
            {
                // Running the test for 5s to check for segfaults
                while (world_ptr->getMostRecentTimestamp() < Timestamp::fromSeconds(5))
                {
                    yield("Checking for Segfault: Timestamp not at 5s");
                }
            }
        }};

    std::vector<ValidationFunction> non_terminating_validation_functions = {
        [](std::shared_ptr<World> world_ptr, ValidationCoroutine::push_type& yield) {
            // Wait 2 seconds for robots to start moving adequately far away from the ball
            if (world_ptr->getMostRecentTimestamp() >= Timestamp::fromSeconds(2))
            {
                robotsAvoidBall(0.5, {}, world_ptr, yield);
            }
        }};

    runTest(field, ball_state, friendly_robots, enemy_robots,
            terminating_validation_functions, non_terminating_validation_functions,
            Duration::fromSeconds(10));
}

// Test with fewer robots
INSTANTIATE_TEST_CASE_P(EnemyRobotPositions, EnemyFreekickPlayTest,
                        ::testing::Values(6, 5, 4, 3, 2, 1));

TEST_F(EnemyFreekickPlayTest, test_enemy_free_kick_close_to_net)
{
    BallState ball_state(Point(-2.4, 1), Vector(0, 0));
    auto friendly_robots = TestUtil::createStationaryRobotStatesWithId(
        {Point(-4.5, 0), Point(-3, 1.5), Point(-3, 0.5), Point(-3, -0.5), Point(-3, -1.5),
         Point(-3, -3.0)});
    setFriendlyGoalie(0);
    auto enemy_robots = TestUtil::createStationaryRobotStatesWithId({
        field.enemyGoalCenter(),
        Point(-2.3, 1.05),
        Point(-3.5, 2),
        Point(-1.2, 0),
        Point(-2.3, -1),
        Point(-3.8, -2),
    });
    setEnemyGoalie(0);
    setAIPlay(TYPENAME(EnemyFreekickPlay));
    setRefereeCommand(RefereeCommand::NORMAL_START, RefereeCommand::DIRECT_FREE_THEM);

    std::vector<ValidationFunction> terminating_validation_functions = {
        [](std::shared_ptr<World> world_ptr, ValidationCoroutine::push_type& yield) {
            // Wait for all robots to come to a halt
            robotHalt(world_ptr, yield);
            // Two robots defending close to the enemy robot performing the free kick
            Rectangle shadowing_free_kicker_rect(Point(-3.5, 1.25), Point(-2.75, 0.5));
            robotInPolygon(1, shadowing_free_kicker_rect, world_ptr, yield);
            robotInPolygon(3, shadowing_free_kicker_rect, world_ptr, yield);

            // Two friendly robots in position to shadow enemy robots. Rectangles are
            // chosen to be generally in the way of the the front 2 enemy robots and the
            // enemy robot performing the free kick, based on where the enemy robots are
            // initialized in the test.
            Rectangle robot_four_shadowing_rect(Point(-2.5, -0.5), Point(-2, -1));
            Rectangle robot_five_shadowing_rect(Point(-1.8, 0.5), Point(-1.2, 0));
            robotInPolygon(4, robot_four_shadowing_rect, world_ptr, yield);
            robotInPolygon(5, robot_five_shadowing_rect, world_ptr, yield);

            // One Friendly robot defending the exterior of defense box to the left of the
            // goalie
            Point goalie_position = world_ptr->friendlyTeam().goalie()->position();
            Rectangle crease_defender_rect(
                Point(goalie_position.x(), goalie_position.y() + 0.3),
                Point(goalie_position.x() + 0.3, goalie_position.y()));
            robotInPolygon(2, crease_defender_rect, world_ptr, yield);
        }};

    std::vector<ValidationFunction> non_terminating_validation_functions = {
        [](std::shared_ptr<World> world_ptr, ValidationCoroutine::push_type& yield) {
            // Wait 6 seconds for robots to start moving adequately far away from the ball
            // Due to tight space between defense area and the robot performing the enemy
            // free kick we wait longer than in the other tests
            if (world_ptr->getMostRecentTimestamp() >= Timestamp::fromSeconds(6))
            {
                robotsAvoidBall(0.5, {}, world_ptr, yield);
            }
        }};

    runTest(field, ball_state, friendly_robots, enemy_robots,
            terminating_validation_functions, non_terminating_validation_functions,
            Duration::fromSeconds(10));
}

TEST_F(EnemyFreekickPlayTest, test_enemy_free_kick_chipper_robots_close_to_net)
{
    BallState ball_state(Point(-1.1, 1.943), Vector(0, 0));
    auto friendly_robots = TestUtil::createStationaryRobotStatesWithId(
        {Point(-4.5, 0), Point(-3, 1.5), Point(-3, 0.5), Point(-3, -0.5), Point(-3, -1.5),
         Point(-3, -3.0)});
    setFriendlyGoalie(0);
    auto enemy_robots = TestUtil::createStationaryRobotStatesWithId({
        field.enemyGoalCenter(),
        Point(-1, 2),
        Point(-3, 2.5),
        Point(-1, 0),
        Point(-3.2, -0.5),
        Point(-4, 1.4),
    });
    setEnemyGoalie(0);
    setAIPlay(TYPENAME(EnemyFreekickPlay));
    setRefereeCommand(RefereeCommand::NORMAL_START, RefereeCommand::DIRECT_FREE_THEM);

    std::vector<ValidationFunction> terminating_validation_functions = {
        [](std::shared_ptr<World> world_ptr, ValidationCoroutine::push_type& yield) {
            // Wait for all robots to come to a halt
            robotHalt(world_ptr, yield);
            // Two robots defending close to the enemy robot performing the free kick
            Rectangle shadowing_free_kicker_rect(Point(-2.1, 2), Point(-1.5, 1.25));
            robotInPolygon(1, shadowing_free_kicker_rect, world_ptr, yield);
            robotInPolygon(4, shadowing_free_kicker_rect, world_ptr, yield);

            // Two friendly robots in position to shadow enemy robots. Rectangles are
            // chosen to be generally in the way of the the front 2 enemy robots and the
            // enemy robot performing the free kick, based on where the enemy robots are
            // initialized in the test.
            Rectangle robot_three_shadowing_rect(Point(-3.25, 0), Point(-2.75, -0.5));
            Rectangle robot_five_shadowing_rect(Point(-1.25, 0.5), Point(-0.75, 0));
            robotInPolygon(3, robot_three_shadowing_rect, world_ptr, yield);
            robotInPolygon(5, robot_five_shadowing_rect, world_ptr, yield);

            // One Friendly robot defending the exterior of defense box to the left of the
            // goalie
            Point goalie_position = world_ptr->friendlyTeam().goalie()->position();
            Rectangle crease_defender_rect(
                Point(goalie_position.x(), goalie_position.y() + 0.5),
                Point(goalie_position.x() + 0.3, goalie_position.y()));
            robotInPolygon(2, crease_defender_rect, world_ptr, yield);
        }};

    std::vector<ValidationFunction> non_terminating_validation_functions = {
        [](std::shared_ptr<World> world_ptr, ValidationCoroutine::push_type& yield) {
            // Wait 2 seconds for robots to start moving adequately far away from the ball
            if (world_ptr->getMostRecentTimestamp() >= Timestamp::fromSeconds(2))
            {
                robotsAvoidBall(0.5, {}, world_ptr, yield);
            }
        }};

    runTest(field, ball_state, friendly_robots, enemy_robots,
            terminating_validation_functions, non_terminating_validation_functions,
            Duration::fromSeconds(10));
}

TEST(EnemyFreeKickPlayInvariantAndIsApplicableTest, test_invariant_and_is_applicable)
{
    auto play_config = std::make_shared<ThunderbotsConfig>()->getPlayConfig();

    auto world = ::TestUtil::createBlankTestingWorld();

    // EnemyFreeKickPlay: the play under test
    auto enemy_free_kick_play = EnemyFreekickPlay(play_config);

    // Test 1: Ensure play will start and stay running for our free kick play
    world.updateGameState(::TestUtil::createGameState(RefereeCommand::DIRECT_FREE_THEM,
                                                      RefereeCommand::DIRECT_FREE_THEM));

    ASSERT_TRUE(enemy_free_kick_play.isApplicable(world));
    ASSERT_TRUE(enemy_free_kick_play.invariantHolds(world));

    // Test 2: Ensure doesn't run when NOT our free kick play
    world.updateGameState(::TestUtil::createGameState(RefereeCommand::DIRECT_FREE_US,
                                                      RefereeCommand::DIRECT_FREE_THEM));

    ASSERT_FALSE(enemy_free_kick_play.isApplicable(world));
    ASSERT_FALSE(enemy_free_kick_play.invariantHolds(world));

    // Test 3: Ensure play goes from normal start command to enemy free kick play
    world.updateGameState(::TestUtil::createGameState(RefereeCommand::DIRECT_FREE_THEM,
                                                      RefereeCommand::NORMAL_START));

    ASSERT_TRUE(enemy_free_kick_play.isApplicable(world));
    ASSERT_TRUE(enemy_free_kick_play.invariantHolds(world));
}
