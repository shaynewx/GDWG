#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H

#include <initializer_list>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iomanip>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

// TODO: Make both graph and edge generic
//       ... this won't just compile
//       straight away
namespace gdwg {
	template<typename N, typename E>
	class graph;

	// Edge: An Abstract BASE Class
	template<typename N, typename E>
	class edge {
	 public:
		// Pure virtual function
		virtual ~edge() = default;
		[[nodiscard]] virtual std::string print_edge() const = 0;
		[[nodiscard]] virtual bool is_weighted() const = 0;
		virtual std::optional<E> get_weight() const = 0;
		virtual std::pair<N, N> get_nodes() const = 0;
		virtual bool operator==(const edge& other) const = 0;

	 private:
		friend class graph<N, E>;
	};

	// Class of weighted edges
	template<typename N, typename E>
	class weighted_edge : public edge<N, E> {
	 public:
		// Constructor for weighted edges
		weighted_edge(const N& src, const N& dst, const E& weight)
		: src_(src)
		, dst_(dst)
		, weight_(weight) {}

		// Return a string representation of the edge
		[[nodiscard]] std::string print_edge() const override {
			std::ostringstream oss; // 定义输出字符流
			oss << src_ << " -> " << dst_ << " | W | " << weight_;
			return oss.str();
		}

		// Return whether the edge is a weighted edge (true)
		[[nodiscard]] bool is_weighted() const override {
			return true;
		}

		// Return weight
		std::optional<E> get_weight() const override {
			return weight_;
		}

		// Returns the src and dst nodes of an edge
		std::pair<N, N> get_nodes() const override {
			return {src_, dst_};
		}

		// Compare two edges for equality (operator== overload)
		bool operator==(const edge<N, E>& other) const override {
			auto const* other_edge = dynamic_cast<const weighted_edge<N, E>*>(&other); // Convert other to weighted_edge
			if (other_edge) {
				return this->src_ == other_edge->src_ and this->dst_ == other_edge->dst_
				       and this->weight_ == other_edge->weight_;
			}
			return false;
		}

		// operator!= overload
		bool operator!=(const edge<N, E>& other) const {
			return !(*this == other);
		}

	 private:
		N src_;
		N dst_;
		E weight_;
	};

	// Class of Unweighted edge
	template<typename N, typename E>
	class unweighted_edge : public edge<N, E> {
	 public:
		// Constructor for unweighted edges
		unweighted_edge(const N& src, const N& dst)
		: src_(src)
		, dst_(dst) {}

		// Return a string representation of the edge
		std::string print_edge() const override {
			std::ostringstream oss;
			oss << src_ << " -> " << dst_ << " | U";
			return oss.str();
		}

		// Return whether the edge is a weighted edge (false)
		bool is_weighted() const override {
			return false;
		}

		// Return weight (nullopt)
		std::optional<E> get_weight() const override {
			return std::nullopt;
		}

		// Returns the src and dst nodes of an edge
		std::pair<N, N> get_nodes() const override {
			return {src_, dst_};
		}

		// Compare two edges for equality (operator== overload)
		bool operator==(const edge<N, E>& other) const override {
			auto const* other_edge = dynamic_cast<const unweighted_edge<N, E>*>(&other); // Convert other to
			                                                                             // unweighted_edge
			if (other_edge) {
				return this->src_ == other_edge->src_ and this->dst_ == other_edge->dst_;
			}
			return false;
		}

		// operator!= overload
		bool operator!=(const edge<N, E>& other) const {
			return !(*this == other);
		}

	 private:
		N src_;
		N dst_;
	};

	// Class of Graph
	template<typename N, typename E>
	class graph {
	 public:
		// Default constructor with noexcept
		graph() noexcept
		: adj_list_{}
		, nodes_{} {}

		// Initializer list constructor, accepts an initializer list as a parameter
		graph(std::initializer_list<N> il) {
			// Clear the state of the current object
			adj_list_.clear();
			nodes_.clear();

			// Inserting elements into the initializer list
			for (auto const& node : il) {
				nodes_.emplace(node);
			}
		}

