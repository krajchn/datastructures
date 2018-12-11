#ifndef _LRUCACHE_HPP_INCLUDED_
#define	_LRUCACHE_HPP_INCLUDED_

#include <unordered_map>
#include <mutex>
#include <memory>

namespace cache
{
    template<typename keyType, typename valType>
    class Node {
    public:
        std::shared_ptr<keyType> key;
        std::shared_ptr<valType> data;
        class std::shared_ptr<Node<keyType, valType>> prev;
        class std::shared_ptr<Node<keyType, valType>> next;
        
        ~Node() {
			key.reset();
			data.reset();
		}
    };

    template<typename keyType,
        typename valType,
        typename hashType = std::hash<keyType>,
        typename equalType = std::equal_to<keyType>>

        class LRU {

        typedef std::shared_ptr<Node<keyType, valType>> value_t;
        typedef std::unordered_map<keyType,
            value_t,
            hashType,
            equalType> cache_t;
        public:
            LRU(const int & inMax) :
                mSize(0),
                mMax(inMax)
            {
            }

            LRU() : mSize(0),
                mMax(10000)
            {
            }

            void SetLimit(const int & inLimit)
            {
                mMax = inLimit;
            }
            void put(const keyType & inKey, const valType & inData)
            {

                if (exists(inKey))
                    return;

                std::lock_guard<std::mutex> guard(this->mLock);
                if (mSize == mMax) {
                    erase();
                }

                if (!head)
                {
                    head = createNode(inKey, inData);
                    tail = head;
                }
                else {
                    auto node = createNode(inKey, inData);
                    node.get()->prev = tail;
                    tail.get()->next = node;
                    tail = node;
                }
                mCache.insert(std::pair<keyType, value_t>(inKey, tail));
                ++mSize;
            }

            bool get(const keyType &inKey, valType & outData)
            {
                std::lock_guard<std::mutex> guard(this->mLock);
                typename cache_t::const_iterator it = mCache.find(inKey);
                if (it != mCache.end())
                {
                    auto node = it->second;
                    // No need to change the order 
                        // when the node is already at the head
                    if (node->prev) {
                        if (node->next) {
                            node->next->prev = node->prev;
                            node->prev->next = node->next;
                            node->next.reset();
                        }
                        else {
                            node->prev->next.reset();
                        }

                        tail = node->prev;

                        // break the links
                        node->prev.reset();

                        node->next = head;
                        head->prev = node;
                        outData = *(node.get()->data);
                        head = node;
                    }
                    else {
                        outData = *(node.get()->data);

                    }
                    return true;
                }
                return false;
            }

            bool exists(const keyType &inKey)
            {
                std::lock_guard<std::mutex> guard(this->mLock);
                typename cache_t::const_iterator it = mCache.find(inKey);
                if (it != mCache.end())
                {
                    return true;
                }

                return false;
            }
        private:
            void erase() {
                typename cache_t::const_iterator it = mCache.find(*(tail.get()->key));
                if (it != mCache.end())
                {
                    tail->prev->next.reset();
                    auto node = tail->prev;
                    tail.reset();
                    tail = node;
                    --mSize;
                    it->second.reset();
                    mCache.erase(it);
                }

            }

            value_t createNode(keyType inKey, valType inData)
            {
                value_t node = std::make_shared<Node<keyType, valType>>();
                node.get()->key = std::make_shared<keyType>(inKey);
                node.get()->data = std::make_shared<valType>(inData);
                return node;
            }

        public:
            void traverse()
            {
                auto node = head;
                do
                {
                    node = node->next;
                    if (node->next == NULL)
                    {
                        // do something
                    }
                } while (node->next);
            }

        private:
            std::mutex mLock;
            cache_t mCache;
            value_t head, tail;
            int mMax, mSize;
    };
}

#endif
