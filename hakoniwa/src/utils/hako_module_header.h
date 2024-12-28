#ifndef _HAKO_MODULE_HEADER_H_
#define _HAKO_MODULE_HEADER_H_

#define HAKO_MODULE_MAGICNO         0xFEFE8528
#define HAKO_MODULE_VERSION         0x00001010
#define HAOKO_MODULE_HEADER_NAME   "hako_module_header"
typedef struct {
    unsigned int magicno;
    unsigned int version;
    /*
     * module type
     */
    const char* (*get_type) (void);
    /*
     * module name
     */
    const char* (*get_name) (void);
} HakoModuleHeaderType;

#endif /* _HAKO_MODULE_HEADER_H_ */