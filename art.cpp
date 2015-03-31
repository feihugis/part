#include "art.hpp"

template <typename V>
int Leaf<V>::count = 0;
template <typename V>
int ArtNode4<V>::count = 0;
template <typename V>
int ArtNode16<V>::count = 0;
template <typename V>
int ArtNode48<V>::count = 0;
template <typename V>
int ArtNode256<V>::count = 0;
#ifdef USE_VARNODE
template <typename V>
int ArtVarNode<V>::count = 0;
#endif

template<typename V>
Node<V>* Node<V>::clone(const Node<V>* n) {
    if (!n) return NULL;

    switch (n->type) {
    case NODE4: return new ArtNode4<V>(*static_cast<const ArtNode4<V>*>(n));
    case NODE16: return new ArtNode16<V>(*static_cast<const ArtNode16<V>*>(n));
    case NODE48: return new ArtNode48<V>(*static_cast<const ArtNode48<V>*>(n));
    case NODE256: return new ArtNode256<V>(*static_cast<const ArtNode256<V>*>(n));
    case LEAF: return new Leaf<V>(*static_cast<const Leaf<V>*>(n));
#ifdef USE_VARNODE
    case VARNODE: {
        const ArtVarNode<V>* n_var = static_cast<const ArtVarNode<V>*>(n);
        void* place = malloc(offsetof(class ArtVarNode<V>, children[n_var->capacity]));
        return new(place) ArtVarNode<V>(n_var->capacity, *n_var);
    }
#endif
    }

    abort();
}

template<typename V>
const Leaf<V>* Node<V>::minimum(const Node<V>* n) {
    if (!n) return NULL;

    switch (n->type) {
    case NODE4: return static_cast<const ArtNode4<V>*>(n)->minimum();
    case NODE16: return static_cast<const ArtNode16<V>*>(n)->minimum();
    case NODE48: return static_cast<const ArtNode48<V>*>(n)->minimum();
    case NODE256: return static_cast<const ArtNode256<V>*>(n)->minimum();
    case LEAF: return static_cast<const Leaf<V>*>(n)->minimum();
#ifdef USE_VARNODE
    case VARNODE: return static_cast<const ArtVarNode<V>*>(n)->minimum();
#endif
    }

    abort();
}

template<typename V>
void Node<V>::insert(Node<V>* n, Node<V>** ref, const unsigned char* key, int key_len, V value,
                     int depth, bool force_clone) {
    // If we are at a NULL node, inject a leaf
    if (!n) {
        return Node<V>::switch_ref(ref, new Leaf<V>(key, key_len, value));
    } else {
        switch(n->type) {
        case NODE4: case NODE16: case NODE48: case NODE256: case VARNODE:
            return static_cast<ArtNode<V>*>(n)->insert(ref, key, key_len, value, depth, force_clone);
        case LEAF:
            return static_cast<Leaf<V>*>(n)->insert(ref, key, key_len, value, depth, force_clone);
        }
        abort();
    }
}

template<typename V>
void Node<V>::iter(Node<V>* n, std::function<void(const unsigned char*, uint32_t, V)> cb) {
    if (!n) return;

    switch (n->type) {
    case NODE4: return static_cast<ArtNode4<V>*>(n)->iter(cb);
    case NODE16: return static_cast<ArtNode16<V>*>(n)->iter(cb);
    case NODE48: return static_cast<ArtNode48<V>*>(n)->iter(cb);
    case NODE256: return static_cast<ArtNode256<V>*>(n)->iter(cb);
    case LEAF: return static_cast<Leaf<V>*>(n)->iter(cb);
#ifdef USE_VARNODE
    case VARNODE: return static_cast<ArtVarNode<V>*>(n)->iter(cb);
#endif
    }

    abort();
}

template<typename V>
void Node<V>::iter_nodes(Node<V>* n, std::function<bool(Node*, int)> cb, int depth) {
    if (!n) return;

    switch (n->type) {
    case NODE4: return static_cast<ArtNode4<V>*>(n)->iter_nodes(cb, depth);
    case NODE16: return static_cast<ArtNode16<V>*>(n)->iter_nodes(cb, depth);
    case NODE48: return static_cast<ArtNode48<V>*>(n)->iter_nodes(cb, depth);
    case NODE256: return static_cast<ArtNode256<V>*>(n)->iter_nodes(cb, depth);
    case LEAF: return static_cast<Leaf<V>*>(n)->iter_nodes(cb, depth);
    }

    abort();
}

