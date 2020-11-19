#include <KatherineTimepix3.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(example, m) {
    py::class_<KatherineTimepix3>(m, "KatherineTimepix3")
        .def(py::init<const char*>())
        .def("getChipID", &Pet::setName)
        .def("getIP", &Pet::getName);
}