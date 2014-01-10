package it.uniroma1.lcl.babelnet;

import it.uniroma1.lcl.babelnet.utils.BadImageList;
import it.uniroma1.lcl.jlt.util.Language;
import it.uniroma1.lcl.jlt.util.Strings;
import it.uniroma1.lcl.jlt.web.WGet;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.codec.digest.DigestUtils;

/**
 * An image in BabelNet.
 * 
 * @author navigli, ponzetto, vannella
 *
 */
public class BabelImage
{
	/*
	 * Used to capture Wikipedia image redirections
	 */
	Pattern CANONICAL_PATTERN =
		Pattern.compile("<link rel=\"canonical\" href=\".*/(.*?)\" />");
	
	/*
	 * Size image 
	 */
	private static final int imageSize = BabelNetConfiguration.getInstance().getBabelImageSize().getSize();

	/*
	 * The default English Wikipedia HOST
	 * 
	 */
	private static final String EN_WIKIPEDIA_HOST = "en.wikipedia.org";

	/*
	 * The default English Wikipedia PATH
	 * 
	 */
	private static final String EN_WIKIPEDIA_PATH = "/wiki/";
	
	/*
	 * The default image URL
	 * 
	 */
	private static final String URL_PREFIX = "http://upload.wikimedia.org/wikipedia/commons/";
	/*
	 * The thumb image URL
	 * 
	 */
	private static final String URL_PREFIX_THUMB = "http://upload.wikimedia.org/wikipedia/commons/thumb/";

	/*
	 * Sometimes image-URL are .../wikipedia/LANGUAGE/
	 */
	private static final String BACKOFF_URL_PREFIX = "http://upload.wikimedia.org/wikipedia/";

	/**
	 * The short name / MediaWiki page name for the image, e.g.
	 * 
	 * "File:Haile-newyork-cropforfocus.jpg"
	 */
	private final String name;

	/**
	 * The language of the Wikipedia <b>this</b> image comes from
	 */
	private final Language language;
	
	/**
	 * The URL thumb to the actual image, e.g.
	 * 
	 * http://upload.wikimedia.org/wikipedia/commons/9/94/Haile-newyork-cropforfocus.jpg/120px-Haile-newyork-cropforfocus.jpg
	 *  
	 */
	private final String thumbUrl;
	
	/**
	 * The URL to the actual image, e.g.
	 * 
	 * http://upload.wikimedia.org/wikipedia/commons/9/94/Haile-newyork-cropforfocus.jpg
	 * 
	 */
	private final String url;

	/**
	 * Validate URL of the image
	 */
	private String urlValidated = null;
	
	/**
	 * Validate URL of the thumb image
	 */
	private String urlThumbValidated = null;
	
	/**
	 * True if bad or censored image
	 */
	private final boolean badImage;

	/**
	 * Creates a new instance of {@link BabelImage}
	 * 
	 * @param imageName
	 *            the MediaWiki page name for the image - something like
	 *            "LANGUAGE:Image:..."
	 */
	public BabelImage(String imageName)
	{
		boolean bSpecialImage = false;
		// identifies the language
		int colonIdx = imageName.indexOf(":");
		if (colonIdx == -1) 
			throw new RuntimeException("Invalid image name: " + imageName);
		Language language = Language.valueOf(imageName.substring(0, colonIdx));
		
		// removes trailing "File:" or "Image:"
		String cleanImageName = imageName.substring(colonIdx+1);
		colonIdx = cleanImageName.indexOf(":");
		if (colonIdx != -1) cleanImageName = cleanImageName.substring(colonIdx+1);
		//se è un'image particolare ha un tag in più
		if(cleanImageName.startsWith("MANUAL:"))
		{
			bSpecialImage = true;
			colonIdx = cleanImageName.indexOf(":");
			if (colonIdx != -1) cleanImageName = cleanImageName.substring(colonIdx+1);
			cleanImageName = Strings.forceFirstCharUppercase(cleanImageName);
		}
		else cleanImageName = Strings.forceFirstCharUppercase(cleanImageName);
		
		// e.g. "Paula_Radcliffe.jpg"
		this.name = cleanImageName.trim();
		//System.out.println("name "+name);
		this.badImage = BadImageList.getInstance().isBadImage(name);
		// Language.EN for instance
		this.language = language;
		
		if(bSpecialImage)
		{
			this.thumbUrl = cleanImageName;
			this.url = cleanImageName;
		}
		else
		{
			this.thumbUrl = createThumbURL(name);
			this.url = createURL(name);
		}
	}
	
