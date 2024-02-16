#include "hako_params.hpp"
#include <stdlib.h>
#include <string.h>
#include <iostream>

typedef struct {
    const char *name;
    const char* value;
} HakoParamStringType;

typedef struct {
    const char *name;
    int value;
} HakoParamIntegerType;

#define HAKO_PARAM_STRING_NUM 4
static HakoParamStringType hako_param_string[HAKO_PARAM_STRING_NUM] = {
    {
       HAKO_CAPTURE_SAVE_FILEPATH,
        "./capture.bin"
    },
    {
        HAKO_BYPASS_IPADDR,
        "127.0.0.1"
    },
    {
        HAKO_CUSTOM_JSON_PATH,
        "../config/custom.json"
    },
    {
        DRONE_CONFIG_PATH,
        "../config"
    },
};
#define HAKO_PARAM_INTEGER_NUM 1
static HakoParamIntegerType hako_param_integer[HAKO_PARAM_INTEGER_NUM] = {
    {
        HAKO_BYPASS_PORTNO,
        54001
    },
};

void hako_param_env_init()
{
    for (int i = 0; i < HAKO_PARAM_STRING_NUM; i++) {
        char *value = getenv(hako_param_string[i].name);
        if ((value != NULL) && (strlen(value) > 0 )) {
            hako_param_string[i].value = value;
        }
        std::cout << hako_param_string[i].name << " : " << hako_param_string[i].value <<std::endl;
    }
    for (int i = 0; i < HAKO_PARAM_INTEGER_NUM; i++) {
        char *value = getenv(hako_param_integer[i].name);
        if ((value != NULL) && (strlen(value) > 0 )) {
            hako_param_integer[i].value = atoi(value);
        }
        std::cout << hako_param_integer[i].name << " : " << hako_param_integer[i].value <<std::endl;
    }
    return;
}

const char* hako_param_env_get_string(const char* param_name)
{
    size_t param_name_len = strlen(param_name);
    for (int i = 0; i < HAKO_PARAM_STRING_NUM; i++) {
        if (param_name_len != strlen(hako_param_string[i].name)) {
            continue;
        }
        if (strcmp(param_name, hako_param_string[i].name) != 0) {
            continue;
        }
        return hako_param_string[i].value;
    }
    return NULL;
}

bool hako_param_env_get_integer(const char* param_name, int* value)
{
    size_t param_name_len = strlen(param_name);
    for (int i = 0; i < HAKO_PARAM_INTEGER_NUM; i++) {
        if (param_name_len != strlen(hako_param_integer[i].name)) {
            continue;
        }
        if (strcmp(param_name, hako_param_integer[i].name) != 0) {
            continue;
        }
        *value = hako_param_integer[i].value;
        return true;
    }
    return false;
}
