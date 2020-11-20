#include <KatherineTimepix3.h>
#include <RadiationAngleReconstructor.h>
#include <string>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace py = pybind11;

PYBIND11_MODULE(Tpx3Angles, m) {
    py::class_<KatherineTimepix3>(m, "KatherineTimepix3")
        .def(py::init<const char*>())
        .def("getChipID", &KatherineTimepix3::getChipID)
        .def("getIP", &KatherineTimepix3::getIP)
        .def("initialize", &KatherineTimepix3::initialize)
        .def("disconnect", &KatherineTimepix3::disconnect)
        .def("getReadoutDevice", &KatherineTimepix3::getReadoutDevice)
        .def("getTemperature", &KatherineTimepix3::getTemperature)
        .def(py::self == py::self)
        .def("__repr__",
            [](KatherineTimepix3& a) {
                return "<KatherineTimepix3 chipID: " + a.getChipID() + ">";
            });
}