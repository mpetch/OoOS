#include "fs/ramfs.hpp"
#include "rtc.h"
ramfs_folder_inode::ramfs_folder_inode(std::string const& name) : folder_inode_base{ name, reinterpret_cast<uint64_t>(this) } {}
bool ramfs_folder_inode::fsync() { for(tnode_dir::iterator first = __my_dir.begin(), last = __my_dir.end(); first != last; ) { if(first->operator bool()) first = __my_dir.erase(first); else ++first; } return true; }
uint64_t ramfs_folder_inode::num_folders() const noexcept { return __my_subdir_cnt; }
uint64_t ramfs_folder_inode::num_files() const noexcept { return __my_file_cnt; }
bool ramfs_folder_inode::unlink(std::string const &what) { bool result = __my_dir.erase(what) != 0; if(result) syscall_time(&this->modif_time); return result; }
tnode* ramfs_folder_inode::add(inode_base* n) { auto result = __my_dir.emplace(n, n->name()); if(result.second) syscall_time(&this->modif_time); return std::addressof(*(result.first)); }
bool ramfs_folder_inode::link(tnode *original, std::string const &alias) { bool result = (__my_dir.emplace(mklink(original, alias))).second; if(result) syscall_time(&this->modif_time); return result; }
tnode *ramfs_folder_inode::find(std::string const& name) { tnode_dir::iterator i = __my_dir.find(name); if(i != __my_dir.end()) { return std::addressof(*i); } return nullptr; }
ramfs_file_inode::ramfs_file_inode(std::string const& name, int fd) : file_inode_base{ name, fd, reinterpret_cast<uint64_t>(this) } {}
ramfs_file_inode::size_type ramfs_file_inode::write(const_pointer src, size_type n) { size_t result = this->sputn(src, n); if(result) syscall_time(&this->modif_time); return result; }
ramfs_file_inode::size_type ramfs_file_inode::read(pointer dest, size_type n) { return this->sgetn(dest, n); }
ramfs_file_inode::pos_type ramfs_file_inode::seek(off_type off, std::ios_base::seekdir way) { return this->seekoff(off, way); }
ramfs_file_inode::pos_type ramfs_file_inode::seek(pos_type pos) { return this->seekpos(pos); }
bool ramfs_file_inode::fsync() { this->__q_on_modify(); return true; }
uint64_t ramfs_file_inode::size() const noexcept { return this->__qsize(); }
ramfs_device_inode::ramfs_device_inode(std::string const &name, int fd, vfs_filebuf_base<char> *dev_buffer) : file_inode_base{ name, fd, reinterpret_cast<uint64_t>(dev_buffer) }, __my_device{ dev_buffer } {}
bool ramfs_device_inode::fsync() { return __my_device->pubsync() == 0; }
uint64_t ramfs_device_inode::size() const noexcept { return __my_device->in_avail(); }
ramfs_device_inode::size_type ramfs_device_inode::write(const_pointer src, size_type n) { size_type result = __my_device->sputn(src, n); if(result) syscall_time(&this->modif_time); return result; }
ramfs_device_inode::size_type ramfs_device_inode::read(pointer dest, size_type n) { return __my_device->sgetn(dest, n); }
ramfs_device_inode::pos_type ramfs_device_inode::seek(off_type off, std::ios_base::seekdir way) { return __my_device->pubseekoff(off, way); }
ramfs_device_inode::pos_type ramfs_device_inode::seek(pos_type pos) { return __my_device->pubseekpos(pos); }