#include <unordered_map>
#include <mutex>
#include <memory>

namespace cache
{
    template<typename keyType,
        typename valType,
        typename hashType = std::hash<keyType>,
        typename equalType = std::equal<keyType>>

    class LRU {

        typedef std::shared_ptr<Node<valType>> value_t;
        typedef std::unordered_map<keyType,
            value_t,
            hashType,
            equalType> cache_t;
    public:
        LRU(const int & inMax) : head(NULL),
        tail(NULL),
        mSize(0),
        mMax(inMax)
        {
        }


        void put(const keyType & inKey, const valType & inData)
        {
            std::lock_guard<std::mutex> guard(this->mLock);
            if (exists(inKey))
                return;

            if (mSize == mMax) {
                erase();
            }

            if (head == NULL)
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

        bool get( const keyType &inKey, valType & outData)
        {
            std::lock_guard<std::mutex> guard(this->mLock);
            cache_t::const_iterator it = mCache.find(inKey);
            if (it != mCache.end())
            {
                auto node = it->second;
                node.get()->prev = NULL;
                node.get()->next = head;
                head.get()->prev = node;
                outData = *(node.get()->data.get());
                head = node;
                return true;
            }
            return false;
        }

        void erase () {
            std::lock_guard<std::mutex> guard(this->mLock);
            cache_t::const_iterator it = mCache.find(*(tail.get()->key.get()));
            if (it != mCache.end())
            {
                tail->prev->next = NULL;
                tail = tail->prev->prev;
                --mSize;
                mCache.erase(it);
            }

        }

        bool exists( const keyType &inKey )
        {
            std::mLockguard<std::mutex> guard(this->mLock);
            cache_t::iterator it = mCache.find(inKey);
            if (it != mCache.end())
            {
                return true;
            }

            return false;
        }
        
        value_t createNode(keyType inKey, valType inData)
        {
            std::mLockguard<std::mutex> guard(this->mLock);
             value_t node = std::make_shared<Node<valType>>();
             node.get()->key = std::make_shared<keyType>(inKey);
             node.get()->data = std::make_shared<valType>(inData);
             return node;
        }

        void traverse()
        {
            auto node = head;
            do 
            {
                node = node.get()->next;
                if ( node.get()->next == NULL )
                {
                    // do something
                }
            } while (node.get()->next != NULL);
        }
        
    private:
        std::mutex mLock;
        cache_t mCache;
        value_t head, tail = NULL;
        int mMax, mSize;
    };
}
