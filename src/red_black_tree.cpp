
#include "/home/jeannoel/Algo/jnstl/include/JNSTL/bits/config.h"
#include "/home/jeannoel/Algo/jnstl/include/JNSTL/red_black_tree.h"

namespace jnstl {

bool
rbtree_is_left_child(const rbtree_node_base* x){
  return x == x->mParent->mLeft;
}

bool
node_is_rbtree_header(const rbtree_node_base* x) {
  // both root and mHeader satisfy the first statement
  // mHeader is mRed, root is black
  return x->mParent->mParent == x && x->mColor == mRed;
}

rbtree_node_base*
rbtree_min(const rbtree_node_base* x) {
   while (x->mLeft != nullptr)
      x = x->mLeft;
   return const_cast<rbtree_node_base*>(x);
}

rbtree_node_base*
rbtree_max(const rbtree_node_base* x) {
  while (x->mRight != nullptr)
    x = x->mRight;
  return const_cast<rbtree_node_base*>(x);
}

void
rbtree_rotate_left(rbtree_node_base* x, rbtree_node_base*& root) {
  rbtree_node_base* y = x->mRight;

  x->mRight = y->mLeft;

  if (y->mLeft != nullptr)
    y->mLeft->mParent = x;

  y->mParent = x->mParent;

  if (x == root)
    root = y;
  else if (rbtree_is_left_child(x))
    x->mParent->mLeft = y;
  else
    x->mParent->mRight = y;

  y->mLeft = x;
  x->mParent = y;
}

void
rbtree_rotate_right(rbtree_node_base* x, rbtree_node_base*& root) {
  rbtree_node_base* y = x->mLeft;
  x->mLeft = y->mRight;

  if (y->mRight != nullptr)
    y->mRight->mParent = x;
  
  y->mParent = x->mParent;

  if (x == root) 
    root = y;
  else if (!rbtree_is_left_child(x))
    x->mParent->mRight = y;
  else
    x->mParent->mLeft = y;

  y->mRight  = x;
  x->mParent = y;
}

void
rbtree_transplant(rbtree_node_base* u, rbtree_node_base* v,
                  rbtree_node_base* header) {
  rbtree_node_base*& root = header->mParent;

  if (u == root)
    root = v;
  else if (rbtree_is_left_child(u))
    u->mParent->mLeft = v;
  else
    u->mParent->mRight = v;

  if (v != nullptr)
    v->mParent = u->mParent;
}

void
rbtree_delete_fixup(rbtree_node_base* x,
                    rbtree_node_base* y,
                    rbtree_node_base* header) {
  rbtree_node_base*& root = header->mParent;
  
  while (x != root && (x == nullptr || x->mColor == mBlack)) {
    if (x == y->mLeft) { //rbtree_is_left_child(x)) need to avoid case x == 0
      rbtree_node_base* w = y->mRight;
      if (w->mColor == mRed) {
        w->mColor = mBlack;
        y->mColor = mRed;
        rbtree_rotate_left(y, root);
        w = y->mRight;
      }
      if ((w->mLeft == nullptr || w->mLeft->mColor == mBlack) &&
          (w->mRight == nullptr || w->mRight->mColor == mBlack)) {
        w->mColor = mRed;
        x = y;
        y = y->mParent;
      } else {
        if (w->mRight == nullptr || w->mRight->mColor == mBlack) {
          w->mLeft->mColor = mBlack;
          w->mColor = mRed;
          rbtree_rotate_right(w, root);
          w = y->mRight;
        }
        w->mColor = y->mColor;
        y->mColor = mBlack;
        if (w->mRight != nullptr)
          w->mRight->mColor = mBlack;
        rbtree_rotate_left(y, root);
        
        //x = root;
        // if we reach here it's guaranteed to solved;
        break;
      }
    } else {
      rbtree_node_base* w = y->mLeft;
      if (w->mColor == mRed) {
        w->mColor = mBlack;
        y->mColor = mRed;
        rbtree_rotate_right(y, root);
        w = y->mLeft;
      }
      if ((w->mLeft == nullptr || w->mLeft->mColor == mBlack) &&
          (w->mRight == nullptr || w->mRight->mColor == mBlack)) {
        w->mColor = mRed;
        x = y;
        y = y->mParent;
      } else {
        if (w->mLeft == nullptr || w->mLeft->mColor == mBlack) {
          w->mRight->mColor = mBlack;
          w->mColor = mRed;
          rbtree_rotate_left(w, root);
          w = y->mLeft;
        }
        w->mColor = y->mColor;
        y->mColor = mBlack;
        if (w->mLeft != nullptr)
          w->mLeft->mColor = mBlack;
        rbtree_rotate_right(y, root);

        //x = root;
        // if we reach here it's guaranteed to solved;
        break;
      }
    }
  }

  if (x != nullptr)
    x->mColor = mBlack;
}

rbtree_node_base*
RBTreeIncrement(const rbtree_node_base* x) {

  if (x->mRight != nullptr) {
    x = rbtree_min(x->mRight);
  } else {
    rbtree_node_base* y = x->mParent;
    while (x == y->mRight) {
      x = y;
      y = y->mParent;
    }
    // if we start from highest value in tree x will have reached mHeader
    // return x = mHeader = end().
    // For all other cases return x->mParent;
    // Check x not mHeader.
    if (x->mRight != y)
      x = y;
  }
  return const_cast<rbtree_node_base*>(x);
}

rbtree_node_base*
RBTreeDecrement(const rbtree_node_base* x) {
  // if x == end() == mHeader iterator is decremented to tree max value
  // mHeader->mRight;
  if (node_is_rbtree_header(x)) {
    x = x->mRight;
  } else if (x->mLeft != nullptr) {
    x = rbtree_max(x->mLeft);
  } else { // if x was lowest value x will reach the root of the tree
           // and return x->mParent = mHeader = end()
    rbtree_node_base* y = x->mParent;
    while(x == y->mLeft) {
      x = y;
      y = y->mParent;
    }
    x = y;
  }
  return const_cast<rbtree_node_base*>(x);
}

size_t RBTreeBlackCount(rbtree_node_base* pTop,
                           rbtree_node_base* pBottom) {
  size_t nCount = 0;

  for (; pBottom; pBottom = pBottom->mParent) {
    if(pBottom->mColor == mBlack) 
      ++nCount;

    if(pBottom == pTop) 
      break;
  }
  return nCount;
}

void
RBTreeInsert(rbtree_node_base* pNew, rbtree_node_base* pParent,
             rbtree_node_base* pHeader, bool insert_left) {
  rbtree_node_base*& pRoot = pHeader->mParent;

  pNew->mParent = pParent;
  pNew->mLeft   = nullptr;
  pNew->mRight  = nullptr;
  pNew->mColor  = mRed;

  if (insert_left) {
    pParent->mLeft = pNew;  // implicitly also sets mLeftMost = pNew
                            // when pParent == pHeader
    if (pParent == pHeader) {
      pHeader->mParent = pNew;
      pHeader->mRight  = pNew;
    } else if (pParent == pHeader->mLeft) {
      pHeader->mLeft = pNew;
    }
  } else {
    pParent->mRight = pNew;
    if (pParent == pHeader->mRight)
      pHeader->mRight = pNew;
  }

  pNew->mColor = (pNew == pRoot) ? mBlack : mRed;
  while(pNew != pRoot  && pNew->mParent->mColor == mRed) {
    rbtree_node_base* const pParentParent = pNew->mParent->mParent;

    if (rbtree_is_left_child(pNew->mParent)) {
      rbtree_node_base* pTemp = pParentParent->mRight;

      if (pTemp != nullptr && pTemp->mColor == mRed) {
        pNew->mParent->mColor = mBlack;
                pTemp->mColor = mBlack;
        pParentParent->mColor = mRed;
        pNew = pParentParent;
      } else {
        if (!rbtree_is_left_child(pNew)) { //might need something here for root
          pNew = pNew->mParent;
          rbtree_rotate_left(pNew, pRoot);
        }
        pNew->mParent->mColor = mBlack;
        pParentParent->mColor = mRed;
        rbtree_rotate_right(pParentParent, pRoot);
      }
    } else {
      rbtree_node_base* pTemp = pParentParent->mLeft;
      
      if (pTemp != nullptr && pTemp->mColor == mRed) {
        pNew->mParent->mColor = mBlack;
                pTemp->mColor = mBlack;
        pParentParent->mColor = mRed;
        pNew = pParentParent;
      } else {
        if (rbtree_is_left_child(pNew)) {
          pNew = pNew->mParent;
          rbtree_rotate_right(pNew, pRoot);
        }
        pNew->mParent->mColor = mBlack;
        pParentParent->mColor = mRed;
        rbtree_rotate_left(pParentParent, pRoot);
      }
    }
  }
  pRoot->mColor = mBlack;
}

void
RBTreeErase(rbtree_node_base* z , rbtree_node_base* pHeader) {
  // z has two children possibily null
  // y is z successor located in z right subtree  
  rbtree_node_base*&  left_most = pHeader->mLeft;
  rbtree_node_base*& right_most = pHeader->mRight;
  rbtree_node_base* y = z;
  rbtree_node_base* x = nullptr;
  rbtree_node_base* xparent = nullptr;
  rbtree_color yColor = y->mColor;
  
  if (z->mLeft == nullptr) {
    x = z->mRight; // saving x is useful only for rebalancing later
    xparent = z->mParent;
    rbtree_transplant(z, z->mRight, pHeader);
  } else if (z->mRight == nullptr) {
    x = z->mLeft; // saving x is useful only for rebalancing later
    xparent = z->mParent;
    rbtree_transplant(z, z->mLeft, pHeader);
  } else {
    y = rbtree_min(z->mRight);
    yColor = y->mColor;
    x = y->mRight;
    // saving x is useful only for rebalancing later
    // if at bottom of tree x could be nullptr
    if (y->mParent == z) {
      xparent = y;
      if (x != nullptr)
        x->mParent = y;
    } else {
      //if (y->mRight != nullptr) {
      xparent = y->mParent;
      rbtree_transplant(y, y->mRight, pHeader);
      y->mRight = z->mRight;
      y->mRight->mParent = y;
      //} else {
      //y->mParent->mRight = nullptr;
      //}
    }
    rbtree_transplant(z, y, pHeader);
    y->mLeft = z->mLeft;
    y->mLeft->mParent = y;
    y->mColor = z->mColor;
  }


  if (z == left_most) {  // node removed was smallest, find new smallest
    if (z->mRight != nullptr)
      left_most = rbtree_min(z->mRight);
    else
      left_most = z->mParent;
  }
  if (z == right_most) { // node removed was biggest, find new biggest
    if (z->mLeft != nullptr)
      right_most = rbtree_max(z->mLeft);
    else
      right_most = z->mParent;
  }

  if (yColor == mBlack)
    rbtree_delete_fixup(x, xparent, pHeader);
}

}  // namespace_jnstl
