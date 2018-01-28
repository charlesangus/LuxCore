#!/usr/bin/python
# -*- coding: utf-8 -*-
################################################################################
# Copyright 1998-2018 by authors (see AUTHORS.txt)
#
#   This file is part of LuxCoreRender.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

import os
import socket
import argparse
import logging
import functools

import pyluxcore
import pyluxcoretools.renderfarm.renderfarm as renderfarm
import pyluxcoretools.renderfarm.renderfarmjobsingleimage as jobsingleimage
import pyluxcoretools.utils.loghandler as loghandler
import pyluxcoretools.utils.netbeacon as netbeacon

logger = logging.getLogger(loghandler.loggerName + ".luxcorenetconsole")

class LuxCoreNetConsole:
	def NodeDiscoveryCallBack(self, ipAddress, port):
		self.renderFarm.DiscoveredNode(ipAddress, port, renderfarm.NodeDiscoveryType.AUTO_DISCOVERED)

	def Exec(self, argv):
		parser = argparse.ArgumentParser(description="PyLuxCoreNetConsole")
		parser.add_argument("fileToRender",
							help=".bcf file to render")
		parser.add_argument("-s", "--stats-period", metavar="SECS", type=float,
							default=10.0,
							help="node statistics print period")
		parser.add_argument("-f", "--film-period", metavar="SECS", type=float,
							default=10.0 * 60.0,
							help="node film update period")
		parser.add_argument("-p", "--halt-spp", metavar="SAMPLES_PER_PIXEL", type=int,
							default=0,
							help="samples/pixel halt condition")
		parser.add_argument("-t", "--halt-time", metavar="SECS", type=float,
							default=0,
							help="time halt condition")
		parser.add_argument("-n", "--nodes", metavar="IPADDRESS", nargs="+",
							help="rendering nodes ip addresses")
		# Not possible for single image renderings
		#parser.add_argument("-c", "--halt-conv-threshold", metavar="SHADE", type=float,
		#					default=3.0,
		#					help="convergence threshold halt condition (expressed in 8bit shades: [0, 255])")
		parser.add_argument("-d", "--disable-auto-discover", action='store_true',
							help="disable node auot-discover")

		# Parse command line arguments
		args = parser.parse_args(argv)
		if not args.fileToRender:
			raise TypeError("File to render must be specified")
		configFileNameExt = os.path.splitext(args.fileToRender)[1]
		if configFileNameExt != ".bcf":
			raise TypeError("File to render must a .bcf format")

		#-----------------------------------------------------------------------
		# Create the render farm
		#-----------------------------------------------------------------------

		self.renderFarm = renderfarm.RenderFarm()
		self.renderFarm.SetStatsPeriod(args.stats_period)
		self.renderFarm.SetFilmUpdatePeriod(args.film_period)
		self.renderFarm.Start()

		#-----------------------------------------------------------------------
		# Create the render farm job
		#-----------------------------------------------------------------------

		renderFarmJob = jobsingleimage.RenderFarmJobSingleImage(self.renderFarm, args.fileToRender)
		renderFarmJob.SetFilmHaltSPP(args.halt_spp)
		renderFarmJob.SetFilmHaltTime(args.halt_time)
		#self.renderFarm.SetFilmHaltConvThreshold(args.halt_conv_threshold)
		self.renderFarm.AddJob(renderFarmJob)
		
		#-----------------------------------------------------------------------
		# Add all command line defined nodes
		#-----------------------------------------------------------------------

		for node in args.nodes:
			# Check if the port has been defined
			if node.find(':') != -1:
				(ipAddress, port) = node.split(":")
			else:
				(ipAddress, port) = (node, renderfarm.DEFAULT_PORT)

			# Check if it is a valid ip address
			try:
				socket.inet_aton(ipAddress)
			except socket.error:
				raise SyntaxError("Rendering node ip address syntax error: " + node)

			# Check if it is a valid port
			port = int(port)
			try:
				port = int(port)
			except ValueError:
				raise SyntaxError("Rendering node port syntax error: " + node)

			self.renderFarm.DiscoveredNode(ipAddress, port, renderfarm.NodeDiscoveryType.MANUALLY_DISCOVERED)

		#-----------------------------------------------------------------------
		
		if not args.disable_auto_discover:
			# Start the beacon receiver
			beacon = netbeacon.NetBeaconReceiver(functools.partial(LuxCoreNetConsole.NodeDiscoveryCallBack, self))
			beacon.Start()
		else:
			beacon = None
			
		#-----------------------------------------------------------------------

		try:
			self.renderFarm.HasDone()
		except KeyboardInterrupt:
			pass
		finally:
			if beacon:
				# Stop the beacon receiver
				beacon.Stop()

		# Stop the render farm
		self.renderFarm.Stop()

		logger.info("Done")

def main(argv):
	try:
		pyluxcore.Init(loghandler.LuxCoreLogHandlerDebug)
		logger.info("LuxCore %s" % pyluxcore.Version())

		netConsole = LuxCoreNetConsole()
		netConsole.Exec(argv[1:])
	finally:
		pyluxcore.SetLogHandler(None)

if __name__ == "__main__":
	main(sys.argv)
