#include <Hardware/Monitor/monitordata.h>
#include <GLFW/glfw3.h>

#include <Engine/Core/engine.h>
#include <Hardware/connectionevent.h>

#include <sstream>
using namespace mle;
MonitorData::MonitorData() : m_selectedMonitorIndex(0), m_monitors(), m_monitorsUpdateCallback(){
}
void MonitorData::monitorEventReceiver(GLFWmonitor* monitor, const int event){
    const std::string monitor_name(glfwGetMonitorName(monitor));
    void* user_pointer = glfwGetMonitorUserPointer(monitor);
    if (event == static_cast<int>(ConnectionEvent::Connected))
    {
        // The monitor was connected
        Engine::instance().console().log("The monitor '" + monitor_name + "' is now connected!", Console::getHighestPriorityClassification());
    }
    else if(event == static_cast<int>(ConnectionEvent::Disconnected))
    {
        // The monitor was disconnected
        // In this case only glfwGetMonitorName and glfwGetMonitorUserPointer will be available for the monitor and only before the end of the callback
        Engine::instance().console().log("The monitor '" + monitor_name + "' is now disconnected!", Console::getHighestPriorityClassification());
    }

    Engine::instance().monitorData().updateMonitorList();
}
double MonitorData::calculateDpi(const int width, const int width_mm){
    return static_cast<double>(width) / (static_cast<double>(width_mm) / 25.4);
}
double MonitorData::calculateDpi(const MonitorMode& mode, const int width_mm){
    return calculateDpi(mode.width, width_mm);
}
double MonitorData::getDpi(const MonitorMode& mode, const int width_mm) const{
    return MonitorData::calculateDpi(mode, width_mm);
}
double MonitorData::getDpi(const int width, const int width_mm) const{
    return MonitorData::calculateDpi(width, width_mm);
}
EngineError MonitorData::isMonitorIndexValid(const size_t index) const{
    if(index < m_monitors.size()){
        return (m_monitors[index] != nullptr) ? EngineError::Ok : EngineError::InvalidValue;
    }
    return EngineError::InvalidValue;
}
EngineError MonitorData::isSelectedMonitorValid() const{
    return isMonitorIndexValid(m_selectedMonitorIndex);
}
size_t MonitorData::getSelectedMonitorIndex() const{
    return m_selectedMonitorIndex;
}
MonitorHandle MonitorData::getSelectedMonitor() const{
    return MonitorHandle((isSelectedMonitorValid() == EngineError::Ok ? m_monitors[m_selectedMonitorIndex] : nullptr));
}
MonitorHandle MonitorData::getMonitorAt(const size_t monitor_index) const{
    return MonitorHandle((isMonitorIndexValid(monitor_index) == EngineError::Ok ? m_monitors[monitor_index] : nullptr));
}
size_t MonitorData::getMonitorsCount() const{
    return m_monitors.size();
}
void MonitorData::setMonitorsUpdatedCallback(std::function<void(void)> updatedCallback){
    m_monitorsUpdateCallback = updatedCallback;
}
EngineError MonitorData::updateMonitorList(){
    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    auto e = getAndClearError();
    if(e != EngineError::Ok){
        return e;
    }

    m_monitors.clear();
    m_monitors.resize(count);

    for(size_t i = 0; i < m_monitors.size(); ++i){
        m_monitors[i] = monitors[i];
    }

    const EngineError result = selectMainMonitor();

    if(m_monitorsUpdateCallback){
        m_monitorsUpdateCallback();
    }

    return result;
}
EngineError MonitorData::selectMainMonitor(){
    m_selectedMonitorIndex = 0;
    return isSelectedMonitorValid();
}
EngineError MonitorData::selectMonitorAt(const size_t index){
    if(isMonitorIndexValid(index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    m_selectedMonitorIndex = index;
    return EngineError::Ok;
}
EngineError MonitorData::getMonitorInformation(const size_t monitor_index, MonitorDetails& out_details) const{
    if(isMonitorIndexValid(monitor_index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    EngineError result = EngineError::Ok;
    int error_count = 0;
    out_details.index = monitor_index;
    auto e = getMonitorPosition(out_details.index, out_details.positionX, out_details.positionY);
    if(e != EngineError::Ok){
        result = e;
        error_count++;
    }
    e = getMonitorWorkArea(out_details.index, out_details.workareaPositionX, out_details.workareaPositionY, out_details.workareaWidth, out_details.workareaHeight);
    if(e != EngineError::Ok){
        result = e;
        error_count++;
    }
    e = getMonitorPhysicalSize(out_details.index, out_details.widthMM, out_details.heightMM);
    if(e != EngineError::Ok){
        result = e;
        error_count++;
    }
    e = getMonitorContentScale(out_details.index, out_details.scaleX, out_details.scaleY);
    if(e != EngineError::Ok){
        result = e;
        error_count++;
    }
    e = getMonitorName(out_details.index, out_details.name);
    if(e != EngineError::Ok){
        result = e;
        error_count++;
    }
    e = getMonitorUserPointer(out_details.index, &(out_details.userData));
    if(e != EngineError::Ok){
        result = e;
        error_count++;
    }
    e = getMonitorGammaRamp(out_details.index, out_details.gammaRamp);
    if(e != EngineError::Ok){
        result = e;
        error_count++;
    }
    e = getMonitorModes(out_details.index, out_details.modes);
    if(e != EngineError::Ok){
        result = e;
        error_count++;
    }
    e = getMonitorCurrentMode(out_details.index, out_details.mode);
    if(e != EngineError::Ok){
        result = e;
        error_count++;
    }

    static const int max_errors = 9;
    return error_count == max_errors ? EngineError::ApiUnavailable : EngineError::InvalidValue;
}
EngineError MonitorData::logMonitorInformation(const MonitorDetails& monitor_details, Console& logger, const LogClassification classification) const{  
    std::stringstream stream;
    stream << "/log " << Console::stringFromClassification(Console::getHighestPriorityClassification()) << Console::getCommandInputSeparator() << "Monitor name: '" << monitor_details.name << "', indexed at: '" << monitor_details.index << "'.\n" <<
    "Position: {" << monitor_details.positionX << "::" << monitor_details.positionY << "}\n" <<
    "Workarea position: {" << monitor_details.workareaPositionX << "::" << monitor_details.workareaPositionY << "}\n" <<
    "Workarea size: {" << monitor_details.workareaWidth << "::" << monitor_details.workareaHeight << "}\n" <<
    "Content scale: {" << monitor_details.scaleX << "::" << monitor_details.scaleY << "}\n" <<
    "Monitor size(mm): {" << monitor_details.widthMM << "::" << monitor_details.heightMM << "}\n" <<
    "Monitor dpi: '" << getDpi(monitor_details.mode.width, monitor_details.widthMM) << "'\n" <<
    "Current mode:\n" <<
    "\twidth: '" << monitor_details.mode.width << "'\n" <<
    "\theight: '" << monitor_details.mode.height << "'\n" <<
    "\tred bits: '" << monitor_details.mode.redBits << "'\n" <<
    "\tgreen bits: '" << monitor_details.mode.greenBits << "'\n" <<
    "\tblue bits: '" << monitor_details.mode.blueBits << "'\n" <<
    "\trefresh rate: '" << monitor_details.mode.refreshRate << "'\n" <<
    "All modes:\n";
    for(size_t i = 0; i < monitor_details.modes.size(); ++i){
        const MonitorMode& mode = monitor_details.modes[i];

        stream << "\tMode n: '" << i << "'\n" <<
        "\twidth: '" << mode.width << "'\n" <<
        "\theight: '" << mode.height << "'\n" <<
        "\tred bits: '" << mode.redBits << "'\n" <<
        "\tgreen bits: '" << mode.greenBits << "'\n" <<
        "\tblue bits: '" << mode.blueBits << "'\n" <<
        "\trefresh rate: '" << mode.refreshRate << "'\n";
        if(i != monitor_details.modes.size() - 1){
            stream << "\n";
        }
    }
    stream << "Gamma ramp:\n" << "\tRed Green Blue\n";
    for(size_t i = 0; i < monitor_details.gammaRamp.reds.size(); ++i){
        unsigned short red = monitor_details.gammaRamp.reds[i], green = monitor_details.gammaRamp.greens[i], blue = monitor_details.gammaRamp.blues[i];
        stream << "\t" << red << " " << green << " " << blue;
        if(i != monitor_details.gammaRamp.reds.size() - 1){
            stream << "\n";
        }
    }

    return logger.log(stream.str(), classification) ? EngineError::Ok : EngineError::InvalidValue;
}
EngineError MonitorData::logMonitorsInformation(Console& logger, const LogClassification classification) const{
    if(!m_monitors.size()){
        return EngineError::InvalidValue;
    }

    EngineError result = EngineError::Ok;
    logger.log("Retrieving all connected monitors information...\nTotal monitors found: " + std::to_string(m_monitors.size()), classification);
    for(size_t i = 0; i < m_monitors.size(); ++i){
        MonitorDetails details;
        auto e = getMonitorInformation(i, details);
        if(e != EngineError::Ok){
            result = e;
            if(e != EngineError::ApiUnavailable){
                logMonitorInformation(details, logger, classification);
            }
        }
    }
    return result;
}
EngineError MonitorData::logMonitorInformation(const size_t monitor_index, Console& logger, const LogClassification classification) const{
    if(isMonitorIndexValid(monitor_index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    MonitorDetails details;
    if(getMonitorInformation(monitor_index, details) != EngineError::ApiUnavailable){
        return logMonitorInformation(details, logger, classification);
    }
    return EngineError::ApiUnavailable;
}
EngineError MonitorData::getMonitorPosition(const size_t monitor_index, int& out_x, int& out_y) const{
    if(isMonitorIndexValid(monitor_index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    glfwGetMonitorPos(m_monitors[monitor_index], &out_x, &out_y);
    return getAndClearError();
}
EngineError MonitorData::getMonitorPosition(int& out_x, int& out_y) const{
    return getMonitorPosition(m_selectedMonitorIndex, out_x, out_y);
}
EngineError MonitorData::getMonitorWorkArea(const size_t monitor_index, int& out_x, int& out_y, int& out_width, int& out_height) const{
    if(isMonitorIndexValid(monitor_index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    glfwGetMonitorWorkarea(m_monitors[monitor_index], &out_x, &out_y, &out_width, &out_height);
    return getAndClearError();
}
EngineError MonitorData::getMonitorWorkArea(int& out_x, int& out_y, int& out_width, int& out_height) const{
    return getMonitorWorkArea(m_selectedMonitorIndex, out_x, out_y, out_width, out_height);
}
EngineError MonitorData::getMonitorPhysicalSize(const size_t monitor_index, int& out_widthMM, int& out_heightMM) const{
    if(isMonitorIndexValid(monitor_index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    glfwGetMonitorPhysicalSize(m_monitors[monitor_index], &out_widthMM, &out_heightMM);
    return getAndClearError();
}
EngineError MonitorData::getMonitorPhysicalSize(int& out_widthMM, int& out_heightMM) const{
    return getMonitorPhysicalSize(m_selectedMonitorIndex, out_widthMM, out_heightMM);
}
EngineError MonitorData::getMonitorContentScale(const size_t monitor_index, float& out_scaleX, float& out_scaleY) const{
    if(isMonitorIndexValid(monitor_index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    glfwGetMonitorContentScale(m_monitors[monitor_index], &out_scaleX, &out_scaleY);

    return getAndClearError();
}
EngineError MonitorData::getMonitorContentScale(float& out_scaleX, float& out_scaleY) const{
    return getMonitorContentScale(m_selectedMonitorIndex, out_scaleX, out_scaleY);
}
EngineError MonitorData::getMonitorName(const size_t monitor_index, std::string& out_name) const{
    if(isMonitorIndexValid(monitor_index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    out_name = glfwGetMonitorName(m_monitors[monitor_index]);
    return getAndClearError();
}
EngineError MonitorData::getMonitorName(std::string& out_name) const{
    return getMonitorName(m_selectedMonitorIndex, out_name);
}
EngineError MonitorData::setMonitorUserPointer(const size_t monitor_index, void* pointer) const{
    if(isMonitorIndexValid(monitor_index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    glfwSetMonitorUserPointer(m_monitors[monitor_index], pointer);
    return getAndClearError();
}
EngineError MonitorData::setMonitorUserPointer(void* pointer) const{
    return setMonitorUserPointer(m_selectedMonitorIndex, pointer);
}
EngineError MonitorData::getMonitorUserPointer(const size_t monitor_index, void** out_pointer) const{
    if(isMonitorIndexValid(monitor_index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    *out_pointer = glfwGetMonitorUserPointer(m_monitors[monitor_index]);
    return getAndClearError();
}
EngineError MonitorData::getMonitorUserPointer(void** out_pointer) const{
    return getMonitorUserPointer(m_selectedMonitorIndex, out_pointer);
}
EngineError MonitorData::setMonitorGamma(const size_t monitor_index, const float gamma) const{
    if(isMonitorIndexValid(monitor_index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    glfwSetGamma(m_monitors[monitor_index], gamma);
    return getAndClearError();
}
EngineError MonitorData::setMonitorGamma(const float gamma) const{
    return setMonitorGamma(m_selectedMonitorIndex, gamma);
}
EngineError MonitorData::setMonitorGammaRamp(const size_t monitor_index, const GammaRamp& data) const{
    if(isMonitorIndexValid(monitor_index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    GLFWgammaramp ramp;
    ramp.size = unsigned int(data.reds.size());
    ramp.red = new unsigned short[ramp.size];
    ramp.green = new unsigned short[ramp.size];
    ramp.blue = new unsigned short[ramp.size];
    for(size_t i = 0; i < data.blues.size(); ++i){
        ramp.red[i] = data.reds[i];
        ramp.green[i] = data.greens[i];
        ramp.blue[i] = data.blues[i];
    }
    glfwSetGammaRamp(m_monitors[monitor_index], &ramp);

    delete[] ramp.red;
    delete[] ramp.green;
    delete[] ramp.blue;
    return getAndClearError();
}
EngineError MonitorData::setMonitorGammaRamp(const GammaRamp& data) const{
    return setMonitorGammaRamp(m_selectedMonitorIndex, data);
}
EngineError MonitorData::getMonitorGammaRamp(const size_t monitor_index, GammaRamp& out_gammaRamp) const{
    if(isMonitorIndexValid(monitor_index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    const GLFWgammaramp* ramp = glfwGetGammaRamp(m_monitors[monitor_index]);
    
    unsigned int size = ramp == nullptr ? 0 : ramp->size;
    out_gammaRamp.reds.resize(size);
    out_gammaRamp.greens.resize(size);
    out_gammaRamp.blues.resize(size);
    for(size_t i = 0; i < size; ++i){
        out_gammaRamp.reds[i] = ramp->red[i];
        out_gammaRamp.greens[i] = ramp->green[i];
        out_gammaRamp.blues[i] = ramp->blue[i];
    }

    return getAndClearError();
}
EngineError MonitorData::getMonitorGammaRamp(GammaRamp& out_gammaRamp) const{
    return getMonitorGammaRamp(m_selectedMonitorIndex, out_gammaRamp);
}
EngineError MonitorData::getMonitorModes(const size_t monitor_index, std::vector<MonitorMode>& out_modes) const{
    if(isMonitorIndexValid(monitor_index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    int count;
    const GLFWvidmode* modes = glfwGetVideoModes(m_monitors[monitor_index], &count);

    out_modes.resize(count);
    for(size_t i = 0; i < out_modes.size(); ++i){
        const GLFWvidmode* mode = modes + i;
        MonitorMode& out_mode = out_modes[i];
        if(mode != nullptr){
            out_mode.redBits = mode->redBits;
            out_mode.greenBits = mode->greenBits;
            out_mode.blueBits = mode->blueBits;
            out_mode.width = mode->width;
            out_mode.height = mode->height;
            out_mode.refreshRate = mode->refreshRate;
        }else{
            out_mode.redBits = 0;
            out_mode.greenBits = 0;
            out_mode.blueBits = 0;
            out_mode.width = 0;
            out_mode.height = 0;
            out_mode.refreshRate = 0;
        }
    }
    return getAndClearError();
}
EngineError MonitorData::getMonitorModes(std::vector<MonitorMode>& out_modes) const{
    return getMonitorModes(m_selectedMonitorIndex, out_modes);
}
EngineError MonitorData::getMonitorCurrentMode(const size_t monitor_index, MonitorMode& out_mode) const{
    if(isMonitorIndexValid(monitor_index) != EngineError::Ok){
        return EngineError::InvalidValue;
    }

    const GLFWvidmode* mode = glfwGetVideoMode(m_monitors[monitor_index]);

    if(mode != nullptr){
        out_mode.redBits = mode->redBits;
        out_mode.greenBits = mode->greenBits;
        out_mode.blueBits = mode->blueBits;
        out_mode.width = mode->width;
        out_mode.height = mode->height;
        out_mode.refreshRate = mode->refreshRate;
    }else{
        out_mode.redBits = 0;
        out_mode.greenBits = 0;
        out_mode.blueBits = 0;
        out_mode.width = 0;
        out_mode.height = 0;
        out_mode.refreshRate = 0;
    }
    
    return getAndClearError();
}
EngineError MonitorData::getMonitorCurrentMode(MonitorMode& out_mode) const{
    return getMonitorCurrentMode(m_selectedMonitorIndex, out_mode);
}
EngineError MonitorData::getAndClearError(){
    const char** s = nullptr;
    return static_cast<EngineError>(glfwGetError(s));
}