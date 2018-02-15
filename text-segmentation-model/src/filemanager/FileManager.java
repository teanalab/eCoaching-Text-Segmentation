package filemanager;

public class FileManager {
	public static void preprocessRawData(String inputDir, String outputDir, String codeMapFile) {
		try {
			Writer.prepareDataAsCodeAndText(inputDir, outputDir, codeMapFile);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static void formatInstancesForLDA(String inputDataDirectory, String formatInputDataDirectory) {
		Writer.removeFilesFromDirectory(formatInputDataDirectory);
		Writer.createInputForLDAFormat(inputDataDirectory, formatInputDataDirectory);
	}

	public static void createGoldStandard(String inputDir, String outputFile) {
		
	}
	
	public static void createArffFileWithTextAndCode(
			String inputDataDirectory, String arffTextFile, String codeMapFile) {
		Writer.createArffFileWithTextAndCode(inputDataDirectory, arffTextFile, codeMapFile);
	}
}
