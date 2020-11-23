import sys, os, math
import cv2
import numpy as np
import pyTimepixAngles as ta

pngImg = None
finished = False

def createController(basePath: str) -> ta.Tpx3DosageMeasurement:
    con = ta.Tpx3DosageMeasurement(basePath)
    con.setFilterSetup(basePath + "/FilterSetup.obj", 256, 256)
    return con

def initControllerMeas(con: ta.Tpx3DosageMeasurement, basePath: str, qualli: str, angle: float, idx: int, useUnShadowsSource = False):
    if useUnShadowsSource == True:
        con.SetSimulationFileName(basePath + "/InData/" + qualli + "/noShadow/Data_" + str(angle) + ".t3pa")
    else:
        con.SetSimulationFileName(basePath + "/InData/" + qualli + "/Data_" + str(angle) + ".t3pa")
        
    if con.doDataDrivenMode():
        print("\nStarted Data driven mode: " + str(angle) + "!");

def getImgAt(basePath: str, qualli: str, degree: float, frameNumber: int):
    p = basePath + "/outImgs/"+ qualli +"/img_" + "{:10.6f}".format(degree) + "_(" + str(frameNumber) + ").png"
    print("Try read: " + p)
    return cv2.imread(p)

def onCalcFinished(image, angles: ta.FVector3D, shadows: ta.FShadowSetup):
    setup = ta.RadiationAngleReconstructor.getShadowSetup()
    angles.X = angles.X * (180.0 / math.pi)
    angles.Y = angles.Y * (180.0 / math.pi)
    angles.Z = angles.Z * (180.0 / math.pi)
    if angles.X == float("inf"):
        return

    cp = pngImg.copy()
    for s in setup.singleShadows:
        cv2.ellipse(cp, cv2.Point(s.getCenterX(), s.getCenterY()), cv2.Size(s.radiusX, s.radiusY), (180.0 / math.pi) * setup.rotationAngle2D, 0, 360, cv2.Scalar(0, 0, 255), 2)

    cv2.imshow("Shadows", cp)
    finished = True

if __name__ == "__main__":
    print("Version: " + ta.__version__)

    ## Test if we have access to the most basic constructs
    coord = ta.katherine_coord_t()
    print(coord)
    coord.x = 1
    print(coord)
    assert coord.x == 1
    ta.RadiationAngleReconstructor.SetShadowThreshold(1.1)
    print("ShadowThreshold: " + str(ta.RadiationAngleReconstructor.GetShadowThreshold()))
    assert ta.RadiationAngleReconstructor.GetShadowThreshold() > 1.09 and ta.RadiationAngleReconstructor.GetShadowThreshold() < 1.11

    ## initialize the openCL execution. Devices can be choosen or selected by default
    ta.initializeExecutor(False)

    bp = os.path.dirname(sys.argv[0])
    q = input("Wähle Strahlenqualität (C60/A80)")
    e_in = input("Evaluation des controllers(c) oder einzelner module(m)?")

    if e_in[0] == 'c':
        e_in = input("Sollen Test Daten(d) oder Bilder(b) ausgewertet werden oder sollen die unverarbeiteten Bilder ausgegeben (o) werden? [bb[x] = bilder bluring mit x radius; bpp = Zeige Wahrscheinlichkeiten aus Bildern und Positionen; bpa = Zeige Wahrscheinlichkeiten aus Bildern und Winkeln]")
        imgPath = bp + "/outImgs/" + q
        expectedAngles = []
        if q == "C60":
            expectedAngles = [0.0, 10.0, -10.0, 20.0, -20.0, 30.0, -30.0, 35.0, -35.0, 40.0, -40.0, 45.0, -45.0]
            ta.RadiationAngleReconstructor.SetShadowThreshold(1.06)
        else:
            expectedAngles = [0.0]
            ta.RadiationAngleReconstructor.SetShadowThreshold(1.06)

        if e_in[0] == 'b':
            con = createController(bp)
            print(con)
            ta.RadiationAngleReconstructor.SetFilterSetup(con.getFilterSetup())
            middlePinIdx = con.getFilterSetup().getFilterIndexByName("Pin9_Cube.000")

            bUseYRot = input("Should use y-Rotation? (y/n)") == "y"
            bRestrictShadows = input("Should use shadow border? (y/n)") == "y"

            for i in range(0, len(expectedAngles)):
                ta.RadiationAngleReconstructor.SetShadowThreshold(1.07)
                ta.RadiationAngleReconstructor.SetMaxThreadCount(48)
                ta.RadiationAngleReconstructor.SetPreviousFoundRotation(ta.FVector2D(0,0))
                ta.RadiationAngleReconstructor.SetCurrentScore(0)
                
                pngImg = getImgAt(bp, q, expectedAngles[i - 1], 0)
                img = np.zeros([pngImg.shape[0], pngImg.shape[1]], dtype=np.uint8)
                img = cv2.cvtColor(pngImg, cv2.COLOR_RGBA2GRAY)
                cv2.imshow("Read", img)
                cv2.waitKey(1)

                integratedPixels = ta.OCLImage2D()
                integratedPixels.setHostPointerMode(ta.EOCLAccessTypes.ATRead)
                integratedPixels.setHostPointer(img.data)
                integratedPixels.setVariableChanged(True)

                print("Start calc of: " + str(expectedAngles[i - 1]))
                rar = ta.RadiationAngleReconstructor(integratedPixels.makeShared()).calcParallelInRayAngle(onCalcFinished)

                while(not finished):
                    pass