"""

"""

# from dubins_path import DubinsPath
from dubins_path_time import DubinsPath
from export_motion_mat import create_Mfile, create_CSVfile

import numpy as np

SAMPLING_RATE = 1000 # Hz

INIT_POSE = [0, 0, np.pi/2] # x, y, theta
INIT_TWIST = [0, 0, 0] # \dot{x}, \dot{y}, \dot{\theta}


if __name__ == '__main__':
    dp = DubinsPath(initPose=INIT_POSE, initTwist=INIT_TWIST, samplingRate=SAMPLING_RATE)

    velocity = 0.5 # m/s

    # make square
    for _ in range(4):
        dp.straightLine(length=1, velocity=velocity)
        # dp.turnAntiClock(0.5, np.pi/2, velocity)
        dp.turnClock(turning_radius=0.25, theta=np.pi/2, velocity=velocity)

    dp.plotPath()
    dp.showFigures()


    # create_Mfile(dp.velSamples, fileName="./square_path_clockwise.m", variableName="sq_path_cw", lineBreaks=True)
    # create_CSVfile(dp.velSamples, fileName="./square_path_cw.csv", labels="")

    create_Mfile(dp.velSamples_stamped, fileName="./square_path_1m50cm_clockwise.m", variableName="sq_path_cw", lineBreaks=True)
    # create_CSVfile(dp.velSamples, fileName="./square_path_cw.csv", labels="")
