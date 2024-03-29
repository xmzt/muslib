from pylib0_xmzt.denumlib import Denum

class AufiEException(Exception): Exception

#------------------------------------------------------------------------------------------------------------------------
# AufiE
#------------------------------------------------------------------------------------------------------------------------

def AufiEMeta(name, bases, namespace, **kwds):
    cls = type(name, bases, namespace).init()
    cls.codeCInit('int', 'AufiE')
    for i,iden in enumerate((
            'Cb',
            'AudNimp',
            'ChunkInvalid',
            'ChunksFull',
            'FolderNameInvalid',
            'OtherException',
            'TracknosNope',
            'VerifyChunkAud',
            'VerifyChunkFin',
            'VerifyChunkFinMissing',
            'VerifyChunkNaud',
            'VerifyChunkType',
            'VerifyHead',
            'VerifyMagic',
            'VerifySrcName',
            
            'Apev2',
            'Apev2HeadIncomplete',
            'Apev2InFooter',
            'Apev2Incomplete',
            'Apev2ItemHeadIncomplete',
            'Apev2ItemKeyIncomplete',
            'Apev2ItemValIncomplete',
            'Apev2MagicInvalid',
            
            'FlacFrameChunkNNe1',
            'FlacMagicAudHeadTooSmall',
            'FlacOtherChunkN',
            'FlacPaddingMisfit',
            'FlacStreaminfosNNe1',

            'FlacFrame',
            'FlacFrameBlockZInvalid',
            'FlacFrameBlockZVariableInconsistent',
            'FlacFrameChannelAssignmentInvalid',
            'FlacFrameFootCrcMismatch',
            'FlacFrameFootIncomplete',
            'FlacFrameHeadCrcMismatch',
            'FlacFrameHeadIncomplete',
            'FlacFramePartitionIncomplete',
            'FlacFrameResidualIncomplete',
            'FlacFrameResidualTypeInvalid',
            'FlacFrameSampleIInconsistent',
            'FlacFrameSampleIInvalid',
            'FlacFrameSampleRateInvalid',
            'FlacFrameSampleZInvalid',
            'FlacFrameSubframeHeadIncomplete',
            'FlacFrameSubframeIncomplete',
            'FlacFrameSubframeLpcQlpCoeffPrecisionInvalid',
            'FlacFrameSubframeLpcQlpShiftInvalid',
            'FlacFrameSubframeSampleZInvalid',
            'FlacFrameSubframeSyncInvalid',
            'FlacFrameSubframeTypeInvalid',
            'FlacFrameSyncIncomplete',
            'FlacFrameSyncInvalid',

            'FlacMeta',
            'FlacMetaAudHeadTooSmall',
            'FlacMetaIncomplete',
            'FlacMetaSeekpointIncomplete',
            'FlacMetaStreaminfoLenInvalid',
            'FlacMetaTypeInvalid',
            'FlacMetaVorbisCommentHeadIncomplete',
            'FlacMetaVorbisCommentItemIncomplete',
            'FlacMetaAufiFlacMeta',

            'Id3v1',
            'Id3v1Incomplete',
            'Id3v1EnhancedIncomplete',
            'Id3v1MagicInvalid',

            'Id3v2',
            'Id3v2ExtCrcIncomplete',
            'Id3v2ExtFlagBytesNInvalid',
            'Id3v2ExtIncomplete',
            'Id3v2ExtRestrictionsBytesNInvalid',
            'Id3v2ExtRestrictionsIncomplete',
            'Id3v2FrameCompressionNimp',
            'Id3v2FrameEncryptionNimp',
            'Id3v2FrameIncomplete',
            'Id3v2FrameUnsynchronizationNimp',
            'Id3v2FramePayloadEmpty',
            'Id3v2FramePayloadNenc',
            'Id3v2FramePayloadNterm8',
            'Id3v2FramePayloadNterm16',
            'Id3v2FramePayloadNsep8',
            'Id3v2FramePayloadNsep16',
            'Id3v2FrameUnimplemented',
            'Id3v2FrameUnknown',
            'Id3v2HeadCompressionNimp',
            'Id3v2HeadIncomplete',
            'Id3v2HeadUnsynchronizationNimp',
            'Id3v2HeadVersionInvalid',
            'Id3v2Incomplete',
            'Id3v2MagicInvalid',
            'Id3v2PaddingInvalid',

            'Lyrics3v2',
            'Lyrics3v2Incomplete',
            'Lyrics3v2MagicInvalid',

            'Mp3FrameChunkNNe1',
            'Mp3FrequencyInconsistent',
            'Mp3OtherChunkN',

            'Mp4BoxHeadIncomplete',
            'Mp4BoxIncomplete',
            'Mp4BoxNimp',
            'Mp4BoxZInvalid',
            'Mp4ContainerStackFull',
            'Mp4OtherChunkN',
            
            'Mpeg1AudFrame',
            'Mpeg1AudFrameIncomplete',
            'Mpeg1AudFrameLen0',
            'Mpeg1AudFrameSyncIncomplete',
            'Mpeg1AudFrameSyncInvalid',
            
            'SysAlloc',
            'SysMmapAud',
            'SysMmapNaud',
            'SysMmapSrc',
            'SysOpenAud',
            'SysOpenNaud',
            'SysOpenSrc',
            'SysStatAud',
            'SysStatNaud',
            'SysStatSrc',
            'SysTruncateAud',
            'SysTruncateAud1',
            'SysTruncateNaud',
            'SysTruncateNaud1',
            )):
        cls.add(iden, -1-i, -1)

    cls._SysBeg = cls.firstIdenPre('Sys')
    return cls
        
class AufiE(Denum, metaclass=AufiEMeta):
    @classmethod
    def isSys(cls, x):
        return cls._SysBeg >= x

    @classmethod
    def ignoreDFromPres(cls, *pres):
        ignoreD = {}
        for item in cls.ItemV:
            ignore = 0
            for pre in pres: 
                if item.iden.startswith(pre):
                    ignore = 1
                    break
            ignoreD[item.val] = ignore
        return ignoreD
