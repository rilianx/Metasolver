/*
 * Gen.h
*/

#ifndef PLUGINS_MOP_STRATEGIES_GEN_H_
#define PLUGINS_MOP_STRATEGIES_GEN_H_

#include <list>
#include "SearchStrategy.h"

using namespace metasolver;

class Gen {

	public:

	    virtual ~Gen() { }

		virtual Gen* clone()=0;

		virtual void mutate()=0;

		virtual void mutate2()=0;

		virtual void write(std::ostream&) const=0;

		friend std::ostream& operator<<(std::ostream &strm, const Gen &a){ a.write(strm); return strm; }

};


#endif //PLUGINS_MOP_STRATEGIES_GEN_H_
