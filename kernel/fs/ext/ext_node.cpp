#include "fs/ext.hpp"
ext_vnode::ext_vnode(extfs *parent, ext_inode *inode) :
    vfs_filebuf_base<char>      {}, 
    parent_fs                   { parent }, 
    on_disk_node                { inode }
                                {}
ext_vnode::ext_vnode(extfs *parent, uint32_t inode_number) : ext_vnode{ parent, parent->read_inode(inode_number) } {}
ext_vnode::~ext_vnode() 
{ 
    if(on_disk_node) std::allocator<ext_inode>{}.deallocate(on_disk_node, 1UL);
}
void ext_vnode::add_block(uint64_t block_number, char *data_ptr) { block_data.emplace_back(block_number, data_ptr); }
int ext_vnode::__ddwrite() { return 0; /* disk r/w through fsync only */ }
size_t ext_vnode::block_of_data_ptr(size_t offs) { return offs / parent_fs->block_size(); }
uint64_t ext_vnode::next_block()
{
    
}
std::streamsize ext_vnode::__overflow(std::streamsize n)
{
    return std::streamsize();
}
std::streamsize ext_file_vnode::__ddread(std::streamsize n)
{
    return std::streamsize();
}
std::streamsize ext_file_vnode::__ddrem() { return std::streamsize(size() - (this->__capacity())); }
ext_file_vnode::size_type ext_file_vnode::write(const_pointer src, size_type n) 
{
    size_t pre_wr_cur_block = block_of_data_ptr(this->__size());
    size_type result = this->sputn(src, n);
    size_t post_wr_next_block = block_of_data_ptr(this->__size()) + 1UL;
    for(size_t i = pre_wr_cur_block; i < post_wr_next_block && i < block_data.size(); i++) block_data[i].dirty = true;
    return result;
}
ext_file_vnode::size_type ext_file_vnode::read(pointer dest, size_type n) { return this->sgetn(dest, n); }
ext_file_vnode::pos_type ext_file_vnode::seek(off_type off, std::ios_base::seekdir way) { return this->data_buffer<char>::seekoff(off, way); }
ext_file_vnode::pos_type ext_file_vnode::seek(pos_type pos) { return this->data_buffer<char>::seekpos(pos); }
bool ext_file_vnode::fsync() { return parent_fs->persist(this); }
uint64_t ext_file_vnode::size() const noexcept { return qword(on_disk_node->size_lo, on_disk_node->dir_acl_block); }
ext_file_vnode::pos_type ext_file_vnode::tell() const { return this->vfs_filebuf_base<char>::tell(); }
ext_file_vnode::ext_file_vnode(extfs *parent, uint32_t inode_number, int fd) : ext_vnode{ parent, inode_number }, file_node{ "", fd, inode_number } { mode = on_disk_node->mode; }
ext_file_vnode::ext_file_vnode(extfs *parent, uint32_t inode_number, ext_inode *inode_data, int fd) : ext_vnode{ parent, inode_data }, file_node{ "", fd, inode_number } { mode = on_disk_node->mode; }
std::streamsize ext_directory_vnode::__ddread(std::streamsize n) { return std::streamsize(0); }
std::streamsize ext_directory_vnode::__ddrem() { return std::streamsize(0); }
tnode *ext_directory_vnode::find(std::string const& name) { if(tnode_dir::iterator i = __my_dir.find(name); i != __my_dir.end()) return i.base(); else return nullptr; }
bool ext_directory_vnode::link(tnode* original, std::string const& target)
{
    return false;
}
tnode *ext_directory_vnode::add(fs_node* n)
{
    return nullptr;
}
bool ext_directory_vnode::unlink(std::string const& name)
{
    return false;
}
uint64_t ext_directory_vnode::num_files() const noexcept { return __n_files; }
uint64_t ext_directory_vnode::num_subdirs() const noexcept { return __n_subdirs; }
std::vector<std::string> ext_directory_vnode::lsdir() const { std::vector<std::string> result{}; for(tnode_dir::const_iterator i = __my_dir.begin(); i != __my_dir.end(); i++) result.push_back(i->name()); return result; }
bool ext_directory_vnode::fsync() { return parent_fs->persist(this); }
ext_directory_vnode::ext_directory_vnode(extfs *parent, uint32_t inode_number) : ext_vnode{ parent, inode_number }, directory_node{ "", inode_number } { mode = on_disk_node->mode; }
ext_directory_vnode::ext_directory_vnode(extfs *parent, uint32_t inode_number, ext_inode *inode_data) : ext_vnode{ parent, inode_data }, directory_node{ "", inode_number } { mode = on_disk_node->mode; }