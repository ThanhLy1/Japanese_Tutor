#include "ebisu.h"
#include "gtest/gtest.h"

class EbisuTest : public ::testing::Test {
protected:
    Ebisu ebisu;

    void SetUp() override {
        ebisu = Ebisu();
    }
};

TEST_F(EbisuTest, TestAlphaSetterGetter) {
    double newAlpha = 4.0;
    ebisu.setAlpha(newAlpha);
    EXPECT_EQ(ebisu.getAlpha(), newAlpha);
}

TEST_F(EbisuTest, TestBetaSetterGetter) {
    double newBeta = 2.0;
    ebisu.setBeta(newBeta);
    EXPECT_EQ(ebisu.getBeta(), newBeta);
}

TEST_F(EbisuTest, TestTSetterGetter) {
    double newT = 3.0;
    ebisu.setT(newT);
    EXPECT_EQ(ebisu.getT(), newT);
}

TEST_F(EbisuTest, TestPredictRecall) {
    // Check if it gives reasonable results.
    double elapsed = 1.0;
    double expected = 0.75;  // Replace with expected result.
    EXPECT_NEAR(ebisu.predictRecall(elapsed), expected, 0.01);
}

TEST_F(EbisuTest, TestUpdateRecall) {
    double success = 2.0;
    double total = 3.0;
    double elapsed = 1.0;
    ebisu.updateRecall(success, total, elapsed);
    // Verify that the parameters were updated as expected.
    // Replace with expected results.
    double expectedAlpha = 5.0;
    double expectedBeta = 2.0;
    double expectedT = 1.6;
    EXPECT_NEAR(ebisu.getAlpha(), expectedAlpha, 0.01);
    EXPECT_NEAR(ebisu.getBeta(), expectedBeta, 0.01);
    EXPECT_NEAR(ebisu.getT(), expectedT, 0.01);
}

TEST_F(EbisuTest, TestModelToPercentileDecay) {
    // Replace with expected result.
    double expected = 1.0; 
    EXPECT_NEAR(ebisu.modelToPercentileDecay(), expected, 0.01);
}

TEST_F(EbisuTest, TestPercentileDecayToModel) {
    double percentileDecay = 0.5;
    // Replace with expected result.
    double expected = 1.0; 
    EXPECT_NEAR(ebisu.percentileDecayToModel(percentileDecay), expected, 0.01);
}
