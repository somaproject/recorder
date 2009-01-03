#ifndef SINKSTATS_H
#define SINKSTATS_H



namespace soma {
  namespace recorder {
    
    
    struct SinkStats {
      unsigned int pktCount; 
      unsigned int latestSeq;
      unsigned int dupeCount; 
      unsigned int pendingCount; 
      unsigned int missingPacketCount;
      unsigned int reTxRxCount; 
      unsigned int outOfOrderCount; 
    }; 
  }
}

#endif // SINKSTATS_H
