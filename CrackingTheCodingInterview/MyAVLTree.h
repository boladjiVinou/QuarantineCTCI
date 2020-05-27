#pragma once
#include "AVLTreeNode.h"
#include "MyStack.h"
template<typename T>
class MyAVLTree {
public:
	MyAVLTree(T rootValue) {
		this->_root =  new AVLTreeNode<T>(nullptr, rootValue);
	}
	MyAVLTree() {
		this->_root = nullptr;
	}
	~MyAVLTree() {
		std::function<void(Visited<T>*)> func = [](Visited<T>* visited)
		{
			delete  (AVLTreeNode<T>*)visited;
			visited = nullptr;
		};
		Visitor<T> visitor = Visitor<T>(func);
		this->_root->PostOrderVisit(visitor);
	}
	void setRoot(T value) {
		this->_root = new AVLTreeNode<T>(nullptr, value);
	}
	bool IsABinarySearchTree(const AVLTreeNode<T>& node) const { // O(n)
		if (node._leftChild != nullptr && node._leftChild->_value > this->_value) {
			return false;
		}
		if (node._rightChild != nullptr && node._rightChild->_value < this->_value) {
			return false;
		}
		bool result = true;
		if (this->_root->_leftChild != nullptr) {
			result &= IsABinarySearchTree(node._leftChild);
		}
		if (result && this->_root->_rightChild != nullptr) {
			result &= IsABinarySearchTree(node._rightChild);
		}
		return result;
	}
	bool IsAnAVL(const AVLTreeNode<T>& node) const { // O(n)
		if (node._leftChild != nullptr && node._leftChild->_value > this->_value) {
			return false;
		}
		if (node._rightChild != nullptr && node._rightChild->_value < this->_value) {
			return false;
		}
		bool result = true;
		if (this->_root->_leftChild != nullptr) {
			result &= IsABinarySearchTree(node._leftChild);
		}
		if (result && this->_root->_rightChild != nullptr) {
			result &= IsABinarySearchTree(node._rightChild);
		}
		return result;
	}
	AVLTreeNode<T>* InsertValue(T value) {
		AVLTreeNode<T>* res = (AVLTreeNode<T>*)this->_root->AppendNode(value);
		this->_lastInserted = res;
		return res;
	}
	void InsertNode(AVLTreeNode<T>* node) {
		this->_root.AppendNode(node);
		Rebalance(node);
	}
    // https://www.youtube.com/watch?v=g4y2h70D6Nk
	//http://www.mathcs.emory.edu/~cheung/Courses/323/Syllabus/Trees/AVL-delete.html
	void RemoveNode(AVLTreeNode<T>* node) {
		AVLTreeNode<T>* parent = node->_parent;
		// If the node doesnt have children we simply remove it from the tree
		if (node->_leftChild == nullptr && node->_rightChild == nullptr) {
			if (node->_parent->_leftChild == node) {
				parent->_leftChild = nullptr;
				parent->DecreaseLeftHeight();
			}
			else {
				parent->_rightChild = nullptr;
				parent->DecreaseRightHeight();
			}
			delete node;
			node = nullptr;
		}
		// If the node have only one child we remove it and bind its children to its parent
		else if ((node->_leftChild == nullptr && node->_rightChild != nullptr) ||
			(node->_leftChild != nullptr && node->_rightChild == nullptr)) {
			AVLTreeNode<T>* child = (AVLTreeNode<T>*)node->_leftChild;
			if (child == nullptr) {
				child = (AVLTreeNode<T>*)node->_rightChild;
			}
			if (parent != nullptr) {
				if (parent->_leftChild == node) {
					parent->_leftChild = child;
					parent->DecreaseLeftHeight();
				}
				else if (parent->_rightChild == node) {
					parent->_rightChild = child;
					parent->DecreaseRightHeight();
				}
			}
			delete node;
			node = nullptr;
		}
		// If the node have 2 children , we find its successor, and we replace the node by its successor
		else {
			AVLTreeNode<T>* successor = this->FindSuccessor(node);
			AVLTreeNode<T>* child = (AVLTreeNode<T>*)successor->_leftChild;
			if (child == nullptr) {
				child = (AVLTreeNode<T>*)successor->_rightChild;
			}
			if (successor->_parent != nullptr) {
				if (successor->_parent->_leftChild == successor) {
					successor->_parent->_leftChild = child;
					successor->_parent->DecreaseLeftHeight();
				}
				else if (successor->_parent->_rightChild == successor) {
					successor->_parent->_rightChild = child;
					successor->_parent->DecreaseRightHeight();
				}
			}
			node->_value = successor->_value;
			successor->_leftChild = nullptr;
			successor->_rightChild = nullptr;
			delete successor;
			successor = nullptr;
		}
		this->Rebalance(parent);
	}
	AVLTreeNode<T>* FindSuccessor(AVLTreeNode<T>* node) {
		if (node == nullptr) {
			return nullptr;
		}
		// we find the smallest biggest
		if (node->_rightChild != nullptr) {
			AVLTreeNode<T>* current = (AVLTreeNode<T>*)node->_rightChild;
			while (current->_leftChild != nullptr) {
				current = (AVLTreeNode<T>*)current->_leftChild;
			}
			return current;
		}
		else if (node->_leftChild != nullptr) {
			// we search for the less far parent for which the node is in its left subtree
			AVLTreeNode<T>* current = this->_root;
			MyStack< AVLTreeNode<T>*> parentsStack;
			while (current != nullptr) {
				if (current == node) {
					AVLTreeNode<T>* tmp = node;
					while (!parentsStack.IsEmpty() && parentsStack.Top()->_rightChild == tmp) {
						tmp = parentsStack.Top();
						parentsStack.Pop();
					}
					if (!parentsStack.IsEmpty()) {
						return parentsStack.Top();
					}
					else {
						return nullptr;
					}
				}
				else {
					parentsStack.Push(current);
					if (current->_value > node->_value) {
						current = (AVLTreeNode<T>*)current->_leftChild;
					}
					else {
						current = (AVLTreeNode<T>*)current->_rightChild;
					}
				}
			}
		}
		return nullptr;
	}
	void Print() {
		std::cout << this->_root->toString() << endl;;
	}
	AVLTreeNode<T>* GetPreviousInserted() {
		return this->_lastInserted;
	}
private:
	void RightRotation(AVLTreeNode<T>* node) {
		node->DecreaseLeftHeight();
		AVLTreeNode<T>* tmp = (AVLTreeNode<T>*)node->_leftChild;
		tmp->_parent = node->_parent;
		tmp->_rightChild = node;
		tmp->IncreaseRightHeight();
		node->_parent = tmp;
	}
	void LeftRotation(AVLTreeNode<T>* node) {
		node->DecreaseRightHeight();
		AVLTreeNode<T>* tmp = (AVLTreeNode<T>*)node->_rightChild;
		tmp->_parent = node->_parent;
		tmp->_leftChild = node;
		tmp->IncreaseLeftHeight();
		node->_parent = tmp;
	}
	/*
		I have watched this course : https://youtu.be/-9sHvAnLN_w?list=PLpPXw4zFa0uKKhaSz87IowJnOTzh9tiBk
		and https://www.cise.ufl.edu/~nemo/cop3530/AVL-Tree-Rotations.pdf
	*/
	void Rebalance(AVLTreeNode<T>* node) {
		AVLTreeNode<T>* tmp = node;
		bool balanced = true;
		if (tmp != nullptr) {
			while (tmp != nullptr && balanced) {
				balanced = tmp->IsBalanced();
				if (balanced) {
					tmp = tmp->_parent;
				}
			}
			if (!balanced) {
				// We check the grand parent of the problematic node
				AVLTreeNode<T>* gp = tmp->_parent->_parent;
				// if the grand parent left child left child is the node then we do a right rotation
				if ( ((AVLTreeNode<T>*)gp->_leftChild)->_leftChild == tmp) {
					this->RightRotation(tmp);
					this->Rebalance(tmp);
				}
				// if the grand parent right child right child is the node then we do a left rotation
				else if (((AVLTreeNode<T>*)gp->_rightChild)->_rightChild == tmp) {
					this->LeftRotation(tmp);
					this->Rebalance(tmp);
				}
				// if the grand parent left child right child is the node then we do a left right rotation (left rotation of parent, right rotation of grand parent)
				else if (((AVLTreeNode<T>*)gp->_leftChild)->_rightChild == tmp) {
					this->LeftRotation((AVLTreeNode<T>*)gp->_leftChild);
					this->RightRotation(gp);
					this->Rebalance(gp);
				}
				// if the grand parent right child left child is the node then we do a right left rotation (left rotation of parent, right rotation of grand parent)
				else if (((AVLTreeNode<T>*)gp->_rightChild)->_leftChild == tmp) {
					this->LeftRotation((AVLTreeNode<T>*)gp->_rightChild);
					this->RightRotation(gp);
					this->Rebalance(gp);
				}
			}
		}
	}
	AVLTreeNode<T>* _root;
	AVLTreeNode<T>* _lastInserted = nullptr;
};
