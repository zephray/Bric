#include <stdio.h>
#include <signal.h>
#include <FreeRTOS.h>
#include <task.h>
#include <SDL/SDL.h>
#include "app.h"
#include "hal_input.h"

void housekeepingTask(void *arg){
    SDL_Event event;

    while(1){
        if (SDL_PollEvent(&event)) {
            switch (event.type)
            {
            case SDL_QUIT:
                // vTaskEndScheduler is not implemented, but need to exit
                kill(getpid(), SIGKILL);
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                _hal_input_handle_sdl_keyevent(event.key);
                break;
            
            default:
                break;
            }
        }
        // Yield
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


int main(){
    xTaskCreate(housekeepingTask, "Housekeeping Task", 128,
            NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(app_task, "App Task", APP_TASK_HEAPSIZE,
            NULL, APP_TASK_PRIORITY, NULL);
    vTaskStartScheduler();

    return 0;
}

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
    printf("ASSERT: %s : %d\n", pcFileName, (int)ulLine);
    return;
}

void vApplicationMallocFailedHook(void)
{
    printf("Malloc failed\n");
    return;
}