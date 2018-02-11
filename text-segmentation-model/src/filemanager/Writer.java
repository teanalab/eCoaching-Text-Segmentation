package filemanager;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
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
				if (line.indexOf("}", 0) == -1) {
					code = line.split(" ")[0].trim();
					text = line.substring(code.length()).trim().replace(",", "");
				}
				else {
					code = line.substring(0, line.indexOf("}", 0)).trim().replace("{", "").replace("}", "");
					text = line.substring(line.indexOf("}", 0)+1).trim().replace(",", "");
				}
				
				classMap.put(code, code);
				pw.println(code + "," + text.replace("\n", "").trim());
			}						
			br.close();
			pw.close();
		}
		boolean firstLine = true;
		int classId = 1;
		for(String key:classMap.keySet()) {
			if(firstLine) {
				firstLine = false;
			}
			else {
				classPw.println((key + " " + classId));
				classId++;
			}
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
						String text = line.substring(arr[0].length()+1).replace(",", "");
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
}
