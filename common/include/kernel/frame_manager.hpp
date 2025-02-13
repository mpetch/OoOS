#ifndef __FRAME_MGR
#define __FRAME_MGR
#include "heap_allocator.hpp"
#include "vector"
class frame_manager : std::vector<uframe_tag>
{
    typedef std::vector<uframe_tag> __base;
    static frame_manager __inst;
    constexpr frame_manager() = default;
public:    
    using __base::iterator;
    using __base::const_iterator;
    uframe_tag& create_frame(addr_t start_base, addr_t start_extent);
    uframe_tag& duplicate_frame(uframe_tag const& t);
    void destroy_frame(uframe_tag& ft);
    static frame_manager& get();
};
#endif