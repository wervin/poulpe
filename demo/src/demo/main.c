#include "demo/application.h"
#include "demo/error.h"

int main(void)
{
    enum demo_error error = DEMO_ERROR_NONE;

    struct demo_application_info info = {
        .default_width = 800,
        .default_height = 600,
        .fullscreen = true,

        .application_name = "Demo",
        .application_version_major = 1,
        .application_version_minor = 0,
        .application_version_revision = 0
};

    error = demo_application_init(&info);
    if (error != DEMO_ERROR_NONE)
        goto clean;

    error = demo_application_run();
    if (error != DEMO_ERROR_NONE)
        goto clean;

clean:
    demo_application_destroy();
    return error;
}