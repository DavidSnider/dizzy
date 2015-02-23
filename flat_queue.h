/* A vector based implementation of std::queue. It contains mostly
 * the same interface with a few modications.
 * 1. Since this is specifically vector based, the option of a
 *    container base is not given.
 * 2. I haven't implemented the constructors that take allocators.
 *    Maybe I'll get around to it when I'm feeling more competent.
 * 3. I have added the member function reserve_space(unsigned) to
 *    reserve space in the underlying vector to avoid reallocation
 *    during insertions.
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

namespace dizzy{
    template<typename T>
    class flat_queue;

    template<typename T>
    class flat_queue{
    private:
        using size_type = unsigned;
        using value_type = T;
    public:

        flat_queue();
        explicit flat_queue (const std::vector<value_type>& data_);
        explicit flat_queue (std::vector<value_type>&& data_);
        flat_queue (const flat_queue& x);
        flat_queue (flat_queue&& x);

        bool empty() const;
        size_type size() const;

        value_type& front();
        const value_type & front() const;
        value_type& back();
        const value_type & back() const;

        void push (const value_type& val);
        void push (value_type&& val);
        template <class... Args> void emplace(Args&&... args);

        void pop();

        void reserve_space(size_type size);

        void swap (flat_queue& x) noexcept;

    private:
        std::vector<T> data;
        size_type true_front;

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
    };

    template<typename T>
    flat_queue<T>::flat_queue() : true_front{0} {}

    template<typename T>
    flat_queue<T>::flat_queue (const std::vector<T>& data_)
        : data{data_}, true_front{0} {}

    template<typename T>
    flat_queue<T>::flat_queue (std::vector<value_type>&& data_)
        : data{std::forward< std::vector<T> >(data_)}, true_front{0} {}

    template<typename T>
    flat_queue<T>::flat_queue (const flat_queue& x)
        : data{x.data}, true_front{x.true_front} {}

    template<typename T>
    flat_queue<T>::flat_queue (flat_queue&& x)
        : data{std::forward< std::vector<T> >(x.data)},
        true_front{x.true_front} { x.true_front = 0; }

    template<typename T>
    bool flat_queue<T>::empty() const{
        return data.size() == true_front;
    }

    template<typename T>
    typename flat_queue<T>::size_type flat_queue<T>::size() const{
        return data.size() - true_front;
    }

    template<typename T>
    T& flat_queue<T>::front(){ return data[true_front]; }

    template<typename T>
    const T & flat_queue<T>::front() const{
        return data[true_front];
    }

    template<typename T>
    T& flat_queue<T>::back(){ return data.back(); }

    template<typename T>
    const T & flat_queue<T>::back() const{
        return data.back();
    }

    template<typename T>
    void flat_queue<T>::push (const T& val){
        data.push_back(val);
    }

    template<typename T>
    void flat_queue<T>::push (T&& val){
        data.push_back(std::forward<T>(val));
    }

    template <typename T> template <class... Args>
    void flat_queue<T>::emplace (Args&&... args){
        data.emplace_back(std::forward<Args>(args)...);
    }

    template<typename T>
    void flat_queue<T>::pop(){
        ++true_front;
        if(true_front > data.size()/2 ){
            std::vector<T> tempVec;
            tempVec.reserve(size() * 2);
            std::move(begin(data) + true_front, end(data), begin(tempVec));
            std::swap(data, tempVec);
            true_front = 0;
        }
    }

    template<typename T>
    void flat_queue<T>::reserve_space(flat_queue<T>::size_type size){
        data.reserve(size);
    }

    template<typename T>
    void flat_queue<T>::swap(flat_queue& x) noexcept{
        using std::swap;
        swap(data, x.data);
        swap(true_front, x.true_front);
    }

    template<typename T>
    void swap(flat_queue<T>& x, flat_queue<T>& y){
        x.swap(y);
    }

    template <typename T>
    inline bool operator== (const flat_queue<T>& lhs, const flat_queue<T>& rhs){
        if (lhs.size() - lhs.true_front != rhs.size() - rhs.true_front){
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
    inline bool operator< (const flat_queue<T>& lhs, const flat_queue<T>& rhs){
        return std::lexicographical_compare(begin(lhs.data) + lhs.true_front,
                                            end(lhs.data),
                                            begin(rhs.data) + rhs.true_front,
                                            end(rhs.data));
    }

    template <typename T>
    inline bool operator<= (const flat_queue<T>& lhs, const flat_queue<T>& rhs){
        return (lhs == rhs || lhs < rhs);

    }

    template <typename T>
    inline bool operator> (const flat_queue<T>& lhs, const flat_queue<T>& rhs){
        return std::lexicographical_compare(begin(rhs.data) + rhs.true_front,
                                            end(rhs.data),
                                            begin(lhs.data) + lhs.true_front,
                                            end(lhs.data));
    }

    template <typename T>
    inline bool operator>= (const flat_queue<T>& lhs, const flat_queue<T>& rhs){
        return (lhs == rhs || lhs > rhs);
    }

}
