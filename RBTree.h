#pragma once
#include<iostream>

namespace tmp_rbtree_np {

	// color
	struct Red {};
	struct Black {};
	template<typename T> struct is_color : std::false_type {};
	template<> struct is_color<Red> : std::true_type {};
	template<> struct is_color<Black> : std::true_type {};
	template<typename T> concept color = is_color<T>::value;

	// rbtree struct
	template<typename T> struct is_rbtree;
	template<typename T> concept rbtree = is_rbtree<T>::value;

	template<color C, typename A, rbtree L, rbtree R> struct Node;
	struct Leaf;
	template<typename T> struct is_rbtree : std::false_type {};
	template<color C, typename A, typename L, typename R> struct is_rbtree<Node<C,A,L,R>> : std::true_type {};
	template<> struct is_rbtree<Leaf> : std::true_type {};


	struct Leaf {
		static void show_helper(std::ostream& os, const std::string& prefix, bool isleft) {}
		static void show(std::ostream& os) {
			os << "└──  ";
		}
	};

	template<color C, typename T, rbtree L, rbtree R> struct Node {
		using type = T;
		using left = L;
		using right = R;
		static void show_helper(std::ostream& os, const std::string& prefix, bool isleft)
			requires requires(std::ostream& os) { os << type{}; } //type::show(os);
		{
			right::show_helper(os, prefix + (isleft ? "│   " : "    "), false);
			os << prefix << (isleft ? "└──  " : "┌──  ");
			if constexpr (std::is_same_v<C, Red>) os << "*";
			os << type{};
			os << std::endl;
			left::show_helper(os, prefix + (isleft ? "    " : "│   "), true);
		}
		static void show(std::ostream& os) {
			show_helper(os, "", true);
		}
	};

	// ostream
	template<rbtree T>
	std::ostream& operator<<(std::ostream& os, const T& v)
	{
		T::show(os);
		return os;
	}

	// balance interface
	template<typename X, rbtree L, rbtree R> struct balance { using type = Node<Black, X, L, R>; };
	template<typename X, rbtree L, rbtree R> using balance_t = typename balance<X, L, R>::type;

	// to black
	template<rbtree T> struct toblack;
	template<color C, typename T, rbtree L, rbtree R> struct toblack<Node<C, T, L, R>> { using type = Node<Black, T, L, R>; };
	template<rbtree T> using toblack_t = typename toblack<T>::type;

	//insert
	template<typename X, rbtree A, typename = void> struct insert_impl {
		using type = A;
	};
	template<typename X, rbtree A> using insert_impl_t = typename insert_impl<X, A>::type;

	template<typename X> struct insert_impl<X, Leaf> {
		using type = Node<Red, X, Leaf, Leaf>;
	};
	template<typename X, typename Y, rbtree L, rbtree R> struct insert_impl < X, Node<Black, Y, L, R>, std::enable_if_t <(X{} < Y{})>> {
		using type = balance_t<Y, insert_impl_t<X, L>, R>;
	};
	template<typename X, typename Y, rbtree L, rbtree R> struct insert_impl < X, Node<Black, Y, L, R>, std::enable_if_t <(X{} > Y{})>> {
		using type = balance_t<Y, L, insert_impl_t<X, R>>;
	};
	//template<typename X, typename Y, rbtree L, rbtree R> struct insert_impl<X, Node<Black, Y, L, R>, std::enable_if_t<std::is_same_v<apply_t<X, Y>, Od_eq>>> {
	//	using type = Node<Black, Y, L, R>;
	//};
	template<typename X, typename Y, rbtree L, rbtree R> struct insert_impl<X, Node<Red, Y, L, R>, std::enable_if_t< (X{} < Y{}) >> {
		using type = Node<Red, Y, insert_impl_t<X, L>, R>;
	};
	template<typename X, typename Y, rbtree L, rbtree R> struct insert_impl<X, Node<Red, Y, L, R>, std::enable_if_t< (X{} > Y{}) >> {
		//using type = balance_t<L, Y, insert_impl_t<X, R>>;
		using type = Node<Red, Y, L, insert_impl_t<X, R>>;
	};
	//template<typename X, typename Y, rbtree L, rbtree R> struct insert_impl<X, Node<Red, Y, L, R>, std::enable_if_t<std::is_same_v<apply_t<X, Y>, Od_eq>>> {
	//	using type = Node<Black, Y, L, R>;
	//};

	// member
	template<typename X, rbtree A, typename = void> struct member;
	template<typename X, rbtree A> using member_t = typename member<X, A>::type;
	template <typename X, rbtree A> inline constexpr bool member_v = member_t<X, A>::value;
	template<typename X> struct member<X, Leaf> { using type = std::false_type; };
	template<typename X, color C, typename Y, rbtree L, rbtree R> struct member<X, Node<C, Y, L, R>, std::enable_if_t< (X{} < Y{})>> {
		using type = member_t<X, L>;
	};
	template<typename X, color C, typename Y, rbtree L, rbtree R> struct member<X, Node<C, Y, L, R>, std::enable_if_t< (X{} > Y{})>> {
		using type = member_t<X, R>;
	};
	template<typename X, color C, typename Y, rbtree L, rbtree R> struct member<X, Node<C, Y, L, R>, std::enable_if_t< (X{} == Y{})>> {
		using type = std::true_type;
	};

