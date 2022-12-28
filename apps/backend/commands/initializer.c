#include <pthread.h>
#include <unistd.h>
#include "initializer.h"
#include "../../../shared/helpers/helpers.h"

int command_try_execution(char* command, struct string_list* arguments, int pid_response){
    printf("    > Verifying command existence\n");
    int exit = 0;

    if(command_validate_name(command,"list")){
        exec_command_list(pid_response);
    }
    else if(command_validate_name(command,"login") && pid_response != -1 && arguments != NULL){
        exec_command_verify_login(arguments, pid_response);
    }
    else if(command_validate_name(command,"cash") && pid_response != -1){
        exec_command_cash(pid_response);
    }
    else if(command_validate_name(command,"users")){
        exec_command_list_users();
    }else if(command_validate_name(command,"kick")){
        exec_command_kick(arguments);
    }else if(command_validate_name(command,"prom")){
        exec_command_prom();
    }else if(command_validate_name(command,"reprom")){
        exec_command_reprom();
    }else if(command_validate_name(command,"cancel")){
        exec_command_cancel_prom(arguments);
    }else if(command_validate_name(command,"testlib")){
        test_users_lib();
    }else if(command_validate_name(command,"close")){
        printf("     > closing the application...\n");
        exit = 1;
    }else{
        printf("     > Couldn't find any command for: %s\n", command);
    }
    return exit;
}

void command_handler_start(Backend* app){
    creset();
    size_t bufsize = 255;
    char buffer[bufsize];

    if(app->threads.running == 0){
        pthread_exit(0);
    }

    printf(" > [INFO] WE ARE LISTENING TO ANY COMMAND:\n");
    fgets(buffer,bufsize,stdin);

    if(app->threads.running == 0){
        pthread_exit(0);
    }

    printf(" > Command received: %s", buffer);

    size_t len = strlen(buffer);

    if(len > 1){
        //remove a possible added \n
        if (buffer[len - 1] == '\n') {  // FAILS when len == 0
            buffer[len -1] = '\0';
        }

        struct string_list* arguments = get_command_arguments(buffer);

        //command_arguments_display(arguments);
        int close_app = command_try_execution(arguments->string,arguments->next,-1);
        if(!close_app){
            //clean the arguments memory allocated to avoid memory leaks
            clean_linked_list(arguments);
            command_handler_start(app);
        }else{
            kill(getpid(), SIGINT);
        }
    }else{
        printf("> Invalid command provided, please try again..\n");
        command_handler_start(app);
    }
}



