#include "./commands.h"
#include "../../../shared/helpers/helpers.h"
#include "../../../users_lib.h"
#include "../notifier.h"
#include "../models/backend.h"

// ======== ONLY FRONTEND COMMANDS =========
void exec_command_cash(struct Backend* app, int pid_response){
    if(pid_response != -1){ //only available for frontend apps
        printf("     > Executing the cash command \n");

        char* target_username;
        bool foundUser = false;
        printf("     > Searching for the user with pid: %d\n", pid_response);

        if(app->users != NULL){
            User* current_user = app->users;
            int total_users = 0;
            while (total_users < app->config->max_users_allowed) {
                if(current_user->pid == pid_response){
                    printf("     > Found the user! : %s \n", current_user->username);
                    foundUser = true;
                    target_username = current_user->username;
                }
                current_user++;
                total_users++;
            }

            if(foundUser){
                //get the cash of the user
                int userBalance = getUserBalance(target_username);
                char message_to_send[255] = "";
                sprintf(message_to_send, "%d", userBalance);
                send_message_frontend(message_to_send, pid_response);
            }
        }
    }
}

void exec_command_verify_login(struct string_list* arguments, struct Backend* app, int pid_response){
    if(pid_response != -1 && arguments != NULL) {

        if(arguments->string != NULL && arguments->next != NULL){

            char* username = arguments->string;
            char* password = arguments->next->string;
            printf("     > Executing the verify login command\n");

            Config *conf = get_env_variables();
            int result = loadUsersFile(conf->f_users);

            int login = isUserValid(username, password);

            if(app != NULL){
                // find the user in the users list and update the pid
                User* current_user = app->users;
                int total_users = 0;
                while (total_users < app->config->max_users_allowed) {
                    if(strcmp(current_user->username, username) == 0){
                        current_user->pid = pid_response;
                        printf("     > User authenticated and defined pid on the user list: %s   -> %d\n", current_user->username, current_user->pid);
                        break;
                    }
                    current_user++;
                    total_users++;
                }
            }

            if (login == -1) {
                perror("      > Error validating user from file \n");
            } else if (login == 0) {
                perror("      > User doesn't exist or his password is invalid \n");
            }else{
                printf("      > User %s with password %s is logged in\n", username, password);
                send_message_frontend("LOGIN_SUCCESS", pid_response);
            }
        }
    }
}

// ======== ONLY BACKEND COMMANDS =========
void exec_add_money_to_user(struct Backend* app, int pid_response, struct string_list* arguments){
    if(arguments != NULL) {

        if(arguments->string != NULL){

            printf("     > Executing the add money command \n");

            //find username based on the pid
            char* target_username;
            bool foundUser = false;

            User* current_user = app->users;
            int total_users = 0;
            while (total_users < app->config->max_users_allowed) {
                if(current_user->pid == pid_response){
                    foundUser = true;
                    target_username = current_user->username;
                    break;
                }
                current_user++;
                total_users++;
            }

            char* amount = arguments->string;

            if(verify_is_number(amount) && foundUser){
                //get the cash of the user
                int userBalance = getUserBalance(target_username);
                int amountToAdd = atoi(amount);
                int newBalance = userBalance + amountToAdd;

                printf("      > Adding %d to the user: %s \n", amountToAdd, target_username);
                printf("        > current balance will be: %d + %d = %d \n", userBalance, amountToAdd, newBalance);

                char message_to_send[255] = "";

                if (updateUserBalance(target_username, getUserBalance(target_username) + amountToAdd) == -1){
                    sprintf(message_to_send, "error updating the balance");

                }else{
                    sprintf(message_to_send, "balance updated to %d", newBalance);
                }

                //notify the frontend
                send_message_frontend(message_to_send, pid_response);
            }

        }
    }
}

//list promoters
void exec_command_prom() {
    printf("     > Executing the list promoters command\n");
}

//update promoters
void exec_command_reprom() {
    printf("     > Executing the update promoters command\n");

}

//cancel promoter
void exec_command_cancel_prom(struct string_list *arguments) {
    if (arguments != NULL) {
        printf("     > Executing the cancel promoter command, canceling promoter: %s\n", arguments->string);
    } else {
        printf("     > Failed to execute the cancel promoter command, please provide the exec promoter name, example: cancel <exec-promoter-name>\n");
    }
}

void exec_command_kick(struct string_list *arguments) {
    if (arguments != NULL) {
        printf("     > Executing the kick user command, kicking the user: %s\n", arguments->string);
    } else {
        printf("     > Failed to execute the kick command, please provide the username, example: kick <username>\n");
    }
}

void exec_command_list_users() {
    printf("     > Executing the list users command\n");
}

// ======== BOTH FRONTEND AND BACKEND COMMANDS =========

//list all items
void exec_command_list() {
    printf("     > Executing the list all items command\n");

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int i = 0;

    fp = fopen("file_items.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1)
    {
        Item it;
        // the %20s is to avoid buffer overflows
        sscanf(line, "%20s %20s %20s %d %d %d %20s %20s", it.identifier, it.name, it.category, &it.duration, &it.current_value, &it.buy_now_value, it.seller_name, it.bidder_name);
        printf("     > %s %s %s %d %d %d %s %s\n", it.identifier, it.name, it.category, it.duration, it.current_value, it.buy_now_value, it.seller_name, it.bidder_name);
    }

    fclose(fp);
    if (line)
        free(line);
}
