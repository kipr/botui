#ifndef _SINGLETON_H_
#define _SINGLETON_H_

/*! \class Singleton
 * \brief Implements a simple Singleton Pattern
 */
template <typename T> class Singleton {
  public:
    /*! \return Reference to singleton object */
    static T &ref() {
        static T instance;
        return instance;
    }

    static T *ptr() { return &ref(); }

  protected:
    Singleton() {}
    Singleton(const Singleton &) {}
    ~Singleton() {}
};

#endif