	/**
	 * Gets the MediaWiki page name for <b>this</b> image.
	 * 
	 * @return the MediaWiki page name for <b>this</b> image.
	 */
	public String getName()
	{
		return name;
	}
	/**
	 * Gets the MediaWiki page language for <b>this</b> image.
	 * 
	 * @return the MediaWiki page language for <b>this</b> image.
	 */
	public Language getLanguage()
	{
		return  language;
	}
	
	/**
	 * Gets the full URL <b>this</b> image.
	 * 
	 * @return the URL from which <b>this</b> image can be retrieved.
	 */
	public String getURL()
	{
		return url;
	}
	
	/**
	 * Gets the thumb URL <b>this</b> image.
	 * 
	 * @return the URL from which <b>this</b> image can be retrieved.
	 */
	public String getThumbURL()
	{
		return thumbUrl;
	}
	
	/**
	 * Is it a bad or censored image
	 * 
	 * @return true if <b>this</b> image is bad, false otherwise.
	 */
	public boolean isBadImage()
	{
		return badImage;
	}
	
	/**
	 * Gets a verified version of the full URL <b>this</b> image. Checks whether
	 * possible URLs of this image (including <b>redirections</b>) exist or
	 * return 404.
	 * 
	 * @return a validated, known-to-exists, URL from which <b>this</b> image
	 *         can be retrieved.
	 */
	public String getValidatedURL()
	{
		// try the "base" search first
		String validated = getValidatedURL(url, language);
		if (validated != null) return validated;  
		return getValidatedURL(false);
	}
	
	/**
	 * Gets a verified version of the Thumb URL <b>this</b> image. Checks whether
	 * possible URLs of this image (including <b>redirections</b>) exist or
	 * return 404.
	 * 
	 * @return a validated, known-to-exists, URL from which <b>this</b> image
	 *         can be retrieved.
	 */
	public String getValidatedThumbURL()
	{			   
	 	String validated = getValidatedURL(thumbUrl, language);
		if (validated != null)  return validated;
		return getValidatedURL(true);
	}
	
	private String getValidatedURL(boolean isThumb)
	{
		// try the "base" search first
		// otherwise check whether it's a redirection
		//
		// e.g. http://en.wikipedia.org/wiki/File:Cheese_07_bg_042906.jpg
		if (!isThumb)
			if (urlValidated != null) return urlValidated;
		else
			if (urlThumbValidated  != null) return urlThumbValidated;

		String url = null;
		int sizeSmall = 0;
		try
		{
			URI uri = new URI(
				"http", 
				    EN_WIKIPEDIA_HOST, 
				    EN_WIKIPEDIA_PATH+"File:"+name,
				    null);
			String wikiUrl = uri.toASCIIString();
 
			// get the page content
			ByteArrayOutputStream os = new ByteArrayOutputStream();
			WGet.wGet(wikiUrl, os);
			String content = os.toString();

			// check for a URL labeled as "canonical"
			Matcher matcher = CANONICAL_PATTERN.matcher(content);

			if (matcher.find())
			{
				// got it, e.g. File:Mozzarella_cheese.jpg
				String redirectedName = matcher.group(1);
				String cleanRedirectedName = redirectedName;
				int colonIdx = redirectedName.indexOf(":");
				if (colonIdx != -1) cleanRedirectedName = cleanRedirectedName.substring(colonIdx+1);
				cleanRedirectedName =
					Strings.forceFirstCharUppercase(cleanRedirectedName).trim();
 				Pattern sizePatt  = Pattern.compile("=\"(//upload.wikimedia.org/wikipedia/(commons|"+this.language.toString().toLowerCase()+")/thumb/(?!archive).*?"+cleanRedirectedName+""+"/([0-9]*?)px-"+cleanRedirectedName+".*?)\"");
				Matcher matcherSize =  sizePatt.matcher(content);
				while (matcherSize.find())
				{
					//Se sono qui vuol dire che il thumb non è stato validato, 
					//prende l'indirizzo più piccolo vicino alla grandezza desiderata
					
					if(Integer.parseInt(matcherSize.group(3)) <= imageSize) 
						if (sizeSmall < Integer.parseInt(matcherSize.group(3))) {
							if(matcherSize.group(1).startsWith("//"))
								url = "http:"+matcherSize.group(1);
							else
								url = matcherSize.group(1);
							sizeSmall = Integer.parseInt(matcherSize.group(3));
						}
				}

				// create the redirection's URL and check it exists
 				String redirectedURL = createURL(cleanRedirectedName);
				urlValidated = getValidatedURL(redirectedURL, language);
				
				//se non trova nulla imposta l'url originale validato
				if(url == null) url = urlValidated;

				// create the thumb URL 
				urlThumbValidated = url;

				// if validate thumbs
				if (!isThumb) return urlValidated;
				else return urlThumbValidated;
			}
			return null;
		}
		catch (URISyntaxException e)
		{
			return null;
		}
	}

