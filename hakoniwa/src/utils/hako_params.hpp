#ifndef _HAKO_PARAMS_HPP_
#define _HAKO_PARAMS_HPP_

/*
 * string params
 */
#define HAKO_CAPTURE_SAVE_FILEPATH "HAKO_CAPTURE_SAVE_FILEPATH"
#define HAKO_BYPASS_IPADDR "HAKO_BYPASS_IPADDR"
/*
 * integer params
 */
#define HAKO_BYPASS_PORTNO "HAKO_BYPASS_PORTNO"

extern void hako_param_env_init();
extern const char* hako_param_env_get_string(const char* param_name);
extern bool hako_param_env_get_integer(const char* param_name, int* value);

#endif /* _HAKO_PARAMS_HPP_ */