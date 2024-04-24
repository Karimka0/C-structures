#include <array>
#include <iostream>

template<size_t N>
class StackStorage {
    void *top;
    char arr[N];
    size_t last = N;

    template<typename U, size_t M>
    friend
    class StackAllocator;

public:
    StackStorage() : top(arr) {}

    StackStorage(const StackStorage &) = delete;

    StackStorage &operator=(const StackStorage &) = delete;
};


template<typename T, size_t N>
class StackAllocator {
    StackStorage<N>& storage;

    template<typename U, size_t M>
    friend
    class StackAllocator;

public:
    const static bool propagate_on_container_copy_assignment = false;

    using value_type = T;

    StackAllocator() = default;

    StackAllocator(StackStorage<N> &other) : storage(other) {}

    template<typename U>
    StackAllocator(const StackAllocator<U, N> &other) noexcept: storage(other.storage) {}

    ~StackAllocator() = default;

    T *allocate(size_t count) {
        if (std::align(alignof(T), sizeof(T) * count, storage.top, storage.last)) {
            T* last_top = static_cast<T*>(storage.top);
            storage.top = (char *) storage.top + sizeof(T) * count;
            storage.last -= sizeof(T) * count;
            return last_top;
        } else {
            throw std::bad_alloc();
        }
    }

    template<typename U>
    struct rebind {
        using other = StackAllocator<U, N>;
    };

    void deallocate(T *, size_t) {}

    bool operator==(const StackAllocator &other) const {
        return &storage == &other.storage;
    }

    bool operator!=(const StackAllocator &other) const {
        return !(*this == other);
    }

};


template<typename T, typename Alloc=std::allocator<T>>
class List {
private:

    struct BaseNode {
        BaseNode *prev = nullptr;
        BaseNode *next = nullptr;

        BaseNode() = default;

        BaseNode(BaseNode *node, BaseNode *node2) : prev(node), next(node2) {}

    };

    struct Node : BaseNode {
        T value;

        Node(BaseNode *node, BaseNode *node2, const T &val) : BaseNode(node, node2), value(val) {}
    };

    size_t sz_;
    BaseNode fakeNode_;
    using NodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
    using NodeTraits = typename std::allocator_traits<NodeAlloc>;
    [[no_unique_address]]NodeAlloc alloc_;

public:

    List() : sz_(0), fakeNode_(&fakeNode_, &fakeNode_) {}

    List(const size_t n, const T &sample, Alloc alloc = Alloc()) : sz_(n), fakeNode_(&fakeNode_, &fakeNode_),
                                                                   alloc_(alloc) {
        try {
            Node *newNode = nullptr;
            for (size_t i = 0; i < n; ++i) {
                newNode = NodeTraits::allocate(alloc_, 1);
                NodeTraits::construct(alloc_, &newNode->value, sample);
                newNode->prev = fakeNode_.prev;
                fakeNode_.prev->next = newNode;
                fakeNode_.prev = newNode;
                newNode->next = &fakeNode_;
            }
        } catch (...) {
            delete_nodes();
            throw;
        }
    }

    List(const Alloc &alloc) : sz_(0), fakeNode_(&fakeNode_, &fakeNode_), alloc_(alloc) {}

    List(const size_t n, Alloc alloc = Alloc()) : sz_(n), fakeNode_(&fakeNode_, &fakeNode_), alloc_(alloc) {
        try {
            Node *newNode = nullptr;
            for (size_t i = 0; i < n; ++i) {
                newNode = NodeTraits::allocate(alloc_, 1);
                NodeTraits::construct(alloc_, &newNode->value);
                newNode->prev = fakeNode_.prev;
                fakeNode_.prev->next = newNode;
                fakeNode_.prev = newNode;
                newNode->next = &fakeNode_;
            }
        } catch (...) {
            delete_nodes();
            throw;
        }
    }

    List(const List &other) : sz_(0), fakeNode_(&fakeNode_, &fakeNode_),
                              alloc_(NodeTraits ::select_on_container_copy_construction(other.alloc_)) {
        try {
            auto it = other.begin();
            for (; it != other.end(); ++it) {
                push_back(*it);
            }
        } catch (...) {
            delete_nodes();
            throw;
        }
    }

