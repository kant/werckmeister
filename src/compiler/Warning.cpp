#include "Warning.hpp"
#include <sheet/Document.h>
#include <sstream>
#include <com/tools.h>

namespace sheet {
    std::string Warning::toString(std::shared_ptr<Document> document) const
    {
        std::stringstream ss;
        ss << "[WARNING] ";
        com::documentMessage(ss, document, sourceObject.sourceId, sourceObject.sourcePositionBegin, message);
        return ss.str();
    }

    std::string Warning::getSourceFile(std::shared_ptr<Document> document) const
    {
        return document->findSourcePath(sourceObject.sourceId);
    }
}