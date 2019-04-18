import collections
import pyrogue as pr

class FlippedUInt(pr.UInt):
    @classmethod
    def toBytes(cls, value, bitSize):
        value = int(f'{value:b}'.zfill(bitSize)[::-1], 2)
        return pr.UInt.toBytes(value, bitSize)

    @classmethod
    def fromBytes(cls, ba, bitSize):
        value = pr.UInt.fromBytes(ba, bitSize)
        return int(f'{value:b}'.zfill(bitSize)[::-1], 2)


class KpixAsic(pr.Device):
    def __init__(self, sysConfig=None, version=12, **kwargs):
        super().__init__(**kwargs)

        self.forceCheckEach = True

        self.calChannel = 0

        STATUS = 0x0000*4
        CONFIG = 0x0001*4
        TIMER_A = 0x0008*4
        TIMER_B = 0x0009*4
        TIMER_C = 0x000A*4
        TIMER_D = 0x000B*4
        TIMER_E = 0x000C*4
        TIMER_F = 0x000D*4
        CAL_DELAY_0 = 0x0010*4
        CAL_DELAY_1 = 0x0011*4
        DAC_0 = 0x0020*4
        DAC_1 = 0x0021*4
        DAC_2 = 0x0022*4
        DAC_3 = 0x0023*4
        DAC_4 = 0x0024*4
        DAC_5 = 0x0025*4
        DAC_6 = 0x0026*4
        DAC_7 = 0x0027*4
        DAC_8 = 0x0028*4
        DAC_9 = 0x0029*4
        CONTROL = 0x0030*4
        CHAN_MODE_A = list(range(0x0040*4, 0x0060*4, 4))
        CHAN_MODE_B = list(range(0x0060*4, 0x0080*4, 4))        

        # Status regs
        self.add(pr.RemoteVariable(
            name = 'StatCmdPerr',
            offset=STATUS,
            mode='RO',
            bitOffset=0,
            bitSize=1))

        self.add(pr.RemoteVariable(
            name = 'StatDataPerr',
            offset=STATUS,
            mode='RO',
            bitOffset=1,
            bitSize=1))

        self.add(pr.RemoteVariable(
            name = 'StatTempEn',
            offset=STATUS,
            mode='RO',
            bitOffset=2,
            bitSize=1))

        self.add(pr.RemoteVariable(
            name = 'StatTempIdValue',
            offset=STATUS,
            mode='RO',
            bitOffset=24,
            bitSize=8))

        # Config Regs
        self.add(pr.RemoteVariable(
            name = 'CfgAutoReadDisable',
            offset=CONFIG,
            bitOffset=2,
            bitSize=1,
            base=pr.Bool))

        self.add(pr.RemoteVariable(
            name = 'CfgForceTemp',
            offset=CONFIG,
            bitOffset=3,
            bitSize=1,
            base=pr.Bool))

        self.add(pr.RemoteVariable(
            name = 'CfgDisableTemp',
            offset=CONFIG,
            bitOffset=4,
            bitSize=1,
            base=pr.Bool))

        self.add(pr.RemoteVariable(
            name = 'CfgAutoStatusReadEn',
            offset=CONFIG,
            bitOffset=5,
            bitSize=1,
            base=pr.Bool))

        # Timer stuff

        self.add(pr.RemoteVariable(
            name = 'TimeResetOn',
            offset=TIMER_A,
            bitOffset=0,
            bitSize=16))

        self.add(pr.RemoteVariable(
            name = 'TimeResetOff',
            offset=TIMER_A,
            bitOffset=16,
            bitSize=16))

        self.add(pr.RemoteVariable(
            name = 'TimeOffsetNullOff',
            offset=TIMER_B,
            bitOffset=0,
            bitSize=16))

        self.add(pr.RemoteVariable(
            name = 'TimeLeakageNullOff',
            offset=TIMER_B,
            bitOffset=16,
            bitSize=16))

        self.add(pr.RemoteVariable(
            name = 'TimeDeselDelay',
            offset=TIMER_F,
            bitOffset=0,
            bitSize=8))

        self.add(pr.RemoteVariable(
            name = 'TimeBunchClkDelay',
            offset=TIMER_F,
            bitOffset=8,
            bitSize=16))

        self.add(pr.RemoteVariable(
            name = 'TimeDigitizeDelay',
            offset=TIMER_F,
            bitOffset=24,
            bitSize=8))

        self.add(pr.RemoteVariable(
            name = 'TimePowerUpDigOn',
            offset=TIMER_C,
            bitOffset=0,
            bitSize=16))

        self.add(pr.RemoteVariable(
            name = 'TimeThreshOff',
            offset=TIMER_C,
            bitOffset=16,
            bitSize=16))

        self.add(pr.RemoteVariable(
            name = 'TimerD',
            offset=TIMER_D,
            bitOffset=0,
            bitSize=32,
            hidden=True))

        self.add(pr.LinkVariable(
            name = 'TrigInhibitOff',
            dependencies = [self.TimerD, self.TimeBunchClkDelay],
            linkedGet = lambda: round(((self.TimerD.value()-self.TimeBunchClkDelay.value())-1)/8),
            linkedSet = lambda value, write: self.TimerD.set((round(value)*8)+self.TimeBunchClkDelay.value()+1, write=write)))

        # setComp(0,1,1,'')
        self.add(pr.RemoteVariable(
            name = 'BunchClockCountRaw',
            offset=TIMER_E,
            bitOffset=0,
            bitSize=16,
            hidden=True))

        self.add(pr.LinkVariable(
            name = 'BunchClockCount',
            dependencies = [self.BunchClockCountRaw],
            linkedGet = lambda: self.BunchClockCountRaw.value() + 1,
            linkedSet = lambda value, write: self.BunchClockCountRaw.set(value - 1, write)))
        

        self.add(pr.RemoteVariable(
            name = 'TimePowerUpOn',
            offset=TIMER_E,
            bitOffset=16,
            bitSize=16))


        # Calibration Stuff
        self.add(pr.RemoteVariable(
            name = 'Cal0Delay',
            offset=CAL_DELAY_0,
            bitOffset=0,
            bitSize=13))

        self.add(pr.RemoteVariable(
            name = 'Cal1Delay',
            offset=CAL_DELAY_0,
            bitOffset=16,
            bitSize=13))

        self.add(pr.RemoteVariable(
            name = 'Cal2Delay',
            offset=CAL_DELAY_1,
            bitOffset=0,
            bitSize=13))

        self.add(pr.RemoteVariable(
            name = 'Cal3Delay',
            offset=CAL_DELAY_1,
            bitOffset=16,
            bitSize=13))

        self.add(pr.RemoteVariable(
            name = 'CalCount',
            offset=[CAL_DELAY_0, CAL_DELAY_0, CAL_DELAY_1, CAL_DELAY_1],
            bitOffset=[15, 31, 15, 31],
            bitSize=[1, 1, 1, 1],
            enum = {
                0x0: '0',
                0x1: '1',
                0x3: '2',
                0x7: '3',
                0xf: '4'}))

        # These registers dont exist in the local kpix
        def makeDacVar(name, offset):
            # Raw KPIX regs
            for i in range(4):
                self.add(pr.RemoteVariable(
                    name = f'{name}Raw{i}',
                    offset = offset,
                    bitOffset = i*8,                    
                    bitSize = 8,
                    hidden = True))
            raws = [self.node(f'{name}Raw{i}') for i in range(4)]

            # Link Variable that gets set
            # Set all Raw RemoteVariables to the same value
            def ls(value, write):
                for v in raws:
                    v.set(value, write=write)

            self.add(pr.LinkVariable(
                name = name,
                mode = 'RW',
                dependencies = raws,
                linkedGet = lambda: self.node(f'{name}Raw0').value(),
                linkedSet = ls))
            link = self.node(name)

            def dacToVolt():
                dac = link.value()
                if dac >= 0xf6:
                    return 2.5 - ((0xff-dac) * 50.0 * 0.0001)
                else:
                    return dac * 100.0 * 0.0001

            # View of variable as voltage
            self.add(pr.LinkVariable(
                name = f'{name}Volt',
                mode = 'RO',
                disp = '{:0.3f}',
                dependencies = [self.node(name)],
                linkedGet = dacToVolt))

        makeDacVar(name = 'DacPreThresholdA', offset=DAC_0)
        makeDacVar(name = 'DacPreThresholdB', offset=DAC_1)
        makeDacVar(name = 'DacRampThresh', offset=DAC_2)
        makeDacVar(name = 'DacRangeThreshold', offset=DAC_3)
        makeDacVar(name = 'DacCalibration', offset=DAC_4)
        makeDacVar(name = 'DacEventThreshold', offset=DAC_5)
        makeDacVar(name = 'DacShaperBias', offset=DAC_6)
        makeDacVar(name = 'DacDefaultAnalog', offset=DAC_7)
        makeDacVar(name = 'DacThresholdA', offset=DAC_8)
        makeDacVar(name = 'DacThresholdB', offset=DAC_9)        
        

        #DacCalibrationCharge
        
        self.add(pr.RemoteVariable(
            name = 'CntrlDisPerReset',
            offset=CONTROL,
            bitOffset=0,
            bitSize=1,
            base=pr.Bool))

        self.add(pr.RemoteVariable(
            name = 'CntrlEnDcReset',
            offset=CONTROL,
            bitOffset=1,
            bitSize=1,
            base=pr.Bool))

        self.add(pr.RemoteVariable(
            name = 'CntrlHighGain',
            offset=CONTROL,
            bitOffset=2,
            bitSize=1,
            base=pr.Bool))

        self.add(pr.RemoteVariable(
            name = 'CntrlNearNeighbor',
            offset=CONTROL,
            bitOffset=3,
            bitSize=1,
            base=pr.Bool))

        self.add(pr.RemoteVariable(
            name = 'CntrlCalSource',
            offset=CONTROL,
            bitOffset=[6, 4], # I'm pretty sure this wont work
            bitSize=[1, 1],
            enum = {
                0: 'Disable',
                1: 'Internal',
                2: 'External',
                3: '-'}))

        self.add(pr.RemoteVariable(
            name = 'CntrlForceTrigSource',
            offset=CONTROL,
            bitOffset=[7, 5], # I'm pretty sure this wont work
            bitSize=[1, 1],
            enum = {
                0: 'Disable',
                1: 'Internal',
                2: 'External',
                3: '-'}))

        self.add(pr.RemoteVariable(
            name = 'CntrlHoldTime',
            offset=CONTROL,
            bitOffset=8,
            bitSize=3,
            enum = {
                0: '8x',
                1: '16x',
                2: '24x',
                3: '32x',
                4: '40x',
                5: '48x',
                6: '56x',
                7: '64x'}))
        
        self.add(pr.RemoteVariable(
            name = 'CntrlCalibHigh',
            offset=CONTROL,
            bitOffset=11,
            bitSize=1,
            base=pr.Bool))

        self.add(pr.RemoteVariable(
            name = 'CntrlShortIntEn',
            offset=CONTROL,
            bitOffset=12,
            bitSize=1,
            base=pr.Bool))

        self.add(pr.RemoteVariable(
            name = 'CntrlForceLowGain',
            offset=CONTROL,
            bitOffset=13,
            bitSize=1,
            base=pr.Bool))

        self.add(pr.RemoteVariable(
            name = 'CntrlLeakNullDisable',
            offset=CONTROL,
            bitOffset=14,
            bitSize=1,
            base=pr.Bool))


        self.add(pr.RemoteVariable(
            name = 'CntrlPolarity',
            offset=CONTROL,
            bitOffset=15,
            bitSize=1,
            enum = {
                0: 'Negative',
                1: 'Positive'}))

        self.add(pr.RemoteVariable(
            name = 'CntrlTrigDisable',
            offset=CONTROL,
            bitOffset=16,
            bitSize=1,
            base=pr.Bool))


        self.add(pr.RemoteVariable(
            name = 'CntrlDisPwrCycle',
            offset=CONTROL,
            bitOffset=24,
            bitSize=1,
            base=pr.Bool))

        self.add(pr.RemoteVariable(
            name = 'CntrlFeCurr',
            offset=CONTROL,
            bitOffset=25,
            bitSize=3,
            base = FlippedUInt,
            enum = {
                0: '1uA',
                1: '31uA',
                2: '61uA',
                3: '91uA',
                4: '121uA',
                5: '151uA',
                6: '181uA',
                7: '221uA'}))


        self.add(pr.RemoteVariable(
            name = 'CntrlDiffTime',
            offset=CONTROL,
            bitOffset=28,
            bitSize=2,
            enum = {
                0: 'Normal',
                1: 'Half',
                2: 'Third',
                3: 'Quarter'}))

        self.add(pr.RemoteVariable(
            name = 'CntrlMonSource',
            offset=CONTROL,
            bitOffset=30,
            bitSize=2,
            base = FlippedUInt,
            enum = {
                0: 'None',
                1: 'Amp',
                2: 'Shaper',
                3: '-'}))

        for i, addr in enumerate(CHAN_MODE_A):
            self.add(pr.RemoteVariable(
                name = f'ChanModeA_{i}',
                offset = addr,
                mode = 'RW',
                bitOffset = 0,
                bitSize = 32,
                hidden = True))

        for i, addr in enumerate(CHAN_MODE_B):
            self.add(pr.RemoteVariable(
                name = f'ChanModeB_{i}',
                offset = addr,
                mode = 'RW',
                bitOffset = 0,
                bitSize = 32,
                hidden = True))

        d = {(0,0): 'B',
             (0,1): 'D',
             (1,0): 'A',
             (1,1): 'C'}
        drev = {v:k for k,v in d.items()}
        
        def getChanMode(dev, var):
            l = []
            a = var.dependencies[0].value()
            b = var.dependencies[1].value()
            for row in range(32):
                val = ((((b >> (row)) & 1) ), ((a >> (row)) & 1))
                l.append(d[val])
            s =  ' '.join([''.join(x for x in l[i:i+8]) for i in range(0, 32, 8)])
            #print(f'getChanMode = {s}')
            return s

        def setChanMode(dev, var, value, write):
            value = ''.join(value.split()) # remove whitespace
            regA = 0
            regB = 0
            for row in range(32):
                b,a = drev[value[row]]
                regA |= a << (row)
                regB |= b << (row)

            #print(f'setChanMode(value={value}) - regA={regA:x}, regB = {regB:x}')
                
            var.dependencies[0].set(value = regA, write=write)
            var.dependencies[1].set(value = regB, write=write)

        colModes = []
        for col in range(32):
            colModes.append(pr.LinkVariable(
                name = f'Chan_{col*32:d}_{col*32+31:d}',
                mode = 'RW',
                dependencies = [self.node(f'ChanModeA_{col}'), self.node(f'ChanModeB_{col}')],
                linkedGet = getChanMode,
                linkedSet = setChanMode))
        self.add(colModes)

