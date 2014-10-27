#include"iostream"
#include"time.h"
#include"cGeneticAlgorithmHepothesis.h"
using namespace std;
using namespace San;
#ifndef __CGENETICALGORITHM_H__
#define __CGENECITALGORITHM_H__

enum eGENETICALGORITHMSELECTSTRATEGYTYPE
{
	GASST_DEFAULT,
	GASST_TOURNAMENT,
	GASST_RANK,
};
typedef eGENETICALGORITHMSELECTSTRATEGYTYPE eGASELECTMETHOD;

template<class HypothesisClass>
struct GAINDIVIDUAL
{
	HypothesisClass* pHypothesis;
	sfloat Fitness;
	bool bSurvived;
};

template<class I,class O,class UserDataType,class HypothesisClass>
class cGeneticAlgorithm
{
private:
	vector<GAINDIVIDUAL<HypothesisClass>> m_PopulationSpace;
	uint32 m_PopulationSize;
	sfloat m_ReplacementRate;
	sfloat m_MutationRate;
	eGASELECTMETHOD m_SelectMethod;
	uint32 m_MaxFitnessIndex;
protected:
	void _TournamentMethod()
	{
		for (uint32 seek = 0; seek < this->m_PopulationSize; seek = seek + 2)
		{
			uint32 H1Index = ::rand() % (this->m_PopulationSize - seek);
			uint32 H2Index = ::rand() % (this->m_PopulationSize - seek);
			while (H1Index == H2Index)
			{
				H2Index = ::rand() % (this->m_PopulationSize - seek);
			}
			if (this->m_PopulationSpace[H1Index].Fitness <= this->m_PopulationSpace[H2Index].Fitness)
			{
				this->m_PopulationSpace[H1Index].Fitness = 0.4;
				this->m_PopulationSpace[H2Index].Fitness = 0.6;
			}
			else
			{
				this->m_PopulationSpace[H1Index].Fitness = 0.6;
				this->m_PopulationSpace[H2Index].Fitness = 0.4;
			}

			GAINDIVIDUAL<HypothesisClass> Hypothesis = this->m_PopulationSpace[this->m_PopulationSize - seek - 1];
			this->m_PopulationSpace[this->m_PopulationSize - seek - 1] = this->m_PopulationSpace[H1Index];
			this->m_PopulationSpace[H1Index] = Hypothesis;

			Hypothesis = this->m_PopulationSpace[this->m_PopulationSize - seek - 2];
			this->m_PopulationSpace[this->m_PopulationSize - seek - 2] = this->m_PopulationSpace[H2Index];
			this->m_PopulationSpace[H2Index] = Hypothesis;
		}

		if (this->m_PopulationSize % 2 == 1)
		{
			this->m_PopulationSpace[this->m_PopulationSize - 1].Fitness = 0.5;
		}
	};
	void _RankMethod()
	{
		for (uint32 seek = 0; seek < this->m_PopulationSize; seek = seek + 1)
		{
			uint32 MaxAccuracyIndex = seek;
			for (uint32 seek_max = seek; seek_max < this->m_PopulationSize; seek_max = seek_max + 1)
			{
				if (this->m_PopulationSpace[seek].Fitness <= this->m_PopulationSpace[seek_max].Fitness)
				{
					MaxAccuracyIndex = seek_max;
				}
			}
			GAINDIVIDUAL<HypothesisClass> Hypothesis = this->m_PopulationSpace[seek];
			this->m_PopulationSpace[seek] = this->m_PopulationSpace[MaxAccuracyIndex];
			this->m_PopulationSpace[MaxAccuracyIndex] = Hypothesis;
			this->m_PopulationSpace[seek].Fitness = this->m_PopulationSize + 1 - seek;
		}
	};
public:
	cGeneticAlgorithm()
		:m_PopulationSize(1000),
		m_ReplacementRate(0.5),
		m_MutationRate(0.1),
		m_SelectMethod(GASST_DEFAULT)
	{
		//::srand(time(0));
	};
	~cGeneticAlgorithm()
	{
		if (!this->m_PopulationSpace.empty())
		{
			uint32 Size = this->m_PopulationSpace.size();
			for (uint32 seek = 0; seek < Size; seek = seek + 1)
			{
				delete this->m_PopulationSpace[seek].pHypothesis;
				this->m_PopulationSpace[seek].pHypothesis = nullptr;
			}
			this->m_PopulationSpace.clear();
		}
	};
	bool iSetAlgorithmParams(const uint32 Population, const sfloat ReplacementRate, const sfloat MutationRate, const eGASELECTMETHOD Method)
	{
		if (Population < 2)
		{
			return false;
		}
		if ((ReplacementRate < 0.0) || (MutationRate < 0.0))
		{
			return false;
		}
		this->m_PopulationSize = Population;
		this->m_SelectMethod = Method;
	}
	void iGeneratePolulation(const GAINSTANCESET<I, O> &TrainingSet, const UserDataType &UserData, const sfloat Threshold, const uint32 MaxTimes)
	{
#pragma region Clear previous population space and regenerate new one
		if (!this->m_PopulationSpace.empty())
		{
			uint32 Size = this->m_PopulationSpace.size();
			for (uint32 seek = 0; seek < Size; seek = seek + 1)
			{
				delete this->m_PopulationSpace[seek].pHypothesis;
				this->m_PopulationSpace[seek].pHypothesis = nullptr;
			}
			this->m_PopulationSpace.clear();
		}
		this->m_PopulationSpace.resize(this->m_PopulationSize);
#pragma endregion

#pragma region Create indivdual hypothesis and evaluate
		cGeneticAlgorithmHypothesis<I, O, UserDataType>* pHypothesis;
		sfloat MaxFitness = -1.0;
		for (uint32 seek = 0; seek < this->m_PopulationSize; seek = seek + 1)
		{
			this->m_PopulationSpace[seek].pHypothesis = new HypothesisClass();

			pHypothesis = (cGeneticAlgorithmHypothesis<I, O, UserDataType>*)this->m_PopulationSpace[seek].pHypothesis;
			pHypothesis->iInitialize(seek, UserData, TrainingSet);

			this->m_PopulationSpace[seek].Fitness = pHypothesis->iEvaluate(TrainingSet);
			this->m_PopulationSpace[seek].bSurvived = true;
			if (MaxFitness <= this->m_PopulationSpace[seek].Fitness)
			{
				MaxFitness = this->m_PopulationSpace[seek].Fitness;
				this->m_MaxFitnessIndex = seek;
			}
		}
#pragma endregion

		uint32 IterationCount = 0;

		uint32 SurvivalSize = this->m_PopulationSize - this->m_PopulationSize*this->m_ReplacementRate;
		uint32 OffspringSize = this->m_PopulationSize*this->m_ReplacementRate;
		uint32 MutationSize = this->m_PopulationSize*this->m_MutationRate;

		OffspringSize = OffspringSize + OffspringSize % 2;
		vector<GAINDIVIDUAL<HypothesisClass>> OffspringPopulation(OffspringSize);

		while (true)
		{
			pHypothesis = (cGeneticAlgorithmHypothesis<I, O, UserDataType>*)this->m_PopulationSpace[this->m_MaxFitnessIndex].pHypothesis;
			::wcout << L"Iteration: " << IterationCount << L"\tMaxFitness: " << MaxFitness << L"\tLength: " << ::gloIToS(pHypothesis->iGetHypothesisSize()).c_str() << L"\n";

			if ((MaxFitness >= Threshold) || (IterationCount >= MaxTimes))
			{
				break;
			}

#pragma region Crossover
			for (uint32 seek = 0; seek < OffspringSize; seek = seek + 2)
			{
				vector<cGeneticAlgorithmHypothesis<I, O, UserDataType>*> CurrentOffspringVector(2);

				while (true)
				{
					uint32 H1 = ::rand() % this->m_PopulationSize;
					uint32 H2 = ::rand() % this->m_PopulationSize;

					while (H1 == H2)
					{
						H2 = ::rand() % this->m_PopulationSize;
					}

					CurrentOffspringVector[0] = nullptr;
					CurrentOffspringVector[1] = nullptr;

					pHypothesis = (cGeneticAlgorithmHypothesis<I, O, UserDataType>*)this->m_PopulationSpace[H1].pHypothesis;
					if (pHypothesis->iCrossoverSP((cGeneticAlgorithmHypothesis<I, O, UserDataType>*)this->m_PopulationSpace[H2].pHypothesis, CurrentOffspringVector))
					{
						OffspringPopulation[seek].pHypothesis = (HypothesisClass*) CurrentOffspringVector[0];
						OffspringPopulation[seek + 1].pHypothesis = (HypothesisClass*) CurrentOffspringVector[1];
						break;
					}
				}
			}
#pragma endregion

#pragma region Select
			switch (this->m_SelectMethod)
			{
			case GASST_DEFAULT:
				break;
			case GASST_TOURNAMENT:
				this->_TournamentMethod();
				break;
			case GASST_RANK:
				this->_RankMethod();
				break;
			default:
				break;
			}

			sfloat FitnessSum = 0.0;
			for (uint32 seek = 0; seek < this->m_PopulationSize; seek = seek + 1)
			{
				FitnessSum = FitnessSum + this->m_PopulationSpace[seek].Fitness;
				this->m_PopulationSpace[seek].bSurvived = false;
			}

			/*Save the best one*/
			this->m_PopulationSpace[this->m_MaxFitnessIndex].bSurvived = true;
			GAINDIVIDUAL<HypothesisClass> Indivdual = this->m_PopulationSpace[this->m_MaxFitnessIndex];
			this->m_PopulationSpace[this->m_MaxFitnessIndex] = this->m_PopulationSpace[this->m_PopulationSize - 1];
			this->m_PopulationSpace[this->m_PopulationSize - 1] = Indivdual;
			this->m_MaxFitnessIndex = this->m_PopulationSize - 1;

			/*Random select other hypothesis*/
			uint32 SurvivalCount = 1;
			while (SurvivalCount != SurvivalSize)
			{
				uint32 Index = ::rand() % (this->m_PopulationSize - SurvivalCount);
				sfloat Probability = this->m_PopulationSpace[Index].Fitness / FitnessSum;
				sfloat RandomRate = ::rand() % 10000;
				RandomRate = RandomRate / 10000.0;

				if (Probability > RandomRate)
				{
					this->m_PopulationSpace[Index].bSurvived = true;
					GAINDIVIDUAL<HypothesisClass> Indivdual = this->m_PopulationSpace[Index];
					this->m_PopulationSpace[Index] = this->m_PopulationSpace[this->m_PopulationSize - SurvivalCount - 1];
					this->m_PopulationSpace[this->m_PopulationSize - SurvivalCount - 1] = Indivdual;

					SurvivalCount = SurvivalCount + 1;
				}
			}

			/*Normailize the probalility*/
			//FitnessSum = FitnessSum - this->m_PopulationSpace[this->m_PopulationSize - 1].Fitness;

			//for (uint32 seek = 0; seek < this->m_PopulationSize - 1; seek = seek + 1)
			//{
			//	this->m_PopulationSpace[seek].Fitness = this->m_PopulationSpace[seek].Fitness / FitnessSum;
			//}

			//while (SurvivalCount != SurvivalSize)
			//{
			//	sfloat RandomVal = (sfloat) (::rand() % 100000000) / 100000000.0;

			//	sfloat ProbabilitySum = 0.0;
			//	for (uint32 seek = 0; seek < this->m_PopulationSize - 1; seek = seek + 1)
			//	{
			//		ProbabilitySum = ProbabilitySum + this->m_PopulationSpace[seek].Fitness;

			//		if ((RandomVal < ProbabilitySum) && (this->m_PopulationSpace[seek].bSurvived == false))
			//		{
			//			this->m_PopulationSpace[seek].bSurvived = true;
			//			SurvivalCount = SurvivalCount + 1;
			//			break;
			//		}
			//	}

			//	//::cout << SurvivalCount << "\n";
			//}

			//uint32 SurvivalIndex = this->m_PopulationSize - 2;

			//for (int32 seek = 0; seek < SurvivalIndex; seek = seek + 1)
			//{
			//	if (this->m_PopulationSpace[seek].bSurvived)
			//	{
			//		GAINDIVIDUAL<HypothesisClass> Indivdual = this->m_PopulationSpace[seek];
			//		this->m_PopulationSpace[seek] = this->m_PopulationSpace[SurvivalIndex];
			//		this->m_PopulationSpace[SurvivalIndex] = Indivdual;

			//		seek = seek - 1;
			//		SurvivalIndex = SurvivalIndex - 1;
			//	}
			//}
#pragma endregion

#pragma region Update
			for (uint32 seek = 0; seek < this->m_PopulationSize; seek = seek + 1)
			{
				if (!this->m_PopulationSpace[seek].bSurvived)
				{
					delete this->m_PopulationSpace[seek].pHypothesis;
					this->m_PopulationSpace[seek] = OffspringPopulation[seek];
					OffspringPopulation[seek].pHypothesis = nullptr;
				}
				else
				{
					break;
				}
			}
			for (uint32 seek = 0; seek < OffspringSize; seek = seek + 1)
			{
				if (OffspringPopulation[seek].pHypothesis!=nullptr)
				{
					delete OffspringPopulation[seek].pHypothesis;
					OffspringPopulation[seek].pHypothesis = nullptr;
				}
			}
#pragma endregion

#pragma region Mutate

			/*Skip the best hypothesis and mutate other hypothesis space*/
			uint32 MutationSetSize = this->m_PopulationSize - 1;

			for (uint32 seek = 0; seek < MutationSetSize; seek = seek + 1)
			{
				sfloat RandomRate = ::rand() % 1000;
				RandomRate = RandomRate / 1000.0;

				if (RandomRate < this->m_MutationRate)
				{
					pHypothesis = (cGeneticAlgorithmHypothesis<I, O, UserDataType>*)this->m_PopulationSpace[seek].pHypothesis;
					pHypothesis->iMutation();
				}
			}
#pragma endregion

#pragma region Evaluate
			MaxFitness = -1.0;
			for (uint32 seek = 0; seek < this->m_PopulationSize; seek = seek + 1)
			{
				pHypothesis = (cGeneticAlgorithmHypothesis<I, O, UserDataType>*)this->m_PopulationSpace[seek].pHypothesis;

				this->m_PopulationSpace[seek].Fitness = pHypothesis->iEvaluate(TrainingSet);
				this->m_PopulationSpace[seek].bSurvived = true;
				if (MaxFitness <= this->m_PopulationSpace[seek].Fitness)
				{
					MaxFitness = this->m_PopulationSpace[seek].Fitness;
					this->m_MaxFitnessIndex = seek;
				}
			}
			IterationCount = IterationCount + 1;
#pragma endregion
		}
	}

