#ifndef INPUTPORTBOXINFO_H
#define INPUTPORTBOXINFO_H

using namespace std;

	struct InputPortBoxInfo
	{
		int _arcId;
		int _tuplesEnqSinceLastCheck;

		InputPortBoxInfo(int arcId, int tuplesEnqSinceLastCheck) :
			_arcId(arcId),
			_tuplesEnqSinceLastCheck(tuplesEnqSinceLastCheck)
		{
		}
	};

#endif
