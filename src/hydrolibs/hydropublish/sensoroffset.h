#ifndef HYDROPUBLISH_SENSOROFFSET_H
#define HYDROPUBLISH_SENSOROFFSET_H


namespace hydropublish {

struct SensorOffset {
    SensorOffset()
        : x(0),y(0),z(0),
          roll(0),pitch(0),yaw(0)
        {}

    double x;
    double y;
    double z;
    double roll;
    double pitch;
    double yaw;
};

}

#endif