template<typename V>
int Node<V>::node_size(Node<V>* n) {
    if (!n) return 0;

    switch (n->type) {
    case NODE4: return sizeof(ArtNode4<V>);
    case NODE16: return sizeof(ArtNode16<V>);
    case NODE48: return sizeof(ArtNode48<V>);
    case NODE256: return sizeof(ArtNode256<V>);
    case LEAF: return sizeof(Leaf<V>) + static_cast<Leaf<V>*>(n)->key_len + sizeof(V);
    }

    abort();
}

template<typename V>
int Node<V>::decrement_refcount(Node<V>* n) {
    if (!n) return 0;

    switch (n->type) {
    case NODE4: return static_cast<ArtNode4<V>*>(n)->decrement_refcount();
    case NODE16: return static_cast<ArtNode16<V>*>(n)->decrement_refcount();
    case NODE48: return static_cast<ArtNode48<V>*>(n)->decrement_refcount();
    case NODE256: return static_cast<ArtNode256<V>*>(n)->decrement_refcount();
    case LEAF: return static_cast<Leaf<V>*>(n)->decrement_refcount();
#ifdef USE_VARNODE
    case VARNODE: return static_cast<ArtVarNode<V>*>(n)->decrement_refcount();
#endif
    }

    abort();
}

template<typename V>
Node<V>** ArtNode<V>::find_child(ArtNode<V>* n, unsigned char c) {
    if (!n) return NULL;

    switch (n->type) {
    case NODE4: return static_cast<ArtNode4<V>*>(n)->find_child(c);
    case NODE16: return static_cast<ArtNode16<V>*>(n)->find_child(c);
    case NODE48: return static_cast<ArtNode48<V>*>(n)->find_child(c);
    case NODE256: return static_cast<ArtNode256<V>*>(n)->find_child(c);
#ifdef USE_VARNODE
    case VARNODE: return static_cast<ArtVarNode<V>*>(n)->find_child(c);
#endif
    }

    abort();
}

template<typename V>
void ArtNode<V>::add_child(ArtNode<V>* n, Node<V>** ref, unsigned char c, Node<V>* child,
                           bool force_clone) {
    if (!n) return;

    switch (n->type) {
    case NODE4: return static_cast<ArtNode4<V>*>(n)->add_child(ref, c, child, force_clone);
    case NODE16: return static_cast<ArtNode16<V>*>(n)->add_child(ref, c, child, force_clone);
    case NODE48: return static_cast<ArtNode48<V>*>(n)->add_child(ref, c, child, force_clone);
    case NODE256: return static_cast<ArtNode256<V>*>(n)->add_child(ref, c, child, force_clone);
#ifdef USE_VARNODE
    case VARNODE: return static_cast<ArtVarNode<V>*>(n)->add_child(ref, c, child, force_clone);
#endif
    }

    abort();
}

template<typename V>
void Leaf<V>::insert(Node<V>** ref, const unsigned char *key,
                     int key_len, V value, int depth,
                     bool force_clone) {
    bool clone = force_clone || this->refcount > 1;
    // Need to replace this leaf with a node
    if (!matches(key, key_len)) {
        if (clone) {
            // Updating an existing value, but need to create a new leaf to
            // reflect the change
            Node<V>::switch_ref(ref, new Leaf<V>(key, key_len, value));
        } else {
            // Updating an existing value, and safe to make the change in
            // place
            this->value = value;
            this->checkpointed = false;
        }
    } else {
        // New value, we must split the leaf into a node4
        auto result = new ArtNode4<V>;
        auto ref_old = *ref;
        Node<V>::switch_ref_no_decrement(ref, result);

        // Create a new leaf
        Leaf<V>* l2 = new Leaf<V>(key, key_len, value);

        // Determine longest prefix
        int longest_prefix = longest_common_prefix(l2, depth);
        result->partial_len = longest_prefix;
        memcpy(result->partial, key + depth, min(MAX_PREFIX_LEN, longest_prefix));
        // Add the leafs to the new node4
        result->add_child(ref, this->key[depth + longest_prefix], this, false);
        result->add_child(ref, l2->key[depth+longest_prefix], l2, false);

        Node<V>::decrement_refcount(ref_old);

        // TODO: avoid the increment to self immediately followed by decrement
    }
}

