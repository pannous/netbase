package it.uniroma1.lcl.knowledge.graph;

import it.uniroma1.lcl.babelnet.BabelPointer;
import it.uniroma1.lcl.knowledge.KnowledgeConfiguration;

/**
 * An enumeration of different strategies to score a {@link KnowledgeGraphPath}.
 * 
 * @author ponzetto
 * 
 */
public enum KnowledgeGraphPathScorer
{
	/**
	 * Weight a path as 1.0/Math.exp(path_length-1)
	 * 
	 */
	INVERSE_PATH_LENGTH
	{
		@Override
		public double score(KnowledgeGraphPath path)
		{
			return 1.0/Math.exp((double)path.getPointers().size()-1);
		}
	},
	
	/**
	 * Weights a path as the product of the weight along the paths
	 */
	PATH_PROBABILITY
	{
		@Override
		public double score(KnowledgeGraphPath path)
		{
			KnowledgeConfiguration config = KnowledgeConfiguration.getInstance();
			boolean bInterpolate = 
				config.doInterpolateConceptGraphPathWeights();
			double lambda =
				config.getConceptGraphPathInterpolationLambda();
			
			double sum = 0.0;
			
			// for each edge
			for (KnowledgeGraphPathEdge edge : path.asEdgeList())
			{
				double weight = edge.getWeight();
				// add an epsilon as pseudo-smoothing
				if (weight == 0.0) weight = 0.00000000001;
				
				if (bInterpolate)
				{
					if (edge.getPointer() == BabelPointer.SEMANTICALLY_RELATED)
						// babel relations receive a score of lambda*weight
						weight *= lambda;
					else
						weight *= (1-lambda);
				}
				sum += Math.log(weight);
			}
			
			return -sum;
		}
	};

	/**
	 * Scores a {@link KnowledgeGraphPath}
	 * 
	 * @param path
	 * @return a score for the input {@link KnowledgeGraphPath}
	 */
	public abstract double score(KnowledgeGraphPath path);
}
