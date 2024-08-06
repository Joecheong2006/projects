#ifndef _ENTRY_POINT_H_
#define _ENTRY_POINT_H_

extern application_setup create_application();

int main() {
    application_setup setup = create_application();

    if (!setup.on_initialize(setup.app)) {
        LOG_FATAL("%s", "failed to initialize application\n");
        exit(1);
    }
    while (setup.is_running(setup.app)) {
        setup.on_update(setup.app);
    }
    
    setup.on_terminate(setup.app);
}

#endif
