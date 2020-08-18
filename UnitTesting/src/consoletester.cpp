#include <gmock/gmock.h>

#include <Engine/Debug/console.h>
#ifndef MLENGINE_STATIC
#include <Engine/Core/engine.h>
#endif
#include <Engine/Time/timermanager.h>
#include <iostream>
#include <fstream>
#include <vector>

void clearFile(const std::string& file){
    std::ofstream f;
    f.open(file, std::ios::out | std::ios::trunc);
    if(f.is_open()){
        f.close();
    }
}
void copy(char* end, char* start){
    std::strcpy(end, start);
}
std::string getFileString(const std::string& s){
    if(s.size() == 0){
        return s;
    }
#ifdef MLE_UNITTEST_DEBUG
    return "DEBUG_" + s;
#else
    return "RELEASE_" + s;
#endif
}

class ConsoleTest : public ::testing::Test
{
protected:
    mle::Console* m_console;
    mle::TimerManager* m_timeManager;
    virtual void SetUp()
    {      
#ifdef MLENGINE_STATIC
        m_timeManager = new mle::TimerManager();
        m_console = new mle::Console(getFileString("ConsoleTestTest.txt"), *m_timeManager);
#else
        m_timeManager = &mle::Engine::instance().timerManager();
        m_console = &mle::Engine::instance().console();
#endif
        while(m_console->isLoggingToFile()){
            m_timeManager->advanceTime(100);
        }
        clearFile(getFileString("ConsoleTestTest.txt"));
        m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
        m_console->addCommand("/loggg", [](const std::string&){});
    }