#         for col in range(32):
#             for row in range(32):
#                 self.add(pr.LinkVariable(
#                     name = f'Chan[{col}][{row}]',
#                     mode = 'WO',
#                     dependencies = [colModes[col]]
#                     linkedSet = lambda dev,var,value,write: 
                    

#    def readBlocks(self, recurse=True, variable=None, checkEach=False):
#        super().readBlocks(recurse=recurse, variable=variable, checkEach=True)

#         for i, offset in enumerate(CHAN_MODE_A):
#             self.add(pr.RemoteVariable(
#                 name = f'CHAN_MODE_A[{i}]',
#                 offset = offset,
#                 bitOffset = 0,
#                 bitSize = 32,
#                 mode = 'RW'))

#         for i, offset in enumerate(CHAN_MODE_B):
#             self.add(pr.RemoteVariable(
#                 name = f'CHAN_MODE_B[{i}]',
#                 offset = offset,
#                 bitOffset = 0,
#                 bitSize = 32,
#                 mode = 'RW'))
            

        # Channel mode variables
 #        for col in range(32):
#             for row in range(32):
#                 self.add(pr.RemoteVariable(
#                     name = f'ChanMode_{col}_{row}',
#                     mode = 'RW',
#                     offset = [CHAN_MODE_A[col], CHAN_MODE_B[col]],
#                     bitOffset = 31-row,
#                     bitSize = 1,
#                     hidden = True,
#                     enum = {
#                         0b00: 'B',
#                         0b01: 'D',
#                         0b10: 'A',
#                         0b11: 'C'}))

