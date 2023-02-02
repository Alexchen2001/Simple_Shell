#include <iostream>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

int main() {

    string command;

    cout << "myshell$";
    getline(cin, command);

    stringstream line(command);
    vector<string> tokens;
    string token;

    while (getline(line, token, ' ')) {
        tokens.push_back(token);
    }

    // an array of char pointers
    char ***argc = new char**[10];
    for (int i = 0; i < 10; i++) {
        argc[i] = new char*[20];
        for(int j = 0; j < 20; j++){
            argc[i][j] = NULL;
        }
    }

    // populate the array with the commands
    int comNum = 0;
    int charNum = 0;

    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i] == "|") {
            argc[comNum][charNum] = (char*)NULL;
            comNum++;
            charNum = 0;
        } else {
            argc[comNum][charNum] = new char[tokens[i].length() + 1];
            strcpy(argc[comNum][charNum], tokens[i].c_str());
            charNum++;
        }
    }
    argc[comNum][charNum] = (char*)NULL;

    // File descriptor
    int fd[comNum][2];
    int status;

    for (int i = 0; i < comNum; i++) {
        if (pipe(fd[i]) < 0) {
            cerr << "Fail to create pipe" << endl;
            return -1;
        }
    }

    for (int i = 0; i <= comNum; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // child
            if (i < comNum) {
                dup2(fd[i][1], 1);
                close(fd[i][1]);
                close(fd[i][0]);
            }


            execvp(argc[i][0], argc[i]);
            perror("fail to execute");
            exit(1);

        } else if (pid > 0) {
            if (i < comNum) {
                dup2(fd[i][0], 0);
                close(fd[i][1]);
                close(fd[i][0]);
            }

            pid_t child_pid = wait(&status);

            if (status == 0) {
                cout << "process " << child_pid << " exits with 0" << endl;
            }

        } else {
            perror("fork failed");
        }
    }

    // deallocate
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 20; j++) {
            delete[] argc[i][j];
        }
        delete[] argc[i];
    }
    delete[] argc;

    return 0;
}