template<typename V>
void ArtNode<V>::insert(Node<V>** ref, const unsigned char *key,
                        int key_len, V value, int depth,
                        bool force_clone) {
    bool do_clone = force_clone || this->refcount > 1;

    // Check if given node has a prefix
    if (partial_len) {
        // Determine if the prefixes differ, since we need to split
        int prefix_diff = prefix_mismatch(key, key_len, depth);
        if ((uint32_t)prefix_diff >= partial_len) {
            depth += partial_len;
            goto RECURSE_SEARCH;
        }
        if (debug) std::cout << "Splitting at depth " << depth << "\n";
        if (debug) std::cout << "prefix_diff " << prefix_diff << "\n";
        if (debug) std::cout << "partial_len " << partial_len << "\n";

        // Create a new node
        auto result = new ArtNode4<V>;
        auto ref_old = *ref;
        Node<V>::switch_ref_no_decrement(ref, result); // don't decrement yet,
                                                       // because doing so might
                                                       // destroy self
        result->partial_len = prefix_diff;
        memcpy(result->partial, partial, min(MAX_PREFIX_LEN, prefix_diff));

        // Adjust the prefix of the old node
        ArtNode<V>* this_writable = do_clone ? static_cast<ArtNode<V>*>(Node<V>::clone(this)) : this;
        if (partial_len <= MAX_PREFIX_LEN) {
            result->add_child(ref, this_writable->partial[prefix_diff], this_writable, false);
            this_writable->partial_len -= (prefix_diff + 1);
            memmove(this_writable->partial, this_writable->partial + prefix_diff + 1,
                    min(MAX_PREFIX_LEN, this_writable->partial_len));
        } else {
            this_writable->partial_len -= (prefix_diff+1);
            const Leaf<V>* l = Node<V>::minimum(this);
            result->add_child(ref, l->key[depth + prefix_diff], this_writable, false);
            memcpy(this_writable->partial, l->key + depth + prefix_diff + 1,
                   min(MAX_PREFIX_LEN, this_writable->partial_len));
        }

        this_writable->checkpointed = false;

        // Insert the new leaf
        auto l = new Leaf<V>(key, key_len, value);
        result->add_child(ref, key[depth + prefix_diff], l, false);

        Node<V>::decrement_refcount(ref_old);

        return;
    }

RECURSE_SEARCH:;

    // Clone self if necessary
    ArtNode<V>* this_writable = do_clone ? static_cast<ArtNode<V>*>(Node<V>::clone(this)) : this;
    this_writable->checkpointed = false;
    if (do_clone) {
        Node<V>::switch_ref(ref, this_writable);
    }
    // Do the insert, either in a child (if a matching child already exists) or in self
    Node<V>** child = ArtNode<V>::find_child(this_writable, key[depth]);
    if (child) {
        Node<V>::insert(*child, child, key, key_len, value, depth+1, false);
    } else {
        // No child, node goes within us
        auto l = new Leaf<V>(key, key_len, value);
        ArtNode<V>::add_child(this_writable, ref, key[depth], l, false);
        // If `this` was full and `do_clone` is true, we will clone a full node
        // and then immediately delete the clone in favor of a larger node.
        // TODO: avoid this
    }
}


template<typename V>
void ArtNode4<V>::add_child(Node<V>** ref, unsigned char c, Node<V>* child,
                            bool force_clone) {
    if (this->num_children < 4) {
        int idx;
        for (idx = 0; idx < this->num_children; idx++) {
            if (c < keys[idx]) break;
        }

        ArtNode4<V>* target = force_clone ? new ArtNode4<V>(*this) : this;

        // Shift to make room
        memmove(target->keys + idx + 1, target->keys + idx, target->num_children - idx);
        memmove(target->children + idx + 1, target->children + idx,
                (target->num_children - idx) * sizeof(Node<V>*));

        // Insert element
        target->keys[idx] = c;
        target->children[idx] = child;
        child->refcount++;
        target->num_children++;

        if (force_clone) {
            // Update the parent pointer to the new node
            Node<V>::switch_ref(ref, target);
        }
    } else {
        // Copy the node4 into a new node16
        auto result = new ArtNode16<V>(this);
        // Update the parent pointer to the node16
        Node<V>::switch_ref(ref, result);
        // Insert the element into the node16 instead
        result->add_child(ref, c, child, false);
    }
}

