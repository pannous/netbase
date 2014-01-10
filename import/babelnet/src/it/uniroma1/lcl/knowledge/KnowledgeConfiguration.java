package it.uniroma1.lcl.knowledge;

import it.uniroma1.lcl.jlt.util.Types;
import it.uniroma1.lcl.knowledge.custom.CustomKB;
import it.uniroma1.lcl.knowledge.graph.KnowledgeGraphScorer;
import it.uniroma1.lcl.knowledge.graph.filter.FilteringStrategy;
import it.uniroma1.lcl.knowledge.graph.filter.KnowledgeGraphPathFilter;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.configuration.ConfigurationException;
import org.apache.commons.configuration.PropertiesConfiguration;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * The {@link PropertiesConfiguration} hub for the Knowledge graph API.
 * 
 * @author ponzetto
 * 
 */
public class KnowledgeConfiguration
{
	private org.apache.commons.configuration.PropertiesConfiguration config = null;
	private static final Log log = LogFactory.getLog(KnowledgeConfiguration.class);

	static private KnowledgeConfiguration instance = null;
	static private String CONFIG_DIR = "config/";
	static public String CONFIG_FILE = "knowledge.properties";

	/**
	 * Private constructor. By default loads config/knowledge.properties
	 * 
	 * @throws ConfigurationException
	 */
	private KnowledgeConfiguration()
	{
		File configFile = new File(CONFIG_DIR, CONFIG_FILE);
		
		boolean bDone = false;
		if (configFile.exists())
		{
			log.info("Loading " + CONFIG_FILE + " FROM " + configFile.getAbsolutePath());
			try
			{
				config = new PropertiesConfiguration(configFile);
				bDone = true;
			}
			catch (ConfigurationException ce)
			{
				ce.printStackTrace();
			}
		}
		
		if (!bDone)
		{
			log.info("Knowledge graph starts with empty configuration");
			config = new PropertiesConfiguration();
		}
	}

	/**
	 * Used to access {@link KnowledgeConfiguration}
	 * 
	 * @return an instance of {@link KnowledgeConfiguration}
	 */
	public static synchronized KnowledgeConfiguration getInstance()
	{
		if (instance == null)
		{
			instance = new KnowledgeConfiguration();
		}
		return instance;
	}

	/**
	 * Set the file from which to load the new properties
	 * 
	 * @param configurationFile
	 *            the file under config/ to load the properties from
	 */
	public void setConfigurationFile(File configurationFile)
	{
		log.info("Changing configuration properties to " + configurationFile);
		try
		{
			config = new PropertiesConfiguration(configurationFile);
			config.setBasePath(
				configurationFile.getParentFile().getAbsolutePath());
		}
		catch (ConfigurationException ce)
		{
			ce.printStackTrace();
			log.info("Setting Knowledge graph to an empty configuration");
			config = new PropertiesConfiguration();
		}
	}
	
	public CustomKB getKnowledgeBase()
	{
		String kbName = config.getString("knowledgeBase");
		log.info("Trying to load " + kbName + " knowledge base");

		CustomKB kb = null;
		
		try 
		{
			Class<CustomKB> c = 
				Types.uncheckedCast(ClassLoader.getSystemClassLoader().loadClass(kbName),
									new Types<Class<CustomKB>>());
			kb = c.newInstance();
		}
		catch (ClassNotFoundException e){e.printStackTrace();}
		catch (InstantiationException e){e.printStackTrace();}
		catch (IllegalAccessException e){e.printStackTrace();}
		
		return kb;
	}
	
	public String getKnowledgeBaseDataPath(CustomKB kb)
	{
		return config.getString("knowledgeBaseDataPath." + kb.getClass().getSimpleName());
	}
	
	public String getKnowledgeBasePathIndexDir(KnowledgeBase kb)
	{
		return config.getString("knowledge.graph.pathIndex") + File.separator + kb.name().toLowerCase() + "_paths";
	}
	
	public KnowledgeGraphScorer getKnowledgeGraphScorer()
	{
		return KnowledgeGraphScorer.valueOf(config.getString("knowledge.graph.scorer"));
	}

	public boolean doInterpolateConceptGraphPathWeights()
	{
		return config.getBoolean("knowledge.graph.scorer.interpolate");
	}

	public double getConceptGraphPathInterpolationLambda()
	{
		return config.getDouble("knowledge.graph.scorer.interpolate.lambda");
	}

	public List<KnowledgeGraphPathFilter> getKnowledgeGraphPathFilters()
	{
		List<KnowledgeGraphPathFilter> filters = new ArrayList<KnowledgeGraphPathFilter>();
		for (Object filter : config.getList("knowledge.graph.filters"))
		{
			String filterName = filter.toString();
			if (!filterName.isEmpty())
			{
				FilteringStrategy strategy = FilteringStrategy.valueOf(filterName);
				filters.add(strategy.getFilter());
			}
		}
		return filters;
	}

	public double getConceptGraphPathFilterWeigthThreshold()
	{
		return config.getDouble("knowledge.graph.filter.weight.threshold");
	}

	public int getConceptGraphPathFilterLengthThreshold()
	{
		return config.getInt("knowledge.graph.filter.length.threshold");
	}

	public List<String> getConceptGraphPathIllegalPointers()
	{
		List<String> pointers = new ArrayList<String>();
		for (Object pointer : config.getList("knowledge.graph.filter.illegalPointers"))
			pointers.add(pointer.toString());
		return pointers;
	}

	public List<String> getConceptGraphPathLegalPointers()
	{
		List<String> pointers = new ArrayList<String>();
		for (Object pointer : config.getList("knowledge.graph.filter.legalPointers"))
			pointers.add(pointer.toString());
		return pointers;
	}

}