#         def getChanMode(dev, var):
#             # Combine into a string with a space every 8 chars
#             return ' '.join([''.join(dep.valueDisp() for dep in var.dependencies[i:i+8]) for i in range(0, 32, 8)])
        
#         def setChanMode(dev, var, value):
#             value = ''.join(value.split()) # remove whitespace
#             for i, dep in enumerate(var.dependencies):
#                 dep.setDisp(value[i], write=(i==31)) # Only write on last value


#         for col in range(32):
#             self.add(pr.LinkVariable(
#                 name = f'Chan_{col*32:d}_{col*32+31:d}',
#                 mode = 'RW',
#                 dependencies = [self.node(f'ChanMode_{col}_{row}') for row in range(32)],
#                 linkedGet = getChanMode,
#                 linkedSet = setChanMode))

    def setCalibrationMode(self):
        self.CntrlCalSource.setDisp("Internal", write=True)
        self.CntrlForceTrigSource.setDisp("Internal", write=True)
        self.CntrlTrigDisable.set(True, write=True)

    def setCalibration(self, channel, dac):
        row = channel%32
        col = channel//32
        
        self.DacCalibration.set(dac, write=False)

        # Turn off last cal channel
        oldCol = self.calChannel//32
        modestring = ['D' for x in range(32)]
        self.node(f'Chan_{oldCol*32:d}_{oldCol*32+31:d}').setDisp(''.join(modestring), write=False)

        # Turn on new cal channel
        modestring[row] = 'C'
        self.node(f'Chan_{col*32:d}_{col*32+31:d}').setDisp(''.join(modestring), write=False)
        self.calChannel = channel
        self.writeBlocks()
        self.verifyBlocks()
        self.checkBlocks()
        

