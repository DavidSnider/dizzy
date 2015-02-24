/* A vector based implementation of std::queue. It contains mostly
 * the same interface with a few (okay many) modications.
 * 1. Since this is specifically vector based, the option of a
 *    container base is not given.
 * 2. I haven't implemented the constructors that take allocators.
 *       I will not be implementing these as I understand "Nobody overrides
 *       anything but the global one these days anyway"
 * 3. I have added three new member functions:
 *    - reserve(size_type): reserve space in the underlying vector
 *      to avoid reallocation during insertions. If given a size less
 *      than the current size, this does nothing, otherwise triggers
 *      a reallocation of the internal vector.
 *    - compress(double): reallocate the internal vector with
 *      allocation size proportional to size() * the argument,
 *      which has a default value of 2.
 *    - shrink_to_fit(): equivilant to compress(1). Analogous
 *      to the equivilant vector public member function
 * 4. I use std::equal and std::lexicographical_compare for the
 *    implementations of the relational operators as due to the
 *    possibly different undefined spaces prior to the beginning
 *    of the queue, just comparing vectors would not work.
 */


#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <iterator>
#include <cmath>

namespace dizzy{

    template<typename T>
    class flat_queue{
    public:

        using size_type = std::size_t;
        using container = std::vector<T>;
        using value_type = T;
        using pointer = typename container::pointer;
        using const_pointer = typename container::const_pointer;
        using reference = typename container::reference;
        using const_reference = typename container::const_reference;

        using iterator = container::iterator;
        using const_iterator = container::const_iterator;
        using reverse_iterator = container::reverse_iterator;
        using const_reverse_iterator = container::const_reverse_iterator;

        flat_queue();
        explicit flat_queue(const container& data_);
        explicit flat_queue(container&& data_);
        flat_queue(const flat_queue& x);
        flat_queue(flat_queue&& x);

        bool empty() const;
        size_type size() const;

        reference& front();
        const_reference& front() const;
        reference& back();
        const_reference& back() const;

        void push(const value_type& val);
        void push(value_type&& val);
        template <class... Args> void emplace(Args&&... args);

        void pop();

        void shrink_to_fit();
        void reserve(size_type new_size);
        void compress(double mult_factor = 2.0);

        void swap(flat_queue& x) noexcept;

        iterator begin() noexcept;
        const_iterator begin() const noexcept;
        iterator end() noexcept;
        const_iterator end() const noexcept;
        reverse_iterator rbegin() noexcept;
        const_reverse_iterator rbegin() const noexcept;
        reverse_iterator rend() noexcept;
        const_reverse_iterator rend() const noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;
        const_reverse_iterator crbegin() const noexcept;
        const_reverse_iterator crend() const noexcept;

        template<typename U>
        friend bool operator== (const flat_queue<U>& lhs,
                                const flat_queue<U>& rhs);
        template<typename U>
        friend bool operator!= (const flat_queue<U>& lhs,
                                const flat_queue<U>& rhs);
        template<typename U>
        friend bool operator<  (const flat_queue<U>& lhs,
                                const flat_queue<U>& rhs);
        template<typename U>
        friend bool operator<= (const flat_queue<U>& lhs,
                                const flat_queue<U>& rhs);
        template<typename U>
        friend bool operator>  (const flat_queue<U>& lhs,
                                const flat_queue<U>& rhs);
        template<typename U>
        friend bool operator>= (const flat_queue<U>& lhs,
                                const flat_queue<U>& rhs);

    private:
        container data;
        size_type true_front;

    };

    template<typename T>
    flat_queue<T>::flat_queue() : true_front{0} {}

    template<typename T>
    flat_queue<T>::flat_queue(const container& data_)
        : data{data_}, true_front{0} {}

    template<typename T>
    flat_queue<T>::flat_queue(container&& data_)
        : data{std::move(data_)}, true_front{0} {}

    template<typename T>
    flat_queue<T>::flat_queue(const flat_queue& x)
        : data{x.data}, true_front{x.true_front} {}

    template<typename T>
    flat_queue<T>::flat_queue(flat_queue&& x)
        : data{std::move(x.data)}, true_front{x.true_front} { x.true_front = 0;}

    template<typename T>
    bool flat_queue<T>::empty() const {
        return data.size() == true_front;
    }

    template<typename T>
    typename flat_queue<T>::size_type flat_queue<T>::size() const {
        return data.size() - true_front;
    }

    template<typename T>
    typename flat_queue<T>::reference& flat_queue<T>::front() {
        return data[true_front];
    }