	// balance
	template<typename X, typename LX, rbtree LL, rbtree LR, typename RX, rbtree RL, rbtree RR> struct balance<X, Node<Red, LX, LL, LR>, Node<Red, RX, RL, RR>> {
		using type = Node<Red, X, Node<Black, LX, LL, LR>, Node<Black, RX, RL, RR>>;
	};
	template<typename X, typename LX, typename LLX, rbtree LLL, rbtree LLR, rbtree LR, rbtree R> struct balance<X, Node<Red, LX, Node<Red, LLX, LLL, LLR>, LR>, R> {
		using type = Node<Red, LX, Node<Black, LLX, LLL, LLR>, Node<Black, X, LR, R>>;
	};
	template<typename X, typename LX, rbtree LL, typename LRX, rbtree LRL, rbtree LRR, rbtree R> struct balance<X, Node<Red, LX, LL, Node<Red, LRX, LRL, LRR>>, R> {
		using type = Node<Red, LRX, Node<Black, LX, LL, LRL>, Node<Black, X, LRR, R>>;
	};
	template<typename X, rbtree L, typename RX, rbtree RL, typename RRX, rbtree RRL, rbtree RRR> struct balance<X, L, Node<Red, RX, RL, Node<Red, RRX, RRL, RRR>>> {
		using type = Node<Red, RX, Node<Black, X, L, RL>, Node<Black, RRX, RRL, RRR>>;
	};
	template<typename X, rbtree L, typename RX, typename RLX, rbtree RLL, rbtree RLR, rbtree RR> struct balance<X, L, Node<Red, RX, Node<Red, RLX, RLL, RLR>, RR>> {
		using type = Node<Red, RLX, Node<Black, X, L, RLL>, Node<Black, RX, RLR, RR>>;
	};

	template<typename X, rbtree A> using insert_t = toblack_t<insert_impl_t<X, A>>;


	// delete
	// sub1 :: RB a -> RB a
	template<rbtree T> struct sub1;
	template<typename X, rbtree L, rbtree R> struct sub1<Node<Black, X, L, R>> { using type = Node<Red, X, L, R>; };
	template<rbtree T> using sub1_t = typename sub1<T>::type;

	// balleft :: a -> RB a -> RB a -> RB a
	template<typename X, rbtree L, rbtree R> struct balleft;
	template<typename X, rbtree L, rbtree R> struct balleft2;  // avoid ambiguity
	template<typename X, rbtree L, rbtree R> struct balleft3;  // avoid ambiguity
	template<typename X, rbtree L, rbtree R> using balleft_t = typename balleft<X, L, R>::type;
	template<typename X, typename LX, rbtree LL, rbtree LR, rbtree R> struct balleft<X, Node<Red, LX, LL, LR>, R> {
		using type = Node<Red, X, Node<Black, LX, LL, LR>, R>;
	};
	template<typename X, rbtree L, rbtree R> struct balleft { using type = typename balleft2<X, L, R>::type; };

	template<typename X, rbtree L, typename RX, rbtree RL, rbtree RR> struct balleft2<X, L, Node<Black, RX, RL, RR>> {
		using type = balance_t<X, L, Node<Red, RX, RL, RR>>;
	};
	template<typename X, rbtree L, rbtree R> struct balleft2 { using type = typename balleft3<X, L, R>::type; };

	template<typename X, rbtree L, typename RX, typename RLX, rbtree RLL, rbtree RLR, rbtree RR> struct balleft3<X, L, Node<Red, RX, Node<Black, RLX, RLL, RLR>, RR>> {
		using type = Node<Red, RLX, Node<Black, X, L, RLL>, balance_t<RX, RLR, sub1_t<RR>>>;
	};

	//balright :: a -> RB a -> RB a -> RB a
	template<typename X, rbtree L, rbtree R> struct balright;
	template<typename X, rbtree L, rbtree R> struct balright2;  // avoid ambiguity
	template<typename X, rbtree L, rbtree R> struct balright3;  // avoid ambiguity
	template<typename X, rbtree L, rbtree R> using balright_t = typename balright<X, L, R>::type;
	template<typename X, rbtree L, typename RX, rbtree RL, rbtree RR> struct balright<X, L, Node<Red, RX, RL, RR>> {
		using type = Node<Red, X, L, Node<Black, RX, RL, RR>>;
	};
	template<typename X, rbtree L, rbtree R> struct balright { using type = typename balright2<X, L, R>::type; };

	template<typename X, typename LX, rbtree LL, rbtree LR, rbtree R> struct balright2<X, Node<Black, LX, LL, LR>, R> {
		using type = balance_t<X, Node<Red, LX, LL, LR>, R>;
	};
	template<typename X, rbtree L, rbtree R> struct balright2 { using type = typename balright3<X, L, R>::type; };

