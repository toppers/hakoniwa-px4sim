#ifndef _HAKO_PARAMS_HPP_
#define _HAKO_PARAMS_HPP_

extern void hako_param_env_init();
extern const char* hako_param_env_get_string(const char* param_name);
extern bool hako_param_env_get_int(const char* param_name, int* value);

#endif /* _HAKO_PARAMS_HPP_ */