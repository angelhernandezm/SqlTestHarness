//
// Created by angel on 30/01/2017.
//

#ifndef SQLTESTHARNESS_GENERICCALLBACK_H
#define SQLTESTHARNESS_GENERICCALLBACK_H

#include <gtk/gtk.h>

template<typename T>
using GenericCallback = void (T::*)();

#endif //SQLTESTHARNESS_GENERICCALLBACK_H

