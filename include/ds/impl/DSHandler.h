#ifndef DS_IMPL_DSHANDLER_H
#define DS_IMPL_DSHANDLER_H

#include <vector>

namespace ds { namespace impl {

    template<typename E>
    struct DSHandler {

        DSHandler() : processors() {}

        virtual ~DSHandler() {}

        template<typename T>
        T* addProcessor(T* processor)
        {
            this->processors.push_back(processor);
            return processor;
        }

    protected:
        std::vector<E*> processors;
    };

}}

#endif /* DS_IMPL_DSHANDLER_H */