	/**
	 * Gets a verified version of the full input URL <b>this</b> image. Checks
	 * whether possible URLs of this image exist or return 404.
	 * 
	 * @return a validated, known-to-exists, URL from which an image can be
	 *         retrieved.
	 */
	private static String getValidatedURL(String url, Language language)
	{
 		String validated = "";
		if (testURLexists(url))
		{
			validated = url;
		}
		else
		{
			String alternateUrl =
				BACKOFF_URL_PREFIX + language.name().toLowerCase() + "/" +
				url.substring(URL_PREFIX.length());
			if (testURLexists(alternateUrl)) validated = alternateUrl;
		}
		
		// not much to do here...
		if (validated.isEmpty()) return null;

		// check that it is not already URL encoded
		if (isURLEncoded(validated)) return validated;

		try
		{
			// encode the URL in an appropriate way
			int idx = validated.indexOf("/", 7);
			String host = validated.substring(7, idx);
			String path = validated.substring(idx);
			URI uri = new URI("http", host,  path, null);
			return uri.toASCIIString();

		}
		catch (URISyntaxException e) { return null; }
	}
	
	@Override
	public String toString()
	{
		return "<a href=\"" + url + "\">" + name + "</a>";
	}

	/**
	 * Checks whether a given URL exists, namely it does return a 404 error
	 * code
	 * 
	 * @param urlString
	 * @return whether an input URL returns a 404.
	 */
	private static boolean testURLexists(String urlString)
	{
		try
		{
			int responseCode = getResponseCode(urlString);
			return (responseCode == HttpURLConnection.HTTP_OK);
		}
		catch (MalformedURLException mue) { return false; }
		catch (IOException ioe) { return false; }
	}
	
	/**
	 * Gets the response code for an input URL string
	 * 
	 * @param urlString
	 * @return the response code for an input URL
	 * @throws MalformedURLException
	 * @throws IOException
	 */
	public static int getResponseCode(String urlString) throws MalformedURLException, IOException
	{
	    URL url = new URL(urlString); 
	    HttpURLConnection huc =  (HttpURLConnection)  url.openConnection(); 
	    huc.setRequestMethod("HEAD");
	    huc.connect();
	    return huc.getResponseCode();
	}
	
	/**
	 * Creates a thumb-fledged URL from an image name
	 * 
	 * @param name
	 * @return
	 */
	private static String createThumbURL(String name)
	{
		String url = "";
		try
		{
			byte[] nameBytes = name.getBytes("UTF-8");
			// e.g. "aeda0affe44d1e537748dbed05a82a68"
			String md5Hex = DigestUtils.md5Hex(nameBytes);
			// "a"
			String hash1 = md5Hex.substring(0, 1);
			// "ae"
			String hash2 = md5Hex.substring(0, 2);
			// http://upload.wikimedia.org/wikipedia/commons/a/ae/Paula_Radcliffe.jpg
			//for(String size : tmpWordsList){
			url = URL_PREFIX_THUMB + hash1 + "/" + hash2 + "/" + name+"/"+imageSize+"px-"+name;
			return url;
				//}
		}
		catch (UnsupportedEncodingException uee)
		{
			throw new RuntimeException("Cannot init: " + uee);
		}
	}
	
