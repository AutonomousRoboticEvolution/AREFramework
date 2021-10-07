#define MAX 1
#define MIN 0
#define HI_IGNORE_VALUE 4095 //if this value appears in sample array, ignore it. Anything greater than 1023 should never happen for analog readings.
#define LO_IGNORE_VALUE -1 //if this value appears in sample array, ignore it. 0 should never happen for real IR readings.
#define IR_PRUNING_FACTOR 16 //Remove this many values from both groups after splitting beacon readings into high/low

/*
* GETBEACONLEVEL
* - Divides the samples in the sampling window into two groups, low and high
* - Each group is averaged to filter out noise and give a more reliable low and high value
* - The difference between these two averages, high - low, is then the measured beacon level
* - This should still work under different levels of background IR radiation, just the numbers will be different
*/
int getBeaconLevel(FIFO* samplingWindow, int windowSize, int wrappingBitmask) {

  
  int maximumValueInSample = 1;
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


/*
* GETBEACONLEVELALT
* - Divides the samples in the sampling window into two groups, low and high
* - Each group is averaged to filter out noise and give a more reliable low and high value
* - The difference between these two averages, high - low, is then the measured beacon level
* - This should still work under different levels of background IR radiation, just the numbers will be different
*/
int getBeaconLevelAlt(FIFO* samplingWindow, int windowSize, int wrappingBitmask) {

  //Create two new buffers
  int lowGroup[windowSize] = {0};
  int highGroup[windowSize] = {0};
  int lowIndex=0, highIndex=0;
  int currentSample;
  long runningTotal = 0;

  //Initialise buffer contents to their 'ignore' states by dafault
  for (int i=0; i<windowSize; ++i) {
    lowGroup[i] = LO_IGNORE_VALUE;
    highGroup[i] = HI_IGNORE_VALUE;
  }

  //Calculate the mean value of the whole window
  for (int i=0; i<windowSize; ++i) {
    //Sum all the values
    runningTotal += fifoReadOffset(samplingWindow, i, wrappingBitmask);
  }
  int meanVal = (float)runningTotal/windowSize; //Calculate mean

  //Divide into two groups either side of the mean
  for (int i=0; i<windowSize; ++i) {
    currentSample = fifoReadOffset(samplingWindow, i, wrappingBitmask);
    if (currentSample <= meanVal) {
      lowGroup[lowIndex++] = currentSample;
    } else {
      highGroup[highIndex++] = currentSample;
    }
  }

//  Serial.println("Low index  High index (start)");
//  Serial.print(lowIndex);
//  Serial.print("         ");
//  Serial.println(highIndex);

  int indexOfValueToMove;
  //Equalise the group sizes
  //If more elements in low group, move largest values to high group
  if (lowIndex > highIndex) {
    while (lowIndex > highIndex) { 
      indexOfValueToMove = findIndexOfMaxOrMin(lowGroup, windowSize, MAX); //Find the biggest value of low group
      highGroup[highIndex++] = lowGroup[indexOfValueToMove]; //Copy it to the high group
      lowGroup[indexOfValueToMove] = LO_IGNORE_VALUE;
      lowIndex -= 1; //Low group is now one element smaller
    }  
  }
  //If more elements in high group, move smallest values to low group
  if (lowIndex < highIndex) {
    while (lowIndex < highIndex) { 
      indexOfValueToMove = findIndexOfMaxOrMin(highGroup, windowSize, MIN); //Find the smallest value of high group
      lowGroup[lowIndex++] = highGroup[indexOfValueToMove]; //Copy it to the low group
      highGroup[indexOfValueToMove] = HI_IGNORE_VALUE;
      highIndex -= 1; //High group is now one element smaller
    }  
  }

//  Serial.println("Low index  High index (after shifting)");
//  Serial.print(lowIndex);
//  Serial.print("         ");
//  Serial.println(highIndex);

//  //TODO remove or encapsulate
//  Serial.println("Low group      High Group");
//  for(int i=0; i<windowSize; ++i) {
//    if(lowGroup[i] != LO_IGNORE_VALUE) Serial.print(lowGroup[i]); else Serial.print(" - ");
//    Serial.print("                     ");
//    if(highGroup[i] != HI_IGNORE_VALUE) Serial.println(highGroup[i]); else Serial.print(" - ");
//  }

  //Prune the n most outlying values from both groups
  int indexOfValueToPrune;
  for (int j=0; j<IR_PRUNING_FACTOR; ++j) {
    //Prune MIN values from low group
    indexOfValueToPrune = findIndexOfMaxOrMin(lowGroup, windowSize, MIN);
//    Serial.print("Pruning from lowGroup: ");
//    Serial.println(lowGroup[indexOfValueToPrune]);
    lowGroup[indexOfValueToPrune] = LO_IGNORE_VALUE;
    //Prune MAX values from high group
    indexOfValueToPrune = findIndexOfMaxOrMin(highGroup, windowSize, MAX);
//    Serial.print("Pruning from highGroup: ");
//    Serial.println(highGroup[indexOfValueToPrune]);
    highGroup[indexOfValueToPrune] = HI_IGNORE_VALUE;
  }

//  //TODO remove or encapsulate
//  Serial.println("Low group      High Group (PRUNED)");
//  for(int i=0; i<lowIndex; ++i) {
//    Serial.print(lowGroup[i]);
//    Serial.print("                     ");
//    Serial.println(highGroup[i]);
//  }

  //Calculate two averages, one for low, one for high
  long lowTotal = 0; 
  long highTotal = 0;
  int numLows = 0;
  int numHighs = 0;

  //Total the lows
  for (int i=0; i<=windowSize; ++i) {
    if (lowGroup[i] != LO_IGNORE_VALUE) {
      lowTotal += lowGroup[i];
      numLows++;
    }
  }

  //Total the highs
  for (int i=0; i<=windowSize; ++i) {
    if (highGroup[i] != HI_IGNORE_VALUE) {
      highTotal += highGroup[i];
      numHighs++;
    }
  }

  //Calculate the averaged low value and the averaged high value
  //Note this is integer division so the fractional part will be discarded
  int averageLow = lowTotal/numLows;
  int averageHigh = highTotal/numHighs;

//  Serial.print("averageLow: ");
//  Serial.println(averageLow);
//  Serial.print("averageHigh: ");
//  Serial.println(averageHigh);

  //Calculate the difference; this is the beacon level reading
  int differentialReading = averageHigh - averageLow;

  Serial.println(differentialReading);

  //Map this down to a 1-byte range
  int mappedReading = map(differentialReading, 0, IR_MAX_DIFFERENTIAL_READING, 0, 255);
  //If reading exceeds max, map() function will output something in excess of 255, so constrain this
  if (mappedReading > 255) {
    mappedReading = 255;
  }

  return mappedReading;
}

/*
  findIndexOfMaxOrMin
  @brief Given an array, find the index of the largest or smallest value
  @param intArray Array pointer to an array of int
  @param arraySize Size of the array
  @param maxNotMin TRUE if desired value is MAX, FALSE if desired value is MIN
  @return Index of the max or min value within the array
*/
int findIndexOfMaxOrMin (int* intArray, int arraySize, bool maxNotMin) {
  int maximumValueInSample = 0;
  int minimumValueInSample = 1023;
  int currentSample;
  int indexOfMinMaxValue = 0;

  //Find min and max values
  for (int i=0; i<arraySize; ++i) {
    //Read next sample from the window
    currentSample = intArray[i];

    //Update max or min value if new one found, and keep track of its index in the array
    if (maxNotMin) {
      if (currentSample > maximumValueInSample && currentSample != HI_IGNORE_VALUE) {
        maximumValueInSample = currentSample;
        indexOfMinMaxValue = i;
      }
    } else {
      if (currentSample < minimumValueInSample && currentSample != LO_IGNORE_VALUE) {
        minimumValueInSample = currentSample;
        indexOfMinMaxValue = i;
      }
    }    
  }

  //Return the index found
  return indexOfMinMaxValue;
}
