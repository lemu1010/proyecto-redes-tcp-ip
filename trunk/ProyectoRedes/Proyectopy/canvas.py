class Junction():
	def __init__(self, canvas, pkt1, pkt2,pkt_number):
		self.canvas = canvas
		self.x1 = (pkt1.dequeued_time + pkt1.received_time)/2
		self.x2 = (pkt2.dequeued_time + pkt2.received_time)/2
		self.y = (pkt1.src_node.queue_out_y + pkt1.dst_node.queue_in_y)/2
		self.x_number =	pkt1.enqueued_time
		self.y_number = pkt1.src_node.queue_in_y
		self.pkt_number = pkt_number
	def draw(self, terminal):
		terminal.line((self.x1,self.y),(self.x2,self.y), (0,0,0))		
		terminal.text(self.x_number,self.y_number-5,self.pkt_number,8)

class Node():
	def __init__(self, canvas):
		self.canvas = canvas
		self.order = len(self.canvas.node_list)
		self.queue_in_y  = 100*self.order+20
		self.queue_out_y = 100*self.order+20+20
		
		
		self.queue_size_evolution = [(0,0)]
	
	def queue_size_increase(self,timestamp):
		new_size = self.queue_size_evolution[-1][1]+1
		self.queue_size_evolution.append((timestamp,new_size))
	def queue_size_decrease(self,timestamp):
		new_size = self.queue_size_evolution[-1][1]-1
		self.queue_size_evolution.append((timestamp,new_size))

	def draw(self, terminal):
		#print "max", self.canvas.max_timestamp
		#draws line after queue
		terminal.line((self.canvas.min_timestamp,self.queue_out_y),(self.canvas.max_timestamp,self.queue_out_y))
		#draws line before queue
		terminal.line((self.canvas.min_timestamp,self.queue_in_y),(self.canvas.max_timestamp,self.queue_in_y))
		terminal.text(self.canvas.min_timestamp,self.queue_in_y+0.05,self.order,14)# NODE'S NUMBER
		#	for q_size_event in self.queue_size_evolution:
		#		(timestamp, size) = q_size_event
		#		#print timestamp, size
		#		terminal.line((timestamp*1000,self.y),(timestamp*1000,self.y+10*size))

class Packet():
	def __init__(self, canvas, name, ip_src_node, ip_dst_node, src_port, dst_port, sequence_number,packet_flag,packet_size,packet_id):
		self.canvas = canvas
		self.name = name
		self.received 	= False
		self.enqueued 	= False
		self.dequeued 	= False
		self.dropped 	= False
		self.error 		= False
		self.sequence_number = sequence_number
		self.is_a_retransmission = False
		self.packet_flag=packet_flag
		self.packet_size=packet_size
		self.x1=0;
		self.x2=0;
		self.y1=0;
		self.y2=0;
		self.id=packet_id;	
		#self.src_node = self.canvas.node_list[ip_src_node]
		#self.dst_node = self.canvas.node_list[ip_dst_node]
		
	
	def add_event(self, packet_id, timestamp, event_type, hop_src_node, hop_dst_node):
		self.src_node = self.canvas.node_list[hop_src_node]
		self.dst_node = self.canvas.node_list[hop_dst_node]
		#print 'DATOS PAQUETE ' , self.src_node.order, self.dst_node.order
