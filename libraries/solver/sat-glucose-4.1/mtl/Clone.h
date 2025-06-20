#ifndef GLUCOSE41_Glucose_Clone_h
#define GLUCOSE41_Glucose_Clone_h


namespace Glucose41 {

    class Clone {
        public:
          virtual Clone* clone() const = 0;
    };
};

#endif