package filemanager;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import main.Entry;

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
	
	public static Map<String, String> readMap(String fileName, String delimiter) throws IOException {
  
	    BufferedReader br = new BufferedReader(new FileReader(fileName));
	    Map<String, String> mergeCodeMap = new HashMap<String, String>();  
	    
	    try {
	    		        
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
	        br.close();
	        System.out.print("Error in reading file: "+ fileName);
	        return mergeCodeMap;
	    }
	}
}
