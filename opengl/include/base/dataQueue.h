
#ifndef _DATA_QE_QUEST_H_
#define _DATA_QE_QUEST_H_

namespace base
{
	template<class T>
	class Node
	{
	public:
		T*	data_ = nullptr;
		struct Node<T> *pre_ = nullptr;
		struct Node<T> * next_ = nullptr;
	};

	template<class T>
	class DataQequest
	{
	public:
		DataQequest();
		bool					addRequest(T * request);
		T*						firstRequest()const;
		void					removeFistNode();
		bool					empty()const;
		void					removeRequest(T * request);
		Node<T>*				getHead();
	private:
		void					removeNode(Node<T>*);
		void					pushBack(Node<T>*);
	private:
		Node<T>* head_ = nullptr;
		Node<T>* back_ = nullptr;
	};

	template<class T>
	Node<T>* DataQequest<T>::getHead()
	{
		return head_;
	}

	template<class T>
	DataQequest<T>::DataQequest()
	{
		head_ = back_ = nullptr;
	}

	template<class T>
	void DataQequest<T>::pushBack(Node<T>*node)
	{
		if (back_)
		{
			back_->next_ = node;
			node->pre_ = back_;
			back_ = node;
		}
		else
		{
			back_ = head_ = node;
		}
	}

	template<class T>
	void DataQequest<T>::removeNode(Node<T>*node)
	{
		if (node)
		{
			Node<T> * tNode = node;
			tNode->data_->isNode_ = false;

			if (back_ == head_)
			{
				back_ = head_ = nullptr;
			}
			else
			{
				if (tNode == head_)
				{
					head_ = tNode->next_;
				}
				if (tNode == back_)
				{
					back_ = tNode->pre_;
				}
			}

			if (tNode->pre_)
			{
				tNode->pre_->next_ = tNode->next_;
			}
			if (tNode->next_)
			{
				tNode->next_->pre_ = tNode->pre_;
			}
			delete tNode;
			tNode = nullptr;
		}
	}

	template<class T>
	void DataQequest<T>::removeRequest(T * request)
	{
		struct Node<T>* t = head_;
		while (t)
		{
			if (t->data_ != request) t = t->next_;
		}
		if (t)
		{
			removeNode(t);
		}
	}


	template<class T>
	void DataQequest<T>::removeFistNode()
	{
		removeNode(head_);
	}

	template<class T>
	T*  DataQequest<T>::firstRequest() const
	{
		if (head_)
		{
			return head_->data_;
		}
		return nullptr;
	}

	template<class T>
	bool DataQequest<T>::empty() const
	{
		return (head_ == back_) && (head_ == nullptr);
	}

	template<class T>
	bool DataQequest<T>::addRequest(T * request)
	{
		struct Node<T>* newNode = new Node<T>;
		if (!newNode) return false;
		newNode->data_ = request;
		newNode->data_->isNode_ = true;
		pushBack(newNode);
	}

}

#endif
