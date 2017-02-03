//
// Created by angel on 30/01/2017.
//


#ifndef SQLTESTHARNESS_BASECONTROLLER_H
#define SQLTESTHARNESS_BASECONTROLLER_H

    class BaseController {
    public:
        virtual ~BaseController() = 0;

    protected:
        virtual void Initialize() = 0;
    };


#endif //SQLTESTHARNESS_BASECONTROLLER_H
