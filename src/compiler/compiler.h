#ifndef COMPILER_H
#define COMPILER_H

#include <fm/common.hpp>
#include "forward.hpp"

namespace sheet {
	namespace compiler {
		class Compiler {
		public:
			Compiler();
			Compiler(const Compiler&) = delete;
			Compiler & operator=(const Compiler&) = delete;
			void context(AContextPtr context) { context_ = context; }
			AContextPtr context() const { return context_; }
			void compile(DocumentPtr document);
		private:
			AContextPtr context_;
			DocumentPtr document_;
		};
	}
}

#endif