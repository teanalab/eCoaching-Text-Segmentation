package main;

import java.time.LocalDateTime;

import filemanager.FileManager;
import model.SegmentationModel;

public class AppMain {

	public static void main(String[] args) {
		// start execution
		System.out.println("Start execution at: " + LocalDateTime.now());
				
		// configuration:
		String rawDataDirectory = "../../../data-source/ecoaching-text-segmentation-data/input-data-raw";
		String inputDataDirectory = "../../../data-source/ecoaching-text-segmentation-data/input-data";
		String goldStandardFile = "data/gold.txt";
		String arffFile = "data/stem.arff";
		
		// pre-process data to run LDA model for getting the features values
		FileManager.preprocessRawData(rawDataDirectory, inputDataDirectory);
		
		
		// create gold standard from pre-processed data
		FileManager.createGoldStandard(inputDataDirectory, goldStandardFile);
		
		// use LDA output for creating the arff file with all feature sets
		// 1. probability that a word can belongs to a topic
		// 2. same probability array for previous and next words
		// 3. one hot encode vector for present, previous and next word
		// 4. include vector for special character such as !,. etc.
		// 5. add label as break (1) or not (0)
		FileManager.createArffFile(goldStandardFile, arffFile);
		
		// run model with arff file to evaluate the model performance
		SegmentationModel model = new SegmentationModel();
		model.runSVM(arffFile);
		
		// finish execution
		System.out.println("Finished execution at: " + LocalDateTime.now());
	}
}
