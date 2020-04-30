SM_TARGETBASE=$(AURORA_SRC_DIR)/store
SM_PROJBASE=$(AURORA_SRC_DIR)/store

%.o: %.C
	$(COMPILE.c) $<

#################################################################################
# The object files, from each module, that ultimately appear in the Storage
# Manager's library.
#################################################################################

OBJ_STATS=\
	PeriodStat.o \
	SmRuntimeStats.o \
	SmRuntimeStatsSnapshot.o

OBJ_IOLIB=\
        PageAddr.o \
        PageFile.o \
        PageDir.o \
        RwOpRunner.o \
        PdGrowthRunner.o \
        IOCB.o \
        ApdMasterRunner.o \
        AsynchPageDir.o

OBJ_BUFFERCACHE=\
        AvailablePageSet.o \
        FramelessPagesMap.o \
        PagesWithFramesMap.o \
        BufferCacheControlBlock.o \
        SortedPageMap.o \
        BcIoDispatchRunnable.o \
        BufferCache.o


OBJ_SM=\
	PageSlotAddr.o \
	IntraPageSlotRange.o \
	QviewTailPtrMap.o \
	SimplePageRing.o \
	QTopology.o \
	QMetadata.o \
	SuperBoxPlanStep.o \
	SuperBoxPacer.o \
	StorageMgr.o \
	PagePriorityTracker.o \
	CxnPtInfo.o \
	TimevalAvgTracker.o \
	PagePool.o \
	TupleQueue.o

OBJ_NEWSM_ONLY=\
	PagePool.o \
	TupleQueue.o

OBJ_NEWSM_ONLY_FULLPATH=\
	$(SM_PROJBASE)/obj/PagePool.o \
	$(SM_PROJBASE)/obj/TupleQueue.o