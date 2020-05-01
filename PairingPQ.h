// Project identifier: 0E04A31E0D60C01986ACB20081C9D8722A1899B6

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include <deque>
#include <utility>
#include "Eecs281PQ.h"

// A specialized version of the 'priority_queue' ADT implemented as a pairing
// heap.
template <typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
  // This is a way to refer to the base class object.
  using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

 public:
  // Each node within the pairing heap
  class Node {
   public:
    // TODO: After you add add one extra pointer (see below), be sure to
    // initialize it here.
    explicit Node(const TYPE& val)
        : elt{val}, child{nullptr}, sibling{nullptr}, prev{nullptr} {}

    // Description: Allows access to the element at that Node's position.
    // There are two versions, getElt() and a dereference operator, use
    // whichever one seems more natural to you.
    // Runtime: O(1) - this has been provided for you.
    const TYPE& getElt() const { return elt; }
    const TYPE& operator*() const { return elt; }

    // The following line allows you to access any private data members of this
    // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
    // statement in PairingPQ's add_node() function).
    friend PairingPQ;

   private:
    TYPE elt;
    Node* child;
    Node* sibling;
    Node* prev;
    // TODO: Add one extra pointer (parent or previous) as desired.
  };  // Node

  // Description: Construct an empty priority_queue with an optional comparison
  // functor. Runtime: O(1)
  explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR())
      : BaseClass{comp} {}  // PairingPQ()

  // Description: Construct a priority_queue out of an iterator range with an
  // optional
  //              comparison functor.
  // Runtime: O(n) where n is number of elements in range.
  // TODO: when you implement this function, uncomment the parameter names.
  template <typename InputIterator>
  PairingPQ(InputIterator start, InputIterator end,
            COMP_FUNCTOR comp = COMP_FUNCTOR())
      : BaseClass{comp} {
    while (start != end) {
      Node* n = new Node(*start);
      root = meld(root, n);
      start++;
      _size++;
    }
  }  // PairingPQ()

  // Description: Copy constructor.
  // Runtime: O(n)
  PairingPQ(const PairingPQ& other) : BaseClass{other.compare} {
    std::deque<Node*> nodes;
    nodes.push_back(other.root);
    if (other.root) {
      while (!nodes.empty()) {
        Node* n = nodes.back();
        nodes.pop_back();
        if (n->child) {
          nodes.push_front(n->child);
        }
        if (n->sibling) {
          nodes.push_front(n->sibling);
        }
        root = meld(root, new Node(**n));
      }
    }
    _size = other._size;
  }  // PairingPQ()

  // Description: Copy assignment operator.
  // Runtime: O(n)
  // TODO: when you implement this function, uncomment the parameter names.
  PairingPQ& operator=(const PairingPQ& rhs) {
    PairingPQ temp(rhs);
    std::swap(root, temp.root);
    std::swap(_size, temp._size);
    return *this;
  }  // operator=()

  // Description: Destructor
  // Runtime: O(n)
  ~PairingPQ() {
    std::deque<Node*> nodes;
    nodes.push_back(root);
    if (root) {
      while (!nodes.empty()) {
        Node* n = nodes.back();
        nodes.pop_back();
        if (n->child) {
          nodes.push_front(n->child);
        }
        if (n->sibling) {
          nodes.push_front(n->sibling);
        }
        if (n) {
          delete n;
          n = nullptr;
        }
      }
    }
  }  // ~PairingPQ()

  // Description: Assumes that all elements inside the priority_queue are out of
  // order and
  //              'rebuilds' the priority_queue by fixing the priority_queue
  //              invariant.
  // Runtime: O(n)
  virtual void updatePriorities() { *this = *this; }  // updatePriorities()

  // Description: Add a new element to the priority_queue. This is almost done,
  //              in that you should implement push functionality in the
  //              addNode() function, and this function should call addNode().
  // Runtime: O(1)
  // TODO: when you implement this function, uncomment the parameter names.
  virtual void push(const TYPE& val) {
    Node* n = new Node(val);
    root = meld(root, n);
    _size++;
  }  // push()

  /*
  TYPE elt;
          Node *child;
          Node *sibling;
          Node *prev;
  */

  // Description: Remove the most extreme (defined by 'compare') element from
  //              the priority_queue.
  // Note: We will not run tests on your code that would require it to pop an
  // element when the priority_queue is empty. Though you are welcome to if you
  // are familiar with them, you do not need to use exceptions in this project.
  // Runtime: Amortized O(log(n))
  virtual void pop() {
    std::vector<Node*> nodes;
    Node* n = root->child;
    if (root) {
      delete root;
      root = nullptr;
    }
    while (n) {
      // if there is a leftover node to meld with
      if (n->sibling) {
        n = n->sibling->sibling;
        nodes.push_back(meld(n, n->sibling));
      }
      // if the next node over doesnt exist
      else {
        nodes.push_back(n);
        n = n->sibling;
      }
    }
    root = nullptr;
    while (!nodes.empty()) {
      Node* s = nodes.back();
      nodes.pop_back();
      s->sibling = nullptr;
      s->prev = nullptr;
      root = meld(root, s);
    }
    _size--;

  }  // pop()

  // Description: Return the most extreme (defined by 'compare') element of
  //              the heap.  This should be a reference for speed.  It MUST be
  //              const because we cannot allow it to be modified, as that
  //              might make it no longer be the most extreme element.
  // Runtime: O(1)
  virtual const TYPE& top() const { return root->elt; }  // top()

  // Description: Get the number of elements in the priority_queue.
  // Runtime: O(1)
  virtual std::size_t size() const { return _size; }  // _size()

  // Description: Return true if the priority_queue is empty.
  // Runtime: O(1)
  virtual bool empty() const { return _size == 0; }  // empty()

  // Description: Updates the priority of an element already in the
  // priority_queue by
  //              replacing the element refered to by the Node with new_value.
  //              Must maintain priority_queue invariants.
  //
  // PRECONDITION: The new priority, given by 'new_value' must be more extreme
  //               (as defined by comp) than the old priority.
  //
  // Runtime: As discussed in reading material.
  // TODO: when you implement this function, uncomment the parameter names.
  void updateElt(Node* node, const TYPE& new_value) {
    if (node->prev) {
      // if previous is a parent
      if (node->prev->child == node) {
        node->prev->child = node->sibling;
      } else {
        node->prev->sibling = node->sibling;
      }
      if (node->sibling) {
        node->sibling->prev = node->prev;
      }

    } else {
      return;
    }

    node->prev = nullptr;
    node->sibling = nullptr;
    node->elt = new_value;

    root = meld(root, node);
  }  // updateElt()

  // Description: Add a new element to the priority_queue. Returns a Node*
  // corresponding
  //              to the newly added element.
  // Runtime: O(1)
  // TODO: when you implement this function, uncomment the parameter names.
  // NOTE: Whenever you create a node, and thus return a Node *, you must be
  // sure to
  //       never move or copy/delete that node in the future, until it is
  //       eliminated by the user calling pop().  Remember this when you
  //       implement updateElt() and updatePriorities().
  Node* addNode(const TYPE& val) {
    Node* n = new Node(val);
    root = meld(root, n);
    _size++;
    return n;
  }  // addNode()

 private:
  // TODO: Add any additional member functions or data you require here.
  // TODO: We recommend creating a 'meld' function (see the Pairing Heap
  // papers).

  size_t _size = 0;

  Node* root = nullptr;

  // returns
  Node* meld(Node* lhs, Node* rhs) {
    if (!lhs) {
      return rhs;
    }
    if (!rhs) {
      return lhs;
    }
    // if rhs is higher priority
    if (this->compare(**lhs, **rhs)) {
      Node* rhsKid = rhs->child;
      rhs->child = lhs;
      lhs->sibling = rhsKid;
      if (rhsKid) {
        rhsKid->prev = lhs;
      }
      lhs->prev = rhs;
      return rhs;
    }
    // if lhs is higher priority
    else {
      Node* lhsKid = lhs->child;
      lhs->child = rhs;
      rhs->sibling = lhsKid;
      if (lhsKid) {
        lhsKid->prev = rhs;
      }
      rhs->prev = lhs;
      return lhs;
    }
  }
};

#endif  // PAIRINGPQ_H
