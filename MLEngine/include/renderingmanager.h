#ifndef RENDERINGMANAGER_H
#define RENDERINGMANAGER_H

#include <mlengine_global.h>

#include <windowhintsdata.h>
#include <windowsharedata.h>
#include <monitordata.h>

#include <list>
#include <string>

struct GLFWwindow;
namespace mle{
class MLENGINE_SHARED_EXPORT RenderingManager{ //TODO: test all
public:
    bool isInitialized() const;
    bool isRunningLoop() const;
    void stopLoop();
    bool init();
    void release();
    int loop();
    bool addWindow(const std::string& title, const int width, const int height, const MonitorData::Handle& monitor_data, const WindowShareData& share, const WindowHintsData& hints);
    WindowShareData getCurrentWindow() const;
    static RenderingManager& instance();

    ~RenderingManager();
private: RenderingManager();
    std::list<GLFWwindow*> m_windows;
    bool m_initialized;
    bool m_run;
};
};
#endif