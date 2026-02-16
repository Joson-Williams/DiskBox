# GPT分区表支持

#### 概述

GPT（GUIDPartitionTable，全局唯一标识分区表）是一种基于UEFI（统一可扩展固件接口）标准的磁盘分区结构。</br>GPT引入了先进的数据结构，解决了MBR在容量限制、分区数量及数据安全性方面的固有缺陷。

#### DiskBox对GPT（GUID分区表）的支持情况
**最大磁盘容量** </br>
GPT（GUID分区表）理论上支持8ZiB的总磁盘大小.</br>DiskBox内部采用`unsigned long long`类型来索引扇区，最多能表示 18446744073709551615（约 1.84×10¹⁹个）扇区。</br>当采用512字节作为扇区大小时，DiskBox大约可以寻址9EB的磁盘空间。

**最大分区数量** </br>
GPT（GUID分区表）理论上支持无限多个主分区。</br>注意：Windows等平台对分区的支持上限为128个，超出128个时，你将无法在Windows中查看并检索它们。</br>DiskBox未对主分区数作出任何限制，但当索引的逻辑分区数超过一定值时，将发出警告。

**数据冗余**</br>
GPT（GUID分区表）有位于磁盘首尾、双向冗余的主分区表和备份分区表。</br>读取分区时，DiskBox会自动检查两分区表并做出校验。</br>当分区表损坏时，DiskBox会尝试在内存中重建分区表，并根据该分区表进行读取。</br>出于安全考虑，DiskBox不会尝试自动修复分区表并写入磁盘，但可以手动操作。

**校验机制**</br>
GPT（GUID分区表）在磁盘头部和分区表自带CRC32校验和以确保数据完整性。</br>DiskBox中的校验是自动化的。校验不成功时，DiskBox会发出警告。


**LBA0(保护性MBR)**</br>
为了向后兼容旧版MBR工具，GPT磁盘的第一个扇区(LBA0)保留给MBR数据。该区域包含一个类型为0xEE的特殊分区，覆盖整个磁盘的LBA范围。这使得不支持GPT的MBR工具会将该磁盘识别为不可操作，从而防止了对分区的误删除或误格式化。其最后两个字节仍为标准的结束标志0x55AA。</br>DiskBox会自动配置保护性MBR，大多情况下，这无需作出更改。

**LBA2–33(主分区表项)**</br>
该区域包含分区表项数组，默认大小足以容纳128个分区条目，每个条目128字节。每个条目记录了对应分区的起始LBA、结束LBA、分区类型GUID及唯一分区GUID等信息。</br>DiskBox会按需自动扩展分区表长度。</br>请注意：某些软件默认只读取128个分区条目，在DiskBox扩展后，这些条目可能会使得这些软件无法读取。</br>请注意：Windows等平台对主分区的支持上限为128个，超出128个时，你将无法在Windows中查看并检索它们。</br>DiskBox的分区读取不依赖于Windows。

**操作系统支持**</br>
GPT（GUID分区表）的支持性如下：
- **Windows**：从WindowsXP x64Edition开始支持GPT；后续的Vista、7、8、10、11等64位版本均支持从GPT磁盘启动，但要求计算机使用UEFI固件。
- **Linux**：Linux内核很早就内置了对GPT的支持。主流发行版普遍使用GPT作为大容量磁盘和UEFI启动的标准分区方案。
- **macOS**：自2006年起，随着Apple过渡到Intel处理器，GPT成为macOS安装磁盘的默认分区格式。

请注意：DiskBox目前仅支持Windows。暂时没有macOS和Linux的兼容计划。

</br>

==================</br>
你当前的位置：support.joson.zh-cn/diskbox/gpt-support</br>最后编辑于：2026/2/16

"Windows"是Microsoft Corp.的注册商标。</br>"Apple""macOS"是Apple Inc.的注册商标。</br>"Linux"是Linus Torvalds在美国和其他国家和地区的注册商标。
