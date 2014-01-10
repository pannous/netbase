package it.uniroma1.lcl.babelnet;

import it.uniroma1.lcl.jlt.util.Language;
import it.uniroma1.lcl.jlt.util.Pair;

import java.io.File;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.apache.commons.configuration.ConfigurationException;
import org.apache.commons.configuration.PropertiesConfiguration;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * The {@link PropertiesConfiguration} hub for the BabelNet API.
 * 
 * @author navigli, ponzetto
 *
 */
public class BabelNetConfiguration
{
	private org.apache.commons.configuration.PropertiesConfiguration config = null;
	private static final Log log = LogFactory.getLog(BabelNetConfiguration.class);

	static private BabelNetConfiguration instance = null;
	static private String CONFIG_DIR = "config/";
	static public String CONFIG_FILE = "babelnet.properties";

	private String baseAbsolutePath = null;
	
	/**
	 * Private constructor. By default loads config/babelnet.properties
	 * 
	 * @throws ConfigurationException
	 */
	private BabelNetConfiguration()
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
			log.info("BabelNet starts with empty configuration");
			config = new PropertiesConfiguration();
		}
	}

	/**
	 * Used to obtain object instance
	 * 
	 * @return an instance of {@link BabelNetConfiguration}
	 */
	public static synchronized BabelNetConfiguration getInstance()
	{
		if (instance == null)
		{
			instance = new BabelNetConfiguration();
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
			log.info("Setting BabelNet to an empty configuration");
			config = new PropertiesConfiguration();
		}
	}
	
	public String getBabelNetBaseDir()
	{
		return config.getString("babelnet.dir");
	}
	
	public String getBabelNetFullFile()
	{
		return config.getString("babelnet.fullFile");
	}
	
	public String getBabelNetLexiconFile()
	{
		return config.getString("babelnet.dictFile");
	}

	public String getBabelNetGlossFile()
	{
		return config.getString("babelnet.glossFile");
	}
	
	public String getBabelNetRelationFile()
	{
		return config.getString("babelnet.relFile");
	}
	
	public String getBabelNetLexiconIndexDir()
	{
		return config.getString("babelnet.lexiconDir");
	}
	
	public String getBabelNetDictIndexDir()
	{
		return config.getString("babelnet.dictDir");
	}

	public String getBabelNetGlossIndexDir()
	{
		return config.getString("babelnet.glossDir");
	}
	
	public String getBabelNetGraphIndexDir()
	{
		return config.getString("babelnet.graphDir");
	}
	
	public boolean getBabelNetDumpLexiconOnly()
	{
		return config.getBoolean("babelnet.dump.lexiconOnly");
	}
	
	public Pair<Integer, Integer> getBabelNetDumpStartEnd()
	{
		List<?> startEnd = config.getList("babelnet.dump.startEnd");
		if (startEnd.size() != 2)
			throw new RuntimeException("Invalid start/end: " + startEnd);
		return new Pair<Integer, Integer>(Integer.parseInt(startEnd.get(0).toString()),
										   Integer.parseInt(startEnd.get(1).toString()));
	}
	
	public String getConcept2BabelId()
	{
		return config.getString("babelnet.concept2babelid");
	}

	public String getBabelId2Concept()
	{
		return config.getString("babelnet.babelid2concept");
	}

	public String getBabelTranslations()
	{
		return config.getString("babelnet.translations");
	}

	public Set<Language> getBabelLanguages()
	{
		Set<Language> languages = new HashSet<Language>();
		
		for (Object lang : config.getList("babelnet.languages"))
			languages.add(Language.valueOf(lang.toString()));
		
		return languages;
	}

	public double getBabelEdgeWeightThreshold()
	{
		return config.getDouble("babelnet.minEdgeWeight");
	}

	public boolean getBabelNetUseRedirectionSenses()
	{
		return config.getBoolean("babelnet.useRedirectionSenses");
	}

	public int getBabelcoWindowRadius()
	{
		return config.getInt("babelco.windowRadius");
	}

	public String getBabelcoDBUser()
	{
		return config.getString("babelco.db.user");
	}

	public String getBabelcoDBPassword()
	{
		return config.getString("babelco.db.password");
	}

	public String getBabelcoDBName()
	{
		return config.getString("babelco.db.name");
	}

	public String getBabelcoDBURL()
	{
		return config.getString("babelco.db.url");
	}

	public BabelImageSize getBabelImageSize()
	{
		return BabelImageSize.valueOf(config.getString("babelnet.image.size"));
	}
	
	public boolean isBadImageFilterActive()
	{
		return config.getBoolean("babelnet.image.filter");
	}

	public String getBadImageListPath()
	{
		return (baseAbsolutePath != null ? baseAbsolutePath : "") + config.getString("babelnet.image.path");
	}
	
	public void setBasePath(String baseAbsolutePath)
	{
		this.baseAbsolutePath = baseAbsolutePath;
	}
}
