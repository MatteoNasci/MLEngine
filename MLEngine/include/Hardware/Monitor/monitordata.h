#ifndef MONITORDATA_H
#define MONITORDATA_H

#include <mlengine_global.h>

#include <Engine/Debug/console.h>
#include <Engine/Core/engineerror.h>
#include <Hardware/Monitor/monitorhandle.h>
#include <Hardware/Monitor/monitormode.h>
#include <Hardware/Monitor/gammaramp.h>
#include <Hardware/Monitor/monitordetails.h>

#include <string>
#include <functional>
#include <vector>
//https://www.glfw.org/docs/latest/group__monitor.html
struct GLFWmonitor;
namespace mle{
class MLENGINE_SHARED_EXPORT MonitorData{ //TODO: test all
public:
    EngineError isMonitorIndexValid(const size_t index) const;
    EngineError isSelectedMonitorValid() const;
    EngineError selectMainMonitor();
    EngineError selectMonitorAt(const size_t index);
    EngineError updateMonitorList();
    size_t getSelectedMonitorIndex() const;
    size_t getMonitorsCount() const;
    void setMonitorsUpdatedCallback(std::function<void(void)> updatedCallback);
    double getDpi(const MonitorMode& mode, const int width_mm) const;
    double getDpi(const int width, const int width_mm) const;
    EngineError getMonitorInformation(const size_t monitor_index, MonitorDetails& out_details) const;
    EngineError logMonitorsInformation(Console& logger, const LogClassification classification) const;
    EngineError logMonitorInformation(const size_t monitor_index, Console& logger, const LogClassification classification) const;
    EngineError logMonitorInformation(const MonitorDetails& monitor_details, Console& logger, const LogClassification classification) const;


    EngineError getMonitorPosition(const size_t monitor_index, int& out_x, int& out_y) const;
    EngineError getMonitorPosition(int& out_x, int& out_y) const;
    EngineError getMonitorWorkArea(const size_t monitor_index, int& out_x, int& out_y, int& out_width, int& out_height) const;
    EngineError getMonitorWorkArea(int& out_x, int& out_y, int& out_width, int& out_height) const;
    EngineError getMonitorPhysicalSize(const size_t monitor_index, int& out_widthMM, int& out_heightMM) const;
    EngineError getMonitorPhysicalSize(int& out_widthMM, int& out_heightMM) const;
    EngineError getMonitorContentScale(const size_t monitor_index, float& out_scaleX, float& out_scaleY) const;
    EngineError getMonitorContentScale(float& out_scaleX, float& out_scaleY) const;
    EngineError getMonitorName(const size_t monitor_index, std::string& out_name) const;
    EngineError getMonitorName(std::string& out_name) const;
    EngineError setMonitorUserPointer(const size_t monitor_index, void* pointer) const;
    EngineError setMonitorUserPointer(void* pointer) const;
    EngineError getMonitorUserPointer(const size_t monitor_index, void** out_pointer) const;
    EngineError getMonitorUserPointer(void** out_pointer) const;
    EngineError setMonitorGamma(const size_t monitor_index, const float gamma) const;
    EngineError setMonitorGamma(const float gamma) const;
    EngineError setMonitorGammaRamp(const size_t monitor_index, const GammaRamp& data) const;
    EngineError setMonitorGammaRamp(const GammaRamp& data) const;
    EngineError getMonitorGammaRamp(const size_t monitor_index, GammaRamp& out_gammaRamp) const;
    EngineError getMonitorGammaRamp(GammaRamp& out_gammaRamp) const;
    EngineError getMonitorModes(const size_t monitor_index, std::vector<MonitorMode>& out_modes) const;
    EngineError getMonitorModes(std::vector<MonitorMode>& out_modes) const;
    EngineError getMonitorCurrentMode(const size_t monitor_index, MonitorMode& out_mode) const;
    EngineError getMonitorCurrentMode(MonitorMode& out_mode) const;
    MonitorHandle getSelectedMonitor() const;
    MonitorHandle getMonitorAt(const size_t monitor_index) const;

    static double calculateDpi(const MonitorMode& mode, const int width_mm);
    static double calculateDpi(const int width, const int width_mm);

    friend class Engine;

    MonitorData();
    MonitorData(const MonitorData& rhs) = delete;
    MonitorData(MonitorData&& rhs) = delete;
    MonitorData& operator=(const MonitorData& rhs) = delete;
    MonitorData& operator=(MonitorData&& rhs) = delete;
private: 
    static void monitorEventReceiver(GLFWmonitor* monitor, const int event);
    static EngineError getAndClearError();

    EngineError setMonitorCallback();
private:
    size_t m_selectedMonitorIndex;
    std::vector<GLFWmonitor*> m_monitors;

    std::function<void(void)> m_monitorsUpdateCallback;
};
};
#endif