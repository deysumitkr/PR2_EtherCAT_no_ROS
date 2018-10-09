"""

"""

# from dubins_path import DubinsPath
from dubins_path_time import DubinsPath
from export_motion_mat import create_Mfile, create_CSVfile

from matplotlib import pyplot as plt

import numpy as np
import copy

SAMPLING_RATE = 1000 # Hz

SMOOTHING_WINDOW = 25

INIT_POSE = [0, 0, np.pi/2] # x, y, theta
INIT_TWIST = [0, 0, 0] # \dot{x}, \dot{y}, \dot{\theta}

def avg(mat_in, win):
    mat = np.array(mat_in)
  
    win = int(win)
    
    x=copy.copy(mat[:,1])
    y=copy.copy(mat[:,2])
    th=copy.copy(mat[:,3])
    
    for i in range(win/2, mat.shape[0] - win/2 -1):
        xm = 0
        ym = 0
        thm = 0
        for j in range(-win/2, win/2):
          xm += x[i-j]
          ym += y[i-j]
          thm += th[i-j]
        
        mat[i,1] = float(xm) / float(win)
        mat[i,2] = float(ym) / float(win)
        mat[i,3] = float(thm) / float(win)
    return mat

if __name__ == '__main__':
    dp = DubinsPath(initPose=INIT_POSE, initTwist=INIT_TWIST, samplingRate=SAMPLING_RATE)

    velocity = 0.5 # m/s
    
    dp.maintainStatus(SMOOTHING_WINDOW)

    # make square
    for _ in range(4):
        dp.straightLine(length=1, velocity=velocity)
        # dp.turnAntiClock(0.5, np.pi/2, velocity)
        dp.turnClock(turning_radius=0.5, theta=np.pi/2, velocity=velocity)

    print np.array(dp.velSamples_stamped).shape
    mat = np.array(avg(dp.velSamples_stamped, SMOOTHING_WINDOW))
    

    # dp.plotPath()
    # dp.plotVelocities()
    # dp.showFigures()
    
    plt.plot(mat[:,1])
    plt.plot(mat[:,2])
    plt.show()
    
    
    


    # create_Mfile(dp.velSamples, fileName="./square_path_clockwise.m", variableName="sq_path_cw", lineBreaks=True)
    # create_CSVfile(dp.velSamples, fileName="./square_path_cw.csv", labels="")

    # create_Mfile(dp.velSamples_stamped, fileName="./square_path_1m50cm_clockwise2.m", variableName="sq_path_cw", lineBreaks=True)
    
    create_Mfile(mat, fileName="./square_path_1m50cm_clockwise2.m", variableName="sq_path_cw", lineBreaks=True)
    
    # create_CSVfile(dp.velSamples, fileName="./square_path_cw.csv", labels="")
