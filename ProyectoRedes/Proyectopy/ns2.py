import canvas

class Ns2_trace():
	def scan_trace(self, file):
		mycanvas = canvas.Canvas()
		for line in file:
			line = line.split()
			event_type = line[0]
			timestamp = float(line[1])
			hop_src_node = int(line[2])
			hop_dst_node = int(line[3])
			packet_name = line[4] # TCP or ACK
			packet_size = int(line[5])
			packet_flag = line[6] # C is a control packet 
#			print packet_flag[0]
			ip_src_node = int(line[2]) # todo : change this
			ip_dst_node = int(line[3]) # todo : change this
			src_port = 0# todo : change this
			dst_port = 0# todo : change this
			packet_id = int(line[11])
			sequence_number = int(line[10])
			mycanvas.add_event(timestamp, packet_id, packet_name, event_type, hop_src_node, hop_dst_node, ip_src_node, ip_dst_node, src_port, dst_port, sequence_number,packet_flag,packet_size) 
		return mycanvas

