#ifndef DS_IMPL_DSHANDLER_H
#define DS_IMPL_DSHANDLER_H

#include <vector>
#include <type_traits>

namespace ds { namespace impl {

    template<typename E>
    struct DSHandler {

        DSHandler() : processors() {}

        virtual ~DSHandler() {}

        template<typename T>
        std::shared_ptr<T> addProcessor(std::shared_ptr<T> processor)
        {
            auto ptr = std::dynamic_pointer_cast<E>(processor);
            this->processors.emplace_back(ptr);
            return processor;
        }

    protected:
        std::vector<std::shared_ptr<E>> processors;
    };

}}

#endif /* DS_IMPL_DSHANDLER_H */

