#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/icon_i.h>
#include <input/input.h>
#include <stdlib.h>

#include "assets_icons.h"

#define DINO_START_X 0
#define DINO_START_Y 42

#define FPS 60

#define DINO_RUNNING_MS_PER_FRAME 100

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    FuriMutex* mutex;
    FuriTimer* timer;
    uint32_t last_tick;
    const Icon* dino_icon;
    int dino_frame_ms;
} GameState;

static void timer_callback(void* ctx) {
    furi_assert(ctx);
    GameState* game_state = ctx;
    furi_mutex_acquire(game_state->mutex, FuriWaitForever);

    uint32_t ticks_elapsed = furi_get_tick() - game_state->last_tick;
    int delta_time_ms = ticks_elapsed * 1000 / furi_kernel_get_tick_frequency();

    // dino update
    game_state->dino_frame_ms += delta_time_ms;
    // TODO: switch by dino state
    if(game_state->dino_frame_ms >= DINO_RUNNING_MS_PER_FRAME) {
        if(game_state->dino_icon == &I_DinoRun0) {
            game_state->dino_icon = &I_DinoRun1;
        } else {
            game_state->dino_icon = &I_DinoRun0;
        }
        game_state->dino_frame_ms = 0;
    }
    furi_mutex_release(game_state->mutex);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void render_callback(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    GameState* game_state = ctx;
    furi_mutex_acquire(game_state->mutex, FuriWaitForever);

    //  canvas_draw_xbm(canvas, 0, 0, dino_width, dino_height, dino_bits);
    canvas_draw_icon(canvas, DINO_START_X, DINO_START_Y, game_state->dino_icon);

    furi_mutex_release(game_state->mutex);
}

static void game_state_init(GameState* const game_state) {
    game_state->last_tick = furi_get_tick();
    game_state->dino_frame_ms = 0;
    game_state->dino_icon = &I_Dino;
}

int32_t trexrunner_app(void* p) {
    UNUSED(p);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    GameState* game_state = malloc(sizeof(GameState));
    game_state_init(game_state);

    game_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!game_state->mutex) {
        FURI_LOG_E("T-rex runner", "cannot create mutex\r\n");
        free(game_state);
        return 255;
    }
    // BEGIN IMPLEMENTATION

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, game_state);
    view_port_input_callback_set(view_port, input_callback, event_queue);
    game_state->timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, game_state);

    furi_timer_start(game_state->timer, (uint32_t)furi_kernel_get_tick_frequency() / FPS);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        furi_mutex_acquire(game_state->mutex, FuriWaitForever);
        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypeShort) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyLeft:
                        break;
                    case InputKeyRight:
                        break;
                    case InputKeyOk:
                        break;
                    case InputKeyBack:
                        // Exit the app
                        processing = false;
                        break;
                    case InputKeyMAX:
                        break;
                    }
                }
            }
        }
        view_port_update(view_port);
        furi_mutex_release(game_state->mutex);
    }
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_mutex_free(game_state->mutex);
    furi_timer_free(game_state->timer);
    free(game_state);
    return 0;
}
