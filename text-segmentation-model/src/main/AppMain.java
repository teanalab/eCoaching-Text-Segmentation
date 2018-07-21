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
		String formatInputDataDirectory = "../../../data-source/ecoaching-text-segmentation-data/input-data-format";
		String codeMapFile = "../config/stem/code-map-ecoaching.cfg";
		String arffTextFile = "../../../data-source/ecoaching-text-segmentation-data/data/text_stem.arff";
		String arffFileStr2WordVector = "../../../data-source/ecoaching-text-segmentation-data/data/stem_Str2WordVector.arff";
		String goldStandardFile = "../../../data-source/ecoaching-text-segmentation-data/data/gold";
		
		// use the correct topic models 
		String wordProbabilitiesFile = "../../../data-source/ecoaching-text-segmentation-data/topic/lta/stem/cls-lms.txt"; 
		//String wordProbabilitiesFile = "../../../data-source/ecoaching-text-segmentation-data/topic/mg-lda/stem/cls-tops.txt"; 
		//String wordProbabilitiesFile = "../../../data-source/ecoaching-text-segmentation-data/topic/lda/stem/cls-tops.txt"; 

		// pre-process data to run LDA model for getting the features values
		//FileManager.preprocessRawData(rawDataDirectory, inputDataDirectory, codeMapFile);
		
		// create one file for all documents after removing non-ascii characters
		//FileManager.createArffFileWithTextAndCode(inputDataDirectory, arffTextFile, codeMapFile);
		
		// prepare data for Formatter of LDA model, [all instances in one file named with class]
		//FileManager.formatInstancesForLDA(inputDataDirectory, formatInputDataDirectory);

		// run C++ code from parent directory for LDA model; generate topic distribution
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		
		// by using terminal run stringToVector from data directory; generate arffFileStr2WordVector file
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		// use LDA output for creating the arff file with all feature sets
		// 1. probability that a word can belongs to a topic
		// 2. same probability array for previous and next words
		// 3. one hot encode vector for previous and next word
		// 4. include vector for special character such as !,. etc.
		// 5. add label as break (1) or not (0)
		
		FileManager.createGoldStandard(arffTextFile, arffFileStr2WordVector, goldStandardFile,
				wordProbabilitiesFile, true, "mg-lda");

		// run model with arff file to evaluate the model performance
		//SegmentationModel model = new SegmentationModel();
		//model.runSVM(goldStandardFile);

		// finish execution
		System.out.println("Finished execution at: " + LocalDateTime.now());
	}
}
