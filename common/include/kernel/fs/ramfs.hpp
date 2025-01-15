#ifndef __RAMFS
#define __RAMFS
#include "fs/fs.hpp"
#include "fs/generic_binary_buffer.hpp"
#include "fs/vfs_filebuf_base.hpp"
class ramfs_folder_inode : public folder_inode_base
{
    tnode_dir __my_dir{};
    size_t __my_file_cnt{};
    size_t __my_subdir_cnt{};
public:
    virtual tnode* find(std::string const& name) override;
    virtual bool link(tnode* original, std::string const& alias) override;
    virtual tnode* add(inode_base*) override;
    virtual bool unlink(std::string const& what) override;
    virtual uint64_t num_files() const noexcept override;
    virtual uint64_t num_folders() const noexcept override;
    ramfs_folder_inode(std::string const& name);
    virtual bool fsync() override;
};
class ramfs_file_inode final : generic_binary_buffer<char>, public file_inode_base
{
	using file_inode_base::traits_type;
	using file_inode_base::difference_type;
	using file_inode_base::size_type;
	using file_inode_base::pos_type;
	using file_inode_base::off_type;
	using file_inode_base::pointer;
	using file_inode_base::const_pointer;
public:
    virtual size_type write(const_pointer src, size_type n) override;
    virtual size_type read(pointer dest, size_type n) override;
    virtual pos_type seek(off_type, std::ios_base::seekdir) override;
    virtual pos_type seek(pos_type) override;
    ramfs_file_inode(std::string const& name, int fd);
    virtual bool fsync() override;
    virtual uint64_t size() const noexcept override;
};
class ramfs_device_inode final : public file_inode_base
{
    using file_inode_base::traits_type;
	using file_inode_base::difference_type;
	using file_inode_base::size_type;
	using file_inode_base::pos_type;
	using file_inode_base::off_type;
	using file_inode_base::pointer;
	using file_inode_base::const_pointer;
    vfs_filebuf_base<char>* __my_device;
public:
    virtual size_type write(const_pointer src, size_type n) override;
    virtual size_type read(pointer dest, size_type n) override;
    virtual pos_type seek(off_type, std::ios_base::seekdir) override;
    virtual pos_type seek(pos_type) override;
    ramfs_device_inode(std::string const& name, int fd, vfs_filebuf_base<char>* dev_buffer);
    virtual bool fsync() override;
    virtual uint64_t size() const noexcept override;
};
class ramfs final : public fs_base
{
    ramfs_folder_inode __root_dir{ "" };
    std::set<ramfs_file_inode, std::less<file_inode_base>> __file_nodes{};
    std::set<ramfs_folder_inode, std::less<folder_inode_base>> __folder_nodes{};
protected:
    virtual folder_inode_base* get_root_directory() override;
    virtual void dlfilenode(file_inode_base* fd) override;
    virtual void dldirnode(folder_inode_base* dd) override;
    virtual file_inode_base* mkfilenode(folder_inode_base* parent, std::string const& name) override;
    virtual folder_inode_base* mkdirnode(folder_inode_base* parent, std::string const& name) override;
public:
    ramfs() = default;
};
#endif