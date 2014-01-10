package it.uniroma1.lcl.babelnet.utils;


import it.uniroma1.lcl.babelnet.BabelNet;
import it.uniroma1.lcl.babelnet.BabelNetConfiguration;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashSet;

/**
 * List of censored or bad images
 *  
 * @author vannella
 *
 */
public class BadImageList
{ 
	static private BadImageList instance;
	
	/**
	 * List of censored or bad images
	 */
	private HashSet<String> badImageList = new HashSet<String>();

	private BadImageList() throws IOException
	{
		BufferedReader reader = new BufferedReader(new FileReader(		
				BabelNetConfiguration.getInstance().getBadImageListPath()));
		
		while (reader.ready())
		{
			String image = reader.readLine();
			badImageList.add(image.toLowerCase().replace(" ", "_"));
		}
	}
	
	/**
	 * Used to access {@link BabelNet}
	 * 
	 * @return an instance of {@link BabelNet}
	 */
	public static synchronized BadImageList getInstance()
	{
		try
		{
			if (instance == null) instance = new BadImageList();
			return instance;
		}
		catch (IOException e)
		{
			throw new RuntimeException("Could not init BabelNetListImageBad: " + e.getMessage());
		}
	}
	
	public boolean isBadImage(String titleImage)
	{
		return badImageList.contains(titleImage.toLowerCase().replace(" ", "_"));
	}
}
