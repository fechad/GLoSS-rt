/**
 * @file bindings.cpp
 * @brief Python bindings definition
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "utils.hpp"
#include "gloss.cpp"

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
           initialize
           compute
           saveResults
    )pbdoc";

    m.def("printHelloWorld", &printHelloWorld, R"pbdoc(
        A simple hello world.
    )pbdoc");

      m.def("getVersion", &gloss::getVersion, R"pbdoc(
        Returns the current version of gloss.
    )pbdoc");

    m.def("initialize", &gloss::initialize, R"pbdoc(
        Initializes the GLoSS module with antenna file, tiff file, and ground tiff file.
    )pbdoc",
        py::arg("antenna_file"), py::arg("tiff_file"), py::arg("ground_tiff_file"));
    
    m.def("compute", &gloss::compute, R"pbdoc(
        Computes the LoS paths for the initialized antennas.
    )pbdoc");

    m.def("saveResults", &gloss::saveResults, R"pbdoc(
        Saves the computed LoS paths to JSON files.
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}