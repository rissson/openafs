#ifndef UKERNEL
/* This section for kernel libafs compiles only */

#ifndef AFS_PARAM_H
#define AFS_PARAM_H

#define AFS_ENV                 1
#define AFS_64BIT_IOPS_ENV 1
#define AFS_PPC_ENV 1
#define AFS_VFSINCL_ENV 1

#include <afs/afs_sysnames.h>

#define AFS_DARWIN_ENV
#define AFS_DARWIN70_ENV
#undef  AFS_NONFSTRANS
#define AFS_NONFSTRANS
#define AFS_SYSCALL             230
#define AFS_NAMEI_ENV 1
#define DARWIN_REFBASE 3

/* File system entry (used if mount.h doesn't define MOUNT_AFS */
#define AFS_MOUNT_AFS    "afs"

/* Machine / Operating system information */
#define sys_ppc_darwin_12   1
#define sys_ppc_darwin_13   1
#define sys_ppc_darwin_14   1
#define sys_ppc_darwin_60   1
#define sys_ppc_darwin_70   1
#define SYS_NAME        "ppc_darwin_70"
#define SYS_NAME_ID     SYS_NAME_ID_ppc_darwin_70
#define AFSBIG_ENDIAN   1
#define AFS_HAVE_FFS    1	/* Use system's ffs. */

#define AFS_GCPAGS                1	/* if nonzero, garbage collect PAGs */
#define RXK_LISTENER_ENV         1

#ifdef KERNEL
#undef MACRO_BEGIN
#undef MACRO_END
#include <kern/macro_help.h>
#define AFS_GLOBAL_SUNLOCK        1
#define AFS_VFS34       1	/* What is VFS34??? */
#define afsio_iov       uio_iov
#define afsio_iovcnt    uio_iovcnt
#define afsio_offset    uio_offset
#define afsio_seg       uio_segflg
#define afsio_resid     uio_resid
#define AFS_UIOSYS      UIO_SYSSPACE
#define AFS_UIOUSER     UIO_USERSPACE
#define AFS_CLBYTES     CLBYTES
#define AFS_KALLOC(x)   kalloc(x)
#define AFS_KFREE(x,y)  kfree(x,y)
#define v_count         v_usecount
#define v_vfsp          v_mount
#define vfs_bsize       mnt_stat.f_bsize
#define vfs_fsid        mnt_stat.f_fsid
#define va_nodeid       va_fileid
#define vfs_vnodecovered mnt_vnodecovered
#define direct          dirent
#define vnode_t         struct vnode *
#define mount_t         struct mount *

//#define VN_RELE(vp)     vrele(((struct vnode *)(vp)))
//#define VN_HOLD(vp)     VREF(((struct vnode *)(vp)))
#define BIND_8_COMPAT

#endif
#endif /* AFS_PARAM_H */

#else /* !defined(UKERNEL) */

/* This section for user space compiles only */

#ifndef AFS_PARAM_H
#define AFS_PARAM_H

#define AFS_VFSINCL_ENV 1	/* NOBODY uses this.... */
#define AFS_ENV                 1
#define AFS_PPC_ENV 1

#include <afs/afs_sysnames.h>
#define AFS_USERSPACE_ENV
#define AFS_USR_DARWIN_ENV
#define AFS_USR_DARWIN70_ENV
#undef  AFS_NONFSTRANS
#define AFS_NONFSTRANS
#define AFS_SYSCALL             230
#define DARWIN_REFBASE 0

/* File system entry (used if mount.h doesn't define MOUNT_AFS */
#define AFS_MOUNT_AFS    "afs"

/* Machine / Operating system information */
#define sys_ppc_darwin_12   1
#define sys_ppc_darwin_13   1
#define sys_ppc_darwin_14   1
#define sys_ppc_darwin_60   1
#define sys_ppc_darwin_70   1
#define SYS_NAME        "ppc_darwin_70"
#define SYS_NAME_ID     SYS_NAME_ID_ppc_darwin_70
#define AFSBIG_ENDIAN   1
#define AFS_HAVE_FFS    1	/* Use system's ffs. */

#define AFS_UIOSYS      UIO_SYSSPACE
#define AFS_UIOUSER     UIO_USERSPACE

#define AFS_GCPAGS                0	/* if nonzero, garbage collect PAGs */
#define RXK_LISTENER_ENV          1

#define AFS_VFS34       1	/* What is VFS34??? */
#define afsio_iov       uio_iov
#define afsio_iovcnt    uio_iovcnt
#define afsio_offset    uio_offset
#define afsio_seg       uio_segflg
#define afsio_resid     uio_resid
#define AFS_UIOSYS      UIO_SYSSPACE
#define AFS_UIOUSER     UIO_USERSPACE
#define        VATTR_NULL      usr_vattr_null

#define AFS_DIRENT
#ifndef CMSERVERPREF
#define CMSERVERPREF
#endif

#define BIND_8_COMPAT
#endif /* AFS_PARAM_H */

#endif /* !defined(UKERNEL) */