    virtual void TearDown()
    {
        delete m_console;
    }
};
TEST_F(ConsoleTest, initConsoleLogFilename){
    EXPECT_TRUE((getFileString("ConsoleTestTest.txt") == m_console->logFilename()));
}
TEST_F(ConsoleTest, initConsoleLogText){
    mle::TimerManager m;  
    mle::Console c(getFileString("TestingInitialLog.txt"), m);  
    EXPECT_TRUE(c.isLoggingToFile()); 
}
TEST_F(ConsoleTest, initConsoleLogTextWaiting){
    mle::TimerManager m;  
    mle::Console c(getFileString("TestingInitialLog.txt"), m);  
    while(c.isLoggingToFile()){
        m.advanceTime(100);
    }
    
    std::ifstream log_file;
    log_file.open(getFileString("TestingInitialLog.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 100;
        char a[size];
        a[0] = '\0';
        log_file.getline(a, size);
        EXPECT_FALSE(a[0] == '\0');
        log_file.getline(a, size);
        EXPECT_TRUE(a[0] == '\0');

        log_file.close();
        clearFile(getFileString("TestingInitialLog.txt"));
    }    
    else{
        FAIL();
    }   
}
TEST_F(ConsoleTest, initConsoleClassification){
    mle::TimerManager m;  
    mle::Console c(getFileString(""), m);  
    EXPECT_EQ(c.getMinimumLogClassificationToProcess(), mle::LogClassification::Normal);          
}
TEST_F(ConsoleTest, initConsoleFilename){
    mle::TimerManager m;  
    mle::Console c(getFileString("ciaomelomelo.txt"), m);  
    EXPECT_TRUE((getFileString("ciaomelomelo.txt") == c.logFilename()));          
}
TEST_F(ConsoleTest, initConsoleInvalidFileNoThrow){
    mle::TimerManager m;    
    EXPECT_NO_THROW({
        mle::Console c(getFileString(""), m);  
        EXPECT_TRUE((getFileString("") == c.logFilename()));    
    });
}
TEST_F(ConsoleTest, initConsoleInvalidFileNoThrow2){
    mle::TimerManager m;    
    EXPECT_NO_THROW({
        mle::Console c("", m);      
        c.log("boh", mle::LogClassification::Info);
        while(c.isLoggingToFile()){
            m.advanceTime(100);
        }
    });
}
TEST_F(ConsoleTest, setConsoleClassification){
    mle::TimerManager m;    
    mle::Console c(getFileString(""), m);
    c.setMinimumLogClassificationToProcess(mle::LogClassification::Command);
    EXPECT_EQ(c.getMinimumLogClassificationToProcess(), mle::LogClassification::Command);
}
TEST_F(ConsoleTest, setConsoleInvalidClassification){
    mle::TimerManager m;    
    mle::Console c(getFileString(""), m);
    c.setMinimumLogClassificationToProcess((mle::LogClassification)-2);
    EXPECT_EQ(c.getMinimumLogClassificationToProcess(), ((mle::LogClassification)-2));
}
TEST_F(ConsoleTest, setConsoleInvalidClassification2){
    mle::TimerManager m;    
    mle::Console c(getFileString(""), m);
    c.setMinimumLogClassificationToProcess((mle::LogClassification)std::numeric_limits<int>::max());
    EXPECT_EQ(c.getMinimumLogClassificationToProcess(), mle::Console::getHighestPriorityClassification());
}

TEST_F(ConsoleTest, isLogCommandEmptyLog){
    EXPECT_FALSE(mle::Console::isMsgCommand("")) << "Empty log should not be considered a command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandSpacesLog){
    EXPECT_FALSE(mle::Console::isMsgCommand("     ")) << "Empty log should not be considered a command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandOnlySlashLog){
    EXPECT_TRUE(mle::Console::isMsgCommand("/")) << "A single slash log should be considered as a command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandOnlyBackSlashLog){
    EXPECT_FALSE(mle::Console::isMsgCommand("\\")) << "A single backslash log should not be considered as a command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandRandomLog){
    EXPECT_TRUE(mle::Console::isMsgCommand("/fire")) << "Normal command should be considered as command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandRandomWithInputLog){
    EXPECT_TRUE(mle::Console::isMsgCommand("/fire-50.5")) << "Normal command with input should be considered as command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandOnlyInputLog){
    EXPECT_FALSE(mle::Console::isMsgCommand("-50.5")) << "Log with only input format is not a command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandSlashAndInputLog){
    EXPECT_TRUE(mle::Console::isMsgCommand("/-50.5")) << "Log with only slash + input format is a command." << std::endl;
}
TEST_F(ConsoleTest, isLogCommandRandomWithFirstSpacesLog){
    EXPECT_FALSE(mle::Console::isMsgCommand("      /fire")) << "Normal command should be considered as command only if the slash is at the beginning of the log." << std::endl;
}

TEST_F(ConsoleTest, isNormalClassEnoughForWarning){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Warning);
    EXPECT_FALSE(m_console->isClassificationProcessable(mle::LogClassification::Normal));
}
TEST_F(ConsoleTest, isWarningClassEnoughForWarning){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Warning);
    EXPECT_TRUE(m_console->isClassificationProcessable(mle::LogClassification::Warning));
}
TEST_F(ConsoleTest, isInfoClassEnoughForWarning){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Warning);
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
TEST_F(ConsoleTest, addCommandInvalidKey){
    EXPECT_NO_THROW({
        EXPECT_FALSE(m_console->addCommand("", [](const std::string& s){}));
    });
}
TEST_F(ConsoleTest, addCommandInvalidFunc){
    EXPECT_NO_THROW({
        EXPECT_FALSE(m_console->addCommand("/d", nullptr));
    });
}
TEST_F(ConsoleTest, addCommandInvalidKey2){
    EXPECT_NO_THROW({
        EXPECT_FALSE(m_console->addCommand("ciao/boh", [](const std::string& s){}));
    });
}
TEST_F(ConsoleTest, addCommandValid){
    EXPECT_NO_THROW({
        EXPECT_TRUE(m_console->addCommand("/my_commdand", [](const std::string& s){}));
    });
}
TEST_F(ConsoleTest, addCommandDuplicateCommand){
    EXPECT_NO_THROW({
        EXPECT_TRUE(m_console->addCommand("/my_cfommand", [](const std::string& s){}));
        EXPECT_FALSE(m_console->addCommand("/my_cfommand", [](const std::string& s){}));
    });
}
TEST_F(ConsoleTest, addCommandValidRemoval){
    EXPECT_NO_THROW({
        EXPECT_TRUE(m_console->addCommand("/my_commgand", [](const std::string& s){}));
        EXPECT_TRUE(m_console->removeCommand("/my_commgand"));
        EXPECT_FALSE(m_console->removeCommand("/my_commgand"));
        EXPECT_TRUE(m_console->addCommand("/my_commgand", [](const std::string& s){}));
    });
}

