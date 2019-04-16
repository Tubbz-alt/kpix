import rogue
import pyrogue.gui
import sys
import argparse

pyrogue.addLibraryPath('../python/')
pyrogue.addLibraryPath('../../firmware/submodules/surf/python')

import KpixDaq

rogue.Logging.setFilter('pyrogue.SrpV3', rogue.Logging.Debug)
# Set the argument parser
parser = argparse.ArgumentParser()

parser.add_argument(
    "--ip", 
    type     = str,
    required = False,
    default = '192.168.2.10',
    help     = "IP address",
)  

parser.add_argument(
    "--hwEmu", 
    required = False,
    action = 'store_true',
    help     = "hardware emulation (false=normal operation, true=emulation)",
)

parser.add_argument(
    "--sim", 
    required = False,
    action = 'store_true',
    help     = "hardware emulation (false=normal operation, true=emulation)",
)  

parser.add_argument(
    "--pollEn", 
    required = False,
    action = 'store_true',
    help     = "enable auto-polling",
)

args = parser.parse_args()
print(args)

with KpixDaq.DesyTrackerRoot(debug=True, **vars(args)) as root:

    root.DesyTracker.KpixDaqCore.KpixAsicArray.KpixAsic[0].enable.set(True)
    root.DesyTracker.KpixDaqCore.KpixAsicArray.KpixAsic[1].enable.set(True)    
    # Create GUI
    appTop = pyrogue.gui.application(sys.argv)
    guiTop = pyrogue.gui.GuiTop(group='guiGroup')
    guiTop.addTree(root)
    guiTop.resize(1000,1000)

    # Run gui
    appTop.exec_()
