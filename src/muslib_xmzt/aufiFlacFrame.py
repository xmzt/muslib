from aufi_xmzt import aufiBase
from muslib_xmzt import util
import aufiC

#--------------------------------------------------------------------------------------------------------------------
# Logc
#--------------------------------------------------------------------------------------------------------------------

class Logc(util.Logc):
    def wrap(self, *args):
        return self.main.logr('[flacFrame] ', *args)

    #--------------------------------------------------------------------------------------------------------------------
    # c callbacks

    def parseE(self, pos, e):
        return self.main.logr(f'[ERROR flacFrame] <{pos}> {aufiBase.AufiE.des(e)}')

    def frame(self, pos):
        return self.wrap(f'<{pos}> frame')

    def head(self, blockZVariable, blockZ, sampleRate, channelAssignment, sampleZ, sampleI0):
        return self.wrap(f'head').inLines(f'blockZVariable=0x{blockZVariable:02x}',
                                   f'blockZ=0x{blockZ:02x}',
                                   f'sampleRate=0x{sampleRate:02x}',
                                   f'channelAssignment=0x{channelAssignment:02x}',
                                   f'sampleZ=0x{sampleZ:02x}',
                                   f'sampleI0=0x{sampleI0:02x}')
        
    def headSampleI(self, sampleI):
        return self.wrap(f'headSampleI {sampleI}')
        
    def blockZ(self, blockZ):
        return self.wrap(f'blockZ {blockZ}')
        
    def sampleRate(self, sampleRate):
        return self.wrap(f'sampleRate {sampleRate}')
        
    def channels(self, channelsN, channelsSampleZInc):
        return self.wrap(f'channels {channelsN=} channelsSampleZInc=0x{channelsSampleZInc:02x}')
        
    def sampleZ(self, sampleZ):
        return self.wrap(f'sampleZ {sampleZ}')
        
    def sampleI(self, sampleI):
        return self.wrap(f'sampleI {sampleI}')
        
    def subframe(self, pos, bitI, subframeI):
        return self.wrap(f'<{pos}.-{bitI}> subframe {subframeI=}')
        
    def subframeType(self, type):
        return self.wrap(f'subframeType {type}')
        
    def subframeWasted(self, wasted):
        return self.wrap(f'subframeWasted {wasted}')
        
    def subframeSampleZ(self, sampleZ):
        return self.wrap(f'subframeSampleZ {sampleZ}')
        
    def subframeConstant(self, pos, bitI):
        return self.wrap(f'<{pos}.-{bitI}> subframeConstant')
        
    def subframeConstantVal(self, val):
        return self.wrap(f'subframeConstantVal {val}')
        
    def subframeVerbatim(self, pos, bitI):
        return self.wrap(f'<{pos}.-{bitI}> subframeVerbatim')
        
    def subframeVerbatimVal(self, index, val):
        return self.wrap(f'subframeVerbatimVal {index=} {val=}')
        
    def subframeFixed(self, pos, bitI, predictorOrder):
        return self.wrap(f'<{pos}.-{bitI}> subframeFixed {predictorOrder=}')
        
    def subframeFixedWarmup(self, index, val):
        return self.wrap(f'subframeFixedWarmup {index=} {val=}')
        
    def subframeLpc(self, pos, bitI, predictorOrder):
        return self.wrap(f'<{pos}.-{bitI}> subframeLpc {predictorOrder=}')
        
    def subframeLpcWarmup(self, index, val):
        return self.wrap(f'subframeLpcWarmup {index=} {val=}')
        
    def subframeLpcQlpCoeffPrecision(self, lpcQlpCoeffPrecision):
        return self.wrap(f'subframeLpcQlpCoeffPrecision {lpcQlpCoeffPrecision}')
        
    def subframeLpcQlpShift(self, lpcQlpShift):
        return self.wrap(f'subframeLpcQlpShift {lpcQlpShift}')
        
    def subframeLpcCoeffVal(self, index, val):
        return self.wrap(f'subframeLpcCoeffVal {index=} {val=}')
        
    def residual(self, pos, bitI):
        return self.wrap(f'<{pos}.-{bitI}> residual')
        
    def residualType(self, type):
        return self.wrap(f'residualType {type}')
        
    def residualRiceZ(self, riceZ):
        return self.wrap(f'residualRiceZ {riceZ}')
        
    def residualPartitions(self, order, n, nzSamplesN):
        return self.wrap(f'residualPartitions {order=} {n=} {nzSamplesN=}')
        
    def partition(self, pos, bitI, partitionI):
        return self.wrap(f'<{pos}.-{bitI}> partition {partitionI=}')
        
    def partitionSamplesN(self, samplesN):
        return self.wrap(f'partitionSamplesN {samplesN}')
        
    def partitionRice(self, rice):
        return self.wrap(f'partitionRice {rice}')
        
    def partitionSampleZ(self, sampleZ):
        return self.wrap(f'partitionSampleZ {sampleZ}')
        
    def partitionVal(self, index, val):
        return self.wrap(f'partitionVal {index=} {val=}')
        
    def foot(self, pos, bitI):
        return self.wrap(f'<{pos}.-{bitI}> foot')

#--------------------------------------------------------------------------------------------------------------------
# Parser
#--------------------------------------------------------------------------------------------------------------------

class Parser(util.ParserBase):
    def __init__(self, main):
        super().__init__(main, main.logcFlacFrame)

        self.state = aufiC.FlacFrameParseState()

        # c callbacks
        if main.parseDbg:
            self.frame = self.logc.frame
            self.head = self.logc.head
            self.headSampleI = self.logc.headSampleI
            self.blockZ = self.logc.blockZ
            self.sampleRate = self.logc.sampleRate
            self.channels = self.logc.channels
            self.sampleZ = self.logc.sampleZ
            self.sampleI = self.logc.sampleI
            self.subframe = self.logc.subframe
            self.subframeType = self.logc.subframeType
            self.subframeWasted = self.logc.subframeWasted
            self.subframeSampleZ = self.logc.subframeSampleZ
            self.subframeConstant = self.logc.subframeConstant
            self.subframeConstantVal = self.logc.subframeConstantVal
            self.subframeVerbatim = self.logc.subframeVerbatim
            self.subframeVerbatimVal = self.logc.subframeVerbatimVal
            self.subframeFixed = self.logc.subframeFixed
            self.subframeFixedWarmup = self.logc.subframeFixedWarmup
            self.subframeLpc = self.logc.subframeLpc
            self.subframeLpcWarmup = self.logc.subframeLpcWarmup
            self.subframeLpcQlpCoeffPrecision = self.logc.subframeLpcQlpCoeffPrecision
            self.subframeLpcQlpShift = self.logc.subframeLpcQlpShift
            self.subframeLpcCoeffVal = self.logc.subframeLpcCoeffVal
            self.residual = self.logc.residual
            self.residualType = self.logc.residualType
            self.residualRiceZ = self.logc.residualRiceZ
            self.residualPartitions = self.logc.residualPartitions
            self.partition = self.logc.partition
            self.partitionSamplesN = self.logc.partitionSamplesN
            self.partitionRice = self.logc.partitionRice
            self.partitionSampleZ = self.logc.partitionSampleZ
            self.partitionVal = self.logc.partitionVal
            self.foot = self.logc.foot
