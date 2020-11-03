#include <stdio.h>
#include <signal.h>
#include <FreeRTOS.h>
#include <task.h>
#include <SDL/SDL.h>
#include "app.h"

void housekeepingTask(void *arg){
    SDL_Event event;

    while(1){
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                // vTaskEndScheduler is not implemented, but need to exit
                kill(getpid(), SIGKILL);
            }
        }
        // Yield
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


int main(){
    xTaskCreate(housekeepingTask, "Housekeeping Task", 128,
            NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(startup_task, "Startup Task", STARTUP_TASK_HEAPSIZE,
            NULL, STARTUP_TASK_PRIORITY, NULL);
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