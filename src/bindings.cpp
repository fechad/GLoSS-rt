/**
 * @file bindings.cpp
 * @brief Python bindings definition
 */

#include <pybind11/pybind11.h>

#include "utils.hpp"

#include <iostream>

using namespace std;
using namespace gloss; 

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

string printHelloWorld() {
    return "Hello World !";
}

string getVersion() {
    return gloss::getVersion();
}

namespace py = pybind11;

PYBIND11_MODULE(gloss, m) {
    m.doc() = R"pbdoc(
        Pybind11 gloss plugin
        -----------------------

        .. currentmodule:: gloss

        .. autosummary::
           :toctree: _generate

           printHelloWorld
           getVersion
    )pbdoc";

    m.def("printHelloWorld", &printHelloWorld, R"pbdoc(
        A simple hello world.
    )pbdoc");

      m.def("getVersion", &gloss::getVersion, R"pbdoc(
        Returns the current version of gloss.
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}