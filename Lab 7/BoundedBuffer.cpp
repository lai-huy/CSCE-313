#include "BoundedBuffer.h"
#include <assert.h>

using namespace std;


BoundedBuffer::BoundedBuffer(int _cap): cap(_cap), q{queue<vector<char>>{}} {}

BoundedBuffer::~BoundedBuffer() {}

void BoundedBuffer::push(char* msg, int size) {
    // 1. Convert the incoming byte sequence given by msg and size into a vector<char>
    const vector<char> msgVec(msg, msg + size);

    // 2. Wait until there is room in the queue (i.e., queue lengh is less than cap)
    unique_lock<mutex> lock(this->mtx);
    this->push_ready.wait(lock, [this] {return this->q.size() < (size_t) cap;});

    // 3. Then push the vector at the end of the queue
    this->q.push(msgVec);
    lock.unlock();

    // 4. Wake up threads that were waiting for push
    this->pop_ready.notify_one();
}

int BoundedBuffer::pop(char* msg, int size) {
    // 1. Wait until the queue has at least 1 item
    unique_lock<mutex> lock(this->mtx);
    this->pop_ready.wait(lock, [this] {return !this->q.empty();});

    // 2. Pop the front item of the queue. The popped item is a vector<char>
    const vector<char> msgVec = this->q.front();
    this->q.pop();
    lock.unlock();

    // 3. Convert the popped vector<char> into a char*, copy that into msg; assert that the vector<char>'s length is <= size
    int len = msgVec.size();
    assert(len <= size);
    std::copy(msgVec.cbegin(), msgVec.cend(), msg);

    // 4. Wake up threads that were waiting for pop
    this->push_ready.notify_one();

    // 5. Return the vector's length to the caller so that they know how many bytes were popped
    return len;
}

size_t BoundedBuffer::size() {
    return this->q.size();
}