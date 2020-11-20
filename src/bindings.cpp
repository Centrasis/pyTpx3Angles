#include <KatherineTimepix3.h>
#include <GeometryTypes.h>
//#include <RadiationAngleReconstructor.h>
#include <string>
#undef ssize_t
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
 //       .def(py::self == py::self)
        .def("__repr__",
            [](KatherineTimepix3& a) {
                return "<KatherineTimepix3 chipID: " + a.getChipID() + ">";
            });
    
    py::class_<FFilterSetup::FDetector>(m, "FDetector")
        .def(py::init())
        .def_readwrite("widthIn_mm", &FFilterSetup::FDetector::widthIn_mm)
        .def_readwrite("heightIn_mm", &FFilterSetup::FDetector::heightIn_mm)
        .def_readwrite("widthIn_px", &FFilterSetup::FDetector::widthIn_px)
        .def_readwrite("heightIn_px", &FFilterSetup::FDetector::heightIn_px)
//        .def_readwrite("pixelPer_mm_x", &FFilterSetup::FDetector::pixelPer_mm)
        .def("__repr__",
            [](FFilterSetup::FDetector& a) {
                return "<FFilterSetup::FDetector>";
            });

    py::class_<FFilter>(m, "FFilter")
        .def(py::init())
        .def("__repr__",
            [](FFilterSetup::FDetector& a) {
                return "<FFilterSetup::FDetector>";
            });
   
    py::class_<FFilterSetup>(m, "FFilterSetup")
        .def(py::init<FFilterSetup::FDetector, std::vector<FFilter>>())
        .def("__repr__",
            [](FFilterSetup& a) {
                return "<FFilterSetup>";
            });
    /*
    py::class_<RadiationAngleReconstructor>(m, "RadiationAngleReconstructor")
        .def(py::init())
        .def_static("SetFilterSetup", &RadiationAngleReconstructor::SetFilterSetup)
        .def("__repr__",
            [](RadiationAngleReconstructor& a) {
                return "<RadiationAngleReconstructor " + std::to_string((int)&a) + ">";
            });*/
}