	void iPredict(const GAINSTANCESET<I, O> &InstanceSpace, vector<O> &ResultSet)
	{
		cGeneticAlgorithmHypothesis<I, O, UserDataType>* pHypothesis;
		pHypothesis = (cGeneticAlgorithmHypothesis<I, O, UserDataType>*)this->m_PopulationSpace[this->m_MaxFitnessIndex].pHypothesis;
		pHypothesis->iPredict(InstanceSpace, ResultSet);
	}
	sfloat iAccuracy(const GAINSTANCESET<I, O> &InstanceSpace)
	{
		cGeneticAlgorithmHypothesis<I, O, UserDataType>* pHypothesis;
		pHypothesis = (cGeneticAlgorithmHypothesis<I, O, UserDataType>*)this->m_PopulationSpace[this->m_MaxFitnessIndex].pHypothesis;
		return pHypothesis->iAccuracy(InstanceSpace);
	}
	SString iPrintHypothesisSpace() const
	{
		cGeneticAlgorithmHypothesis<I, O, UserDataType>* pHypothesis;
		pHypothesis = (cGeneticAlgorithmHypothesis<I, O, UserDataType>*)this->m_PopulationSpace[this->m_MaxFitnessIndex].pHypothesis;
		return pHypothesis->iPrintHypothesisSpace();
	}
};
#endif