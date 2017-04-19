# eve4ceph-mdti
Merge Diff file To Image for Ceph

Ceph Documentation

[rbd incremental backup](http://docs.ceph.com/docs/master/dev/rbd-diff/)

[rdb – manage rados block device (rbd) images](http://docs.ceph.com/docs/master/man/8/rbd/)

[Original idea ceph_apply_diff](https://gp2x.org/ceph/ceph_apply_diff-0.1.tar.gz)

```
    ______      __                       _              _    ________
   / ____/___  / /____  _________  _____(_)_______     | |  / / ____/
  / __/ / __ \/ __/ _ \/ ___/ __ \/ ___/ / ___/ _ \    | | / / __/
 / /___/ / / / /_/  __/ /  / /_/ / /  / (__  )  __/    | |/ / /___
/_____/_/ /_/\__/\___/_/  / .___/_/  /_/____/\___/     |___/_____/
                         /_/

EnterpriseVE Merge Diff file To Image for Ceph       (Made in Italy)

Usage:
    eve4ceph-mdti <FULL_IMAGE_FILE> <DIFF_FILE>
    eve4ceph-mdti version

Report bugs to <support@enterpriseve.com>. 
```

# Introduction
Ceph implements merge-diff to obtain single diff (rbd merge-diff diff1 diff2 diff_all).
This software assemble diff file into file raw image (rbd export).

[For Backup and Restore Ceph see eve4ceph-barc](https://github.com/EnterpriseVE/eve4ceph-barc)

# Configuration and use
Download package eve4ceph-mdti_?.?.?-?_all.deb, on your host and install:
```
wget https://github.com/EnterpriseVE/eve4ceph-mdti/releases/latest
dpkg -i eve4ceph-mdti_?.?.?-?_all.deb
```
This tool need basically no configuration.

## Execution
```
$ rbd export pool-rbd/vm-111-disk-1@snap1 vm-111-disk-1.img
$ rbd export-diff pool-rbd/vm-111-disk-1 --from-snap snap1 vm-111-disk-1-snap1.diff

$ eve4ceph-mdti vm-111-disk-1.img vm-111-disk-1-snap1.diff
```
The file vm-111-disk-1.img is now current, like if you had run rbd export pool-rbd/vm-111-disk-1.