class LocalKpix(KpixAsic):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.activeVariables = self.find(name='(Stat)') + self.find(name='(Cal)') + self.find(name='(Time)') + self.find(name='(Cfg)') + self.find(name='(TrigInhibitOff)')
        #self.activeVariables.remove(self.TimeBunchClkDelay)
        for v in self.variables.values():
            if v not in self.activeVariables:
                v.hidden = True
        self.enable.hidden = False
        #print('LocalKpix Variables:')
        #for v in self.activeVariables:
        #    print(v.name)
        #print('-------')
        #print('')

        
    def writeBlocks(self, force=False, recurse=True, variable=None, checkEach=False):
        if variable is None:
            #print('Blocks:')
            #for b in self._getBlocks(self.activeVariables):
            #    print(b)
            #print('-----')
            #print('')
            super().writeBlocks(force=force, recurse=recurse, variable=self.activeVariables, checkEach=checkEach)
        else:
            if isinstance(variable, pr.BaseVariable):
                variable = [variable]
            variable = [x for x in variable if x in self.activeVariables]
            print('Blocks')
            print(self._getBlocks(variable))
            super().writeBlocks(force=force, recurse=recurse, variable=variable, checkEach=checkEach)

        
    def readBlocks(self, recurse=True, variable=None, checkEach=False):
        if variable is None:
            super().readBlocks(recurse=recurse, variable=self.activeVariables, checkEach=True)
        else:
            if isinstance(variable, pr.BaseVariable):
                variable = [variable]
            variable = [x for x in variable if x in self.activeVariables]            
            super().readBlocks(recurse=recurse, variable=variable, checkEach=True)
        
    def verifyBlocks(self, recurse=True, variable=None, checkEach=False):
        if variable is None:
            super().verifyBlocks(recurse=recurse, variable=self.activeVariables, checkEach=checkEach)
        else:
            if isinstance(variable, pr.BaseVariable):
                variable = [variable]
            variable = [x for x in variable if x in self.activeVariables]            
            super().verifyBlocks(recurse=recurse, variable=variable, checkEach=True)

    def checkBlocks(self, recurse=True, variable=None):
        if variable is None:
            super().checkBlocks(recurse=recurse, variable=self.activeVariables)
        else:
            if isinstance(variable, pr.BaseVariable):
                variable = [variable]
            variable = [x for x in variable if x in self.activeVariables]            
            super().checkBlocks(recurse=recurse, variable=variable)


# Manipulate entire array together    
# class KpixArray(pr.Device):
#     def __init__(self, array, **kwargs):
#         super().__init__(**kwargs)

#         for v in array[0].variables if v.name != 'enable':
#             allVars = [d.node(v.name) for d in array if d.node(v.name) is not None]
            
#             def ls(value, write):
#                 for x in allVars:
#                     v.set(value=value, write=write)
                
#             self.add(pr.LinkVariable(
#                 name = v.name,
#                 dependencies = allVars,
#                 linkedGet = v.value,
#                 linkedSet = ls))