#		self.id=packet_id;		
		
		if event_type == 'r':
			self.received 		= True
			self.received_time 	= timestamp
		if event_type == '+':
			self.enqueued 		= True
			self.enqueued_time 	= timestamp
			self.src_node.queue_size_increase(timestamp)
		if event_type == '-':
			self.dequeued 		= True
			self.dequeued_time 	= timestamp
			self.src_node.queue_size_decrease(timestamp)
		if event_type == 'd':
			self.dropped		= True
			self.dropped_time 	= timestamp
		if event_type == 'e':
			self.error 			= True
			self.error_time 	= timestamp


		
	def draw(self, terminal):
		
		if self.error:
			raise
		if self.enqueued:
			x1 = self.enqueued_time 
			y1 = self.src_node.queue_in_y
		
		if self.dequeued:
			x2 = self.dequeued_time 
			y2 = self.src_node.queue_out_y
		elif self.dropped:
			x2 = self.dropped_time 
			y2 = self.dst_node.queue_in_y

		if self.received:
			x3 = self.received_time
			y3 = self.dst_node.queue_out_y 
		else:
			#################WARNING#####################"
			terminal.line((0,0),(0,0), (0,0,0))
		
		if self.life == 'normal' :
			style = ""
			if self.name == 'tcp' and self.is_a_retransmission == False:
				color = (0,0,255)
				#style = "stroke-dasharray:1,3;stroke-dashoffset:0"
			elif self.name == 'tcp' and self.is_a_retransmission == True:
				color =(255,0,0)
			elif self.name == 'ack' and self.is_a_retransmission == False:
				color = (100,100,100)
			elif self.name == 'ack' and self.is_a_retransmission == True:
				color = (255,0,0)
				#style = "stroke-dasharray:8,2,1,2;stroke-dashoffset:0"
			else:
				color = (0,180,180)
			#if self.packet_flag[0] =='C':
			if self.packet_flag[0] =='C' or (self.packet_size == 74 and self.name=='tcp'): #i can say that it is a control packet
				color = (0,255,0) 
			terminal.line((x1,y1),(x2,y2),color, style)
			terminal.line((x2,y2),(x3,y3),color, style)			
		elif self.life == 'dropped' :
			color = (255,0,0)
			terminal.line((x1, y1),(x1, y1 +(y1-y2)/50), color)
		

class Canvas():
	def __init__(self):
		self.items = []
		self.height = 0
		self.width = 0
		self.packet_list = {}
		self.node_list = {}
		self.min_timestamp =-1
		self.scale_size = 0.25
		self.packet_number=0;
		self.packet_count=0;

	def thisPacketExist(self,packet_id, src_node, dst_node):
#		print 'DATOS PAQUETE ' , packet_id, src_node, dst_node
		for i, packet in self.packet_list.iteritems():
