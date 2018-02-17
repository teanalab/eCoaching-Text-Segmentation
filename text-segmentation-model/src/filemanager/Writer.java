package filemanager;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;

import main.Helper;

public class Writer {
	
	public static boolean removeDirectory(File directory) {
		if (directory == null)
			return false;
		if (!directory.exists())
			return true;
		if (!directory.isDirectory())
			return false;
		
		System.out.println("Remove directory " + directory.getName());

		String[] list = directory.list();

		if (list != null) {
			for (int i = 0; i < list.length; i++) {
				File entry = new File(directory, list[i]);

				if (entry.isDirectory())
				{
					if (!removeDirectory(entry))
						return false;
				}
				else
				{
					if (!entry.delete())
						return false;
				}
			}
		}		
		return directory.delete();
	}
	
	public static void removeFilesFromDirectory(String directory) {
		System.out.println("Remove files from " + directory);

		File rawDataFolder = new File(directory);
		File []rawFiles = rawDataFolder.listFiles();
		
		for (int i = 0; i < rawFiles.length; ++i) {		
			rawFiles[i].deleteOnExit();
		}
	}
		
	public static void prepareDataAsCodeAndText(String inputDir, String outputDir, String classMapFile) throws Exception {		
		PrintWriter classPw = new PrintWriter(classMapFile);	
		TreeMap<String, String> classMap = new TreeMap<String, String>();
		
		File rawDataFolder = new File(inputDir);
		File []rawFiles = rawDataFolder.listFiles();
		
		for (int i = 0; i < rawFiles.length; ++i) {			
			PrintWriter pw = new PrintWriter(outputDir + "/" + rawFiles[i].getName());			
			BufferedReader br = new BufferedReader(new FileReader(rawFiles[i]));
			String line, text, code;
			
			while ((line = br.readLine()) != null) {	
				if (line.trim().length() > 1) {
					if (line.indexOf("}", 0) == -1) {
						code = line.split("\\s+")[0].trim();
						text = line.substring(code.length()+1).trim().replace("'", " quotemark ").replace("\"", " quotemark ");
					}
					else {
						code = line.substring(0, line.indexOf("}", 0)).trim().replace("{", "").replace("}", "");
						text = line.substring(line.indexOf("}", 0)+2).trim().replace("'", " quotemark ").replace("\"", " quotemark ");
					}
				
					classMap.put(code, code);
					pw.println(code + "," + text.replace("\n", "").trim());
				}
			}						
			br.close();
			pw.close();
		}

		int classId = 1;
		for(String key:classMap.keySet()) {
			classPw.println((key + " " + classId));
			classId++;
		}
		classPw.close();
	}
	
