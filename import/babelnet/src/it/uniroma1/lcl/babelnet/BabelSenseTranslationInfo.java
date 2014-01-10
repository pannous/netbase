package it.uniroma1.lcl.babelnet;

import it.uniroma1.lcl.jlt.util.Strings;

/**
 * A class modeling translation-related information pertaining to a
 * {@link BabelSense}
 * 
 * @author ponzetto
 *
 */
public class BabelSenseTranslationInfo
{
	/*
	 * A confidence score for a translation
	 */
	private final double confidence;
	
	/*
	 * The number of times a source sense was translated as a target sense 
	 */
	private final int numberOfTranslations;
	
	/*
	 * The size of the sample for translating - the number of sense-annotated
	 * examples for the source sense
	 */
	private final int sampleSize;

	/**
	 * Creates a new instance of {@link BabelSenseTranslationInfo}
	 * 
	 * @param confidence
	 * @param numberOfTranslations
	 * @param sampleSize
	 */
	public BabelSenseTranslationInfo(double confidence, int numberOfTranslations,
									 int sampleSize)
	{
		this.confidence = confidence;
		this.numberOfTranslations = numberOfTranslations;
		this.sampleSize = sampleSize;
	}
	
	/**
	 * Gets a confidence score for a translation.
	 * 
	 * @return the confidence score for a translation.
	 */
	public double getConfidence()
	{
		return confidence;
	}
	
	/**
	 * Gets the number of times a source sense was translated as a target sense.
	 * 
	 * @return how many times a source sense was translated as a target sense.
	 */
	public int getNumberOfTranslations()
	{
		return numberOfTranslations;
	}
	
	/**
	 * Gets the size of the sample for translating - the number of
	 * sense-annotated examples for the source sense
	 * 
	 * @return the number of sense-annotated examples for the source sense
	 */
	public int getSampleSize()
	{
		return sampleSize;
	}
	
	@Override
	public String toString()
	{
		return
			new StringBuffer().
					append(Strings.format(confidence)).append("_").
					append(numberOfTranslations).append("_").
					append(sampleSize).toString();
	}
	
	/**
	 * Creates a new instance of a {@link BabelSenseTranslationInfo} from an
	 * input String.
	 * 
	 * @param s
	 * @return a new instance of a {@link BabelSenseTranslationInfo} from the
	 *         given string.
	 */
	public static BabelSenseTranslationInfo fromString(String s)
	{
		// empty string => nothing to parse
		if (s.isEmpty()) return null;
		// check for a sane input: we expect "confidence_numberOfTranslations_sampleSize"
		String[] split = s.split("_");
		if (split.length != 3)
			throw new RuntimeException("Invalid translation weight information: " + s);
		double confidence = Double.parseDouble(split[0]);
		int numberOfTranslations = Integer.parseInt(split[1]);
		int sampleSize = Integer.parseInt(split[2]);
		// create the object and return
		return new BabelSenseTranslationInfo(confidence, numberOfTranslations,
											 sampleSize);
	}
}