#			print 'DATOS PAQUETE ' , packet.id, packet.src_node.order, packet.dst_node.order
#			print 'DATOS PAQUETE1' , packet_id, src_node, dst_node
			#if (packet.id==packet_id and packet.src_node.order==src_node and packet.dst_node.order==dst_node):
			if (packet.id==packet_id and packet.src_node==	self.node_list[src_node] and packet.dst_node==	self.node_list[dst_node]):
				self.packet_number=i;
				return True

		return False

	def add_event(self, timestamp, packet_id, packet_name, event_type, hop_src_node, hop_dst_node, ip_src_node, ip_dst_node, src_port, dst_port, sequence_number, packet_flag, packet_size) :
		if self.min_timestamp == -1:
			self.min_timestamp = timestamp
			
		if not self.node_list.has_key(hop_src_node):
			self.node_list[hop_src_node] = Node(self)
		if not self.node_list.has_key(hop_dst_node):
			self.node_list[hop_dst_node] = Node(self)
		
		
		if self.thisPacketExist(packet_id, hop_src_node, hop_dst_node):
			packet = self.packet_list[self.packet_number]
		else:
			packet = Packet(self, packet_name, ip_src_node, ip_dst_node, src_port, dst_port, sequence_number, packet_flag,packet_size,packet_id)
			self.packet_list[self.packet_count] = packet
			self.packet_count=self.packet_count+1
		packet.add_event(packet_id, timestamp, event_type, hop_src_node, hop_dst_node)		
		self.max_timestamp = timestamp
	
	def drawScale(self, terminal):
		i=self.min_timestamp
		while (i<=self.max_timestamp):
			terminal.line((i,((len(self.node_list)-1)*100)+40),(i,((len(self.node_list)-1)*100)+45), (0,0,0))
			terminal.text(i,((len(self.node_list)-1)*100)+50,(str(i)+'(s)'),8)
			i+=self.scale_size;

	def draw(self, terminal):
		tcp_sequence_number ={}
		ack_sequence_number = {}
		for nid, node in self.node_list.iteritems():
			print nid, node
			node.draw(terminal)
			tcp_sequence_number[nid] = -1;
			ack_sequence_number[nid] = -1;
		
		i=0
		j=0
		tcp_slide_burst_size = 0
		tcp_slide_node =-1;
		ack_slide_burst_size = 0
		
		for pid, packet in self.packet_list.iteritems():
			a= packet.enqueued, packet.dequeued, packet.received, packet.dropped, packet.error 
			if a == (True, True, True, False, False):
				packet.life = 'normal'
			elif a == (True, False, False, True, False):
				packet.life = 'dropped'
			else:
				packet.life = 'weird'

			if packet.name == 'tcp':
				if i==0:
					tcp_sequence_number[packet.src_node.order] = packet.sequence_number
					tcp_slide_burst_time = packet.enqueued_time
					tcp_slide_burst_node = packet.src_node.order;
					tcp_slide_burst_beginning_packet = packet
					i=1
				else :
					if packet.sequence_number < tcp_sequence_number[packet.src_node.order]:
						packet.is_a_retransmission = True
						print 'TCP RETRANSMISSION', packet.sequence_number , tcp_sequence_number[packet.src_node.order]
					else :
						tcp_sequence_number[packet.src_node.order] = packet.sequence_number
						
							
							

					#print len(self.packet_list) , i
					if tcp_slide_burst_time == packet.enqueued_time and tcp_slide_burst_node == packet.src_node.order:
						if packet.life == 'normal':
							tcp_slide_burst_ending_packet = packet
							tcp_slide_burst_size +=1
					else:
						if tcp_slide_burst_size > 1:
							junction = Junction(self,tcp_slide_burst_beginning_packet, tcp_slide_burst_ending_packet,tcp_slide_burst_size)
							junction.draw(terminal)
							#print 'JUNCTION HERE', tcp_slide_burst_size
						tcp_slide_burst_node = packet.src_node.order;
						tcp_slide_burst_size = 1
						tcp_slide_burst_time = packet.enqueued_time
						tcp_slide_burst_beginning_packet = packet
			
			if packet.name == 'ack':
				if j==0:
					ack_sequence_number[packet.src_node.order] = packet.sequence_number
					ack_slide_burst_time = packet.enqueued_time
					ack_slide_burst_beginning_packet = packet
					ack_slide_burst_node = packet.src_node.order
					j=1
				else :
					if packet.sequence_number < ack_sequence_number[packet.dst_node.order]:
						packet.is_a_retransmission = True
						print 'ACK RETRANSMISSION', packet.sequence_number , ack_sequence_number[packet.dst_node.order]
					else :
						ack_sequence_number[packet.dst_node.order] = packet.sequence_number
					
					if ack_slide_burst_time == packet.enqueued_time and ack_slide_burst_node == packet.src_node.order:
						if packet.life == 'normal':
							ack_slide_burst_ending_packet = packet
							ack_slide_burst_size +=1
					else:
						if ack_slide_burst_size > 1:
							junction = Junction(self,ack_slide_burst_beginning_packet, ack_slide_burst_ending_packet,ack_slide_burst_size)
							junction.draw(terminal)
							#print 'JUNCTION HERE', ack_slide_burst_size
						
						ack_slide_burst_size = 1
						ack_slide_burst_time = packet.enqueued_time
						ack_slide_burst_beginning_packet = packet
						ack_slide_burst_node == packet.src_node.order
			
			packet.draw(terminal)
		self.drawScale(terminal)
		
	

		
