# -*- coding: utf-8 -*-

import pydot 
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from cStringIO import StringIO

# first you create a new graph, you do that with pydot.Dot()
graph = pydot.graph_from_dot_file('tree.dot')

# render pydot by calling dot, no file saved to disk
png_str = graph.create_png(prog='dot')
# treat the dot output string as an image file
sio = StringIO()
sio.write(png_str)
sio.seek(0)
img = mpimg.imread(sio)

# plot the image
imgplot = plt.imshow(img)
plt.show(block=True)









