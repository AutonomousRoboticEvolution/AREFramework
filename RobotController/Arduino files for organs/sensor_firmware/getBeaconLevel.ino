/*
* GETBEACONLEVEL
* - Divides the samples in the sampling window into two groups, low and high
* - Each group is averaged to filter out noise and give a more reliable low and high value
* - The difference between these two averages, high - low, is then the measured beacon level
* - This should still work under different levels of background IR radiation, just the numbers will be different
*/
int getBeaconLevel(FIFO* samplingWindow, int windowSize, int wrappingBitmask) {

  
  int maximumValueInSample = 0;
  int minimumValueInSample = 1023;
  int currentSample;
  //Find min and max values
  for (int i=0; i<windowSize; ++i) {
    //Read next sample from the window
    currentSample = fifoReadOffset(samplingWindow, i, wrappingBitmask);

    //Update max or min value if new one found
    if (currentSample > maximumValueInSample) {
      maximumValueInSample = currentSample;
    }
    if (currentSample < minimumValueInSample) {
      minimumValueInSample = currentSample;
    }
  }

  //Calculate two averages, one for low, one for high
  //Achieved by keeping a running total rather than two actual data structures for the groups
  long lowTotal = 0; 
  long highTotal = 0;
  int numLows = 0;
  int numHighs = 0;
  for (int i=0; i<windowSize; ++i) {
    //Read next sample from the window
    currentSample = fifoReadOffset(samplingWindow, i, wrappingBitmask);

    //Add to lows group if closer to min than to max, otherwise add to highs group
    if((currentSample - minimumValueInSample) < (maximumValueInSample - currentSample)) {
      lowTotal += currentSample;
      numLows++;
    } else {
      highTotal += currentSample;
      numHighs++;
    }
  }

  //Calculate the averaged low value and the averaged high value
  //Note this is integer division so the fractional part will be discarded
  int averageLow = lowTotal/numLows;
  int averageHigh = highTotal/numHighs;

  //DEBUG:
  #ifdef SERIAL_DEBUG_FILTERING_RAW_DATA
    Serial.println();
    Serial.println("Window contents:");
    for(int i=0; i<windowSize; ++i) {
      Serial.println(fifoReadOffset(samplingWindow, i, wrappingBitmask));
    }
    Serial.println("LowTotal    NumLows    HighTotal    NumHighs    AverageLow    AverageHigh");
    Serial.print(lowTotal);
    Serial.print("        ");
    Serial.print(numLows);
    Serial.print("            ");
    Serial.print(highTotal);
    Serial.print("          ");
    Serial.print(numHighs);
    Serial.print("        ");
    Serial.print(averageLow);
    Serial.print("           ");
    Serial.println(averageHigh);
  #endif

  //Calculate the difference; this is the beacon level reading
  int differentialReading = averageHigh - averageLow;

  //Map this down to a 1-byte range
  int mappedReading = map(differentialReading, 0, IR_MAX_DIFFERENTIAL_READING, 0, 255);
  //If reading exceeds max, map() function will output something in excess of 255, so constrain this
  if (mappedReading > 255) {
    mappedReading = 255;
  }

  return mappedReading;
}


//FIFO functions

/*********************************************************************************
FIFOWRITE
-Fast function for FIFO buffering
-Writes the given value into the next buffer location
-Based on code from https://stackoverflow.com/questions/9718116/improving-c-circular-buffer-efficiency
-Buffer size must be a power of two for the bitmasking method of index wrapping to work
***********************************************************************************/
void fifoWrite(FIFO* fifo, int value, int wrappingBitmask)
{
  fifo->buf[(fifo->index++) & wrappingBitmask] = value;
}

/*********************************************************************************
FIFOREADOFFSET
-Fast function for reading out of either end of a FIFO buffer
-Reads at an offset from the last value written
-An indexOffset of -1 will return the last value written, -2 the value before that, etc...
-An indexOffset of 0 will return the oldest value, +1 the next oldest, etc...
***********************************************************************************/
int fifoReadOffset(FIFO* fifo, int indexOffset, int wrappingBitmask)
{
  return (fifo->buf[(fifo->index + indexOffset) & wrappingBitmask]);
}