TEST_F(ConsoleTest, removeCommandInvalidKey){
    EXPECT_NO_THROW({
        EXPECT_FALSE(m_console->removeCommand(""));
    });
}
TEST_F(ConsoleTest, removeCommandNotASavedCommand){
    EXPECT_NO_THROW({
        EXPECT_FALSE(m_console->removeCommand("/dfdf"));
    });
}
TEST_F(ConsoleTest, removeCommandInvalidKey2){
    EXPECT_NO_THROW({
        EXPECT_FALSE(m_console->removeCommand("ciao/boh"));
    });
}
TEST_F(ConsoleTest, removeCommandValid){
    EXPECT_NO_THROW({
        EXPECT_TRUE(m_console->removeCommand("/loggg"));
        EXPECT_TRUE(m_console->addCommand("/loggg", [](const std::string&s){}));
    });
}
TEST_F(ConsoleTest, removeCommandDuplicateCommand){
    EXPECT_NO_THROW({
        EXPECT_TRUE(m_console->removeCommand("/loggg"));
        EXPECT_FALSE(m_console->removeCommand("/loggg"));
        EXPECT_TRUE(m_console->addCommand("/loggg", [](const std::string&s){}));
    });
}
TEST_F(ConsoleTest, removeCommandValidRemoval){
    EXPECT_NO_THROW({
        EXPECT_TRUE(m_console->removeCommand("/loggg"));
        EXPECT_TRUE(m_console->addCommand("/my_commanad", [](const std::string& s){}));
        EXPECT_FALSE(m_console->removeCommand("/loggg"));
        EXPECT_TRUE(m_console->addCommand("/loggg", [](const std::string&s){}));
        EXPECT_TRUE(m_console->removeCommand("/my_commanad"));
    });
}
TEST_F(ConsoleTest, checkInitCommandSize){
    std::vector<std::string> out;
    m_console->getCommandList(out);
    EXPECT_EQ(out.size(), m_console->commandsSize());
}
TEST_F(ConsoleTest, checkInitCommandSizeIncrease){
    size_t m_startcommandSize = m_console->commandsSize();
    m_console->addCommand("/b15oh", [](const std::string& s){});
    EXPECT_EQ(m_startcommandSize + 1, m_console->commandsSize());
}
TEST_F(ConsoleTest, checkInitCommandSizeIncrease2){
    size_t m_startcommandSize = m_console->commandsSize();
    m_console->addCommand("/bo11h", [](const std::string& s){});
    m_console->addCommand("/boh2", [](const std::string& s){});
    m_console->addCommand("/bo3h", [](const std::string& s){});
    m_console->addCommand("/bo5h", [](const std::string& s){});
    EXPECT_EQ(m_startcommandSize + 4, m_console->commandsSize());
}
TEST_F(ConsoleTest, checkInitCommandSizeDecrease){
    size_t m_startcommandSize = m_console->commandsSize();
    m_console->removeCommand("/loggg");
    EXPECT_EQ(m_startcommandSize - 1, m_console->commandsSize());
    EXPECT_TRUE(m_console->addCommand("/loggg", [](const std::string&s){}));
}
TEST_F(ConsoleTest, checkCommandContainsInvalid){
    EXPECT_FALSE(m_console->containsCommand("pippo"));
}
TEST_F(ConsoleTest, checkCommandContainsInvalid2){
    EXPECT_FALSE(m_console->containsCommand("/loggg sf"));
}
TEST_F(ConsoleTest, checkCommandContainsValid){
    EXPECT_TRUE(m_console->containsCommand("/loggg"));
}
TEST_F(ConsoleTest, checkCommandContainsValid2){
    m_console->addCommand("/cia3o", [](const std::string& s){});
    EXPECT_TRUE(m_console->containsCommand("/cia3o"));
}
TEST_F(ConsoleTest, clearCommands){
    size_t m_startcommandSize = m_console->commandsSize();
    EXPECT_EQ(m_startcommandSize, m_console->commandsSize());
    m_console->clearCommands();
    EXPECT_EQ(m_console->commandsSize(), 0);
    EXPECT_FALSE(m_console->removeCommand("/loggg"));
    EXPECT_TRUE(m_console->addCommand("/loggg", [](const std::string&s){}));
}
TEST_F(ConsoleTest, checkCommandListSizeWithClear){
    std::vector<std::string> out;
    m_console->getCommandList(out);
    EXPECT_EQ(out.size(), m_console->commandsSize());

    m_console->clearCommands();
    m_console->getCommandList(out);
    EXPECT_EQ(out.size(), 0);

    EXPECT_TRUE(m_console->addCommand("/loggg", [](const std::string&s){}));
}
TEST_F(ConsoleTest, checkCommandListContent){
    m_console->clearCommands();

    EXPECT_TRUE(m_console->addCommand("/a1", [](const std::string&s){}));
    EXPECT_TRUE(m_console->addCommand("/a2", [](const std::string&s){}));
    EXPECT_TRUE(m_console->addCommand("/a3", [](const std::string&s){}));
    EXPECT_TRUE(m_console->addCommand("/a4", [](const std::string&s){}));
    std::vector<std::string> out;
    m_console->getCommandList(out);
    EXPECT_EQ(out.size(), m_console->commandsSize());

    for(size_t i = 0; i < out.size(); ++i){
        const auto& e = out[i];
        EXPECT_TRUE((e == "/a1" || e == "/a2" || e == "/a3" || e == "/a4"));
    }

    EXPECT_TRUE(m_console->addCommand("/loggg", [](const std::string&s){}));
}
TEST_F(ConsoleTest, checkCommandListContent2){
    m_console->clearCommands();

    EXPECT_TRUE(m_console->addCommand("/a1", [](const std::string&s){}));
    EXPECT_TRUE(m_console->addCommand("/a2", [](const std::string&s){}));
    EXPECT_TRUE(m_console->addCommand("/a3", [](const std::string&s){}));
    EXPECT_TRUE(m_console->addCommand("/a4", [](const std::string&s){}));
    EXPECT_TRUE(m_console->addCommand("/a5", [](const std::string&s){}));
    EXPECT_TRUE(m_console->removeCommand("/a4"));
    std::vector<std::string> out;
    m_console->getCommandList(out);
    EXPECT_EQ(out.size(), m_console->commandsSize());

    for(size_t i = 0; i < out.size(); ++i){
        const auto& e = out[i];
        EXPECT_TRUE((e == "/a1" || e == "/a2" || e == "/a3" || e == "/a5"));
    }

    EXPECT_TRUE(m_console->addCommand("/loggg", [](const std::string&s){}));
}
TEST_F(ConsoleTest, stringFromClassValid1){
    EXPECT_TRUE(mle::Console::stringFromClassification(mle::LogClassification::Normal) == "normal");
}
TEST_F(ConsoleTest, stringFromClassValid2){
    EXPECT_TRUE(mle::Console::stringFromClassification(mle::LogClassification::Warning) == "warning");
}
TEST_F(ConsoleTest, stringFromClassValid3){
    EXPECT_TRUE(mle::Console::stringFromClassification(mle::LogClassification::Error) == "error");
}
TEST_F(ConsoleTest, stringFromClassValid4){
    EXPECT_TRUE(mle::Console::stringFromClassification(mle::LogClassification::Critical) == "critical");
}
TEST_F(ConsoleTest, stringFromClassValid5){
    EXPECT_TRUE(mle::Console::stringFromClassification(mle::LogClassification::Fatal) == "fatal");
}
TEST_F(ConsoleTest, stringFromClassValid6){
    EXPECT_TRUE(mle::Console::stringFromClassification(mle::LogClassification::Command) == "command");
}
TEST_F(ConsoleTest, stringFromClassValid7){
    EXPECT_TRUE(mle::Console::stringFromClassification(mle::LogClassification::Info) == "info");
}
TEST_F(ConsoleTest, stringFromClassValidOutOfValue){
    EXPECT_TRUE(mle::Console::stringFromClassification(mle::LogClassification(-7), "") == "");
}
TEST_F(ConsoleTest, stringFromClassValidOutOfValue2){
    EXPECT_TRUE(mle::Console::stringFromClassification(mle::LogClassification(-7), "a") == "a");
}

