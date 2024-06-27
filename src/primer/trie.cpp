#include "primer/trie.h"
#include <string_view>
#include "common/exception.h"
#include <stack>
namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  // throw NotImplementedException("Trie::Get is not implemented.");

  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
  std::shared_ptr<const TrieNode> node = GetRoot();
  if (node == nullptr)
  {
    return nullptr;
  }
  
  for(char c: key){
    if(node->children_.find(c) == node->children_.end()){
      return nullptr;
    }
    node = node->children_.at(c);
  }
  if (node ->is_value_node_ == false)
  {
    return nullptr;
  }
  std::shared_ptr<const TrieNodeWithValue<T>> value_node = std::dynamic_pointer_cast<const TrieNodeWithValue<T>>(node);
  if (value_node)
  {
    return value_node->value_.get();
  }
  return nullptr;  
  // return dynamic_cast<const TrieNodeWithValue<T> *>(node.get())->value_.get();

}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  // throw NotImplementedException("Trie::Put is not implemented.");

  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
  if (key.size() == 0)  
  {
    //如果key为空，则在根节点添加value即可
    std::shared_ptr<T> new_value = std::make_shared<T>(std::move(value));
    std::shared_ptr<TrieNode> new_root = nullptr;
    if (root_!=nullptr)
    {
      new_root = std::make_shared<TrieNodeWithValue<T>>(root_->children_, std::move(new_value));
    }else{
      new_root = std::make_shared<TrieNodeWithValue<T>>(std::move(new_value));
    }
    return Trie(std::move(new_root));
  }
  //拷贝根节点
  std::shared_ptr<TrieNode> new_root = root_ ? root_->Clone() : nullptr;
  if (new_root == nullptr)
  {
    new_root = std::make_shared<TrieNode>();
  }
  Put<T>(new_root,key,std::move(value));
  return Trie(std::move(new_root));
}

template <class T>
auto Trie::Put(std::shared_ptr<TrieNode> new_root,std::string_view key, T value) const -> Trie {
   std::shared_ptr<TrieNode> node = nullptr;
   char ch = key.at(0);
   if (new_root->children_.find(ch) != new_root->children_.end()){
      //有相同子节点 需要clone
      if (key.size() == 1)
      {
        std::shared_ptr<T> new_value = std::make_shared<T>(std::move(value));
        node = std::make_shared<TrieNodeWithValue<T>>(new_root->children_.at(ch)->children_, new_value);
      }
      else{
        node = new_root->children_.at(ch)->Clone();
        Put<T>(node,key.substr(1),std::move(value));
      }
   } else{
      //没有相同的子节点 需要新建
      if (key.size() == 1)
      {
        std::shared_ptr<T> new_value = std::make_shared<T>(std::move(value));
        node = std::make_unique<TrieNodeWithValue<T>>(new_value);
      }
      else{
        node = std::make_unique<TrieNode>();
        Put<T>(node,key.substr(1),std::move(value));
      }
   }
   new_root->children_[ch] = node;
   return Trie(std::move(new_root));
}
auto Trie::Remove(std::string_view key) const -> Trie {
  // throw NotImplementedException("Trie::Remove is not implemented.");

  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
  // std::shared_ptr<TrieNode> new_root = root_? root_->Clone() : nullptr;
  // if (key.size()==0)
  // {
  //   //如果key为空，说明删除的是根节点
  //   new_root = std::make_shared<TrieNode>(root_->children_);
  //   return Trie(std::move(new_root));
  // }
  // if (new_root == nullptr)
  // {
  //   return Trie(std::move(new_root));
  // }
  // std::shared_ptr<TrieNode> curr = new_root;
  // char ch = 0;
  // for (size_t i = 0; i < key.size(); i++)
  // {
  //   ch = key[i];
  //   if (curr && curr->children_.find(ch) != curr->children_.end()){
  //       if (i==key.size()-1)
  //       {
  //         //找到目标 需要删除
  //         std::shared_ptr<const TrieNode> aim_node = curr->children_.at(ch);
  //         if (aim_node->children_.empty())
  //         {
  //           //如果没有子节点 直接删除
  //           curr->children_.erase(ch);
  //         }else{
  //           //有子节点，将value类型变换为非value类型
  //           std::shared_ptr<TrieNode> new_node = std::make_shared<TrieNode>(aim_node->children_);
  //           curr->children_[ch] = new_node;
  //         }
  //       }
  //       else{
  //         curr = std::const_pointer_cast<bustub::TrieNode>(curr->children_[ch]);
  //      }
  //   }
  //   //不存在直接返回
  //   else{
  //     break;
  //   }
  // }
  // return Trie(std::move(new_root));
  std::shared_ptr<TrieNode> new_root=nullptr;
  if (key.size()==0)
  {
    //如果key为空，说明删除的是根节点
    new_root = std::make_shared<TrieNode>(root_->children_);
    return Trie(new_root);
  }
  new_root = root_->Clone(); 
  Remove(new_root,key);
  if(new_root->children_.empty()&&!new_root->is_value_node_){
    new_root=nullptr;
  }
  return Trie(new_root);
}
auto Trie::Remove(std::shared_ptr<TrieNode> new_root,std::string_view key) const -> Trie{
  char ch = key[0];
  std::shared_ptr<TrieNode> node = nullptr;
  if (new_root->children_.find(ch)!=new_root->children_.end())
  {
    //如果是key的最后一位，说明已经找到
    if(key.size()==1){
      //如果目标节点没有子节点 直接删除
      if (new_root->children_[ch]->children_.empty())
      {
        new_root->children_.erase(ch);
      }else{
        std::shared_ptr new_node = std::make_shared<TrieNode>(new_root->children_[ch]->children_);
        new_root->children_[ch] = new_node;
      }
    }else{
      node=new_root->children_.at(ch)->Clone();
      Remove(node, key.substr(1));
      if (!node->children_.empty()|| node->is_value_node_)
      {
        new_root->children_[ch] = node;
      }else{
        new_root->children_.erase(ch);
      }
    }
  }
  return Trie(new_root);
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub
