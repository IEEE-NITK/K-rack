#include <linux/fs.h>
#include <myfs.h>

/*
 *  The Dentry Object

The VFS(Virtual File System) treats directories as files. In the path /bin/vi, both bin and vi are files, bin being the special directory file and vi being a regular file. An inode object represents both these components. Despite this useful unification, the VFS often needs to perform directory-specific operations, such as path name lookup. Path name lookup involves translating each component of a path, ensuring it is valid, and following it to the next component.

To facilitate this, the VFS employs the concept of a directory entry (dentry). A dentry is a specific component in a path. Using the previous example, /, bin, and vi are all dentry objects. The first two are directories and the last is a regular file. This is an important point: dentry objects are all components in a path, including files. Resolving a path and walking its components is a nontrivial exercise, time-consuming and rife with string comparisons. The dentry object makes the whole process easier.

Dentries might also include mount points. In the path /mnt/cdrom/foo, the components /, mnt, cdrom, and foo are all dentry objects. The VFS constructs dentry objects on the fly, as needed, when performing directory operations.

Dentry objects are represented by struct dentry and defined in <linux/dcache.h>. Here is the structure, with comments describing each member:

struct dentry {
	* RCU lookup touched fields
	unsigned int d_flags;		* protected by d_lock
	seqcount_t d_seq;		    * per dentry seqlock
	struct hlist_bl_node d_hash;* lookup hash list
	struct dentry *d_parent;	* parent directory
	struct qstr d_name;
	struct inode *d_inode;		* Where the name belongs to - NULL is
					            * negative
	unsigned char d_iname[DNAME_INLINE_LEN];
                                * small names

	* Ref lookup also touches following
	struct lockref d_lockref;	* per-dentry lock and refcount
	const struct dentry_operations *d_op;
	struct super_block *d_sb;	* The root of the dentry tree
	unsigned long d_time;		* used by d_revalidate
	void *d_fsdata;			    * fs-specific data

	union {
		struct list_head d_lru;	* LRU list
		wait_queue_head_t *d_wait;
                                * in-lookup ones only
	};
	struct list_head d_child;	* child of parent list
	struct list_head d_subdirs;	* our children
	* d_alias and d_rcu can share memory

	union {
		struct hlist_node d_alias;
                                * inode alias list
		struct hlist_bl_node d_in_lookup_hash;
                                * only for in-lookup ones
	 	struct rcu_head d_rcu;
	} d_u;
};

Unlike the previous two objects, the dentry object does not correspond to any sort of on-disk data structure. The VFS creates it on the fly from a string representation of a path name. Because the dentry object is not physically stored on the disk, no flag in struct dentry specifies whether the object is modified (that is, whether it is dirty and needs to be written back to disk).

 *   Dentry State

A valid dentry object can be in one of three states: used, unused, or negative.
A used dentry corresponds to a valid inode (d_inode points to an associated inode) and indicates that there are one or more users of the object (d_count is positive). A used dentry is in use by the VFS and points to valid data and, thus, cannot be discarded.

An unused dentry corresponds to a valid inode (d_inode points to an inode), but indicates that the VFS is not currently using the dentry object (d_count is zero). Because the dentry object still points to a valid object, the dentry is kept around, cached, in case it is needed again. Because the dentry has not been destroyed prematurely, the dentry need not be re-created if it is needed in the future and path name lookups can complete quicker. If it is necessary to reclaim memory, however, the dentry can be discarded because it is not in use.

A negative dentry is not associated with a valid inode (d_inode is NULL) because either the inode was deleted or the path name was never correct to begin with. The dentry is kept around, however, so that future lookups are resolved quickly. Although the dentry is useful, it can be destroyed, if needed, because nothing can actually be using it.

    NOTE: The name is misleading. There is nothing particularly negative about a negative dentry. A better name might be invalid dentry.

A dentry object can also be freed, sitting in the slab object cache, as discussed in the previous chapter. In that case, there is no valid reference to the dentry object in any VFS or any filesystem code.

 *   The Dentry Cache

After the VFS layer goes through the trouble of resolving each element in a path name into a dentry object and arriving at the end of the path, it would be quite wasteful to throw away all that work. Instead, the kernel caches dentry objects in the dentry cache or, simply, the dcache.

The dentry cache consists of three parts:

    Lists of "used" dentries that are linked off their associated inode via the i_dentry field of the inode object. Because a given inode can have multiple links, there might be multiple dentry objects; consequently, a list is used.

    A doubly linked "least recently used" list of unused and negative dentry objects. The list is insertion sorted by time, such that entries toward the head of the list are newest. When the kernel must remove entries to reclaim memory, the entries are removed from the tail; those are the oldest and presumably have the least chance of being used in the near future.

    A hash table and hashing function used to quickly resolve a given path into the associated dentry object.

The hash table is represented by the dentry_hashtable array. Each element is a pointer to a list of dentries that hash to the same value. The size of this array depends on the amount of physical RAM in the system.

The actual hash value is determined by d_hash(). This enables filesystems to provide a unique hashing function.

Hash table lookup is performed via d_lookup(). If a matching dentry object is found in the dcache, it is returned. On failure, NULL is returned.

As an example, assume that you are editing a source file in your home directory, /home/dracula/src/the_sun_sucks.c. Each time this file is accessed (for example, when you first open it, later save it, compile it, and so on), the VFS must follow each directory entry to resolve the full path: /, home, dracula, src, and finally the_sun_sucks.c. To avoid this time-consuming operation each time this path name is accessed, the VFS can first try to look up the path name in the dentry cache. If the lookup succeeds, the required final dentry object is obtained without serious effort. Conversely, if the dentry is not in the dentry cache, the VFS must manually resolve the path by walking the filesystem for each component of the path. After this task is completed, the kernel adds the dentry objects to the dcache to speed up any future lookups.

The dcache also provides the front end to an inode cache, the icache. Inode objects that are associated with dentry objects are not freed because the dentry maintains a positive usage count over the inode. This enables dentry objects to pin inodes in memory. As long as the dentry is cached, the corresponding inodes are cached, too. Consequently, when a path name lookup succeeds from cache, as in the previous example, the associated inodes are already cached in memory.

 *   Dentry Operations

The dentry_operations structure specifies the methods that the VFS invokes on directory entries on a given filesystem.

The dentry_operations structure is defined in <linux/dcache.h>:
struct dentry_operations {
	int (*d_revalidate)(struct dentry *, unsigned int);
	int (*d_weak_revalidate)(struct dentry *, unsigned int);
	int (*d_hash)(const struct dentry *, struct qstr *);
	int (*d_compare)(const struct dentry *, const struct dentry *,
			unsigned int, const char *, const struct qstr *);
	int (*d_delete)(const struct dentry *);
	void (*d_release)(struct dentry *);
	void (*d_prune)(struct dentry *);
	void (*d_iput)(struct dentry *, struct inode *);
	char *(*d_dname)(struct dentry *, char *, int);
	struct vfsmount *(*d_automount)(struct path *);
	int (*d_manage)(struct dentry *, bool);
	struct inode *(*d_select_inode)(struct dentry *, unsigned);
	struct dentry *(*d_real)(struct dentry *, struct inode *);
} ____cacheline_aligned;

The methods are as follows:

    int d_revalidate(struct dentry *dentry,
                      int flags)
    This function determines whether the given dentry object is valid. The VFS calls this function whenever it is preparing to use a dentry from the dcache. Most filesystems set this method to NULL because their dentry objects in the dcache are always valid.

    int d_hash(struct dentry *dentry,
                struct qstr *name)
    This function creates a hash value from the given dentry. The VFS calls this function whenever it adds a dentry to the hash table.

    int d_compare(struct dentry *dentry,
                   struct qstr *name1,
                   struct qstr *name2)
    This function is called by the VFS to compare two filenames, name1 and name2. Most filesystems leave this at the VFS default, which is a simple string compare. For some filesystems, such as FAT, a simple string compare is insufficient. The FAT filesystem is not case sensitive and therefore needs to implement a comparison function that disregards case. This function requires the dcache_lock.

    int d_delete (struct dentry *dentry)
    This function is called by the VFS when the specified dentry object's d_count reaches zero. This function requires the dcache_lock.

    void d_release(struct dentry *dentry)
    This function is called by the VFS when the specified dentry is going to be freed. The default function does nothing.

    void d_iput(struct dentry *dentry,
                 struct inode *inode)
    This function is called by the VFS when a dentry object loses its associated inode (say, because the entry was deleted from the disk). By default, the VFS simply calls the iput() function to release the inode. If a filesystem overrides this function, it must also call iput() in addition to performing whatever filesystem-specific work it requires.
 */

struct dentry* myfs_mount(
    struct file_system_type *fs_type,
    int flags,
    char const *dev,
    void *data )
{
    struct dentry *const entry = mount_bdev(fs_type, flags, dev, data, myfs_fill_sb);
    /*
        mount() may choose to return a subtree of existing filesystem - it doesn't have to create a new one.  The main result from the caller's point of view is a reference to dentry at the root of (sub)tree to be attached; creation of new superblock is a common side effect.

        The most interesting member of the superblock structure that the mount() method fills in is the "s_op" field. This is a pointer to a "struct super_operations" which describes the next level of the filesystem implementation.

        Usually, a filesystem uses one of the generic mount() implementations and provides a fill_super() callback instead. The generic variants are:

            mount_bdev: mount a filesystem residing on a block device

            mount_nodev: mount a filesystem that is not backed by a device

            mount_single: mount a filesystem which shares the instance between all mounts

     */
    if(IS_ERR(entry))
    {
        pr_info("mcryptfs mounting failed.\n");
    }
    else
    {
        pr_info("mcryptfs mounting successful.\n");
    }
    return entry;
}
