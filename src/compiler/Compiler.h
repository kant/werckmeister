#ifndef COMPILER_H
#define COMPILER_H

#include "ICompiler.h"
#include "ISheetTemplateRenderer.h"
#include "ASheetEventRenderer.h"
#include <compiler/context/IContext.h>
#include "ICompilerVisitor.h"
#include "IPreprocessor.h"
#include <fm/ILogger.h>

namespace sheet {
	struct Event;
	namespace compiler {
		class Compiler : public ICompiler {
		private:
			IContextPtr context_;
			DocumentWPtr document_;
			ASheetEventRendererPtr sheetEventRenderer_;
			ISheetTemplateRendererPtr sheetTemplateRenderer_;
			ICompilerVisitorPtr	compilerVisitorPtr_;
			fm::ILoggerPtr	logger_;
			IPreprocessorPtr preprocessor_;
		public:
			Compiler(
				IContextPtr context, 
				ASheetEventRendererPtr sheetEventRenderer,
				ISheetTemplateRendererPtr sheetTemplateRenderer,
				ICompilerVisitorPtr	compilerVisitorPtr,
				fm::ILoggerPtr logger,
				IPreprocessorPtr preprocessor
			) :   context_(context)
				, sheetEventRenderer_(sheetEventRenderer)
				, sheetTemplateRenderer_(sheetTemplateRenderer)
				, compilerVisitorPtr_(compilerVisitorPtr)
				, logger_(logger)
				, preprocessor_(preprocessor)
			{
			}
			Compiler(const Compiler&) = delete;
			Compiler & operator=(const Compiler&) = delete;
			IContextPtr context() const { return context_; }
			virtual void compile(DocumentPtr document) override;
			virtual ~Compiler() = default;
			ASheetEventRendererPtr sheetEventRenderer();
		protected:
			void renderTracks();
			void renderAccompTrack();
		};
	}
}

#endif