		// Range Constructor
		template<typename InputIt>
		graph(InputIt first, InputIt last) {
			for (auto it = first; it != last; ++it) {
				nodes_.emplace(*it);
			}
		}

		// Move Constructor
		graph(graph&& other) noexcept = default;

		// Move assignment operator
		graph& operator=(graph&& other) noexcept = default;

		// Copy Constructor
		graph(graph const& other) = default;

		// Copy assignment operator
		graph& operator=(graph const& other) = default;

		// 2.5 Accessors
		// Check if a specific node exists in the graph
		[[nodiscard]] bool is_node(const N& node) const {
			return nodes_.find(node) != nodes_.end();
		}

		// Check if the node exists in the graph (returns true if not)
		[[nodiscard]] bool empty() const {
			return nodes_.empty();
		}

		// Check if there is an edge of a certain weight between two nodes
		[[nodiscard]] bool is_connected(N const& src, N const& dst) const {
			// Check if the src and dst nodes exist, if not, throw an error
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist "
				                         "in the graph");
			}

			const auto& edges = adj_list_.find(src)->second; // Get the edge list of the source node

			// Check if there is an edge to the target node
			for (const auto& edge : edges) {
				if (edge.first == dst) {
					return true; // Return true if a matching edge is found
				}
			}
			return false;
		}

		// Return the number of nodes in the graph
		[[nodiscard]] std::size_t node_count() const {
			return nodes_.size();
		}

		// Return all nodes in ascending order
		[[nodiscard]] std::vector<N> nodes() {
			// Copy directly from set to vector since it is already sorted
			std::vector<N> result(nodes_.begin(), nodes_.end());
			return result;
		}

		// Return all edges from src to dst in the specified order
		[[nodiscard]] std::vector<std::unique_ptr<edge<N, E>>> edges(N const& src, N const& dst) const {
			// Check if the src and dst nodes exist, if not, throw an error
			if (!is_node(src) or !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::edges if src or dst node don't exist in the "
				                         "graph");
			}

			// Get the edge list of the src node
			const auto& adj_edges = adj_list_.at(src);
			std::vector<std::unique_ptr<edge<N, E>>> edges_list;

			// Traverse the edge list and collect the edges whose target node is dst
			for (const auto& [target, weight] : adj_edges) {
				if (target == dst) {
					if (weight) {
						edges_list.push_back(std::make_unique<weighted_edge<N, E>>(src, dst, *weight));
					}
					else {
						edges_list.push_back(std::make_unique<unweighted_edge<N, E>>(src, dst));
					}
				}
			}

			//  Unweighted edges are placed first, and weighted edges are sorted in ascending order of weight
			std::sort(edges_list.begin(), edges_list.end(), [](const auto& a, const auto& b) {
				if (!a->is_weighted() and b->is_weighted())
					return true;
				if (a->is_weighted() and !b->is_weighted())
					return false;
				return a->is_weighted() and b->is_weighted() and a->get_weight() < b->get_weight();
			});
			return edges_list;
		}

		// Returns all dst nodes starting from the src node, sorted in ascending order
		[[nodiscard]] std::vector<N> connections(N const& src) const {
			// If src does not exist, throw an error
			if (!is_node(src)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't exist in the "
				                         "graph");
			}

			auto it = adj_list_.find(src); // Find the src node in the adj_list_
			std::set<N> unique_connections; // Stores a unique target node
			if (it != adj_list_.end()) {
				for (const auto& edge : it->second) {
					// Insert unique_connections into the target node of each edge
					unique_connections.insert(edge.first);
				}
			}
			// Convert to std::vector output
			return std::vector<N>(unique_connections.begin(), unique_connections.end());
		}

		// 2.4 Modifiers
		// Insert a new node
		bool insert_node(const N& value) {
			// Return a pair, where the second indicates whether the insertion is successful
			auto result = nodes_.insert(value);

			// If the new node is successfully inserted, create an empty edge list for that node
			if (result.second) {
				adj_list_[value] = {};
			}
			return result.second; // If the node is newly inserted, second is true
		}

		// Insert a new edge
		bool insert_edge(N const& src, N const& dst, std::optional<E> weight = std::nullopt) {
			// Check if the src and the dst exist, throw an error if they do not exist
			if (!is_node(src) or !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node does "
				                         "not exist");
			}

			// New edge
			auto new_edge = std::make_pair(dst, weight);

			auto& edges = adj_list_[src];
			if (std::find(edges.begin(), edges.end(), new_edge) != edges.end()) {
				return false; // Return false if the edge already exists
			}

			// Insert the new edge
			edges.push_back(new_edge);

			// Sort the edges to maintain order
			std::sort(edges.begin(), edges.end(), [](const auto& lhs, const auto& rhs) {
				if (lhs.first == rhs.first) {
					return lhs.second < rhs.second; // Sort by weight if dst is the same
				}
				return lhs.first < rhs.first; // Sort by dst
			});

			return true;
		}

		// Replace node (replace old_data stored in the graph with new_data)
		bool replace_node(N const& old_data, N const& new_data) {
			// Return false if there is a node with value new_data
			if (nodes_.find(new_data) != nodes_.end())
				return false;

			// Throw an error if there is no node containing old_data
			if (!is_node(old_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
			}

			// Replace node: first delete the old data, then insert the new data
			nodes_.erase(old_data);
			nodes_.emplace(new_data);
			return true;
		}

		// Migrate the edges and weights on the old data to the new data
		void merge_replace_node(N const& old_data, N const& new_data) {
			// Throw an error if old_data or new_data does not exist
			if (!is_node(old_data) or !is_node(new_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if they "
				                         "don't exist in the graph");
			}

			auto& old_edges = adj_list_[old_data];
			auto& new_edges = adj_list_[new_data];

			// Remove duplicate edges
			for (auto& edge : old_edges) {
				auto it = std::find_if(new_edges.begin(), new_edges.end(), [&edge](const auto& existing_edge) {
					return existing_edge.first == edge.first and existing_edge.second == edge.second;
				});

				if (it == new_edges.end()) {
					// If no identical edge is found, move the edge
					new_edges.push_back(std::move(edge));
				}
			}
			old_edges.clear();
			nodes_.erase(old_data);
			adj_list_.erase(old_data);
		}

		// Delete all nodes whose value is value
		bool erase_node(N const& value) {
			// Return false if the point is not found
			if (!is_node(value) || !is_node(value)) {
				return false;
			}

			// Remove all edges associated with this node from the adjacency list
			adj_list_.erase(value);

			// Iterate over the adjacency lists of all nodes in the graph
			for (auto& pair : adj_list_) {
				// Access the edge list of the current node and delete all edges whose dst node is value
				auto& edges = pair.second;
				edges.erase(
				    std::remove_if(edges.begin(), edges.end(), [value](const auto& edge) { return edge.first == value; }),
				    edges.end());
			}
			// Delete the node
			nodes_.erase(nodes_.find(value));
			return true;
		}

		// Delete the edge from src to dst. If weight is std::nullopt, delete the unweighted edge;
		// otherwise delete the edge with a specific weight
		bool erase_edge(N const& src, N const& dst, std::optional<E> weight = std::nullopt) {
			// Check if the src and dst nodes exist, if not, throw an error
			if (!is_node(src) or !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't exist "
				                         "in the graph");
			}

			// Retrieve the list of edges from the source node
			auto& edges = adj_list_[src];
			auto before = edges.size();

			// Erase edges that match the destination node and the specified weight (if any)
			edges.erase(
			    std::remove_if(edges.begin(),
			                   edges.end(),
			                   [dst, weight](const auto& edge) { return edge.first == dst and edge.second == weight; }),
			    edges.end());
			return edges.size() != before;
		}

		// Delete all nodes
		void clear() noexcept {
			nodes_.clear();
			adj_list_.clear();
		}

		// 2.7 Compare two graphs to see if they are exactly the same (operator== overloaded)
		[[nodiscard]] bool operator==(graph const& other) const {
			// If the node sets are different, then the two graphs are not different
			if (nodes_ != other.nodes_)
				return false;

			// If the adj_lists are different, then the two graphs are different
			if (adj_list_.size() != other.adj_list_.size())
				return false;

			// For each node, check the set of edges
			for (const auto& node : nodes_) {
				// If one of the graphs does not have the adj_list of the node, then the two graphs are different
				if (adj_list_.find(node) == adj_list_.end() or other.adj_list_.find(node) == other.adj_list_.end()) {
					return false;
				}

				const auto& edges_this = adj_list_.at(node);
				const auto& edges_other = other.adj_list_.at(node);

				// If the edge sets of any nodes are not equal, then the two graphs are different
				if (edges_this != edges_other)
					return false;
			}
			return true;
		}

		// 2.8 Output all nodes and edges of a graph
		// All nodes are sorted in ascending order;
		// all edges of each node also need to be output in order, first the unweighted edges,
		// then the weighted edges in ascending order
		friend std::ostream& operator<<(std::ostream& os, const graph<N, E>& g) {
			os << '\n';
			std::vector<std::tuple<N, N, std::optional<E>>> edges; // Temporarily store and sort all edge information

			for (const auto& [node, adj_edges] : g.adj_list_) {
				os << node << " (\n";

				// Clear the edges vector and add all edges of the current node to this vector
				edges.clear();
				for (const auto& edge : adj_edges) {
					edges.emplace_back(node, edge.first, edge.second);
				}

				// First, sort by target node
				// If the target nodes are the same, the unweighted edges are placed before the weighted edges
				// Finally, sort by weight
				std::sort(edges.begin(), edges.end(), [](const auto& lhs, const auto& rhs) {
					if (std::get<1>(lhs) != std::get<1>(rhs))
						return std::get<1>(lhs) < std::get<1>(rhs);
					if (!std::get<2>(lhs).has_value() and std::get<2>(rhs).has_value())
						return true;
					if (std::get<2>(lhs).has_value() and !std::get<2>(rhs).has_value())
						return false;
					return std::get<2>(lhs).value() < std::get<2>(rhs).value();
				});

				// Output all sorted edges
				for (const auto& edge : edges) {
					os << "  " << std::get<0>(edge) << " -> " << std::get<1>(edge) << " | "
					   << (std::get<2>(edge) ? "W | " + std::to_string(std::get<2>(edge).value()) : "U") << '\n';
				}
				os << ")\n";
			}
			return os;
		}

		// 2.9 Class of iterator
		class iterator {
		 public:
			using value_type = struct {
				N from;
				N to;
				std::optional<E> weight;
			};
			using reference = value_type;
			using pointer = void;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;

			using node_iterator = typename std::map<N, std::vector<std::pair<N, std::optional<E>>>>::iterator;
			using edge_iterator = typename std::vector<std::pair<N, std::optional<E>>>::iterator;

			// Default Constructor
			iterator()
			: node_it_()
			, edge_it_()
			, graph_ptr_(nullptr) {}

			// operator* overload
			reference operator*() {
				return {node_it_->first, edge_it_->first, edge_it_->second};
			}

			// Iterator traversal
			// operator++()
			iterator& operator++() {
				++edge_it_; // Increase the edge iterator and move to the next edge
				if (edge_it_ == node_it_->second.end()) {
					// If all edges of the current node have been traversed, increase the node iterator and move to the
					// next node
					++node_it_;
					while (node_it_ != graph_ptr_->adj_list_.end() and node_it_->second.empty()) {
						++node_it_; // Skip the node without edge and find the next node with edge
					}
					if (node_it_ != graph_ptr_->adj_list_.end()) {
						// If there are still nodes that have not been traversed, initialize the edge iterator
						edge_it_ = node_it_->second.begin();
					}
					else {
						edge_it_ = {}; // Consistent with end()
					}
				}
				return *this;
			}

			// operator++(int)
			iterator operator++(int) {
				iterator temp = *this; // Create a copy of the current iterator
				++(*this); // Calling the Operator++
				return temp;
			}

			// operator--()
			iterator& operator--() {
				if (node_it_ == graph_ptr_->adj_list_.begin() and edge_it_ == node_it_->second.begin()) {
					throw std::out_of_range("Iterator cannot decrement past the beginning of the graph");
				}
				// If the current node is at the end of the graph or the edge iterator is at the beginning of a node,
				// find the previous node with an edge
				if (node_it_ == graph_ptr_->adj_list_.end() || edge_it_ == node_it_->second.begin()) {
					do {
						--node_it_;
					} while (node_it_->second.empty());
					edge_it_ = node_it_->second.end(); // Point the edge iterator to the last edge of the node
				}
				--edge_it_; // Decrement the edge iterator, moving to the previous edge
				return *this;
			}

			// operator--(int)
			iterator operator--(int) {
				iterator temp = *this; // Create a copy of the current iterator
				--(*this); // Calling the Operator--
				return temp;
			}

			// operator==
			bool operator==(const iterator& other) const {
				// Compare the current iterator with another iterator for equality
				return node_it_ == other.node_it_ and edge_it_ == other.edge_it_ and graph_ptr_ == other.graph_ptr_;
			}

		 private:
			node_iterator node_it_; // Iterator for the current node
			edge_iterator edge_it_; // Iterator for the current edge
			const graph* graph_ptr_; // Pointer to the graph

			// Construct an iterator function pointing to a specific element
			explicit iterator(node_iterator node_it, edge_iterator edge_it, const graph* graph_ptr)
			: node_it_(node_it)
			, edge_it_(edge_it)
			, graph_ptr_(graph_ptr) {}

			friend class graph;
		};

		// 2.6 Iterator Access
		// Return the iterator pointing to the first element in the container
		[[nodiscard]] iterator begin() const {
			if (adj_list_.empty()) {
				return end();
			}
			// Cast to remove constness
			auto node_it = const_cast<std::map<N, std::vector<std::pair<N, std::optional<E>>>>&>(adj_list_).begin();
			return iterator(node_it, node_it->second.begin(), const_cast<graph*>(this));
		}

		// Return the iterator pointing to the end of the list
		[[nodiscard]] iterator end() const {
			static std::vector<std::pair<N, std::optional<E>>> empty_edge_list;
			// Cast to remove constness
			auto node_it = const_cast<std::map<N, std::vector<std::pair<N, std::optional<E>>>>&>(adj_list_).end();
			return iterator(node_it, empty_edge_list.end(), const_cast<graph*>(this));
		}

		// 2.4.7 Remove the edge pointing to iterator i
		iterator erase_edge(iterator i) {
			if (i == end()) {
				return end();
			}

			auto node_it = i.node_it_;
			auto edge_it = i.edge_it_;

			if (edge_it == node_it->second.end()) {
				return end();
			}

			// Delete the current edge and calculate the position of the next element
			node_it->second.erase(edge_it);
			if (node_it->second.empty()) {
				// If the current node has no more edges, move to the next node with edges
				do {
					++node_it;
					if (node_it == adj_list_.end())
						return end(); // Return end if there are no more nodes
				} while (node_it->second.empty());
				return iterator(node_it, node_it->second.begin(), this);
			}
			else {
				// Otherwise return the next edge of the current node
				if (edge_it != node_it->second.end()) {
					return iterator(node_it, edge_it, this);
				}
				else {
					// If the last edge is deleted, return the first edge of the next node
					return iterator(++node_it, node_it->second.begin(), this);
				}
			}
		}

		// 2.4.8 Removes all edges between iterators [i, s)
		iterator erase_edge(iterator i, iterator s) {
			while (i != s) {
				auto current = i++;
				erase_edge(current);
			}
			return s;
		}

		// 2.5 Return an iterator pointing to edges equivalent to the specified src, dst, and weight
		[[nodiscard]] iterator find(N const& src, N const& dst, std::optional<E> weight = std::nullopt) {
			auto src_it = adj_list_.find(src);
			if (src_it == adj_list_.end()) {
				return end(); // If there is no src, there is no edge, and return end()
			}

			auto& edges = src_it->second;
			for (auto it = edges.begin(); it != edges.end(); ++it) {
				if (it->first == dst and it->second == weight) {
					return iterator(src_it, it, this); // Return the edges that found the match
				}
			}
			return end();
		}

	 private:
		std::map<N, std::vector<std::pair<N, std::optional<E>>>> adj_list_; // Adjacency lists for nodes and edges
		std::set<N> nodes_; // Set of nodes
	};
} // namespace gdwg

#endif // GDWG_GRAPH_H