	/**
	 * Creates a full-fledged URL from an image name
	 * 
	 * @param name
	 * @return
	 */
	private static String createURL(String name)
	{
		String url = "";
		try
		{
			byte[] nameBytes = name.getBytes("UTF-8");
			// e.g. "aeda0affe44d1e537748dbed05a82a68"
			String md5Hex = DigestUtils.md5Hex(nameBytes);
			// "a"
			String hash1 = md5Hex.substring(0, 1);
			// "ae"
			String hash2 = md5Hex.substring(0, 2);
			// http://upload.wikimedia.org/wikipedia/commons/a/ae/Paula_Radcliffe.jpg
			 url = URL_PREFIX + hash1 + "/" + hash2 + "/" + name;
			return url;
		}
		catch (UnsupportedEncodingException uee)
		{
			throw new RuntimeException("Cannot init: " + uee);
		}
	}
	
	private static boolean isURLEncoded(String urlString)
	{
        try
		{
			if (urlString.equals(URLDecoder.decode(urlString, "UTF-8")))
				return false;
			else
				return true;
		}
		catch (UnsupportedEncodingException e)
		{
			return false;
		}
        catch (IllegalArgumentException e)
		{
			return false;
		}
	}
	
	@Override
	public boolean equals(Object o)
	{
		if (o == null || !(o instanceof BabelImage)) return false;
		BabelImage i = (BabelImage)o;
		return i.url.equals(url);
	}
	
	/**
	 * Use for testing
	 * 
	 * @param args
	 */
	public static void main(String[] args)
	{
		try
		{
			List<String> tests = new ArrayList<String>();
			
			// some examples of image redirections
			tests.add("EN:Image:☑.svg");
			tests.add("EN:File:Cheese_07_bg_042906.jpg");
			
			//http://upload.wikimedia.org/wikipedia/en/0/0d/Starwarsrobotchicken.jpg
			
			// some personal heroes
			tests.add("EN:File:Abebe_Bikila_1968c.jpg");
			tests.add("EN:File:Oscar_Pistorius_2_Daegu_2011.jpg");
			// babelnet itself
			tests.add("EN:File:The_BabelNet_structure.png");
			// some images in languages other than English
			tests.add("DE:Bild:Porte-Soufflante.jpg");
			tests.add("ES:Archivo:Han_Dynasty_100_BCE.png");
			tests.add("FR:Fichier:Youri_Djorkaeff_2011.jpg");
			tests.add("IT:File:Bombolo.jpeg");
			// an image with a complex label
			tests.add("EN:File:Bundesarchiv_Bild_183-77625-0001,_Brigade_Komsomol\"_schreibt_an_den_Staatsrat\".jpg");
			// and yet another complex one...
			tests.add("EN:File:Malus_domestica_-_Köhler–s_Medizinal-Pflanzen-108.jpg");
			// and its redirection!
			tests.add("EN:File:Koeh-108.jpg");

			tests.add("EN:Metro_Transit-University_Avenue");
			tests.add("CA:_Metro_Transit-University_Avenue-20081002.JPG");
			tests.add("CA:Metro_Transit-University_Avenue-20081002.JPG");

			tests.add("IT:File:Paese_(Italia)-Gonfalone.png");

			for (String test : tests)
			{
				System.out.println(test);
				BabelImage img = new BabelImage(test);
				System.out.println(
					"TESTING FOR IMAGE: " + test +
					"\n\tURL: " + img.getURL() +
      				//"\n\tFULL VALIDATED URL: " + img.getValidatedURL()+
      				"\n\tTHUMB VALIDATED URL: " + img.getValidatedThumbURL()+


					"\n-----");
			}
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}
