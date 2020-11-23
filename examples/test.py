import sys, os
import pyTimepixAngles as ta

def createController(basePath: str) -> ta.Tpx3DosageMeasurement:
    con = ta.Tpx3DosageMeasurement(basePath)
    con.setFilterSetup(basePath + "/FilterSetup.obj", 256, 256)
    return con

if __name__ == "__main__":
    print("Version: " + ta.__version__)

    bp = os.path.dirname(sys.argv[0])
    con = createController(bp)