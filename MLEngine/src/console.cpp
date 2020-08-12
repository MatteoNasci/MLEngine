#include <console.h>

#include <enginetime.h>

#include <iostream>
#include <fstream>
#include <ios>
#include <utility>
#include <chrono>

using namespace mle;

Console::Console(const std::string& loggingFileName, TimerManager& timerManager) :
    m_commands(), 
    m_loggingFileName(loggingFileName),
    m_minimumLogClassificationAccepted(LogClassification::Normal),
    m_timeManager(timerManager),
    m_logsToFile(),
    m_logToFileTask()
{
    std::ofstream log_file;
    log_file.open(m_loggingFileName, std::ios::out | std::ios::trunc);
    if(log_file.is_open()){
        const size_t time_size = 200;
        char time[time_size];
        EngineTime::nowString(time, time_size);
        log_file << "Logging started at " << time;
        log_file.close();
    }
}
Console::~Console(){
    
}
mle::LogClassification Console::getHighestPriorityClassification(){
    return mle::LogClassification::Info;
}
void Console::setMinimumLogClassificationToProcess(LogClassification minimum){
    if(minimum > Console::getHighestPriorityClassification()){
        minimum = Console::getHighestPriorityClassification();
    }

    m_classificationMutex.lock();
    m_minimumLogClassificationAccepted = minimum;
    m_classificationMutex.unlock();
}
LogClassification Console::getMinimumLogClassificationToProcess(){
    m_classificationMutex.lock();
    const auto result = m_minimumLogClassificationAccepted;
    m_classificationMutex.unlock();

    return result;
}
std::string Console::logFilename() const{
    return m_loggingFileName;
}
bool Console::addCommand(const std::string& command_key, std::function<void(const std::string& input)> command){
    if(!isMsgCommand(command_key)){
        return false;
    }
    if(m_commands.count(command_key)){
        return false;
    }
    m_commands[command_key] = command;
    return true;
}
bool Console::removeCommand(const std::string& command_key){
    if(m_commands.count(command_key)){
        m_commands.erase(command_key);
        return true;
    }
    return false;
}
void Console::clearCommands(){
    m_commands.clear();
}
void Console::getCommandList(std::vector<std::string> out_list) const{
    out_list.resize(m_commands.size());
    size_t i = 0;
    for(const auto& pair : m_commands){
        out_list[i] = pair.first;
        ++i;
    }
}
bool Console::log(const std::string& msg, const LogClassification classification){
    if(isMsgCommand(msg)){
        std::string command_key, input;
        if(!separateCommand(msg, command_key, input)){
            return false;
        }

        return command(command_key, input);
    }

    return logToFile(msg, classification);
}
bool Console::logToFile(const std::string& msg, const LogClassification classification){
    if(!isClassificationProcessable(classification)){
        return false;
    }

    m_logsToFileMutex.lock();
    m_logsToFile.push_back(msg);
    if(!m_logToFileTask.valid()){
        createAsyncTask();
    }
    m_logsToFileMutex.unlock();

    return true;
}
void Console::createAsyncTask(){
    m_logToFileTask = std::async(std::launch::async, &Console::asyncWriteToFile, logFilename(), std::move(m_logsToFile));
    m_logsToFile.clear();
    createTaskTimer();
}
void Console::createTaskTimer(){
    m_timeManager.addTimer(0.3, [this](){if(this != nullptr){this->checkAsyncTask();}});
}
bool Console::asyncWriteToFile(const std::string& filename, std::vector<std::string>&& logs){
    std::ofstream log_file;
    log_file.open(filename, std::ios::out | std::ios::app);
    if(log_file.is_open()){
        const size_t size = logs.size();

        for(size_t i = 0; i < size; ++i){
            log_file << logs[i] << std::endl;
        }
        
        log_file.close();
        return true;
    }
    return false;
}
void Console::checkAsyncTask(){
    m_logsToFileMutex.lock();
    const bool isTaskOver = (m_logToFileTask.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready);
    m_logsToFileMutex.unlock();

    if(!isTaskOver){
        createTaskTimer();
        return;
    }

    m_logsToFileMutex.lock();
    const bool result = m_logToFileTask.get();
    if(m_logsToFile.size()){
        createAsyncTask();
    }
    m_logsToFileMutex.unlock();
}
bool Console::command(const std::string& msg, const std::string& input){
    const bool command_found = m_commands.count(msg);   

    if(command_found){
        logToFile(msg + input, LogClassification::Command);
        const auto& command = m_commands[msg];
        if(command){
            command(input);
        }else{
            logToFile("Failed to find a valid function associated with command '" + msg + "'! The command will be removed.", mle::LogClassification::Error);
            removeCommand(msg);
        }
    }else{
        logToFile("[COMMAND_NOT_FOUND]" + msg + input, LogClassification::Command);
    }

    return command_found;
}

bool Console::isClassificationProcessable(const LogClassification toTest){
    return toTest >= getMinimumLogClassificationToProcess();
}
bool Console::separateCommand(const std::string& fullCommand, std::string& out_command, std::string& out_input) const{
    if(!fullCommand.size()){
        return false;
    }

    const size_t input_start_pos = fullCommand.find_first_of('-');
    const bool separator_found = input_start_pos != std::string::npos && input_start_pos != (fullCommand.size() - 1);
    if(separator_found){
        out_command = fullCommand.substr(0, input_start_pos);
        const size_t actual_input_start = input_start_pos + 1;
        out_input = fullCommand.substr(actual_input_start, fullCommand.size() - actual_input_start);
    }else{
        out_command = fullCommand;
        out_input = "";
    }
    
    return out_command.size();
}
bool Console::isMsgCommand(const std::string& msg) const{
    if(!msg.size()){
        return false;
    }
    return (msg[0] == '/');
}
