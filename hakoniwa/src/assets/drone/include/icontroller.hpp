#ifndef _ICONTROLLER_HPP_
#define _ICONTROLLER_HPP_


#include "hako_module_drone_controller.h"

namespace hako::assets::drone {


class IController {
protected:
    int index = 0;
public:
    virtual ~IController() {}
    virtual mi_drone_control_out_t run(mi_drone_control_in_t &in) = 0;

    void set_index(int _index)
    {
        this->index = _index;
    }
    int get_index()
    {
        return this->index;
    }


};
}

#endif /* _ICONTROLLER_HPP_ */