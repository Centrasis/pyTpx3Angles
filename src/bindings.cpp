#include <KatherineTimepix3.h>
#include <GeometryTypes.h>
#include <RadiationAngleReconstructor.h>
#include <Tpx3DosageMeasurement.h>
#include <PostprocessingThread.h>
#include <string>
#undef ssize_t
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#ifndef VERSION_INFO
#define VERSION_INFO "0.0.1"
#endif

namespace py = pybind11;

PYBIND11_MODULE(Tpx3Angles, m) {
    py::class_<katherine_coord_t>(m, "katherine_coord_t")
        .def(py::init())
        .def_readwrite("x", &katherine_coord_t::x)
        .def_readwrite("y", &katherine_coord_t::y);

    py::class_<katherine_px_f_toa_tot>(m, "katherine_px_f_toa_tot")
        .def(py::init())
        .def_readwrite("ftoa", &katherine_px_f_toa_tot::ftoa)
        .def_readwrite("toa", &katherine_px_f_toa_tot::toa)
        .def_readwrite("tot", &katherine_px_f_toa_tot::tot)
        .def_readwrite("coord", &katherine_px_f_toa_tot::coord);

    py::class_<PixelCluster>(m, "PixelCluster")
        .def(py::init<katherine_px_f_toa_tot_t*, int, uint64_t>())
        .def_readonly("initialHit", &PixelCluster::initialHit)
        .def_readonly("pixelCount", &PixelCluster::pixelCount)
        .def_readonly("duration", &PixelCluster::duration);

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

    py::class_<FVector2D>(m, "FVector2D")
        .def(py::init<float, float>())
        .def_readwrite("X", &FVector2D::X)
        .def_readwrite("Y", &FVector2D::Y)
        .def("__repr__",
            [](FVector2D& a) {
                return "<FVector2D (" + std::to_string(a.X) + ", " + std::to_string(a.Y) + ")>";
            });

    py::class_<FVector3D>(m, "FVector3D")
        .def(py::init<float, float>())
        .def_readwrite("X", &FVector3D::X)
        .def_readwrite("Y", &FVector3D::Y)
        .def_readwrite("Z", &FVector3D::Z)
        .def("Normalize", static_cast<void (FVector3D::*)()>(&FVector3D::Normalize))
        .def("__repr__",
            [](FVector3D& a) {
                return "<FVector3D (" + std::to_string(a.X) + ", " + std::to_string(a.Y) + ", " + std::to_string(a.Z) + ")>";
            });
    
    py::class_<FFilterSetup::FDetector>(m, "FDetector")
        .def(py::init())
        .def_readwrite("widthIn_mm", &FFilterSetup::FDetector::widthIn_mm)
        .def_readwrite("heightIn_mm", &FFilterSetup::FDetector::heightIn_mm)
        .def_readwrite("widthIn_px", &FFilterSetup::FDetector::widthIn_px)
        .def_readwrite("heightIn_px", &FFilterSetup::FDetector::heightIn_px)
        .def("getPixelsPer_mm", &FFilterSetup::FDetector::getPixelsPer_mm)
        .def("calcPixelsPer_mm", &FFilterSetup::FDetector::calcPixelsPer_mm)
        .def("__repr__",
            [](FFilterSetup::FDetector& a) {
                return "<FFilterSetup::FDetector>";
            });

    py::class_<FFilter>(m, "FFilter")
        .def(py::init())
        .def_readwrite("name", &FFilter::name)
        .def("__repr__",
            [](FFilter& a) {
                return "<FFilter " + a.name + ">";
            });
   
    py::class_<FFilterSetup>(m, "FFilterSetup")
        .def(py::init<FFilterSetup::FDetector, std::vector<FFilter>>())
        .def("getFilterIndexByName", &FFilterSetup::getFilterIndexByName)
        .def_readonly("Filters", &FFilterSetup::Filters)
        .def_readonly("Detector", &FFilterSetup::Detector)
        .def("__repr__",
            [](FFilterSetup& a) {
                return "<FFilterSetup>";
            });

    py::class_<FShadow>(m, "FShadow")
        .def(py::init())
        .def_readwrite("radiusX", &FShadow::radiusX)
        .def_readwrite("radiusY", &FShadow::radiusY)
        .def_readwrite("direction", &FShadow::direction);

    py::class_<FShadowSetup>(m, "FShadowSetup")
        .def(py::init())
        .def_readwrite("rotationAngle2D", &FShadowSetup::rotationAngle2D)
        .def_readwrite("score", &FShadowSetup::score)
        .def_readwrite("singleShadows", &FShadowSetup::singleShadows);

    py::class_<RadiationAngleReconstructor>(m, "RadiationAngleReconstructor")
        .def(py::init())
        .def_static("SetFilterSetup", &RadiationAngleReconstructor::SetFilterSetup)
        .def("__repr__",
            [](RadiationAngleReconstructor& a) {
                return "<RadiationAngleReconstructor " + std::to_string((int)&a) + ">";
            });

    py::class_<OCLTypedRingBuffer<katherine_px_f_toa_tot_t, PixelDataBufferSize>>(m, "OCLKatherineDataBuffer")
        .def(py::init());
        /*.def("readNext", [](OCLTypedRingBuffer<katherine_px_f_toa_tot_t, PixelDataBufferSize>& a) { return a.readNext(); })
        .def("readAll", &OCLTypedRingBuffer<katherine_px_f_toa_tot_t, PixelDataBufferSize>::readAll);*/

    py::class_<OCLMemoryVariable<cl::Image2D>>(m, "OCLImage2D")
        .def(py::init());

    py::class_<std::function<void(OCLTypedRingBuffer<katherine_px_f_toa_tot_t, PixelDataBufferSize>* list, size_t begin, size_t end)>>(m, "FProcessingRawPixelEvent").def(py::init<>());
    py::class_<std::function<void(std::shared_ptr<OCLMemoryVariable<cl::Image2D>> image)>>(m, "FProcessingIntegratedResultEvent").def(py::init<>());
    py::class_<std::function<void(std::shared_ptr<OCLMemoryVariable<cl::Image2D>> image, FVector3D& angles, FShadowSetup& shadows, uint64_t passedTime_ns)>>(m, "FProcessingShadowResultEvent").def(py::init<>());
    py::class_<std::function<void(std::shared_ptr<ClusterList> clusters)>>(m, "FDosimetricEvent").def(py::init<>());

    py::class_<PostprocessingThread>(m, "PostprocessingThread")
        .def("addProcessingMethod", static_cast<void (PostprocessingThread::*)(FProcessingRawPixelEvent, bool)>(&PostprocessingThread::addProcessingMethod))
        .def("addProcessingMethod", static_cast<void (PostprocessingThread::*)(FProcessingIntegratedResultEvent, bool)>(&PostprocessingThread::addProcessingMethod))
        .def("addProcessingMethod", static_cast<void (PostprocessingThread::*)(FProcessingShadowResultEvent, bool)>(&PostprocessingThread::addProcessingMethod))
        .def("addProcessingMethod", static_cast<void (PostprocessingThread::*)(FDosimetricEvent, bool)>(&PostprocessingThread::addProcessingMethod))
        .def("Stop", &PostprocessingThread::Stop)
        .def("EndProcessing", &PostprocessingThread::EndProcessing);

    py::class_<Tpx3DosageMeasurement>(m, "Tpx3DosageMeasurement")
#ifndef __SIMULATION__
        .def(py::init())
#else
        .def(py::init<std::string>())
#endif
        .def("searchTimepix", &Tpx3DosageMeasurement::searchTimepix)
        .def("useTimePix", &Tpx3DosageMeasurement::useTimePix)
        .def("getTimePix", &Tpx3DosageMeasurement::getTimePix)
        .def("isOnlineMeasurementActive", &Tpx3DosageMeasurement::isOnlineMeasurementActive)
        .def("areTrustedSourcesActive", &Tpx3DosageMeasurement::areTrustedSourcesActive)
#ifndef __SIMULATION__
        .def("configureTimepix", &Tpx3DosageMeasurement::configureTimepix, "configures assigned timepix or if none is assigned createing a new one from the config file")
        .def("doDataDrivenMode", &Tpx3DosageMeasurement::doDataDrivenMode)
#else
        .def("doDataDrivenMode", [](Tpx3DosageMeasurement& a) { return a.doDataDrivenMode(); })
#endif  
        .def("abortDataReadout", &Tpx3DosageMeasurement::abortDataReadout)
        .def("getPostProcessingThread", &Tpx3DosageMeasurement::getPostProcessingThread)
        .def("getCurrentFrame", &Tpx3DosageMeasurement::getCurrentFrame)
        .def("getCurrentFrameWithFoundShadows", &Tpx3DosageMeasurement::getCurrentFrameWithFoundShadows)
        .def("getNewestToA", &Tpx3DosageMeasurement::getNewestToA)
        .def("AnglesReady", &Tpx3DosageMeasurement::AnglesReady)
        .def("getRadiationDirection", &Tpx3DosageMeasurement::getRadiationDirection)
        .def("IsOperationPending", &Tpx3DosageMeasurement::IsOperationPending)
        .def("getElapsedProcessedTime_ns", &Tpx3DosageMeasurement::getElapsedProcessedTime_ns)
        .def("getHitsPerSecond", &Tpx3DosageMeasurement::getHitsPerSecond)
        .def("setFilterSetup", &Tpx3DosageMeasurement::setFilterSetup)
        .def("getFilterSetup", &Tpx3DosageMeasurement::getFilterSetup)
        
#ifdef __SIMULATION__
        .def("SetSimulationFileName", &Tpx3DosageMeasurement::getCurrentFrameWithFoundShadows)
        .def("SetMaxProcessingTime_ns", &Tpx3DosageMeasurement::getCurrentFrameWithFoundShadows)
#endif
        .def("__repr__",
            [](Tpx3DosageMeasurement& a) { 
#ifndef __SIMULATION__
                return "<Tpx3DosageMeasurement <" + a.getTimePix()->getChipID() + ">>";
#else
                return "<Tpx3DosageMeasurement SIMULATION<>>";
#endif
            });

    m.attr("__version__") = VERSION_INFO;
}