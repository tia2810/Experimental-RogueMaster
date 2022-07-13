#include "../drestorer.h"
#include "cmsis_os2.h"

void drestorer_scene_progress_on_enter(void* context) {
    StorageMoveToSd* app = context;

    widget_add_string_element(
        app->widget, 64, 10, AlignCenter, AlignCenter, FontPrimary, "Moving...");

    view_dispatcher_switch_to_view(app->view_dispatcher, StorageMoveToSdViewWidget);

    drestorer_perform();
    view_dispatcher_send_custom_event(app->view_dispatcher, MoveToSdCustomEventExit);
}

bool drestorer_scene_progress_on_event(void* context, SceneManagerEvent event) {
    StorageMoveToSd* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        view_dispatcher_stop(app->view_dispatcher);
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    }

    return consumed;
}

void drestorer_scene_progress_on_exit(void* context) {
    StorageMoveToSd* app = context;
    widget_reset(app->widget);
}