	public static void createInputForLDAFormat(String inputDataDirectory, String formatInputDataDirectory) {
		
		File rawDataFolder = new File(inputDataDirectory);
		File []rawFiles = rawDataFolder.listFiles();
		
		for (int i = 0; i < rawFiles.length; ++i) {			
			try {
				BufferedReader br = new BufferedReader(new FileReader(rawFiles[i]));
				String line = "";
				
				while ((line = br.readLine()) != null) {
					if (!line.replace(",", "").trim().isEmpty()) {
						String[] arr = line.split(",");
						String code = arr[0].trim();
						String text = line.substring(arr[0].length()+1).trim();
						writeToFile(code, text, formatInputDataDirectory);
					}
				}
				br.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}	
			
	public static void writeToFile(String code, String text, String formatInputDataDirectory){		
		File codeFile = new File(formatInputDataDirectory + "/" + code + ".txt");
		FileWriter fw;
		
		try {
			fw = new FileWriter(codeFile, true);
			fw.write(text + "\n");
			fw.close();
		} catch (Exception e) {
			e.printStackTrace();
		}	
	}
	
	public static void createArffFileWithTextAndCode(
			String inputDataDirectory, String arffTextFile, String codeMapFile) {
		
		Map<String, String> mapCodeToInt = Reader.readMap(codeMapFile, " ");		
		BufferedWriter bw;
		
		File rawDataFolder = new File(inputDataDirectory);
		File []rawFiles = rawDataFolder.listFiles();
		
		try {
			bw = new BufferedWriter(new FileWriter(arffTextFile));
			bw.write("@relation textcodedata\n\n");
			bw.write("@attribute text string\n");
			bw.write("@attribute @@class@@ {1,10,11,12,13,14,15,16,17,18,19,2,20,21,22,23,24,25,26,27,28,29,3,30,31,32,33,34,35,36,37,38,39,4,40,41,42,43,5,6,7,8,9}\n\n");
			bw.write("@data\n");
			
			for (int i = 0; i < rawFiles.length; ++i) {							
				BufferedReader br = new BufferedReader(new FileReader(rawFiles[i]));
				String line = "";
				
				while ((line = br.readLine()) != null) {
					String[] arr = line.split(",");
					if (arr.length > 1) {
						if (!mapCodeToInt.containsKey(arr[0].trim()))
							System.out.println(line);
						String code = mapCodeToInt.get(arr[0].trim());
						String text = line.substring(arr[0].length()+1).toLowerCase()
								.replaceAll("[^\\x00-\\x7F]", "").replaceAll("\"", "").replaceAll("'", "");
						
						String processedText = Reader.getStemTextReplacedBySpecialChars(text);					
						bw.write("'" + processedText + "'," + code + "\n");
					}
				}
				br.close();				
			}
			bw.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}	
	
	public static void createGoldStandard(String arffTextFile, String  arffFileStr2WordVector, 
			String  goldStandardFile, String wordProbabilitiesFile, boolean withTopicDistribution) {
		// create map for topic distribution
		Map<Integer, Map<Integer, Map<String, Double>>> mapWordProbabilityForClass = Reader.getWordProbabilityForClass(wordProbabilitiesFile);
		
		// create word dictionary for the entire collections
		HashMap<String, Integer> mapWordDictionary = Reader.getWordDictionary(arffFileStr2WordVector);
		
		// create gold standard file
		BufferedReader br;
		FileWriter fw;
		
	    try {
	    	br = new BufferedReader(new FileReader(arffTextFile));
	    	fw = new FileWriter(goldStandardFile + "_" + withTopicDistribution + ".arff", false);
	    	
	    	// write the header of the arff file
	    	writeHeader(fw, mapWordProbabilityForClass, mapWordDictionary, withTopicDistribution);
	    	
	    	for (int i = 0; i < 6; i++)
	    		br.readLine();
	    	
	    	StringBuilder sb = new StringBuilder();
        	String prevWordStr = "";
        	String prevWordTopicStr = ""; 
        	String specialCharStr = "";
        	
	        String line = br.readLine();
	        int breakLabel = 0;
	        while (line != null) {
	        	String[] lineArr = line.split(",");
	        	String[] oneLineText = lineArr[0].replace("'", "").split("\\s+");
	        	double arrCurrWordTopicProb[] = new double[mapWordProbabilityForClass.get(1).size()];
	        	breakLabel = 1;
	        	
	        	// process each instance
	        	for (int i = 0; i < oneLineText.length; i++) {
	        		String currWordStr = "";
	        		int currWordIdx = 0;
		        	String currWordTopicStr = ""; 
		        
		        	if (Helper.isSpecialChar(oneLineText[i].trim())) {
		        		specialCharStr = "" + (mapWordDictionary.get(oneLineText[i])) + " 1";
		        		//System.out.println("I am here..");
		        		continue;
		        	}		        	
	        		else if (mapWordDictionary.containsKey(oneLineText[i].trim())) {
		        		currWordStr = "" + (mapWordDictionary.get(oneLineText[i]) + mapWordDictionary.size()) + " 1";
		        		currWordIdx = mapWordDictionary.get(oneLineText[i]);
		        		
		        		if (withTopicDistribution) {
		        			for (int j = 1; j <= arrCurrWordTopicProb.length; j++) {
		        				if (mapWordProbabilityForClass.get(Integer.parseInt(lineArr[1].trim())).get(j-1).containsKey(oneLineText[i].trim()))
			        				arrCurrWordTopicProb[j-1] = mapWordProbabilityForClass.
			        					get(Integer.parseInt(lineArr[1].trim())).get(j-1).get(oneLineText[i].trim()); 
			        			
		        				if (j > 1) {
				        			currWordTopicStr = currWordTopicStr + "," + (mapWordDictionary.size()*2+ arrCurrWordTopicProb.length + j) + " " + arrCurrWordTopicProb[j-1];
				        		}
				        		else {
				        			currWordTopicStr = (mapWordDictionary.size()*2 + arrCurrWordTopicProb.length + j) + " " + arrCurrWordTopicProb[j-1];
				        		}
				        	}
			        	}
		        	}
		        	
	        		sb.append("{0 " + breakLabel);
	        		if (!prevWordStr.isEmpty()) {
	        			sb.append(",");
	        			sb.append(prevWordStr);
	        			//sb.append(mapWordDictionary.containsValue(Integer.parseInt(prevWordStr)-1));
	        		}
		        	
	        		sb.append(",");
	        		//sb.append(oneLineText[i]);
	        		sb.append(currWordStr);
		        	
		        	if (!specialCharStr.isEmpty()) {
		        		sb.append(",");
		        		sb.append(specialCharStr);
		        	}
		        	if (withTopicDistribution) {
		        		if (!prevWordTopicStr.isEmpty()) {
		        			sb.append(",");
			        		sb.append(prevWordTopicStr);
		        		}
		        		
		        		if (!currWordTopicStr.isEmpty()) {
			        		sb.append(",");
		        			sb.append(currWordTopicStr);
		        		}
		        	}
		        	
		        	sb.append("}");
		        	fw.write(sb.toString() + "\n");
		        	sb.setLength(0);
		        	//System.out.println(oneLineText[i-1]+ " " + oneLineText[i]);
		        	//System.out.println(lineArr[1]);

		        	breakLabel = 0;
		        	specialCharStr = "";
		        	prevWordStr = "" + currWordIdx + " 1";
		        	//prevWordStr = "" + currWordIdx;
		        	prevWordTopicStr = "";
		        	for (int j = 1; j < arrCurrWordTopicProb.length; j++) {
		        		if (j > 1) {
		        			prevWordTopicStr = prevWordTopicStr + "," + (mapWordDictionary.size()*2+j) + " " + arrCurrWordTopicProb[j-1];
		        		}
		        		else
		        			prevWordTopicStr = (mapWordDictionary.size()*2+j) + " " + arrCurrWordTopicProb[j-1];
		        	}
	        	}
	        	
	        	line = br.readLine();	            
	        }	        
	        br.close();
	        fw.close();
	        
	    } catch(Exception e) {
	    	e.printStackTrace();
	    }
		
	}
	
	public static void writeHeader(FileWriter fw, Map<Integer, Map<Integer, Map<String, Double>>> mapWordProbabilityForClass, 
			HashMap<String, Integer> mapWordDict, boolean withTopicDistribution) {
		
		TreeMap<String, Integer> mapWordDictionary = Helper.sortMapByValue(mapWordDict);
		try {
			fw.write("@relation goldstandard\n\n");
			fw.write("@attribute @@class@@ {");
			for (int i = 1; i < mapWordProbabilityForClass.size(); i++) {
				fw.write(i + ",");
			}
			fw.write(mapWordProbabilityForClass.size() + "}\n");
			
			for (String key : mapWordDictionary.keySet()) {
				fw.write("@attribute " + key + " numeric\n");
			}
			
			for (String key : mapWordDictionary.keySet()) {
				fw.write("@attribute " + key + " numeric\n");
			}
			
			if (withTopicDistribution) {
				for (int i = 1; i <= mapWordProbabilityForClass.get(41).size(); i++) {
					fw.write("@attribute prevtopicprob" + i + " numeric\n");
				}
				
				for (int i = 1; i <= mapWordProbabilityForClass.get(41).size(); i++) {
					fw.write("@attribute currtopicprob" + i + " numeric\n");
				}
			}
			fw.write("\n@data\n");
			
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
