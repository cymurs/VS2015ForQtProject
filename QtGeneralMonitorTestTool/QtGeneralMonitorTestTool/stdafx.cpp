#include "stdafx.h"

const char *RecordFile = "record.csv";
const char SplitChar = '\n';
const int RecordInterval = 3600 * 1000;
const int interval = 100; // »æÍ¼¼ä¸ô(µ¥Î»: ms)
const int MaxBytesEveryTime = 1024;
const int Buffer4Read = 128 * 1024;