TEST_F(ConsoleTest, classFromStringValid1){
    EXPECT_EQ(mle::Console::classificationFromString("normal"), mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, classFromStringValid2){
    EXPECT_EQ(mle::Console::classificationFromString("warning"), mle::LogClassification::Warning);
}
TEST_F(ConsoleTest, classFromStringValid3){
    EXPECT_EQ(mle::Console::classificationFromString("error"), mle::LogClassification::Error);
}
TEST_F(ConsoleTest, classFromStringValid4){
    EXPECT_EQ(mle::Console::classificationFromString("critical"), mle::LogClassification::Critical);
}
TEST_F(ConsoleTest, classFromStringValid5){
    EXPECT_EQ(mle::Console::classificationFromString("fatal"), mle::LogClassification::Fatal);
}
TEST_F(ConsoleTest, classFromStringValid6){
    EXPECT_EQ(mle::Console::classificationFromString("command"), mle::LogClassification::Command);
}
TEST_F(ConsoleTest, classFromStringValid7){
    EXPECT_EQ(mle::Console::classificationFromString("info"), mle::LogClassification::Info);
}
TEST_F(ConsoleTest, classFromStringValidOutOfValue){
    EXPECT_EQ(mle::Console::classificationFromString("", mle::LogClassification::Normal), mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, classFromStringValidOutOfValue2){
    EXPECT_EQ(mle::Console::classificationFromString("a", mle::LogClassification::Error), mle::LogClassification::Error);
}
TEST_F(ConsoleTest, separateCommandInvalidCommand){
    std::string command, input;
    EXPECT_FALSE(m_console->separateCommand("", command, input));
}
TEST_F(ConsoleTest, separateCommandInvalidCommand1){
    std::string command, input;
    EXPECT_FALSE(m_console->separateCommand("sdgg", command, input));
}
TEST_F(ConsoleTest, separateCommandValidCommand){
    std::string command, input;
    EXPECT_TRUE(m_console->separateCommand("/command_not_present_in_list_of_commands", command, input));
}
TEST_F(ConsoleTest, separateCommandValidCommandNoInput){
    std::string command, input;
    EXPECT_TRUE(m_console->separateCommand("/loggg", command, input));
    EXPECT_TRUE(command == "/loggg");
    EXPECT_TRUE(input.empty());
}
TEST_F(ConsoleTest, separateCommandValidCommandNoInput2){
    std::string command, input;
    EXPECT_TRUE(m_console->separateCommand("/ -", command, input));
    EXPECT_TRUE(command == "/ ");
    EXPECT_TRUE(input.empty());
}
TEST_F(ConsoleTest, separateCommandValidCommandAndInput){
    std::string command, input;
    EXPECT_TRUE(m_console->separateCommand("/-pos=50,50,50", command, input));
    EXPECT_TRUE(command == "/");
    EXPECT_TRUE(input == "pos=50,50,50");
}
TEST_F(ConsoleTest, lookForInputNameInvalid){
    size_t i = 0, l = 0;
    EXPECT_FALSE(mle::Console::lookForInputName("sfasfasfs", i, l));
}
TEST_F(ConsoleTest, lookForInputNameInvalid2){
    size_t i = 0, l = 0;
    EXPECT_FALSE(mle::Console::lookForInputName("", i, l));
}
TEST_F(ConsoleTest, lookForInputNameInvalid3){
    size_t i = 0, l = 0;
    EXPECT_FALSE(mle::Console::lookForInputName("=sfdg", i, l));
}
TEST_F(ConsoleTest, lookForInputNameValid){
    size_t i = 0, l = 0;
    EXPECT_TRUE(mle::Console::lookForInputName("nome=boh", i, l));
    EXPECT_EQ(i, 0);
    EXPECT_EQ(l, 4);
}
TEST_F(ConsoleTest, lookForInputNameValid2){
    size_t i = 0, l = 0;
    EXPECT_TRUE(mle::Console::lookForInputName("n=boh", i, l));
    EXPECT_EQ(i, 0);
    EXPECT_EQ(l, 1);
}
TEST_F(ConsoleTest, lookForInputNameValid3){
    size_t i = 9, l = 0;
    EXPECT_TRUE(mle::Console::lookForInputName("nome=boh other_name=", i, l));
    EXPECT_EQ(i, 9);
    EXPECT_EQ(l, 10);
}
TEST_F(ConsoleTest, lookForInputNameValid4){
    size_t i = 3, l = 0;
    EXPECT_TRUE(mle::Console::lookForInputName("nome=boh", i, l));
    EXPECT_EQ(i, 3);
    EXPECT_EQ(l, 1);
}

TEST_F(ConsoleTest, lookForInputArgInvalid){
    size_t i = 20, l = 0;
    EXPECT_FALSE(mle::Console::lookForInputArgument("sfasfasfs", i, l));
}
TEST_F(ConsoleTest, lookForInputArgInvalid2){
    size_t i = 1, l = 0;
    EXPECT_FALSE(mle::Console::lookForInputArgument("", i, l));
}
TEST_F(ConsoleTest, lookForInputArgInvalid3){
    size_t i = 170, l = 0;
    EXPECT_FALSE(mle::Console::lookForInputArgument("=sfdg", i, l));
}
TEST_F(ConsoleTest, lookForInputArgValid){
    size_t i = 0, l = 0;
    EXPECT_TRUE(mle::Console::lookForInputArgument("nome=boh", i, l));
    EXPECT_EQ(i, 0);
    EXPECT_EQ(l, 8);
}
TEST_F(ConsoleTest, lookForInputArgValid2){
    size_t i = 2, l = 0;
    EXPECT_TRUE(mle::Console::lookForInputArgument("n=boh ", i, l));
    EXPECT_EQ(i, 2);
    EXPECT_EQ(l, 3);
}
TEST_F(ConsoleTest, lookForInputArgValid3){
    size_t i = 5, l = 0;
    EXPECT_TRUE(mle::Console::lookForInputArgument("nome=boh other_name=", i, l));
    EXPECT_EQ(i, 5);
    EXPECT_EQ(l, 3);
}
TEST_F(ConsoleTest, lookForInputArgValid4){
    size_t i = 5, l = 0;
    EXPECT_TRUE(mle::Console::lookForInputArgument("nome=bo h", i, l));
    EXPECT_EQ(i, 5);
    EXPECT_EQ(l, 2);
}
TEST_F(ConsoleTest, lookForInputArgValid5){
    size_t i = 8, l = 0;
    EXPECT_TRUE(mle::Console::lookForInputArgument("nome=boh     other_name=", i, l));
    EXPECT_EQ(i, 13);
    EXPECT_EQ(l, 11);
}
TEST_F(ConsoleTest, lookForInputArgValid6){
    size_t i = 5, l = 0;
    EXPECT_TRUE(mle::Console::lookForInputArgument("nome=boh     other_name=", i, l));
    EXPECT_EQ(i, 5);
    EXPECT_EQ(l, 3);
}
TEST_F(ConsoleTest, lookForInputArgValid7){
    size_t i = 14, l = 0;
    EXPECT_TRUE(mle::Console::lookForInputArgument("ciao=45 other=5             boh=boh      tisio=", i, l));
    EXPECT_EQ(i, 14);
    EXPECT_EQ(l, 1);
}
TEST_F(ConsoleTest, lookForInputNameArgValid){
    size_t i = 0, l = 0;
    EXPECT_TRUE(mle::Console::lookForInputName("ciao=45", i, l));
    EXPECT_EQ(i, 0);
    EXPECT_EQ(l, 4);
    i += l + 1;
    l = 0;
    EXPECT_TRUE(mle::Console::lookForInputArgument("ciao=45", i, l));
    EXPECT_EQ(i, 5);
    EXPECT_EQ(l, 2);
}
TEST_F(ConsoleTest, lookForInputNameArgValid2){
    size_t i = 5, l = 0;
    EXPECT_TRUE(mle::Console::lookForInputName("nome=boh     other_name=5", i, l));
    EXPECT_EQ(i, 5);
    EXPECT_EQ(l, 18);
    i += l + 1;
    l = 0;
    EXPECT_TRUE(mle::Console::lookForInputArgument("nome=boh     other_name=5", i, l));
    EXPECT_EQ(i, 24);
    EXPECT_EQ(l, 1);
}
TEST_F(ConsoleTest, lookForInputNameArgValid3){
    size_t i = 8, l = 0;
    EXPECT_TRUE(mle::Console::lookForInputName("ciao=45 other=5             boh=boh      tisio=", i, l));
    EXPECT_EQ(i, 8);
    EXPECT_EQ(l, 5);
    i += l + 1;
    l = 0;
    EXPECT_TRUE(mle::Console::lookForInputArgument("ciao=45 other=5             boh=boh      tisio=", i, l));
    EXPECT_EQ(i, 14);
    EXPECT_EQ(l, 1);
}

TEST_F(ConsoleTest, separateInputInvalid){
    EXPECT_FALSE(mle::Console::separateInput("asf=1", nullptr));
}
TEST_F(ConsoleTest, separateInputInvalid2){
    EXPECT_FALSE(mle::Console::separateInput("asf=1", std::function<void(std::string&&, std::string&&)>()));
}
TEST_F(ConsoleTest, separateInputValid){
    bool correct = true;
    EXPECT_TRUE(mle::Console::separateInput("", [&correct](std::string&& name, std::string&& value){
        correct = false;
    }));
    EXPECT_TRUE(correct);
}
TEST_F(ConsoleTest, separateInputValid1){
    bool correct = false;
    EXPECT_TRUE(mle::Console::separateInput("ciao=45", [&correct](std::string&& name, std::string&& value){
        EXPECT_TRUE("ciao" == name);
        EXPECT_TRUE("45" == value);
        correct = true;
    }));
    EXPECT_TRUE(correct);
}
TEST_F(ConsoleTest, separateInputValid2){
    bool correct = false;
    EXPECT_TRUE(mle::Console::separateInput("ciao=45 other=5", [&correct](std::string&& name, std::string&& value){
        EXPECT_TRUE((("ciao" == name && value == "45") || ("other" == name && "5" == value)));
        correct = true;
    }));
    EXPECT_TRUE(correct);
}
TEST_F(ConsoleTest, separateInputValid3){
    bool correct = false;
    EXPECT_TRUE(mle::Console::separateInput(" =45", [&correct](std::string&& name, std::string&& value){
        EXPECT_TRUE(" " == name);
        EXPECT_TRUE("45" == value);
        correct = true;
    }));
    EXPECT_TRUE(correct);
}
TEST_F(ConsoleTest, separateInputValid4){
    bool correct = true;
    EXPECT_TRUE(mle::Console::separateInput("=45", [&correct](std::string&& name, std::string&& value){
        FAIL();
        correct = false;
    }));
    EXPECT_TRUE(correct);
}
TEST_F(ConsoleTest, separateInputValid5){
    bool correct = true;
    EXPECT_TRUE(mle::Console::separateInput("name=", [&correct](std::string&& name, std::string&& value){
        FAIL();
        correct = false;
    }));
    EXPECT_TRUE(correct);
}
TEST_F(ConsoleTest, separateInputValid6){
    bool correct = false;
    EXPECT_TRUE(mle::Console::separateInput("ciao=45 other=5             boh=boh      tisio=", [&correct](std::string&& name, std::string&& value){
        EXPECT_TRUE((("ciao" == name && value == "45") || ("other" == name && "5" == value) || ("            boh" == name && "boh" == value)));
        correct = true;
    }));
    EXPECT_TRUE(correct);
}
TEST_F(ConsoleTest, commandNotFound){
    EXPECT_FALSE(m_console->command("", "pippo=3"));
}
TEST_F(ConsoleTest, commandNotFound2){
    EXPECT_FALSE(m_console->command("/random_command_that_does_not_exist_in_this_known_universe_and_plane_of_existance_for_all_we_know_at_the_moment_of_writing_this_string_with_a_keyboard_attached_to_a_computer end of a really long useless string made for no reason at all. The End.", "pippo=3"));
}

TEST_F(ConsoleTest, commandFoundAndExecuted){
    bool processed = false;
    EXPECT_TRUE(m_console->addCommand("/very_usefull_command", [&processed](const std::string&){processed = true;}));
    EXPECT_TRUE(m_console->command("/very_usefull_command", "pippo=3"));
    EXPECT_TRUE(m_console->removeCommand("/very_usefull_command"));
    EXPECT_TRUE(processed);
}
TEST_F(ConsoleTest, commandFoundAndExecutedWithInput){
    bool processed = false;
    EXPECT_TRUE(m_console->addCommand("/very_usefull_command", [&processed](const std::string& input){
        processed = true;
        EXPECT_TRUE("pippo=3" == input);
        }));
    EXPECT_TRUE(m_console->command("/very_usefull_command", "pippo=3"));
    EXPECT_TRUE(m_console->removeCommand("/very_usefull_command"));
    EXPECT_TRUE(processed);
}
TEST_F(ConsoleTest, commandFoundAndExecutedTestLogToFile){
    bool processed = false;
    EXPECT_TRUE(m_console->addCommand("/very_usefull_command", [&processed, this](const std::string& input){
        processed = true;
        EXPECT_TRUE("pippo=3" == input);
        EXPECT_TRUE(m_console->logToFile("pippo=3", mle::Console::getHighestPriorityClassification()));
        }));
    EXPECT_TRUE(m_console->command("/very_usefull_command", "pippo=3"));
    EXPECT_TRUE(m_console->removeCommand("/very_usefull_command"));
    EXPECT_TRUE(processed);

    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("pippo=3") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
}
TEST_F(ConsoleTest, commandFoundAndExecutedTestLogToFile2){
    bool processed = false;
    EXPECT_TRUE(m_console->addCommand("/very_usefull_command", [&processed, this](const std::string& input){
        processed = true;
        EXPECT_TRUE("pippo=3" == input);
        EXPECT_TRUE(m_console->logToFile("pippo=3", mle::Console::getHighestPriorityClassification()));
        }));
    EXPECT_TRUE(m_console->command("/very_usefull_command", "pippo=3"));
    EXPECT_TRUE(m_console->removeCommand("/very_usefull_command"));
    EXPECT_TRUE(processed);

    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("pippo=3") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
}
TEST_F(ConsoleTest, commandFoundAndExecutedTestYesLogToFile){
    bool processed = false;
    EXPECT_TRUE(m_console->addCommand("/very_usefull_command", [&processed, this](const std::string& input){
        processed = true;
        EXPECT_TRUE("pippo=3" == input);
        EXPECT_TRUE(m_console->logToFile("pippo=3", mle::Console::getHighestPriorityClassification()));
        }));
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Command);
    EXPECT_TRUE(m_console->command("/very_usefull_command", "pippo=3"));
    EXPECT_TRUE(m_console->removeCommand("/very_usefull_command"));
    EXPECT_TRUE(processed);
        
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("pippo=3") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, commandFoundAndExecutedTestYesLogToFileWithIsLoggingCheck){
    bool processed = false;
    EXPECT_TRUE(m_console->addCommand("/very_usefull_command", [&processed, this](const std::string& input){
        processed = true;
        EXPECT_TRUE("pippo=3" == input);
        EXPECT_TRUE(m_console->logToFile("pippo=3", mle::Console::getHighestPriorityClassification()));
        }));
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Command);
    EXPECT_TRUE(m_console->command("/very_usefull_command", "pippo=3"));
    EXPECT_TRUE(m_console->isLoggingToFile());
    EXPECT_TRUE(m_console->removeCommand("/very_usefull_command"));
    EXPECT_TRUE(processed);
        
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("pippo=3") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, commandFoundAndExecutedTestNoLogToFile){
    bool processed = false;
    EXPECT_TRUE(m_console->addCommand("/very_usefull_command", [&processed, this](const std::string& input){
        processed = true;
        EXPECT_TRUE("pippo=3" == input);
        EXPECT_FALSE(m_console->logToFile("pippo=3", mle::LogClassification::Fatal));
        }));
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Info);
    EXPECT_TRUE(m_console->command("/very_usefull_command", "pippo=3"));
    EXPECT_TRUE(m_console->removeCommand("/very_usefull_command"));
    EXPECT_TRUE(processed);

    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_FALSE(s.find("pippo=3") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, commandNotFoundTestLogToFile){
    EXPECT_FALSE(m_console->command("/very_usefull_nonexistantcommand", "pippo=3"));
    
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("[COMMAND_NOT_FOUND]") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
}
TEST_F(ConsoleTest, commandNotFoundTestLogToFileWithIsLoggingCheck){
    EXPECT_FALSE(m_console->command("/very_usefull_nonexistantcommand", "pippo=3"));
    EXPECT_TRUE(m_console->isLoggingToFile());
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("[COMMAND_NOT_FOUND]") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
}
TEST_F(ConsoleTest, commandNotFoundTestNoLogToFile){    
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Info);
    EXPECT_FALSE(m_console->command("/very_usefull_nonexistantcodfgfgmmand", "pippo=3"));
    
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("[COMMAND_NOT_FOUND]") == std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, commandNotFoundTestYesLogToFile){    
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Error);
    EXPECT_FALSE(m_console->command("/very_usefull_nonexistantcommand", "pippo=3"));
    
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("[COMMAND_NOT_FOUND]") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, logToFile){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
    EXPECT_TRUE(m_console->logToFile("very_usefull_comment", mle::LogClassification::Critical));
    EXPECT_TRUE(m_console->isLoggingToFile());
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("very_usefull_comment") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, logToFile2){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
    EXPECT_TRUE(m_console->logToFile("very_uasfasfsa  fgasgfsd  asddsf ss  ds dgggsefull_comment", mle::LogClassification::Normal));
    EXPECT_TRUE(m_console->isLoggingToFile());
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("very_uasfasfsa  fgasgfsd  asddsf ss  ds dgggsefull_comment") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, logToFileNoClass){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Command);
    EXPECT_FALSE(m_console->logToFile("very_usefull_comment", mle::LogClassification::Critical));
    EXPECT_FALSE(m_console->isLoggingToFile());
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_FALSE(s.find("very_usefull_comment") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, logToFileNoClass2){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Fatal);
    EXPECT_FALSE(m_console->logToFile("very_usefull_comment", mle::LogClassification::Critical));
    EXPECT_FALSE(m_console->isLoggingToFile());
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_FALSE(s.find("very_usefull_comment") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}

