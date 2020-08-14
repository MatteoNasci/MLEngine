#ifndef MONITORDATA_H
#define MONITORDATA_H

#include <mlengine_global.h>

#include <string>
#include <functional>
#include <vector>
#include <console.h>
//https://www.glfw.org/docs/latest/group__monitor.html
struct GLFWmonitor;
namespace mle{
class RenderingManager;
class MLENGINE_SHARED_EXPORT MonitorData{ //TODO: test all
public:
    struct MLENGINE_SHARED_EXPORT GammaRamp{
        std::vector<unsigned short> reds;
        std::vector<unsigned short> greens;
        std::vector<unsigned short> blues;
    };
    struct MLENGINE_SHARED_EXPORT Mode{
        int width, height, redBits, greenBits, blueBits, refreshRate;
    };
    struct MLENGINE_SHARED_EXPORT Details{
        GammaRamp gammaRamp;
        Mode mode;
        std::vector<Mode> modes;
        std::string name;
        void* userData;
        int positionX, positionY;
        int workareaPositionX, workareaPositionY, workareaWidth, workareaHeight;
        int widthMM, heightMM;
        float scaleX, scaleY;
        size_t index;
    };
    struct MLENGINE_SHARED_EXPORT Handle{
        Handle();  
        
        friend class MonitorData;
        friend class RenderingManager;
    private:
        Handle(GLFWmonitor* in_monitor);
        GLFWmonitor* monitor;
        GLFWmonitor* getMonitor() const;
    };
public:
    bool isMonitorIndexValid(const size_t index) const;
    bool isSelectedMonitorValid() const;
    bool selectMainMonitor();
    bool selectMonitorAt(const size_t index);
    bool updateMonitorList();
    size_t getSelectedMonitorIndex() const;
    size_t getMonitorsCount() const;
    void setMonitorsUpdatedCallback(std::function<void(void)> updatedCallback);
    double getDpi(const Mode& mode, const int width_mm) const;
    double getDpi(const int width, const int width_mm) const;
    bool getMonitorInformation(const size_t monitor_index, MonitorData::Details& out_details) const;
    bool logMonitorsInformation(Console& logger, const LogClassification classification) const;
    bool logMonitorInformation(const size_t monitor_index, Console& logger, const LogClassification classification) const;
    bool logMonitorInformation(const MonitorData::Details& monitor_details, Console& logger, const LogClassification classification) const;


    bool getMonitorPosition(const size_t monitor_index, int& out_x, int& out_y) const;
    bool getMonitorPosition(int& out_x, int& out_y) const;
    bool getMonitorWorkArea(const size_t monitor_index, int& out_x, int& out_y, int& out_width, int& out_height) const;
    bool getMonitorWorkArea(int& out_x, int& out_y, int& out_width, int& out_height) const;
    bool getMonitorPhysicalSize(const size_t monitor_index, int& out_widthMM, int& out_heightMM) const;
    bool getMonitorPhysicalSize(int& out_widthMM, int& out_heightMM) const;
    bool getMonitorContentScale(const size_t monitor_index, float& out_scaleX, float& out_scaleY) const;
    bool getMonitorContentScale(float& out_scaleX, float& out_scaleY) const;
    bool getMonitorName(const size_t monitor_index, std::string& out_name) const;
    bool getMonitorName(std::string& out_name) const;
    bool setMonitorUserPointer(const size_t monitor_index, void* pointer) const;
    bool setMonitorUserPointer(void* pointer) const;
    bool getMonitorUserPointer(const size_t monitor_index, void** out_pointer) const;
    bool getMonitorUserPointer(void** out_pointer) const;
    bool setMonitorGamma(const size_t monitor_index, const float gamma) const;
    bool setMonitorGamma(const float gamma) const;
    bool setMonitorGammaRamp(const size_t monitor_index, const MonitorData::GammaRamp& data) const;
    bool setMonitorGammaRamp(const MonitorData::GammaRamp& data) const;
    bool getMonitorGammaRamp(const size_t monitor_index, MonitorData::GammaRamp& out_gammaRamp) const;
    bool getMonitorGammaRamp(MonitorData::GammaRamp& out_gammaRamp) const;
    bool getMonitorModes(const size_t monitor_index, std::vector<MonitorData::Mode>& out_modes) const;
    bool getMonitorModes(std::vector<MonitorData::Mode>& out_modes) const;
    bool getMonitorCurrentMode(const size_t monitor_index, MonitorData::Mode& out_mode) const;
    bool getMonitorCurrentMode(MonitorData::Mode& out_mode) const;
    MonitorData::Handle getSelectedMonitor();
    MonitorData::Handle getMonitorAt(const size_t monitor_index);

    static double calculateDpi(const Mode& mode, const int width_mm);
    static double calculateDpi(const int width, const int width_mm);
    static MonitorData& instance();
private: MonitorData();
    ~MonitorData();
    static void monitorEventReceiver(GLFWmonitor* monitor, const int event);
private:
    size_t m_selectedMonitorIndex;
    std::vector<GLFWmonitor*> m_monitors;

    std::function<void(void)> m_monitorsUpdateCallback;
};
};
#endif