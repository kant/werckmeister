#ifndef COMPILER_PREPROCESSOR_H
#define COMPILER_PREPROCESSOR_H

#include <fm/common.hpp>
#include "forward.hpp"

namespace sheet {
	namespace compiler {
        /**
         * sets implicite duration to every event with duration = 0.
         * converts two tied note events to one note event.
         */
		class Preprocessor {
		public:
			Preprocessor() = default;
			Preprocessor(const Compiler&) = delete;
			Preprocessor & operator=(const Compiler&) = delete;
			void process(Track &track);
		private:
		};
	}
}

#endif