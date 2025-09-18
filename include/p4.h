#ifndef P4_H
#define P4_H

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

// #define TEST_COMMAND "p4 login -s"
// #define TEST_COMMAND "p4 print -q  //codev_soda/mbdt/ProjectEnvs/Soda/Ecu/main/dependencies.cfg"
// #define TEST_COMMAND "p4 files -e //application_software/ucs/Systems/.../dependencies.cfg"
// #define TEST_COMMAND "p4 files -e //application_software/ucs/.../dependencies.cfg"

#define TEST_COMMAND "p4 login -s"
// #define TEST_COMMAND "dir"

char* p4cmd(const char* command);

#endif