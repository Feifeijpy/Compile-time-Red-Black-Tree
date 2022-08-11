#include"RBTree.h"

using namespace tmp_rbtree_np;

template<std::size_t I> using tmp_int = std::integral_constant<std::size_t, I>;
static_assert(tmp_int<1>{} < tmp_int<2>{});
static_assert(tmp_int<3>{} > tmp_int<2>{});

using rbt1 = insert_t<tmp_int<1>, Leaf>;
using rbt2 = insert_t<tmp_int<3>, rbt1>;
using rbt3 = insert_t<tmp_int<2>, rbt2>;
using rbt4 = insert_t<tmp_int<5>, rbt3>;
using rbt5 = insert_t<tmp_int<6>, rbt4>;
using rbt6 = insert_t<tmp_int<4>, rbt5>;

static_assert(member_v<tmp_int<5>, rbt6>);
static_assert(!member_v<tmp_int<7>, rbt6>);

using rbtd1 = delete_t<tmp_int<5>, rbt6>;
using rbtd2 = delete_t<tmp_int<2>, rbtd1>;
using rbtd3 = delete_t<tmp_int<6>, rbtd2>;

int main() {
	std::cout << rbt2{} << std::endl << std::endl;
	std::cout << rbt3{} << std::endl << std::endl;
	std::cout << rbt4{} << std::endl << std::endl;
	std::cout << rbt5{} << std::endl << std::endl;
	std::cout << rbt6{} << std::endl << std::endl;

	std::cout << rbtd1{} << std::endl << std::endl;
	std::cout << rbtd2{} << std::endl << std::endl;
	std::cout << rbtd3{} << std::endl << std::endl;

	return 0;
}
