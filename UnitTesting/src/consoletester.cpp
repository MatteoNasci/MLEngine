#include <gmock/gmock.h>

#include <console.h>
#include <timemanager.h>
#include <iostream>

class ConsoleTest : public ::testing::Test
{
protected:
    mle::Console* m_console;
    mle::TimeManager* m_timeManager;
    virtual void SetUp()
    {      
        m_timeManager = new mle::TimeManager();
        m_console = new mle::Console("Test.txt", *m_timeManager);
        m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Warning);
    }

    virtual void TearDown()
    {
        delete m_console;
    }
};

TEST_F(ConsoleTest, isLogCommandEmptyLog){
    EXPECT_FALSE(m_console->isLogCommand("")) << "Empty log should not be considered a command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandSpacesLog){
    EXPECT_FALSE(m_console->isLogCommand("     ")) << "Empty log should not be considered a command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandOnlySlashLog){
    EXPECT_TRUE(m_console->isLogCommand("/")) << "A single slash log should be considered as a command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandOnlyBackSlashLog){
    EXPECT_FALSE(m_console->isLogCommand("\\")) << "A single backslash log should not be considered as a command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandRandomLog){
    EXPECT_TRUE(m_console->isLogCommand("/fire")) << "Normal command should be considered as command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandRandomWithInputLog){
    EXPECT_TRUE(m_console->isLogCommand("/fire-50.5")) << "Normal command with input should be considered as command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandOnlyInputLog){
    EXPECT_FALSE(m_console->isLogCommand("-50.5")) << "Log with only input format is not a command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandSlashAndInputLog){
    EXPECT_TRUE(m_console->isLogCommand("/-50.5")) << "Log with only slash + input format is a command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandRandomWithFirstSpacesLog){
    EXPECT_FALSE(m_console->isLogCommand("      /fire")) << "Normal command should be considered as command only if the slash is at the beginning of the log." << std::endl;
}

TEST_F(ConsoleTest, isNormalClassEnoughForWarning){
    EXPECT_FALSE(m_console->isClassificationProcessable(mle::LogClassification::Normal));
}
TEST_F(ConsoleTest, isWarningClassEnoughForWarning){
    EXPECT_TRUE(m_console->isClassificationProcessable(mle::LogClassification::Warning));
}
TEST_F(ConsoleTest, isInfoClassEnoughForWarning){
    EXPECT_TRUE(m_console->isClassificationProcessable(mle::LogClassification::Info));
}
TEST_F(ConsoleTest, isNormalClassEnoughForCritical){
    auto prev = m_console->getMinimumLogClassificationToProcess();
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Critical);

    EXPECT_FALSE(m_console->isClassificationProcessable(mle::LogClassification::Normal));

    m_console->setMinimumLogClassificationToProcess(prev);
}
TEST_F(ConsoleTest, isCriticalClassEnoughForCritical){
    auto prev = m_console->getMinimumLogClassificationToProcess();
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Critical);

    EXPECT_TRUE(m_console->isClassificationProcessable(mle::LogClassification::Critical));

    m_console->setMinimumLogClassificationToProcess(prev);
}
TEST_F(ConsoleTest, isCommandClassEnoughForCritical){
    auto prev = m_console->getMinimumLogClassificationToProcess();
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Critical);

    EXPECT_TRUE(m_console->isClassificationProcessable(mle::LogClassification::Command));

    m_console->setMinimumLogClassificationToProcess(prev);
}
TEST_F(ConsoleTest, isCommandClassEnoughForInfo){
    auto prev = m_console->getMinimumLogClassificationToProcess();
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Info);

    EXPECT_FALSE(m_console->isClassificationProcessable(mle::LogClassification::Command));

    m_console->setMinimumLogClassificationToProcess(prev);
}
TEST_F(ConsoleTest, isInfoClassEnoughForInfo){
    auto prev = m_console->getMinimumLogClassificationToProcess();
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Info);

    EXPECT_TRUE(m_console->isClassificationProcessable(mle::LogClassification::Info));

    m_console->setMinimumLogClassificationToProcess(prev);
}