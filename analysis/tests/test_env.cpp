#include "gtest/gtest.h"
#include "env.h"
class TestEnv : public ::testing::Test {
protected:
    void SetUp() override
    {
        Env &env = Env::GetInstance();
        env.Init();
    }

    void TearDown() override
    {
    }
};

TEST_F(TestEnv, singleton)
{
    Env &instance1 = Env::GetInstance();
    Env &instance2 = Env::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(TestEnv, init)
{
    Env &env = Env::GetInstance();
    EXPECT_GT(env.numaNum, 0);
    EXPECT_GE(env.cpuNum, env.numaNum);
    EXPECT_GT(env.pageMask, 0);
}

TEST_F(TestEnv, cpu)
{
    Env &env = Env::GetInstance();
    EXPECT_EQ(env.cpu2Node.size(), env.cpuNum);
    for (int i = 0; i < env.cpuNum; i++) {
        EXPECT_GE(env.cpu2Node[i], 0);
        EXPECT_LT(env.cpu2Node[i], env.numaNum);
    }
}

TEST_F(TestEnv, cycles)
{
    Env &env = Env::GetInstance();
    EXPECT_EQ(env.cpuMaxCycles.size(), env.cpuNum);
    uint64_t sum = 0;
    int freq = 1000 * 1000 * 1000; // 1GHz
    for (auto &i : env.cpuMaxCycles) {
        sum += i;
        EXPECT_GT(i, freq); // usually greater than > 1GHz
    }
    EXPECT_EQ(sum, env.sysMaxCycles);
}

TEST_F(TestEnv, distance)
{
    Env &env = Env::GetInstance();
    EXPECT_EQ(env.distance.size(), env.numaNum);
    for (auto &i : env.distance) {
        EXPECT_EQ(i.size(), env.numaNum);
        for (auto &j : i) {
            EXPECT_GT(j, 0);
            EXPECT_LE(j, env.maxDistance);
            EXPECT_GE(j, env.maxDistance - env.diffDistance);
        }
    }
}