template<typename V>
void ArtNode16<V>::add_child(Node<V>** ref, unsigned char c, Node<V>* child,
                             bool force_clone) {
    if (this->num_children < 16) {
        __m128i cmp;

        // Compare the key to all 16 stored keys
        cmp = _mm_cmplt_epi8(_mm_set1_epi8(c),
                             _mm_loadu_si128((__m128i*)keys));

        // Use a mask to ignore children that don't exist
        unsigned mask = (1 << this->num_children) - 1;
        unsigned bitfield = _mm_movemask_epi8(cmp) & mask;

        // Check if less than any
        unsigned idx;
        if (bitfield) {
            idx = __builtin_ctz(bitfield);
        } else {
            idx = this->num_children;
        }

        ArtNode16<V>* target = force_clone ? new ArtNode16<V>(*this) : this;

        // Shift to make room
        memmove(target->keys + idx + 1, target->keys + idx, target->num_children - idx);
        memmove(target->children + idx + 1, target->children + idx,
                (target->num_children - idx) * sizeof(Node<V>*));

        // Insert element
        target->keys[idx] = c;
        target->children[idx] = child;
        child->refcount++;
        target->num_children++;

        if (force_clone) {
            // Update the parent pointer to the new node
            Node<V>::switch_ref(ref, target);
        }
    } else {
#ifndef USE_VARNODE
        auto result = new ArtNode48<V>(this);
#endif
#ifdef USE_VARNODE
        void* place = malloc(offsetof(class ArtVarNode<V>, children[32]));
        auto result = new(place) ArtVarNode<V>(this);
#endif

        // Update the parent pointer to the node48
        Node<V>::switch_ref(ref, result);
        // Insert the element into the node48 instead
        result->add_child(ref, c, child, false);
    }
}

template<typename V>
void ArtNode48<V>::add_child(Node<V>** ref, unsigned char c, Node<V>* child,
                             bool force_clone) {
    if (this->num_children < 48) {
        int pos = this->num_children;

        ArtNode48<V>* target = force_clone ? new ArtNode48<V>(*this) : this;

        target->children[pos] = child;
        child->refcount++;
        target->keys[c] = pos + 1;
        target->num_children++;

        if (force_clone) {
            // Update the parent pointer to the new node
            Node<V>::switch_ref(ref, target);
        }
    } else {
        // Copy the node48 into a node256
        auto result = new ArtNode256<V>(this);
        // Update the parent pointer to the node256
        Node<V>::switch_ref(ref, result);
        // Insert the element into the node256 instead
        result->add_child(ref, c, child, false);
    }
}

#ifdef USE_VARNODE
template<typename V>
ArtNode256<V>::ArtNode256(ArtVarNode<V>* other) : ArtNode256<V>() {
    // ArtNode
    this->num_children = other->num_children;
    this->partial_len = other->partial_len;
    memcpy(this->partial, other->partial, min(MAX_PREFIX_LEN, this->partial_len));
    // ArtNode256 from ArtNode48
    for (int i = 0; i < 256; i++) {
        if (other->keys[i]) {
            children[i] = other->children[other->keys[i] - 1];
            children[i]->refcount++;
        }
    }
}
#endif

#ifdef USE_VARNODE
template<typename V>
void ArtVarNode<V>::add_child(Node<V>** ref, unsigned char c, Node<V>* child,
                              bool force_clone) {
    if (this->num_children < capacity) {
        int pos = this->num_children;

        ArtVarNode<V>* target;
        if (force_clone) {
            void* place = malloc(offsetof(class ArtVarNode<V>, children[capacity]));
            target = new(place) ArtVarNode<V>(capacity, *this);
        } else {
            target = this;
        }

        target->children[pos] = child;
        child->refcount++;
        target->keys[c] = pos + 1;
        target->num_children++;

        if (force_clone) {
            // Update the parent pointer to the new node
            Node<V>::switch_ref(ref, target);
        }
    } else if (grow_varnode(capacity) >= 256) {
        // Just allocate a Node256
        auto result = new ArtNode256<V>(this);
        // Update the parent pointer to the node256
        Node<V>::switch_ref(ref, result);
        // Insert the element into the node256 instead
        result->add_child(ref, c, child, false);
    } else {
        // Allocate a new node with more capacity
        uint8_t new_capacity = static_cast<uint8_t>(grow_varnode(capacity));
        void* place = malloc(offsetof(class ArtVarNode<V>, children[new_capacity]));
        ArtVarNode<V>* result = new(place) ArtVarNode<V>(new_capacity, *this);
        // Update the parent pointer to the new node
        Node<V>::switch_ref(ref, result);
        // Insert the element into the new node instead
        result->add_child(ref, c, child, false);
    }
}
#endif
