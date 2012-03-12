import svg
import ns2

#from svglib.svglib import svg2rlg
#from reportlab.graphics import renderPDF

trace_file = 'trazaReal.tr'

trace_type = 'ns2'
terminal = 'svg'
output_file = './out.svg'

if trace_type == 'ns2':
	trace = ns2.Ns2_trace()
elif trace_type == 'omnet++':
	trace = omnet.Omnet_trace()
else:
	raise 'trace type error ' + trace_type

if terminal == 'svg':
	terminal = svg.Svg(output_file)
elif terminal == 'tex':
	terminal = tex.Tex()
else:
	raise 'terminal error ' + terminal

trace_fid = open(trace_file, 'r')
mycanvas = trace.scan_trace(trace_fid)
trace_fid.close()

mycanvas.draw(terminal)
terminal.close()
#time.sleep(1)
#drawing = svg2rlg("out.svg")
#renderPDF.drawToFile(drawing, "out.pdf")




