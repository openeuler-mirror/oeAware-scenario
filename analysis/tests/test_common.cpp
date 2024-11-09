#include <numa.h>
#include "gtest/gtest.h"
#include "common.h"

class TestCommon : public ::testing::Test {
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

TEST_F(TestCommon, TestNetworkInfoInit)
{
    NetworkInfo netInfo;
    netInfo.Init();
    EXPECT_EQ(netInfo.netRxTimes.size(), numa_num_configured_nodes());
}

TEST_F(TestCommon, TestNetworkInfoRemote)
{
    int nodeNum = numa_num_configured_nodes();
    NetworkInfo netInfo;
    netInfo.Init();
    std::unordered_map<std::string,
        std::unordered_map<int, std::unordered_map<uint8_t, std::unordered_map<uint8_t, uint64_t>>>> rxTimes;
    rxTimes["eth1"][0][0][0] = 10; // receive 10 net packets from eth1, irq node 0, thread node 0
    rxTimes["eth2"][0][1][0] = 20;
    rxTimes["eth3"][1][1][0] = 30;
    netInfo.AddRxTimes(rxTimes);
    EXPECT_EQ(netInfo.rxTimes["eth1"][0][0][0], 10);
    EXPECT_EQ(netInfo.rxTimes["eth2"][0][1][0], 20);
    EXPECT_EQ(netInfo.rxTimes["eth3"][1][1][0], 30);
    std::vector<std::vector<uint64_t>> value;
    int numaNum = 4; // 4 is greater than rxTimes[0].size()
    value.resize(numaNum);
    for (int i = 0; i < numaNum; i++) {
        value[i].resize(numaNum);
    }
    netInfo.Node2NodeRxTimes(value);
    EXPECT_EQ(value[0][0], 10); // 10 is the sum of all eth packets
    EXPECT_EQ(value[1][0], 50);

    netInfo.SumRemoteRxTimes();
    EXPECT_EQ(netInfo.remoteRxSum, 60); // 60 = 10 + 50

    netInfo.ClearData();
    EXPECT_EQ(netInfo.netRxSum, 0);
    EXPECT_EQ(netInfo.remoteRxSum, 0);
    EXPECT_EQ(netInfo.rxTimes.size(), 0);
}

TEST(CommonTest, TaskInfoTestInit)
{
    TaskInfo taskInfo;
    int nodeNum = numa_num_configured_nodes();
    EXPECT_GT(nodeNum, 0);
    taskInfo.Init();
    EXPECT_EQ(taskInfo.loopCnt, 0);
    EXPECT_EQ(taskInfo.access.size(), nodeNum);
    for (int i = 0; i < nodeNum; i++) {
        EXPECT_EQ(taskInfo.access[i].size(), nodeNum);
    }
}

TEST(CommonTest, TaskInfoNumaScoreTest)
{
    TaskInfo taskInfo;
    Env &env = Env::GetInstance();
    int numaNum = env.numaNum;
    taskInfo.access.resize(numaNum);
    for (int i = 0; i < numaNum; i++) {
        taskInfo.access[i].resize(numaNum);
        taskInfo.access[i][i] = 100; // every node local access have 100 times
    }
    taskInfo.CalculateNumaScore();
    float numaScore = taskInfo.numaScore;
    EXPECT_GT(numaScore, 0.99); // 0.99 < numaScore < 1.01
    EXPECT_LT(numaScore, 1.01);
    taskInfo.access[0].back() = 100; // add 100 times to remote node
    taskInfo.CalculateNumaScore();
    float badNumaScore = taskInfo.numaScore;
    EXPECT_LE(badNumaScore, numaScore);
}