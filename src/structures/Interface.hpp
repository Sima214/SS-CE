#ifndef SSCE_INTERFACE_HPP
#define SSCE_INTERFACE_HPP
/**
 * @file
 * @brief Interface for data structures elements.
 */

#include <Macros.h>
C_DECLS_START
#include <Interface.h>
C_DECLS_END

#include <cstddef>
#include <functional>

namespace ssce {

/**
 * Cpp->C bridge class for \ref IDataType.
 */
template<class T>
class IDataTypeCpp {
 private:
  /**
   * Maps directly to \ref IDataType.cmp_eq
   * Override to change default behaviour.
   */
  virtual int compare_equal(const IDataType* idt, const void* ap, const void* bp) {
    // Convert pointers to references.
    T& a = *((T*)ap);
    T& b = *((T*)bp);
    return std::equal<T>()(a, b);
  }
  /**
   * Maps directly to \ref IDataType.cmp_l
   * Override to change default behaviour.
   */
  virtual int compare_less(const IDataType* idt, const void* ap, const void* bp) {
    // Convert pointers to references.
    T& a = *((T*)ap);
    T& b = *((T*)bp);
    return std::less<T>()(a, b);
  }
  /**
   * Maps directly to \ref IDataType.cmp_le
   * Override to change default behaviour.
   */
  virtual int compare_less_equal(const IDataType* idt, const void* ap, const void* bp) {
    // Convert pointers to references.
    T& a = *((T*)ap);
    T& b = *((T*)bp);
    return std::less_equal<T>()(a, b);
  }
  /**
   * Maps directly to \ref IDataType.swap
   * Override to change default behaviour.
   */
  virtual void operation_swap(const IDataType* idt, void* ap, void* bp) {
    // Convert pointers to references.
    T& ar = *((T*)ap);
    T& br = *((T*)bp);
    T a = ar;
    T b = br;
    ar = b;
    br = a;
  }
  /**
   * Maps directly to \ref IDataType.hash
   * Override to change default behaviour.
   */
  virtual std::size_t operation_hash(const IDataType* idt, const void* kp) {
    // Convert pointers to references.
    T& k = *((T*)kp);
    return std::hash<T>()(k);
  }
 public:
  /**
   * Constructs an \ref IDataType from the current template.
   */
  MARK_CONST IDataType constructIDataType() {
    IDataType ret = { sizeof(T),
                      0,
                      sizeof(T),
                      this->compare_equal,
                      this->compare_less,
                      this->compare_less_equal,
                      this->operation_less
    }
    return ret;
  }
}

}  // namespace ssce
#endif /*SSCE_INTERFACE_HPP*/