TEST_F(ConsoleTest, log){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
    EXPECT_TRUE(m_console->log("very_usefull_comment", mle::LogClassification::Critical));
    EXPECT_TRUE(m_console->isLoggingToFile());
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("very_usefull_comment") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, log2){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
    EXPECT_TRUE(m_console->log("  a  ", mle::LogClassification::Normal));
    EXPECT_TRUE(m_console->isLoggingToFile());
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("  a  ") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, logNo){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Fatal);
    EXPECT_FALSE(m_console->log("very_usefull_comment", mle::LogClassification::Critical));
    EXPECT_FALSE(m_console->isLoggingToFile());
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_FALSE(s.find("very_usefull_comment") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, log3){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Command);
    EXPECT_TRUE(m_console->log("very_usefull_comment", mle::LogClassification::Info));
    EXPECT_TRUE(m_console->isLoggingToFile());
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("very_usefull_comment") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, logCommand){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
    EXPECT_TRUE(m_console->addCommand("/log_to_command", [this](const std::string& i){EXPECT_TRUE(i == "ciao=10");EXPECT_TRUE(m_console->logToFile("ciao=10", mle::Console::getHighestPriorityClassification()));}));
    EXPECT_TRUE(m_console->log("/log_to_command-ciao=10", mle::LogClassification::Critical));
    EXPECT_TRUE(m_console->removeCommand("/log_to_command"));
    EXPECT_TRUE(m_console->isLoggingToFile());
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("ciao=10") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, logCommand2){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Command);
    EXPECT_TRUE(m_console->addCommand("/log_to_command", [this](const std::string& i){EXPECT_TRUE(m_console->logToFile("/log_to_command", mle::Console::getHighestPriorityClassification()));}));
    EXPECT_TRUE(m_console->log("/log_to_command", mle::LogClassification::Critical));
    EXPECT_TRUE(m_console->removeCommand("/log_to_command"));
    EXPECT_TRUE(m_console->isLoggingToFile());
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("/log_to_command") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, logCommandNoLog2){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Command);
    EXPECT_TRUE(m_console->addCommand("/log_to_command", [this](const std::string& i){EXPECT_TRUE(i == "ciao=10");EXPECT_TRUE(m_console->logToFile("/log_to_command-ciao=10", mle::Console::getHighestPriorityClassification()));}));
    EXPECT_TRUE(m_console->log("/log_to_command-ciao=10", mle::LogClassification::Info));
    EXPECT_TRUE(m_console->removeCommand("/log_to_command"));
    EXPECT_TRUE(m_console->isLoggingToFile());
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("/log_to_command-ciao=10") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, logCommandNoLog){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Info);
    EXPECT_TRUE(m_console->addCommand("/log_to_command", [this](const std::string& i){EXPECT_TRUE(i == "ciao=10");EXPECT_FALSE(m_console->logToFile("/log_to_command-ciao=10", mle::LogClassification::Command));}));
    EXPECT_TRUE(m_console->log("/log_to_command-ciao=10", mle::LogClassification::Critical));
    EXPECT_TRUE(m_console->removeCommand("/log_to_command"));
    EXPECT_FALSE(m_console->isLoggingToFile());
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_FALSE(s.find("/log_to_command-ciao=10") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}
TEST_F(ConsoleTest, logCommandFailed){
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
    EXPECT_TRUE(m_console->addCommand("/log_to_command", [this](const std::string& i){EXPECT_TRUE(i == "ciao=10");EXPECT_TRUE(m_console->logToFile("/log_to_command-ciao=10", mle::Console::getHighestPriorityClassification()));}));
    EXPECT_FALSE(m_console->log("/log_to_command -ciao=10", mle::LogClassification::Critical));
    EXPECT_TRUE(m_console->removeCommand("/log_to_command"));
    EXPECT_TRUE(m_console->isLoggingToFile());
    while(m_console->isLoggingToFile()){
        m_timeManager->advanceTime(100);
    }

    std::ifstream log_file;
    log_file.open(getFileString("ConsoleTestTest.txt"), std::ios::in);
    if(log_file.is_open()){
        const size_t size = 1000;
        char a[size], b[size];
        a[0] = '\0';
        b[0] = a[0];
        
        while(true){
            char b[size];
            log_file.getline(b, size);
            if(b[0] == '\0'){
                break;
            }
            copy(a, b);
        }
        std::string s(a);
        EXPECT_TRUE(s.find("[COMMAND_NOT_FOUND]") != std::string::npos);

        log_file.close();
        clearFile(getFileString("ConsoleTestTest.txt"));
    }  
    else{
        FAIL();
    }
    m_console->setMinimumLogClassificationToProcess(mle::LogClassification::Normal);
}