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

windowSize = numpy.array([300, 200])
impulseResolution = 0.000001
friction = 0.02

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
            self.M.append(map(float, handle.readline().split()))
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

    def __init__(self, position=numpy.array([0, 0]), randomInit=False):
        """
        """
        if randomInit:
            self.position = numpy.array([random.randint(0, windowSize[0]),
                random.randint(0, windowSize[1])])
        else:
            self.position = position
        self.floatPosition = map(float, self.position)
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
        self.floatPosition = (self.floatPosition + self.velocity) % windowSize
        self.position = numpy.array(map(int, self.floatPosition))
        self.velocity -= self.velocity * friction
    #update
#point

def dist(particle1, particle2):
    """
    """
    return ((particle1.floatPosition[0] - particle2.floatPosition[0]) ** 2 +
        (particle1.floatPosition[1] - particle2.floatPosition[1]) ** 2)
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
    draw.circle(screen, color.Color("black"), particle.position, 2)
    particle.update()
    draw.circle(screen, color.Color("white"), particle.position, 2)
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
    particles = map(lambda x: point(randomInit=True),
        range(distanceMatrix.numberOfParticles))

    while True:
        inp = input(event.get())
        if inp != None:
            for particle in particles:
                particle.impulse(impulseResolution * 10000.0 * inp)

        for i in range(distanceMatrix.numberOfParticles):
            for j in range(distanceMatrix.numberOfParticles):
                if i != j:
                    direction = 1
                    if (dist(particles[i], particles[j]) <
                        distanceMatrix.distance(i, j)):
                        direction = -1
                    particles[j].impulse(direction * impulseResolution *
                        (particles[i].position - particles[j].position) *
                        abs(dist(particles[i], particles[j]) -
                        distanceMatrix.distance(i, j)))
                #if

        for particle in particles:
            render(screen, particle)
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
