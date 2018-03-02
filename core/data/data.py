def batches(myList, batchSize=1):
	lenght = len(myList)
	for ndx in range(0, lenght, batchSize):
		yield myList[ndx:min(ndx + batchSize, lenght)]