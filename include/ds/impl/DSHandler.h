
#ifndef DS_IMPL_DSHANDLER_H
#define DS_IMPL_DSHANDLER_H

#include <vector>

namespace ds { namespace impl {

    template<typename E>
    struct DSHandler {

        virtual ~DSHandler() {}

        virtual void addProcessor(E&& processor) {
            this->processors.push_back(std::move(processor));
        }
        

    protected:

        std::vector<E> processors;

    };

    
}}

#endif /* DS_IMPL_DSHANDLER_H */

