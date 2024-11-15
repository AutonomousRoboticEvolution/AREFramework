#ifndef SKELETON_GENERATION_HPP
#define SKELETON_GENERATION_HPP
#include <vector>
#include <PolyVox/RawVolume.h>


namespace are {

namespace skeleton {

using type = PolyVox::RawVolume<uint8_t>;
using coord_t = std::vector<std::vector<std::vector<int>>>;

/**
 * @brief count_number_voxels
 * @param skeleton
 * @param number_voxels
 */
void count_number_voxels(const type& skeleton, int &number_voxels);

/**
 * @brief create_base
 * @param skeleton
 */
void create_base(type &skeleton);

/**
 * @brief empty_space_for_head
 * @param skeleton
 */
void empty_space_for_head(type &skeleton);

/**
 * @brief remove_skeleton_regions
 * @param skeleton
 */
void remove_skeleton_regions(type &skeleton);

/**
 * @brief remove_hoverhangs
 * @param skeleton
 */
void remove_hoverhangs(type& skeleton);

/**
 * @brief find_skeleton_surface
 * @param skeleton
 * @param skeleton_surface
 */
void find_skeleton_surface(const type &skeleton, coord_t &skeleton_surface);
}//are

}//skeleton

#endif //SKELETON_GENERATION_HPP
