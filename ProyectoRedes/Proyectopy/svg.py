#!/usr/bin/env python
"""\
SVG.py - Construct/display SVG scenes.

The following code is a lightweight wrapper around SVG files. The metaphor
is to construct a scene, add objects to it, and then write it to a file
to display it.

This program uses ImageMagick to display the SVG files. ImageMagick also 
does a remarkable job of converting SVG files into other formats.
"""

import os


class Svg():
	def __init__(self, output_file=None):
		self.output_fid = open(output_file, 'w')
		self.header()
	def close(self):
		self.footer()
		self.output_fid.close()
	
	def line(self,start,end, color=(0,0,0), style =""):
		l = Line(start,end, color, style)
		self.output_fid.write(l.strarray())

	def header(self):
		headr = """<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="100%" height="100%" version="1.1" xmlns="http://www.w3.org/2000/svg">\n"""
		self.output_fid.write(headr)
	
	def footer(self):
		footr = """</svg>\n"""
		self.output_fid.write(footr)
	
	def text(self,start,end,text,size):
		t=Text(start,end,text,size)
		self.output_fid.write(t.strarray())
		

class Line:
	def __init__(self,start,end, color, style):
		self.start = start #xy tuple
		self.end = end     #xy tuple
		self.color = color
		self.style = style
		return

	def strarray(self):
		return "  <line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" " % (self.start[0]*5000,self.start[1],self.end[0]*5000,self.end[1]) + 'style="stroke:rgb(%d,%d,%d);stroke-width:1;%s"/>\n' % (self.color[0],self.color[1],self.color[2],self.style)

	
class Circle:
	def __init__(self,center,radius,color):
		self.center = center #xy tuple
		self.radius = radius #xy tuple
		self.color = color   #rgb tuple in range(0,256)
		return

	def strarray(self):
		return ["  <circle cx=\"%d\" cy=\"%d\" r=\"%d\"\n" %\
				(self.center[0],self.center[1],self.radius),
				"    style=\"fill:%s;\"  />\n" % colorstr(self.color)]

class Rectangle:
	def __init__(self,origin,height,width,color):
		self.origin = origin
		self.height = height
		self.width = width
		self.color = color
		return

	def strarray(self):
		return ["  <rect x=\"%d\" y=\"%d\" height=\"%d\"\n" %\
				(self.origin[0],self.origin[1],self.height),
				"    width=\"%d\" style=\"fill:%s;\" />\n" %\
				(self.width,colorstr(self.color))]

class Text:
	def __init__(self,start,end,text,size):
		self.start = start
		self.end = end
		self.text = text
		self.size = size
		return

	def strarray(self):
		return "  <text x=\"%f\" y=\"%f\" font-size=\"%d\">\n" % ((self.start*500)-2,self.end,self.size) + "   %s\n" % self.text +	"  </text>\n"
		
	
def colorstr(rgb): return "#%x%x%x" % (rgb[0]/16,rgb[1]/16,rgb[2]/16)
