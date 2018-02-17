package filemanager;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import main.Entry;
import main.Helper;

public class Reader {
    
	public static Vector<Entry> readProcessedData(String inputFolder, String delimiter) throws Exception {
		
		Vector<Entry> out = new Vector<Entry>();
		
		try {	
			File rawDataFolder = new File(inputFolder);
			File []rawFiles = rawDataFolder.listFiles();
			
			for (int i = 0; i < rawFiles.length; ++i) {
				BufferedReader br = new BufferedReader(new FileReader(rawFiles[i]));			
				String line;
				while ((line = br.readLine()) != null) {
					String[] lineData = line.split(delimiter);
					String label = lineData[0].trim();
					Entry entry = new Entry();
					entry.label = label;
					entry.text = line.substring(lineData[0].trim().length()).trim();
					out.add(entry);
				}
				
				br.close();	
			}
		}
		catch(Exception e){
			System.out.print("Error in reading file: "+ inputFolder);
		}
		
		return out;
	}
	
	public static Map<String, Entry> readCodeBook(String fileName) throws IOException {
		
		Map<String, Entry> codeBook = new HashMap<String, Entry>();  
	    BufferedReader br = new BufferedReader(new FileReader(fileName));
	    
	    try {	    	
	        
	        String line = br.readLine();

	        while (line != null) {
	        	String[] oneLine = line.split("\t");
	        	Entry code = new Entry();
	            code.labelCode = oneLine[0].trim();
	            code.label = oneLine[1].trim();
	            code.text = oneLine[2].trim();
	            codeBook.put(code.labelCode.trim(), code);
	            line = br.readLine();	            
	        }
	        
	        br.close();
	        return codeBook;
	        
	    } catch(Exception e) {
	        br.close();
	        System.out.print("Error in reading file: "+ fileName);
	        return codeBook;
	    }
	}
	
	public static Map<String, String> readMap(String fileName, String delimiter) {
  
	    BufferedReader br;
	    Map<String, String> mergeCodeMap = new HashMap<String, String>();  
	    
	    try {
	    	br = new BufferedReader(new FileReader(fileName));        
	        String line = br.readLine();

	        while (line != null) {
	        	String[] oneLine = line.split(delimiter);
	        	if(oneLine.length > 1)
	        		mergeCodeMap.put(oneLine[0], oneLine[1]);
	        	else
	        		mergeCodeMap.put(oneLine[0], oneLine[0]);
	            line = br.readLine();	            
	        }
	        
	        br.close();
	        return mergeCodeMap;
	        
	    } catch(Exception e) {
	        System.out.print("Error in reading file: "+ fileName);
	        return mergeCodeMap;
	    }
	}
	
	public static String getStemTextReplacedBySpecialChars(String text) {
		String newText = text.replace("?", " questionmark ");
		newText = newText.replace(".", " fullstop ");
		newText = newText.replace(",", " comma ");
		newText = newText.replace(";", " semiclone ");
		newText = newText.replace(":", " clone ");
		newText = newText.replace("!", " exclamation ");
		newText = newText.replace("-", " hyphen ");
		// also consider quotemark; already handled before
		newText = newText.replaceAll("[&\\*%/\\+]", "");
		newText = newText.replaceAll("\\d", "");

		String pText = Helper.stemByPorterStemmer(newText);
		return pText.trim();
	}
	
	public static Map<Integer, Map<Integer, Map<String, Double>>> getWordProbabilityForClass(String wordProbabilitiesFile) {
		
		Map<Integer, Map<Integer, Map<String, Double>>> mapWordProbabilityForClass = new HashMap<Integer, Map<Integer, Map<String, Double>>>();
		Map<Integer, Map<String, Double>> mapTopicWordProb = new HashMap<Integer, Map<String, Double>>();
		Map<String, Double> mapWordProb = new HashMap<String, Double>();
		
		BufferedReader br;
		int classLabel = 0, topicLabel = 0;
	    
	    try {
	    	br = new BufferedReader(new FileReader(wordProbabilitiesFile));        
	        String line = br.readLine();

	        while (line != null) {	        		        	
	        	if (line.contains("<")) {
	        		classLabel = Integer.parseInt(line.replace("<", "").replace(">", ""));
	        		mapTopicWordProb = new HashMap<Integer, Map<String, Double>>();
	        		mapWordProbabilityForClass.put(classLabel, mapTopicWordProb);
	        		line = br.readLine();
	        		continue;
	        	}
	        	
	        	String[] oneLine = line.split("\\s+");
	        	if(oneLine.length > 1) {
	        		mapTopicWordProb.get(topicLabel).put(oneLine[0].trim(), Double.valueOf(oneLine[oneLine.length-1].trim()));
	        		mapWordProbabilityForClass.put(classLabel, mapTopicWordProb);
	        	}
	        	else {
	        		topicLabel = Integer.parseInt(line.trim());
	        		mapWordProb = new HashMap<String, Double>();
	        		mapTopicWordProb.put(topicLabel, mapWordProb);
	        	}
	            line = br.readLine();	            
	        }	        
	        br.close();
	        
	    } catch(Exception e) {
	        e.printStackTrace();
	    }
				
		return mapWordProbabilityForClass;
	}
	
	public static HashMap<String, Integer> getWordDictionary(String arffFileStr2WordVector) {
		
		HashMap<String, Integer> mapWords = new HashMap<String, Integer>();
		
		BufferedReader br;
	    
	    try {
	    	br = new BufferedReader(new FileReader(arffFileStr2WordVector));        
	        String line = br.readLine();
	        int id = 1;

	        while (line != null) {
	        	if (line.contains("@attribute") && !line.contains("@@class@@")) {
	        		String[] oneLine = line.split("\\s+");
	        		mapWords.put(oneLine[1].trim(), id++);
	        	}
	            line = br.readLine();	            
	        }	        
	        br.close();
	        
	    } catch(Exception e) {
	    	e.printStackTrace();
	    }
		
		return mapWords;
	}
	
}
