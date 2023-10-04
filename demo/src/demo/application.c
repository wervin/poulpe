#include "demo/application.h"
#include "demo/renderer.h"
#include "demo/ui.h"
#include "demo/window.h"

enum demo_error demo_application_init(struct demo_application_info *info)
{   
    enum demo_error status = DEMO_ERROR_NONE;

    struct demo_window_info window_info = {
        .default_width = info->default_width,
        .default_height = info->default_height,
        .fullscreen = info->fullscreen,
        .application_name = info->application_name
    };
    
    status = demo_window_init(&window_info);
    if (status != DEMO_ERROR_NONE)
        return status;
    
    status = demo_renderer_init();
    if (status != DEMO_ERROR_NONE)
        return status;
    
    status = demo_ui_init();
    if (status != DEMO_ERROR_NONE)
        return status;

    return DEMO_ERROR_NONE;
}

enum demo_error demo_application_run()
{    
    demo_window_show();
    
    while (!demo_window_should_close())
    {
        demo_window_poll_events();
        demo_ui_draw();
        demo_renderer_draw();
    }

    demo_renderer_wait_idle();

    return DEMO_ERROR_NONE;
}

void demo_application_destroy()
{
    demo_ui_destroy();
    demo_renderer_destroy();
    demo_window_destroy();
}