    List &operator=(const List &other) {
        NodeAlloc new_alloc = NodeTraits::propagate_on_container_copy_assignment::value ? other.alloc_ : alloc_;
        size_t i = 0;
        Node *newNode = nullptr;
        BaseNode new_fake_node(&new_fake_node, &new_fake_node);
        try {
            auto it = other.begin();
            for (; it != other.end(); ++it) {
                newNode = NodeTraits::allocate(new_alloc, 1);
                NodeTraits::construct(new_alloc, &newNode->value, *it);
                ++i;
                newNode->prev = new_fake_node.prev;
                new_fake_node.prev->next = newNode;
                new_fake_node.prev = newNode;
                newNode->next = &new_fake_node;
            }
            delete_nodes();
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                NodeTraits::destroy(new_alloc, &newNode[j].value);
            }
            NodeTraits::deallocate(new_alloc, newNode, other.sz_);
            throw;
        }
        fakeNode_ = new_fake_node;
        fakeNode_.next->prev = &fakeNode_;
        fakeNode_.prev->next = &fakeNode_;
        sz_ = other.sz_;
        if constexpr (NodeTraits::propagate_on_container_copy_assignment::value) {
            alloc_ = new_alloc;
        }
        return *this;
    }

    ~List() {
        delete_nodes();
    }

    void delete_nodes() {
        Node *temp = static_cast<Node *>(fakeNode_.prev);
        while (temp != &fakeNode_) {
            Node *new_temp = static_cast<Node *>(temp->prev);
            NodeTraits::destroy(alloc_, &temp->value);
            NodeTraits::deallocate(alloc_, temp, 1);
            temp = new_temp;
        }
        fakeNode_.prev = fakeNode_.next = &fakeNode_;
        sz_ = 0;
    }

    void push_back(const T &value) {
        Node *newNode = NodeTraits::allocate(alloc_, 1);
        try {
            NodeTraits::construct(alloc_, &newNode->value, value);
            fakeNode_.prev->next = newNode;
            newNode->prev = fakeNode_.prev;
            fakeNode_.prev = newNode;
            newNode->next = &fakeNode_;
            if (sz_ == 0) {
                fakeNode_.next = newNode;
            }
            ++sz_;
        } catch (...) {

        }
    }

    void push_front(const T &value) {
        Node *newNode = NodeTraits::allocate(alloc_, 1);
        try {
            NodeTraits::construct(alloc_, &newNode->value, value);
            newNode->next = fakeNode_.next;
            fakeNode_.next->prev = newNode;
            newNode->prev = &fakeNode_;
            fakeNode_.next = newNode;
            if (sz_ == 0) {
                fakeNode_.prev = newNode;
            }
            ++sz_;
        } catch (...) {

        }
    }

    void pop_back() {
        try {
            Node *prevNode = static_cast<Node *>(fakeNode_.prev->prev);
            NodeTraits::destroy(alloc_, static_cast<Node *>(fakeNode_.prev));
            NodeTraits::deallocate(alloc_, static_cast<Node *>(fakeNode_.prev), 1);
            prevNode->next = &fakeNode_;
            fakeNode_.prev = prevNode;
            --sz_;
        } catch (...) {

        }
    }

    void pop_front() {
        try {
            Node *nextNode = static_cast<Node *>(fakeNode_.next->next);
            NodeTraits::destroy(alloc_, static_cast<Node *>(fakeNode_.next));
            NodeTraits::deallocate(alloc_, static_cast<Node *>(fakeNode_.next), 1);
            nextNode->prev = &fakeNode_;
            fakeNode_.next = nextNode;
            --sz_;
        } catch (...) {

        }
    }

    Alloc get_allocator() const {
        return alloc_;
    }

    size_t size() const {
        return sz_;
    }

    template<bool is_const>
    class base_iterator {

    public:
        using node_type = typename std::conditional<is_const, const BaseNode *, BaseNode *>::type;
        node_type ptr_;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename std::conditional<is_const, const T, T>::type;
        using difference_type = int;
        using pointer = typename std::conditional<is_const, const T *, T *>::type;
        using reference = typename std::conditional<is_const, const T &, T &>::type;

        base_iterator() : ptr_(nullptr) {}

        base_iterator(node_type ptr) : ptr_(ptr) {}

        base_iterator(const base_iterator<false> &other) {
            ptr_ = other.ptr_;
        }

//        base_iterator(const base_iterator<true>& other) {
//            ptr_ = other.ptr_;
//        }

        base_iterator &operator=(const base_iterator<false> &other) {
            ptr_ = other.ptr_;
            return *this;
        }

//        base_iterator &operator=(const base_iterator<true> &other) {
//            ptr_ = other.ptr_;
//            return *this;
//        }

        ~base_iterator() = default;

        base_iterator &operator++() {
            ptr_ = ptr_->next;
            return *this;
        }

        base_iterator operator++(int) {
            base_iterator temp = *this;
            ptr_ = ptr_->next;
            return temp;
        }

        base_iterator operator--() {
            ptr_ = ptr_->prev;
            return *this;
        }

        base_iterator &operator--(int) {
            base_iterator temp = *this;
            ptr_ = ptr_->prev;
            return temp;
        }

        reference operator*() const {
            return static_cast<typename std::conditional<is_const, const Node *, Node *>::type>(ptr_)->value;
        }

        pointer operator->() {
            return &ptr_;
        }

        bool operator==(const base_iterator &other) const {
            return (ptr_ == other.ptr_);
        }

        bool operator!=(const base_iterator &other) const {
            return !(ptr_ == other.ptr_);
        }
    };

    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return iterator(static_cast<BaseNode *>(fakeNode_.next));
    }

    iterator end() {
        return iterator(&fakeNode_);
    }

    const_iterator begin() const {
        return const_iterator(fakeNode_.next);
    }

    const_iterator end() const {
        return const_iterator(&fakeNode_);
    }

    const_iterator cbegin() {
        return const_iterator(fakeNode_.next);
    }

    const_iterator cend() {
        return const_iterator(&fakeNode_);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crend() {
        return const_reverse_iterator(begin());
    }

    void insert(iterator iter, const T &element) {
        iterator prev = iter.ptr_->prev;
        Node *newNode = NodeTraits::allocate(alloc_, 1);
        NodeTraits::construct(alloc_, &(newNode->value), element);
        prev.ptr_->next = newNode;
        iter.ptr_->prev = newNode;
        newNode->next = iter.ptr_;
        newNode->prev = prev.ptr_;
        ++sz_;
    }

    void erase(iterator iter) {
        iterator next = iter.ptr_->next;
        iterator prev = iter.ptr_->prev;
        next.ptr_->prev = prev.ptr_;
        prev.ptr_->next = next.ptr_;
        NodeTraits::destroy(alloc_, &(static_cast<Node *>(iter.ptr_)->value));
        NodeTraits::deallocate(alloc_, static_cast<Node *>(iter.ptr_), 1);
        --sz_;
    }

    void insert(const_iterator iter, const T &element) {
        insert(static_cast<iterator>(const_cast<BaseNode *>(iter.ptr_)), element);
    }

    void erase(const_iterator iter) {
        erase(static_cast<iterator>(const_cast<BaseNode *>(iter.ptr_)));
    }
};