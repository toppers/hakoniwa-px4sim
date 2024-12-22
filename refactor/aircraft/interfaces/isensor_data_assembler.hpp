#ifndef _ISENSOR_DATA_ASSEMBLER_HPP_
#define _ISENSOR_DATA_ASSEMBLER_HPP_

namespace hako::aircraft {

class ISensorDataAssembler {
protected:
    int sample_num = 0;
public:
    virtual ~ISensorDataAssembler() {}
    virtual void set_sample_num(int n)
    {
        this->sample_num = n;
    }
    virtual void add_data(double data) = 0;
    virtual double get_calculated_value() = 0;
    virtual void reset() = 0;
    virtual int size() = 0;
};

}


#endif /* _ISENSOR_DATA_ASSEMBLER_HPP_ */