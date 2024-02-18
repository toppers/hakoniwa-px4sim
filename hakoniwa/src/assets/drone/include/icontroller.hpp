#ifndef _ICONTROLLER_HPP_
#define _ICONTROLLER_HPP_


namespace hako::assets::drone {


typedef struct {
    double pos_x;
    double pos_y;
    double pos_z;
    double euler_x;
    double euler_y;
    double euler_z;
} mi_drone_control_in_t;

typedef struct {
    double thrust;
    double torque_x;
    double torque_y;
    double torque_z;
} mi_drone_control_out_t;

class IController {
protected:
    int index = 0;
public:
    virtual ~IController() {}
    virtual mi_drone_control_out_t run(mi_drone_control_in_t &in) = 0;

    void set_index(int index)
    {
        this->index = index;
    }
    int get_index()
    {
        return this->index;
    }


};
}

#endif /* _ICONTROLLER_HPP_ */