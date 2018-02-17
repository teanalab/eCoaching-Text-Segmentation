package main;

import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;
import java.util.Vector;

public class Helper {
	public static boolean isNumeric(String s) {  
	    return s != null && s.matches("[-+]?\\d*\\.?\\d+");  
	} 

	public static String stemByPorterStemmer(String s) {
		Stemmer st = new Stemmer();
		StringBuilder sb = new StringBuilder();
	    String[] words = s.split("\\s+");
	    
	    for (int i = 0; i < words.length; i++){
			st.add(words[i]);
			sb.append(st.stem());
			sb.append(" ");
			st.clear();
	    }
	    return sb.toString();
	}
	
	public static Map<String, Integer> getDataDistribution(Vector<Entry> data){
		Map<String, Integer> dist = new HashMap<String, Integer>();
		for(Entry e: data){
			if(dist.containsKey(e.label)){
				dist.put(e.label, dist.get(e.label)+1);
			}
			else{
				dist.put(e.label, 1);
			}
		}		
		return dist;
	}
	
	public static void printMap(Map<String, Integer> data){
		TreeMap<String, Integer> tmap = new TreeMap<String, Integer>(data);
		for(String key: tmap.keySet()){
			System.out.println(key + "," + key);
		}		
	}
	
	public static boolean isSpecialChar(String data){
		if (data.contains("exclam")){
			return true;
		}
		else if (data.contains("fullstop")){
			return true;
		}
		else if (data.contains("questionmark")){
			return true;
		}
		else if (data.contains("comma")){
			return true;
		}
		else if (data.contains("hyphen")){
			return true;
		}
		else if (data.contains("clone")){
			return true;
		}
		else if (data.contains("semiclone")){
			return true;
		}
		else
			return false;
	}
	
	public static TreeMap<String, Integer> sortMapByValue(HashMap<String, Integer> map){
		Comparator<String> comparator = new ValueComparator<String, Integer>(map);
		TreeMap<String, Integer> result = new TreeMap<String, Integer>(comparator);
		result.putAll(map);
		return result;
	}
}

//a comparator using generic type
class ValueComparator<K, V extends Comparable<V>> implements Comparator<K>{
 
	HashMap<K, V> map = new HashMap<K, V>();
 
	public ValueComparator(HashMap<K, V> map){
		this.map.putAll(map);
	}
 
	public int compare(K s1, K s2) {
		return map.get(s1).compareTo(map.get(s2));	
	}
}