    template<typename T>
    typename flat_queue<T>::const_reference& flat_queue<T>::front() const {
        return data[true_front];
    }

    template<typename T>
    typename flat_queue<T>::reference& flat_queue<T>::back() {
        return data.back();
    }

    template<typename T>
    typename flat_queue<T>::const_reference& flat_queue<T>::back() const {
        return data.back();
    }

    template<typename T>
    void flat_queue<T>::push(const T& val) { data.push_back(val); }

    template<typename T>
    void flat_queue<T>::push(T&& val) { data.push_back( std::move(val) ); }

    template <typename T> template <class... Args>
    void flat_queue<T>::emplace(Args&&... args) {
        data.emplace_back( std::forward<Args>(args)... );
    }

    template<typename T>
    void flat_queue<T>::pop() {
        ++true_front;
        if( true_front > data.size() / 2 ){
            compress();
        }
    }

    template<typename T>
    void flat_queue<T>::shrink_to_fit() { compress(1.0); }

    template<typename T>
    void flat_queue<T>::reserve(flat_queue<T>::size_type new_size) {
        if(empty()){
            data.reserve(new_size);
        } else{
            compress( new_size / static_cast<double>( size() ) );
        }
    }

    template<typename T>
    void flat_queue<T>::compress(double mult_factor) {
        container tempContainer;
        tempContainer.reserve( ceil(size() * mult_factor) );
        std::move( begin(data) + true_front, end(data),
                  std::back_inserter(tempContainer) );
        std::swap(data, tempContainer);
        true_front = 0;
    }

    template<typename T>
    void flat_queue<T>::swap(flat_queue& x) noexcept {
        using std::swap;
        swap(data, x.data);
        swap(true_front, x.true_front);
    }

    template<typename T>
    void swap(flat_queue<T>& x, flat_queue<T>& y) {
        x.swap(y);
    }

    template <typename T> flat_queue<T>::iterator
    flat_queue<T>::begin() noexcept{ return begin(data) + true_front; }

    template <typename T> flat_queue<T>::const_iterator
    flat_queue<T>::begin() const noexcept{ return cbegin(data) + true_front; }

    template <typename T> flat_queue<T>::iterator
    flat_queue<T>::end() noexcept{ return end(data); }

    template <typename T> flat_queue<T>::const_iterator
    flat_queue<T>::end() const noexcept{ return cend(data); }

    template <typename T> flat_queue<T>::reverse_iterator
    flat_queue<T>::rbegin() noexcept{

    }

    template <typename T> flat_queue<T>::const_reverse_iterator
    flat_queue<T>::rbegin() const noexcept{

    }

    template <typename T> flat_queue<T>::reverse_iterator
    flat_queue<T>::rend() noexcept{

    }

    template <typename T> flat_queue<T>::const_reverse_iterator
    flat_queue<T>::rend() const noexcept{

    }

    template <typename T> flat_queue<T>::const_iterator
    flat_queue<T>::cbegin() const noexcept{

    }

    template <typename T> flat_queue<T>::const_iterator
    flat_queue<T>::cend() const noexcept{

    }

    template <typename T> flat_queue<T>::const_reverse_iterator
    flat_queue<T>::crbegin() const noexcept{

    }

    template <typename T> flat_queue<T>::const_reverse_iterator
    flat_queue<T>::crend() const noexcept{

    }

    template <typename T>
    inline bool operator== (const flat_queue<T>& lhs, const flat_queue<T>& rhs){
        if (lhs.size() != rhs.size()){
            return false;
        }
        return std::equal(begin(lhs.data) + lhs.true_front, end(lhs.data),
                          begin(rhs.data) + rhs.true_front);
    }

    template <typename T>
    inline bool operator!= (const flat_queue<T>& lhs, const flat_queue<T>& rhs){
        return !(lhs == rhs);
    }

    template <typename T>
    inline bool operator< (const flat_queue<T>& lhs, const flat_queue<T>& rhs) {
        return std::lexicographical_compare(begin(lhs.data) + lhs.true_front,
                                            end(lhs.data),
                                            begin(rhs.data) + rhs.true_front,
                                            end(rhs.data));
    }

    template <typename T>
    inline bool operator<= (const flat_queue<T>& lhs, const flat_queue<T>& rhs){
        return !(rhs < lhs);

    }

    template <typename T>
    inline bool operator> (const flat_queue<T>& lhs, const flat_queue<T>& rhs) {
        return rhs < lhs;
    }

    template <typename T>
    inline bool operator>= (const flat_queue<T>& lhs, const flat_queue<T>& rhs){
        return !(lhs < rhs);
    }

}
