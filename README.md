HOMEWORK 3: Custom + High Dimensional Data
==========================================

NAME:  < Max Espinoza >
-----------------------
Assignment # 4
Teammates:

Other Collaboration:
OpenGL SuperBible + Online Resources


### Assignment Writeup:

This dataset isn't that high dimensional as of yet, it is currently just 3 dimensional including color. 
I am doing this as a part of research though, so the progress made is in increasing the number of particles
I can generate. While I maxed out 2,000 on my thinkpad on the previous assignment, with a pure openGL
implementation I can get up to 200,000 - 500,000 particles interactively.

My hypothesis was that given more particles I can get better visualizations of wave-particles, enough so that I can 
start working on acoustical part of my research. Enough so that I can even extend this into 3D from this point on to
create a phonon-mapping implementation. 

<b>Analysis:</b> While I can now render many more particles, there seems to be unexpected behaviour with my implementation
that I spent a lot of time to fix. At first I though it was my understanding to computing where particles split and the
direction they keep. However, the problem might lie elsewhere, maybe rounding errors 
(which means I have to include an epsilon to smooth out the results).

<b>Visualization Analysis:</b> I added in color into my visualization to debug the problem above. The colors go from
yellow -> red with yellow being those particles that have split only once and red with particles that split several times.
I can see with this visualization that there are particle with less splits counts within those waves that have split more.


### Files
Description of submitted files (images/video/code/etc): <br>
particlesystem.h and particlesystem.cpp are both used to hold and update particles in the created simulation.<br>
particle.h contains the actual particle and its attributes. <br>
glCanvas.cpp and glCanvas.h contain all the window manager functions and initial setup for our program.<br>
main.cpp contains the main render loop.<br>
All images are to examples of the visualization working.<br>
Except buggy.png which an issue with having two waves generated from two sources. <br>


<br>
### Times
Approximate time spent this week:<br>
 0.0 hrs   Motivation & Problem Definition <br>
 0.0 hrs   Visualization Design <br>
 0.0 hrs   Data Collection <br>
 20.0 hrs  Visualization Execution<br>
 1.0 hrs   Analysis & Validation<br>
 0.0 hrs   Visualization Revision<br>
 0.0 hrs   Presentation<br>
...........................................<br>
 21.0 hrs   Total time spent<br>


TOTAL time spent this semester:<br>
 0.0 hrs   Motivation & Problem Definition <br>
 0.0 hrs   Visualization Design <br>
 5.0 hrs     Data Collection <br>
 100.0 hrs   Visualization Execution<br>
 0.0 hrs   Analysis & Validation<br>
 0.0 hrs   Visualization Revision<br>
 0.0 hrs   Presentation<br>
...........................................<br>
 105.0 hrs   Total time spent<br>


