"""
Dubin's Paths:
Generate samples from constant velocity continuous paths using straight lines and circle segments
Samples generated for pose and twist
"""

import numpy as np
import matplotlib.pyplot as plt
import math
import copy

SAMPLING_RATE = 10 # Hz

INIT_POSE = [0, 0, np.pi/2] # x, y, theta
INIT_TWIST = [0, 0, 0] # \dot{x}, \dot{y}, \dot{\theta}
INIT_START_TIME = 0 # float


class DubinsPath():

    def __init__(self, initPose = INIT_POSE, initTwist = INIT_TWIST, samplingRate = SAMPLING_RATE):
        self.posSamples = [initPose]
        self.velSamples = [initTwist]
        self.normVel = []
        self.timestamp = [INIT_START_TIME]
        self.addNormVel()
        self.posSamples_stamped = [ [self.timestamp[-1]] + self.posSamples[-1] ]
        self.velSamples_stamped = [ [self.timestamp[-1]] + self.velSamples[-1] ]
        #self.addTimeStampedData()
        self.dt = 1.0/float(samplingRate)

    def addNormVel(self):
        self.normVel += [math.sqrt(self.velSamples[-1][0] ** 2 + self.velSamples[-1][1] ** 2)]

    def addTimeStampedData(self):
        self.posSamples_stamped += [ [self.timestamp[-1]] + self.posSamples[-1] ]
        self.velSamples_stamped += [ [self.timestamp[-1]] + self.velSamples[-1] ]

    def maintainStatus(self, samples):
        # state = self.posSamples[-1]
        vstate = self.velSamples[-1]
        for n in range(samples):
            self.timestamp.append(self.timestamp[-1]+self.dt)
            self.velSamples.append(vstate)
            pos = list(self.posSamples[-1] + np.array(vstate) * self.dt)
            self.posSamples.append(pos)
            self.addTimeStampedData()
            
            

    # length = length of the straight line
    # velocity = velocity along the path
    def straightLine(self, length, velocity):
        state = self.posSamples[-1]
        vel = [velocity * math.cos(state[2]), velocity * math.sin(state[2]), 0]
        R = 0

        while(R < length):
            pos = list(self.posSamples[-1][:2] + np.array(vel[:2]) * self.dt)
            self.timestamp.append(self.timestamp[-1]+self.dt)
            self.posSamples.append(pos + [state[2]])
            self.velSamples.append(vel)
            self.addNormVel()
            self.addTimeStampedData()
            R = math.sqrt((state[0] - pos[0]) ** 2 + (state[1] - pos[1]) ** 2)

    # turning radius = radius of the circle to be traced while turning
    # theta = total angular displacement of the turn
    # velocity = velocity along the path
    def turnAntiClock(self, turning_radius, theta, velocity):
        state = self.posSamples[-1]
        ang2 = state[2]
        dtheta = velocity * self.dt / turning_radius
        while(state[2] + theta > ang2):
            ang2 += dtheta
            pos = self.posSamples[-1]
            vel = [velocity * math.cos(ang2), velocity * math.sin(ang2), dtheta / self.dt]
            self.timestamp.append(self.timestamp[-1]+self.dt)
            pos = list(pos[:2] + np.array(vel[:2]) * self.dt) + [ang2]
            self.velSamples.append(copy.copy(vel))
            self.addNormVel()
            self.posSamples.append(copy.copy(pos))
            self.addTimeStampedData()

    # turning radius = radius of the circle to be traced while turning
    # theta = total angular displacement of the turn
    # velocity = velocity along the path
    def turnClock(self, turning_radius, theta, velocity):
        state = self.posSamples[-1]
        ang2 = state[2]
        dtheta = velocity * self.dt / turning_radius
        while(state[2] - theta  <= ang2):
            ang2 -= dtheta
            pos = self.posSamples[-1]
            vel = [velocity * math.cos(ang2), velocity * math.sin(ang2), dtheta / self.dt]
            self.timestamp.append(self.timestamp[-1]+self.dt)
            pos = list(pos[:2] + np.array(vel[:2]) * self.dt) + [ang2]
            self.velSamples.append(copy.copy(vel))
            self.addNormVel()
            self.posSamples.append(copy.copy(pos))
            self.addTimeStampedData()


    def plotPath(self):
        plt.figure(0)
        plt.clf()
        plt.plot(np.array(self.posSamples_stamped)[:, 1], np.array(self.posSamples_stamped)[:, 2])
        plt.grid(True)
        plt.title('Trajectory')
        plt.xlabel('x-coordinate')
        plt.ylabel('y-coordinate')
        plt.axis('equal')

    def plotVelocities(self):
        plt.figure(2)
        plt.clf()
        # plt.plot(np.array(self.normVel), 'self.', label='v')
        # plt.hold(True)
        plt.plot(np.array(self.velSamples_stamped)[:, 0], np.array(self.velSamples_stamped)[:, 1], 'b', label='vx')
        # plt.hold(True)
        plt.plot(np.array(self.velSamples_stamped)[:, 0], np.array(self.velSamples_stamped)[:, 2], 'g', label='vy')
        plt.grid(True)
        plt.legend()

        plt.title('Velocity Profile')
        plt.xlabel('time')
        plt.ylabel('velocity')

    def showFigures(self):
        plt.show()



# default run when executed directly
if __name__ == '__main__':

    dp = DubinsPath(initPose=INIT_POSE, initTwist=INIT_TWIST, samplingRate=SAMPLING_RATE)

    velocity = 0.25 # m/sec

    # generate a square
    for _ in range(4): # repeat 4 times
        dp.straightLine(5, velocity) # go straight
        dp.turnAntiClock(1.0, np.pi/2.0, velocity) # turn left by 90 degrees

    # display generated samples in console (pose, twist)
    # for p, t in zip(dp.posSamples, dp.velSamples):
    #     print p, t

    print "Number of Samples Generated: ", np.array(dp.posSamples)[:, 0].size
    # print dp.posSamples_stamped

    # Visualize
    dp.plotPath()
    dp.plotVelocities()
    dp.showFigures()
