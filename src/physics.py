#!/usr/bin/env python

"""
"""

import argparse
import pygame
import sys
import time
import random
import numpy

from pygame import locals, color, display, key, event, draw

windowSize = numpy.array([300, 300])
impulseResolution = 0.01
friction = 0.02
entropy = 0.00000001

class matrix():
    """
    """

    def __init__(self, handle):
        """
        """
        self.numberOfParticles = int(handle.readline())
        self.names = map(lambda x: handle.readline()[:-1],
            range(self.numberOfParticles))
        
        self.M = []
        for i in range(self.numberOfParticles - 1):
            self.M.append(map(lambda x: float(x) / 4.0,
                handle.readline().split()))
    #__init__

    def distance(self, i, j):
        """
        """
        if i == j:
            return 0.0
        if i > j:
            return self.M[i-1][j]
        return self.M[j-1][i]
    #distance
#matrix

class point():
    """
    """

    def __init__(self, position=numpy.array([0.5, 0.5]), randomInit=False):
        """
        """
        if randomInit:
            self.position = numpy.array([random.random(), random.random()])
        else:
            self.position = position
        self.velocity = numpy.array([0.0, 0.0])
        self.mass = 1.0
    #__init__

    def impulse(self, velocity):
        """
        """
        self.velocity += velocity
    #impulse

    def update(self):
        """
        """
        displacement = numpy.array([random.random() - 0.5,
            random.random() - 0.5])
        self.position = (self.position + self.velocity) % 1.0
        self.velocity -= (self.velocity * friction) + (displacement * entropy)
    #update

    def pos(self):
        """
        """
        return map(int, (self.position * windowSize))
    #pos
#point

def torusPosition(position1, position2):
    """
    """
    position = numpy.array(position2)

    if position1[0] - position2[0] > 0.5:
        position[0] += 1.0
    if position1[0] - position2[0] < -0.5:
        position[0] -= 1.0
    if position1[1] - position2[1] > 0.5:
        position[1] += 1.0
    if position1[1] - position2[1] < -0.5:
        position[1] -= 1.0

    return position
#torusPosition

def dist(position1, position2):
    """
    """
    return ((position1[0] - position2[0]) ** 2 +
        (position1[1] - position2[1]) ** 2)
#dist

def input(events):
    """
    """
    for event in events:
        if event.type == locals.QUIT:
            sys.exit(0)
        elif event.type == locals.KEYDOWN:
            if event.key == locals.K_ESCAPE:
                sys.exit(0)
            if event.key == locals.K_q:
                sys.exit(0)
            elif event.key == locals.K_a:
                return numpy.array([-1, 0])
            elif event.key == locals.K_s:
                return numpy.array([0, 1])
            elif event.key == locals.K_w:
                return numpy.array([0, -1])
            elif event.key == locals.K_d:
                return numpy.array([1, 0])
        #elif
        #else: 
        #    print event
    #for
    return None
#input

def render(screen, particle):
    """
    """
    draw.circle(screen, color.Color("black"), particle.pos(), 2)
    particle.update()
    draw.circle(screen, color.Color("white"), particle.pos(), 2)
    display.update()
#render

def physics(handle):
    """
    """
    window = display.set_mode(windowSize)
    display.set_caption('Test')
    screen = display.get_surface()
    key.set_repeat(10, 1)

    distanceMatrix = matrix(handle)
    particles = map(lambda x: point(), range(distanceMatrix.numberOfParticles))

    while True:
        inp = input(event.get())
        if inp != None:
            for particle in particles:
                particle.impulse(impulseResolution * 0.01 * inp)

        for i in range(distanceMatrix.numberOfParticles):
            for j in range(distanceMatrix.numberOfParticles):
                if i != j:
                    direction = 1
                    temp = torusPosition(particles[i].position,
                        particles[j].position)
                    if (dist(particles[i].position, temp) <
                        distanceMatrix.distance(i, j)):
                        direction = -1
                    particles[j].impulse(direction * impulseResolution *
                        (particles[i].position - temp) *
                        abs(dist(particles[i].position, temp) -
                        distanceMatrix.distance(i, j)))
                #if

        for particle in particles:
            render(screen, particle)

        sys.stdout.write("%f\r" % sum(map(lambda x: sum(abs(x.velocity)),
            particles)))
    #while
#physics

def main():
    """
    Main entry point.
    """
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=__doc__.split('\n\n\n')[0], epilog="")
    parser.add_argument('input', metavar='INPUT_FILE',
        type=argparse.FileType('r'), help='file used as input')
    args = parser.parse_args()

    physics(args.input)
#main

if __name__ == '__main__':
    main()
