# Semaphore-with-shared-memory-in-process

In a painting shop, boxes are being painted. Assume that each box is painted only in one color and that all boxes are of the same size. 
Changing colors in the paint-ing process is expensive due to the added cost of wasted paint and solvents involved with each color change
in addition to the extra time required for the changing process. Therefore, the objective is to order the boxes in a way to minimize 
the number of paint changeovers.
When boxes arrive at the painting shop, they are placed in a line based on the time of their arrival, 
until it is their turn to be painted. Painting starts only after
all boxes have arrived and have been stored in the storage area. 
The box painting unit has a capacity of two, which means that two boxes of the same color can be painted at the same time. 
In order to minimize the number of paint changeovers, when a painting resource becomes available, 
a box having the same color as the previous boxes has to be painted. Boxes of the same color are painted as 
first-come-first-serve based on the order of their arrival. After both painting stations (re-sources) become empty, 
if there are no boxes of the same color, then a new box with another color should be painted. 
You can choose this box color using a first-come-first-serve approach based on the order of their arrival.
