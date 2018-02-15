package filemanager;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Map;
import java.util.TreeMap;

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
}