	template<typename X, typename LX, rbtree LL, typename LRX, rbtree LRL, rbtree LRR, rbtree R> struct balright3<X, Node<Red, LX, LL, Node<Black, LRX, LRL, LRR>>, R> {
		using type = Node<Red, LRX, balance_t<LX, sub1_t<LL>, LRL>, Node<Black, X, LRR, R>>;
	};

	// app :: RB a -> RB a -> RB a
	template<rbtree L, rbtree R> struct app;
	template<rbtree L, rbtree R> struct app2; // avoid ambiguity
	template<rbtree L, rbtree R> struct app3; // avoid ambiguity
	template<rbtree L, rbtree R> using app_t = typename app<L, R>::type;

	template<rbtree R> struct app<Leaf, R> { using type = R; };
	template<rbtree L> struct app<L, Leaf> { using type = L; };
	template<> struct app<Leaf, Leaf> { using type = Leaf; }; // C++ required
	template<rbtree L, rbtree R> struct app { using type = typename app2<L, R>::type; };

	template<typename LX, rbtree LL, rbtree LR, typename RX, rbtree RL, rbtree RR> struct app2<Node<Red, LX, LL, LR>, Node<Red, RX, RL, RR>> {
		template<rbtree B> struct caseof { using type = Node<Red, LX, LL, Node<Red, RX, B, RR>>; };
		template<typename TX, rbtree TL, rbtree TR> struct caseof<Node<Red, TX, TL, TR>> {
			using type = Node<Red, TX, Node<Red, LX, LL, TL>, Node<Red, RX, TR, RR>>;
		};
		using type = typename caseof<app_t<LR, RL>>::type;
	};
	template<typename LX, rbtree LL, rbtree LR, typename RX, rbtree RL, rbtree RR> struct app2<Node<Black, LX, LL, LR>, Node<Black, RX, RL, RR>> {
		template<rbtree B> struct caseof { using type = balleft_t<LX, LL, Node<Black, RX, B, RR>>; };
		template<typename TX, rbtree TL, rbtree TR> struct caseof<Node<Red, TX, TL, TR>> {
			using type = Node<Red, TX, Node<Black, LX, LL, TL>, Node<Black, RX, TR, RR>>;
		};
		using type = typename caseof<app_t<LR, RL>>::type;
	};
	template<rbtree L, rbtree R> struct app2 { using type = typename app3<L, R>::type; };

	template<rbtree L, typename RX, rbtree RL, rbtree RR> struct app3<L, Node<Red, RX, RL, RR>> {
		using type = Node<Red, RX, app_t<L, RL>, RR>;
	};
	template<typename LX, rbtree LL, rbtree LR, rbtree R> struct app3<Node<Red, LX, LL, LR>, R> {
		using type = Node<Red, LX, LL, app_t<LR, R>>;
	};

	// delete
	template<typename X, rbtree A> struct delete1 {
		template<rbtree B> struct caseof { using type = Leaf; };
		template<color C, typename Y, rbtree L, rbtree R> struct caseof<Node<C, Y, L, R>> {
			using type = Node<Black, Y, L, R>;
		};

		template<rbtree B, typename=void> struct del;
		template<rbtree B> using del_t = typename del<B>::type;
		template<typename Dummy> struct del<Leaf, Dummy> { using type = Leaf; };
		template<typename Y, rbtree L, rbtree R> struct delfromleft {
			using type = Node<Red, Y, del_t<L>, R>;
		};
		template<typename Y, typename LX, rbtree LL, rbtree LR, rbtree R> struct delfromleft<Y, Node<Black, LX, LL, LR>, R> {
			using type = balleft_t<Y, del_t<Node<Black, LX, LL, LR>>, R>;
		};
		template<color C, typename Y, rbtree L, rbtree R> struct del<Node<C, Y, L, R>, std::enable_if_t<(X{} < Y{})>> {
			using type = typename delfromleft<Y, L, R>::type;
		};
		template<typename Y, rbtree L, rbtree R> struct delfromright {
			using type = Node<Red, Y, L, del_t<R>>;
		};
		template<typename Y, rbtree L, typename RX, rbtree RL, rbtree RR> struct delfromright<Y, L, Node<Black, RX, RL, RR>> {
			using type = balright_t<Y, L, del_t<Node<Black, RX, RL, RR>>>;
		};

		template<color C, typename Y, rbtree L, rbtree R> struct del<Node<C, Y, L, R>, std::enable_if_t<(X{} > Y{})>> {
			using type = typename delfromright<Y, L, R>::type;
		};
		template<color C, typename Y, rbtree L, rbtree R> struct del<Node<C, Y, L, R>, std::enable_if_t<(X{} == Y{})>> {
			using type = app_t<L, R>;
		};

		using type = typename caseof<del_t<A>>::type;
	};

	template<typename X, rbtree A> using delete_t = typename delete1<X, A>::type;
}
